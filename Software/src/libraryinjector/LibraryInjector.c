/*
 * LibraryInjector.cpp
 *
 *  Created on: 05.06.2012
 *     Project: Lightpack
 *
 *  Copyright (c) 2012 Timur Sattarov
 *
 *  Lightpack a USB content-driving ambient lighting system
 *
 *  Lightpack is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Lightpack is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "LibraryInjector.h"
#include "olectl.h"

/*! A count of how many objects of our DLL has been created
*/
static volatile LONG comObjectsCount = 0;

/*! A count of how many apps have locked our DLL via calling our
 IClassFactory object's LockServer()
*/
static volatile LONG locksCount = 0;

#define REPORT_LOG_BUF_SIZE 2048
static HANDLE hEventSrc = NULL;

void reportLog(WORD logType, const LPWSTR message, ...) {
    va_list ap;

    WCHAR *reportLogBuf = malloc(REPORT_LOG_BUF_SIZE);
    memset(reportLogBuf, 0, REPORT_LOG_BUF_SIZE);

    va_start( ap, message );
    int sprintfResult = wvsprintfW(reportLogBuf, message, ap);
    va_end( ap );
    if (sprintfResult > -1)
        ReportEventW(hEventSrc, logType, 0, 0x100, NULL, 1, 0, &reportLogBuf, NULL);

    free(reportLogBuf);
}

static BOOL SetPrivilege(HANDLE hToken, LPCTSTR szPrivName, BOOL fEnable) {

    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    LookupPrivilegeValue(NULL, szPrivName, &tp.Privileges[0].Luid);
    tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
    AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof (tp), NULL, NULL);
    return ((GetLastError() == ERROR_SUCCESS));

}

static BOOL AcquirePrivilege() {

    HANDLE hCurrentProc = GetCurrentProcess();
    HANDLE hCurrentProcToken;

    if (!OpenProcessToken(hCurrentProc, TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hCurrentProcToken)) {
        reportLog(EVENTLOG_ERROR_TYPE, L"OpenProcessToken Error 0x%x", GetLastError());
    } else {
        if (!SetPrivilege(hCurrentProcToken, SE_DEBUG_NAME, TRUE)) {
            reportLog(EVENTLOG_ERROR_TYPE, L"SetPrivleges SE_DEBUG_NAME Error 0x%x", GetLastError());
        } else {
            return TRUE;
        }
    }
    return FALSE;
}

static _LibraryInjector(LibraryInjector * this){
    InterlockedIncrement(&comObjectsCount);
    this->refCount = 0;
    this->lpVtbl = &libraryInjectorVtbl;
}

static __LibraryInjector(LibraryInjector * this){
    InterlockedDecrement(&comObjectsCount);
}

static HRESULT STDMETHODCALLTYPE LibraryInjector_QueryInterface(LibraryInjector * this, REFIID interfaceGuid, void **ppv) {
    if (!IsEqualIID(interfaceGuid, &IID_IUnknown) && !IsEqualIID(interfaceGuid, &IID_ILibraryInjector)) {
      *ppv = 0;
      return(E_NOINTERFACE);
    }

    // Fill in the caller's handle
    *ppv = this;

    // Increment the count of callers who have an outstanding pointer to this object
    this->lpVtbl->AddRef(this);

    return(NOERROR);
}

static ULONG STDMETHODCALLTYPE LibraryInjector_AddRef(LibraryInjector * this) {
    return InterlockedIncrement(&(this->refCount));
}

static ULONG STDMETHODCALLTYPE LibraryInjector_Release(LibraryInjector * this) {
    InterlockedDecrement(&(this->refCount));
    if ((this->refCount) == 0) {
        __LibraryInjector(this);
        GlobalFree(this);
        return(0);
    }
    return (this->refCount);
}

