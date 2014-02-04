/*
 * LibraryInjector.c
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

#include <stdarg.h>
#include <olectl.h>

/*! A count of how many objects of our DLL has been created
*/
static volatile LONG comObjectsCount = 0;

/*! A count of how many apps have locked our DLL via calling our
 IClassFactory object's LockServer()
*/
static volatile LONG locksCount = 0;

#define REPORT_LOG_BUF_SIZE 2048
static HANDLE hEventSrc = NULL;

typedef struct {
    const ILibraryInjectorVtbl *lpVtbl;
    volatile LONG refCount;
} LibraryInjector;

typedef struct {
    const IClassFactoryVtbl *lpVtbl;
    volatile LONG refCount;
} ClassFactory;

void freeLibraryInjector(LibraryInjector * injector);
void freeClassFactory(ClassFactory * injector);

int libraryInjectorInit(void) {
    hEventSrc = RegisterEventSourceW(NULL, L"Prismatik-libraryinjector");
    comObjectsCount = locksCount = 0;

    return 1;
}

int isLibraryInjectorActive(void) {
    return (comObjectsCount | locksCount) ? 1 : 0;
}

void libraryInjectorShutdown(void) {
    DeregisterEventSource(hEventSrc);
}

void reportLog(WORD logType, LPCWSTR message, ...) {
    va_list ap;
    int sprintfResult;

    WCHAR *reportLogBuf = malloc(REPORT_LOG_BUF_SIZE);
    memset(reportLogBuf, 0, REPORT_LOG_BUF_SIZE);

    va_start( ap, message );
    sprintfResult = wvsprintfW(reportLogBuf, message, ap);
    va_end( ap );
    if (sprintfResult > -1) {
        LPCWSTR reportStrings[] = { reportLogBuf };
        ReportEventW(hEventSrc, logType, 0, 0x100, NULL, 1, 0, reportStrings, NULL);
    }

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

static BOOL AcquirePrivilege(void) {
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

static HRESULT STDMETHODCALLTYPE LibraryInjector_QueryInterface(ILibraryInjector * this, REFIID interfaceGuid, void **ppv) {
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
        freeLibraryInjector(this);
        return(0);
    }
    return (this->refCount);
}

static HRESULT STDMETHODCALLTYPE LibraryInjector_Inject(ILibraryInjector * this, DWORD ProcessId, LPWSTR ModulePath) {
    UNREFERENCED_PARAMETER(this);

    char CodePage[4096] ={
        0x90,                                     // nop (to replace with int 3h - 0xCC)
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
    reportLog(EVENTLOG_INFORMATION_TYPE, L"injecting library...");
    if(AcquirePrivilege()) {
        int sizeofCP;
        LPVOID Memory;
        LPWSTR DLLName;
        DWORD *LoadLibProc, *LibNameArg, *ExitThreadProc;
        HANDLE hThread;
        HMODULE hKernel32 = GetModuleHandle(L"kernel32.dll");

        HANDLE Process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId);

        if (Process == NULL) {
            reportLog(EVENTLOG_ERROR_TYPE, L"couldn't open process");
            return S_FALSE;
        }

        sizeofCP = wcslen(ModulePath)*2 + SIZE_OF_CODE + 1;
        Memory = VirtualAllocEx(Process, 0, sizeofCP, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

        if (!Memory) {
            reportLog(EVENTLOG_ERROR_TYPE, L"couldn't allocate memory");
            return S_FALSE;
        }

        DLLName = (LPWSTR) ((DWORD) CodePage + SIZE_OF_CODE);
        LoadLibProc = (DWORD*) (CodePage + LOAD_LIB_OFFSET);
        LibNameArg = (DWORD*) (CodePage + LIB_NAME_OFFSET);
        ExitThreadProc = (DWORD*) (CodePage + EXIT_THREAD_OFFSET);

        wcscpy(DLLName, ModulePath);
        *LoadLibProc = (DWORD) GetProcAddress(hKernel32, "LoadLibraryW");
        *ExitThreadProc = (DWORD) GetProcAddress(hKernel32, "ExitThread");
        *LibNameArg = (DWORD)Memory + SIZE_OF_CODE; // need to do this: *EBX = *EBX + (Section)
        ////////////////////////////

        if(!WriteProcessMemory(Process, Memory, CodePage, sizeofCP, 0)) {
            reportLog(EVENTLOG_ERROR_TYPE, L"couldn't write loading library code");
            return S_FALSE;
        }

        hThread = CreateRemoteThread(Process, 0, 0, (LPTHREAD_START_ROUTINE) Memory, 0, 0, 0);
        //    hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CodePage, 0, 0, 0);
        if (!hThread) {
            reportLog(EVENTLOG_ERROR_TYPE, L"couldn't create remote thread");
            return S_FALSE;
        } else {
            reportLog(EVENTLOG_INFORMATION_TYPE, L"library injected successfully");
        }
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);

        return S_OK;
    } else {
        reportLog(EVENTLOG_ERROR_TYPE, L"couldn't acquire privileges to inject");
        return S_FALSE;
    }
}

static ULONG STDMETHODCALLTYPE ClassFactory_AddRef(ClassFactory * this) {
    return InterlockedIncrement(&(this->refCount));
}

// IClassFactory's QueryInterface()
static HRESULT STDMETHODCALLTYPE ClassFactory_QueryInterface(IClassFactory * this, REFIID factoryGuid, void **ppv) {
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
        freeClassFactory(this);
        return (0);
    }
    return (this->refCount);
}