static HRESULT STDMETHODCALLTYPE LibraryInjector_Inject(LibraryInjector * this, DWORD ProcessId, LPWSTR ModulePath) {
    char CodePage[4096] ={
        0x90,                                     // nop (to replace with int 3h)
        0xC7, 0x04, 0xE4, 0x00, 0x00, 0x00, 0x00, // mov DWORD PTR [esp], 0h | DLLName to inject (DWORD)
        0xB8, 0x00, 0x00, 0x00, 0x00,             // mov eax, LoadLibProc
        0xFF, 0xD0,                               // call eax
        0x83, 0xEC, 0x04,                         // sub esp,4
        0xB8, 0x00, 0x00, 0x00, 0x00,             // mov eax, ExitTheadProc
        0xFF, 0xD0                                // call eax
    };
#define LIB_NAME_OFFSET 4
#define LOAD_LIB_OFFSET 9
#define EXIT_THREAD_OFFSET 19
#define SIZE_OF_CODE 25
    if(AcquirePrivilege()) {
        HMODULE hKernel32 = GetModuleHandle(L"kernel32.dll");

        HANDLE Process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId);

        if (Process == NULL) {
            return S_FALSE;
        }

        int sizeofCP = wcslen(ModulePath)*2 + SIZE_OF_CODE + 1;
        LPVOID Memory = VirtualAllocEx(Process, 0, sizeofCP, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

        if (!Memory)
            return S_FALSE;

        LPWSTR *DLLName;
        DWORD *LoadLibProc, *LibNameArg, *ExitThreadProc;

        DLLName = (LPWSTR) ((DWORD) CodePage + SIZE_OF_CODE);
        LoadLibProc = (DWORD*) (CodePage + LOAD_LIB_OFFSET);
        LibNameArg = (DWORD*) (CodePage + LIB_NAME_OFFSET);
        ExitThreadProc = (DWORD*) (CodePage + EXIT_THREAD_OFFSET);

        wcscpy(DLLName, ModulePath);
        *LoadLibProc = (DWORD) GetProcAddress(hKernel32, "LoadLibraryW");
        *ExitThreadProc = (DWORD) GetProcAddress(hKernel32, "ExitThread");
        *LibNameArg = (DWORD) (Memory + SIZE_OF_CODE); // need to do this: *EBX = *EBX + (Section)
        ////////////////////////////

        WriteProcessMemory(Process, Memory, CodePage, sizeofCP, 0);

        HANDLE hThread = CreateRemoteThread(Process, 0, 0, (LPTHREAD_START_ROUTINE) Memory, 0, 0, 0);
        //    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CodePage, 0, 0, 0);
        if (!hThread)
            return S_FALSE;
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);

        return S_OK;
    } else {
        return S_FALSE;
    }
}





static _ClassFactory(ClassFactory * this) {
    InterlockedIncrement(&comObjectsCount);
    this->refCount = 0;
    this->lpVtbl = &classFactoryVtbl;
}

static __ClassFactory(ClassFactory * this) {
    InterlockedDecrement(&comObjectsCount);
}

static ULONG STDMETHODCALLTYPE ClassFactory_AddRef(ClassFactory * this) {
    return InterlockedIncrement(&(this->refCount));
}

// IClassFactory's QueryInterface()
static HRESULT STDMETHODCALLTYPE ClassFactory_QueryInterface(ClassFactory * this, REFIID factoryGuid, void **ppv) {
    if (IsEqualIID(factoryGuid, &IID_IUnknown) || IsEqualIID(factoryGuid, &IID_IClassFactory))
    {
        this->lpVtbl->AddRef(this);
        *ppv = this;

        return(NOERROR);
    }
    *ppv = 0;
    return(E_NOINTERFACE);
}

static ULONG STDMETHODCALLTYPE ClassFactory_Release(ClassFactory * this) {
    InterlockedDecrement(&(this->refCount));
    if ((this->refCount) == 0) {
        __ClassFactory(this);
        GlobalFree(this);
        return (0);
    }
    return (this->refCount);
}

static HRESULT STDMETHODCALLTYPE ClassFactory_CreateInstance(ClassFactory * this, IUnknown *punkOuter, REFIID vTableGuid, void **objHandle) {
    HRESULT				hr;
    register LibraryInjector	*thisobj;

    // Assume an error by clearing caller's handle
    *objHandle = 0;

    // We don't support aggregation in this example
    if (punkOuter)
        hr = CLASS_E_NOAGGREGATION;
    else
    {
        if (!(thisobj = GlobalAlloc(GMEM_FIXED, sizeof(LibraryInjector))))
            hr = E_OUTOFMEMORY;
        else
        {
            _LibraryInjector(thisobj);
            // Fill in the caller's handle with a pointer to the LibraryInjector we just
            // allocated above. We'll let LibraryInjector's QueryInterface do that, because
            // it also checks the GUID the caller passed, and also increments the
            // reference count (to 2) if all goes well
            hr = thisobj->lpVtbl->QueryInterface(thisobj, vTableGuid, objHandle);

//            thisobj->AddRef();

            // Decrement reference count. NOTE: If there was an error in QueryInterface()
            // then Release() will be decrementing the count back to 0 and will free the
            // LibraryInjector for us. One error that may occur is that the caller is asking for
            // some sort of object that we don't support (ie, it's a GUID we don't recognize)
//            thisobj->Release();

        }
    }

    return(hr);
}

// IClassFactory's LockServer(). It is called by someone
// who wants to lock this DLL in memory
static HRESULT STDMETHODCALLTYPE ClassFactory_LockServer(ClassFactory * this, BOOL flock)
{
    if (flock) InterlockedIncrement(&locksCount);
    else InterlockedDecrement(&locksCount);

    return(NOERROR);
}










// Miscellaneous functions ///////////////////////////////////////////////////////

static const WCHAR ObjectDescription[] = L"LibraryInjector COM component";

static const WCHAR ClassKeyName[] = L"Software\\Classes";
static const WCHAR CLSID_Str[] = L"CLSID";
static const WCHAR InprocServer32Name[] = L"InprocServer32";
static const WCHAR ThreadingModel[] = L"ThreadingModel";
static const WCHAR BothStr[] = L"Apartment";
static const WCHAR GUID_Format[] = L"{%08lx-%04lx-%04lx-%02lx%02lx-%02lx%02lx%02lx%02lx%02lx%02lx}";
static const char * guid_format = "{%08lx-%04lx-%04lx-%02lx%02lx-%02lx%02lx%02lx%02lx%02lx%02lx}";

static void stringFromCLSIDW(LPWSTR buffer, REFCLSID ri)
{
    wsprintfW(buffer, GUID_Format,
        ri->Data1,    ri->Data2,    ri->Data3,    ri->Data4[0],
        ri->Data4[1], ri->Data4[2], ri->Data4[3],
        ri->Data4[4], ri->Data4[5], ri->Data4[6],
        ri->Data4[7]);
}

static void stringFromCLSID(LPSTR buffer, REFCLSID ri)
{
    wsprintfA(buffer, guid_format,
        ri->Data1,    ri->Data2,    ri->Data3,    ri->Data4[0],
        ri->Data4[1], ri->Data4[2], ri->Data4[3],
        ri->Data4[4], ri->Data4[5], ri->Data4[6],
        ri->Data4[7]);
}

HMODULE GetCurrentModule()
{
  // Here's a trick that will get you the handle of the module
  // you're running in without any a-priori knowledge:
  // http://www.dotnet247.com/247reference/msgs/13/65259.aspx

  MEMORY_BASIC_INFORMATION mbi;
  static int dummy;
  VirtualQuery( &dummy, &mbi, sizeof(mbi) );

  return (HMODULE)(mbi.AllocationBase);
}

STDAPI DllRegisterServer(void) {
    HKEY		rootKey;
    HKEY		hKey;
    HKEY		hKey2;
    HKEY		hkExtra;
    WCHAR		buffer[100];
    DWORD		disposition;
    HRESULT			result;
    WCHAR			filename[MAX_PATH];

    GetModuleFileNameW(GetCurrentModule(), filename, sizeof(filename));

    result = S_FALSE;
    // Open "HKEY_LOCAL_MACHINE\Software\Classes"
    if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE, &ClassKeyName[0], 0, KEY_WRITE, &rootKey))
    {
        // Open "HKEY_LOCAL_MACHINE\Software\Classes\CLSID"
        if (!RegOpenKeyEx(rootKey, &CLSID_Str[0], 0, KEY_ALL_ACCESS, &hKey))
        {
            // Create a subkey whose name is the ascii string that represents
            // our IExample2 object's GUID
            stringFromCLSIDW(&buffer[0], (REFCLSID)(&CLSID_ILibraryInjector));
            if (!RegCreateKeyEx(hKey, &buffer[0], 0, 0, REG_OPTION_NON_VOLATILE, KEY_WRITE, 0, &hKey2, &disposition))
            {
                // Set its default value to some "friendly" string that helps
                // a user identify what this COM DLL is for. Setting this value
                // is optional. You don't need to do it
                RegSetValueEx(hKey2, 0, 0, REG_SZ, (const BYTE *)&ObjectDescription[0], sizeof(ObjectDescription));

                // Create an "InprocServer32" key whose default value is the path of this DLL
                if (!RegCreateKeyEx(hKey2, &InprocServer32Name[0], 0, 0, REG_OPTION_NON_VOLATILE, KEY_WRITE, 0, &hkExtra, &disposition)) {
                    if (!RegSetValueEx(hkExtra, 0, 0, REG_SZ, (const BYTE *)&filename[0], wcslen(&filename[0]) * 2 + 2)) {
                        // Create a "ThreadingModel" value set to the string "both" (ie, we don't need to restrict an
                        // application to calling this DLL's functions only from a single thread. We don't use global
                        // data in our IExample functions, so we're thread-safe)
                        if (!RegSetValueEx(hkExtra, &ThreadingModel[0], 0, REG_SZ, (const BYTE *)&BothStr[0], sizeof(BothStr))) {
                            result = S_OK;
                        }
                    }

        // Close all keys we opened/created.

                    RegCloseKey(hkExtra);
                }

                RegCloseKey(hKey2);
            }

            RegCloseKey(hKey);
        }

        RegCloseKey(rootKey);
    }
    return result;
}