static HRESULT STDMETHODCALLTYPE ClassFactory_CreateInstance(IClassFactory * this, IUnknown *punkOuter, REFIID vTableGuid, void **objHandle) {
    HRESULT hr;
    register ILibraryInjector * thisobj;

    UNREFERENCED_PARAMETER(this);

    // Assume an error by clearing caller's handle
    *objHandle = 0;

    // We don't support aggregation in this example
    if (punkOuter)
        hr = CLASS_E_NOAGGREGATION;
    else
    {
        if (!(thisobj = allocLibraryInjector()))
            hr = E_OUTOFMEMORY;
        else
        {
            thisobj->lpVtbl->AddRef(thisobj);
            // Fill in the caller's handle with a pointer to the LibraryInjector we just
            // allocated above. We'll let LibraryInjector's QueryInterface do that, because
            // it also checks the GUID the caller passed, and also increments the
            // reference count (to 2) if all goes well
            hr = thisobj->lpVtbl->QueryInterface(thisobj, vTableGuid, objHandle);

            // Decrement reference count. NOTE: If there was an error in QueryInterface()
            // then Release() will be decrementing the count back to 0 and will free the
            // LibraryInjector for us. One error that may occur is that the caller is asking for
            // some sort of object that we don't support (ie, it's a GUID we don't recognize)
            thisobj->lpVtbl->Release(thisobj);

        }
    }

    return(hr);
}

// IClassFactory's LockServer(). It is called by someone
// who wants to lock this DLL in memory
static HRESULT STDMETHODCALLTYPE ClassFactory_LockServer(ClassFactory * this, BOOL flock) {
    UNREFERENCED_PARAMETER(this);

    if (flock) InterlockedIncrement(&locksCount);
    else InterlockedDecrement(&locksCount);

    return (NOERROR);
}

static const ILibraryInjectorVtbl libraryInjectorVtbl = {
    LibraryInjector_QueryInterface,
    LibraryInjector_AddRef,
    LibraryInjector_Release,
    LibraryInjector_Inject
};

static const IClassFactoryVtbl classFactoryVtbl = {
    ClassFactory_QueryInterface,
    ClassFactory_AddRef,
    ClassFactory_Release,
    ClassFactory_CreateInstance,
    ClassFactory_LockServer
};

ILibraryInjector * allocLibraryInjector(void) {
    LibraryInjector * thisobj = GlobalAlloc(GMEM_FIXED, sizeof(LibraryInjector));
    if (!thisobj)
        return NULL;

    InterlockedIncrement(&comObjectsCount);
    thisobj->refCount = 0;
    thisobj->lpVtbl = &libraryInjectorVtbl;

    return (ILibraryInjector *)thisobj;
}

void freeLibraryInjector(LibraryInjector * injector) {
    if (injector) {
        InterlockedDecrement(&comObjectsCount);
        GlobalFree(injector);
    }
}

IClassFactory * allocClassFactory(void) {
    ClassFactory * thisobj = GlobalAlloc(GMEM_FIXED, sizeof(ClassFactory));
    if (!thisobj)
        return NULL;

    InterlockedIncrement(&comObjectsCount);
    thisobj->refCount = 0;
    thisobj->lpVtbl = &classFactoryVtbl;
    return (IClassFactory *)thisobj;
}

void freeClassFactory(ClassFactory * factory) {
    if (factory) {
        InterlockedDecrement(&comObjectsCount);
        GlobalFree(factory);
    }
}