STDAPI DllUnregisterServer(void) {

    HKEY		rootKey;
    HKEY		hKey;
    HKEY		hKey2;
    WCHAR		buffer[100];

    stringFromCLSIDW(buffer, (REFCLSID)(&CLSID_ILibraryInjector));

    // Open "HKEY_LOCAL_MACHINE\Software\Classes"
    if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE, &ClassKeyName[0], 0, KEY_WRITE, &rootKey))
    {
        // Delete our CLSID key and everything under it
        if (!RegOpenKeyEx(rootKey, &CLSID_Str[0], 0, KEY_ALL_ACCESS, &hKey))
        {
            if (!RegOpenKeyEx(hKey, buffer, 0, KEY_ALL_ACCESS, &hKey2))
            {
                RegDeleteKey(hKey2, &InprocServer32Name[0]);

                RegCloseKey(hKey2);
                RegDeleteKey(hKey, &buffer[0]);
            }

            RegCloseKey(hKey);
        }

        RegCloseKey(rootKey);
    }
    return S_OK;
}



/************************ DllGetClassObject() ***********************
 * This is called by the OLE functions CoGetClassObject() or
 * CoCreateInstance() in order to get our DLL's IClassFactory object
 * (and return it to someone who wants to use it to get ahold of one
 * of our LibraryInjector objects). Our IClassFactory's CreateInstance() can
 * be used to allocate/retrieve our LibraryInjector object.
 *
 * NOTE: After we return the pointer to our IClassFactory, the caller
 * will typically call its CreateInstance() function.
 */

LIBRARYINJECTORSHARED_EXPORT HRESULT PASCAL DllGetClassObject(REFCLSID objGuid, REFIID factoryGuid, void **factoryHandle)
{
    register HRESULT		hr;

#ifdef DEBUG
    char guida[100];
    stringFromCLSID(guida, objGuid);
    reportLog(EVENTLOG_INFORMATION_TYPE, "class guid %s",guida);
    stringFromCLSID(guida, factoryGuid);
    reportLog(EVENTLOG_INFORMATION_TYPE, "factory guid %s", guida);
#endif

    ClassFactory * classFactory;
    if (IsEqualCLSID(objGuid, &CLSID_ILibraryInjector))
    {

        classFactory = (ClassFactory *)GlobalAlloc(GMEM_FIXED, sizeof(ClassFactory));
        _ClassFactory(classFactory);
        // Fill in the caller's handle with a pointer to our IClassFactory object.
        // We'll let our IClassFactory's QueryInterface do that, because it also
        // checks the IClassFactory GUID and does other book-keeping
        hr = classFactory->lpVtbl->QueryInterface(classFactory, factoryGuid, factoryHandle);
    }
    else
    {
#ifdef DEBUG
        WCHAR supportedGuid[100], requestedGuid[100];
        stringFromCLSIDW(requestedGuid, objGuid);
        stringFromCLSIDW(supportedGuid, &CLSID_ILibraryInjector);
        reportLog(EVENTLOG_ERROR_TYPE, L"class guid is not matched. supported guid %s, requested guid %s", supportedGuid, requestedGuid);
#endif

        // We don't understand this GUID. It's obviously not for our DLL.
        // Let the caller know this by clearing his handle and returning
        // CLASS_E_CLASSNOTAVAILABLE
        *factoryHandle = 0;
        hr = CLASS_E_CLASSNOTAVAILABLE;
    }

    return(hr);
}





/************************ DllCanUnloadNow() ***********************
 * This is called by some OLE function in order to determine
 * whether it is safe to unload our DLL from memory.
 *
 * RETURNS: S_OK if safe to unload, or S_FALSE if not.
 */

LIBRARYINJECTORSHARED_EXPORT HRESULT PASCAL DllCanUnloadNow(void)
{
    // If someone has retrieved pointers to any of our objects, and
    // not yet Release()'ed them, then we return S_FALSE to indicate
    // not to unload this DLL. Also, if someone has us locked, return
    // S_FALSE
    return((comObjectsCount | locksCount) ? S_FALSE : S_OK);
}


/************************** DllMain() **************************
 * Called by the Windows OS when this DLL is loaded or unloaded.
 */

BOOL WINAPI DllMain(HINSTANCE instance, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            hEventSrc = RegisterEventSourceW(NULL, L"Lightpack");
            comObjectsCount = locksCount = 0;

            // We don't need to do any thread initialization
            DisableThreadLibraryCalls(instance);
        }
        case DLL_PROCESS_DETACH:
        {
            DeregisterEventSource(hEventSrc);
        }
    }

    return(1);
}

