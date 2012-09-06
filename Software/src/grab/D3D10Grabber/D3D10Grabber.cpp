/*
 * D3D10Grabber.cpp
 *
 *  Created on: 29.05.2012
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

#include "D3D10Grabber.hpp"
#include "D3D10GrabberDefs.hpp"

#ifdef D3D10_GRAB_SUPPORT

#include <cstdlib>
#include <stdio.h>
#include <tlhelp32.h>
#include "syslog/syslog.h"
#define WINAPI_INLINE WINAPI
#define DXGI_PRESENT_FUNC_ORD 8
#define D3D9_PRESENT_FUNC_ORD 17
typedef uint8_t UINT8;

#define __in
#define __out
#define __inout
#define __inout_opt
#define __in_bcount(x)
#define __out_bcount(x)
#define __out_bcount_opt(x)
#define __in_bcount_opt(x)
#define __in_opt
#define __out_opt
#define __out_ecount_part_opt(x,y)
#define __in_ecount(x)
#define __out_ecount(x)
#define __in_ecount_opt(x)
#define __out_ecount_opt(x)
#include<initguid.h>
#include"DXGI.h"
#include"D3D10_1.h"
#include"D3D10.h"
#include "d3d9.h"
//#include"D3DX10tex.h"

#include"debug.h"
#include"math.h"
#include <qapplication.h>
#include "LightpackApplication.hpp"
#include "objidl.h"
#include "sddl.h"
#include "psapi.h"
#include "winsock2.h"
#include "ws2tcpip.h"
#include "shlwapi.h"
#include "initguid.h"
#include "calculations.hpp"
// IID_ILibraryInjector   = {029587AD-87F3-4623-941F-E37BF99A6DB2}
DEFINE_GUID(IID_ILibraryInjector  , 0x029587ad, 0x87f3, 0x4623, 0x94, 0x1f, 0xe3, 0x7b, 0xf9, 0x9a, 0x6d, 0xb2);

// CLSID_ILibraryInjector = {FC9D8F66-7B9A-47b7-8C5B-830BFF0E48C9}
DEFINE_GUID(CLSID_ILibraryInjector, 0xfc9d8f66, 0x7b9a, 0x47b7, 0x8c, 0x5b, 0x83, 0x0b, 0xff, 0x0e, 0x48, 0xc9);

const WCHAR lightpackHooksDllName[] = L"lightpack-hooks.dll";

using namespace std;

PVOID D3D10Grabber::m_memMap = NULL;
HANDLE D3D10Grabber::m_sharedMem = NULL;
HANDLE D3D10Grabber::m_mutex = NULL;
MONITORINFO D3D10Grabber::m_monitorInfo;
QTimer * D3D10Grabber::m_processesScanAndInfectTimer;
bool D3D10Grabber::m_isInited = false;
bool D3D10Grabber::m_isStarted = false;
ILibraryInjector * D3D10Grabber::m_libraryInjector = NULL;
WCHAR D3D10Grabber::m_hooksLibPath[300];

UINT D3D10Grabber::m_lastFrameId;

/*!
  We are not allowed to create DXGIFactory inside of Dll, so determine SwapChain->Present vtbl location here.
  To determine location we need to create SwapChain instance and dereference pointer.
 \param hWnd window handle to bind SwapChain to
 \return UINT
*/
UINT GetDxgiPresentOffset(HWND hWnd);

BOOL SetPrivilege(HANDLE hToken, LPCTSTR szPrivName, BOOL fEnable);

BOOL AcquirePrivileges();

QList<DWORD> * getDxProcessesIDs(QList<DWORD> * processes);

PVOID BuildRestrictedSD(PSECURITY_DESCRIPTOR pSD);

// The following function frees memory allocated in the
// BuildRestrictedSD() function
VOID FreeRestrictedSD(PVOID ptr);

D3D10GrabberWorker::D3D10GrabberWorker(QObject *parent, LPSECURITY_ATTRIBUTES lpsa) : QObject(parent) {
    if (NULL == (m_frameGrabbedEvent = CreateEventW(lpsa, false, false, D3D10GRABBER_FRAMEGRABBED_EVENT_NAME))) {
        qCritical() << Q_FUNC_INFO << "unable to create frameGrabbedEvent";
    }
}
D3D10GrabberWorker::~D3D10GrabberWorker() {
    if (m_frameGrabbedEvent)
        CloseHandle(m_frameGrabbedEvent);
}

void D3D10GrabberWorker::runLoop() {
    while(1) {
        if (WAIT_OBJECT_0 == WaitForSingleObject(m_frameGrabbedEvent, 50)) {
            emit frameGrabbed();
            if (!ResetEvent(m_frameGrabbedEvent)) {
                qCritical() << Q_FUNC_INFO << "couldn't reset frameGrabbedEvent";
            }
        }
    }
}

bool D3D10Grabber::initIPC(LPSECURITY_ATTRIBUTES lpsa) {

    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (result != 0) {
        qCritical() << Q_FUNC_INFO << "can't initialize winsocks2. error code " << result;
        return false;
    }

    sockaddr_in localhost;
    in_addr adr;

    m_sharedMem = CreateFileMappingW(INVALID_HANDLE_VALUE, lpsa, PAGE_READWRITE, 0, D3D10GRABBER_SHARED_MEM_SIZE, D3D10GRABBER_SHARED_MEM_NAME );
    if(!m_sharedMem) {
        m_sharedMem = OpenFileMappingW(GENERIC_READ | GENERIC_WRITE, false, D3D10GRABBER_SHARED_MEM_NAME);
        if(!m_sharedMem) {
            qCritical() << Q_FUNC_INFO << "couldn't create memory mapped file. error code " << GetLastError();
            return false;
        }
    }

    m_memMap = MapViewOfFile(m_sharedMem, FILE_MAP_READ, 0, 0, D3D10GRABBER_SHARED_MEM_SIZE );
    if(!m_memMap) {
        qCritical() << Q_FUNC_INFO << "couldn't create map view. error code " << GetLastError();
        freeIPC();
        return false;
    }

    PVOID memMap = MapViewOfFile(m_sharedMem, FILE_MAP_WRITE, 0, 0, D3D10GRABBER_SHARED_MEM_SIZE );
    if(!memMap) {
        qWarning() << Q_FUNC_INFO << "couldn't clear shared memory. error code " << GetLastError();
    } else {
        memset(&m_memDesc, 0, sizeof(m_memDesc));
        m_memDesc.frameId = D3D10GRABBER_BLANK_FRAME_ID;

        m_memDesc.dxgiPresentFuncOffset = GetDxgiPresentOffset(getLightpackApp()->getMainWindowHandle());
        memcpy(memMap, &m_memDesc, sizeof(m_memDesc));
        UnmapViewOfFile(memMap);
    }

    m_mutex = CreateMutexW(lpsa, false, D3D10GRABBER_MUTEX_MEM_NAME);
    if(!m_mutex) {
        m_mutex = OpenMutexW(SYNCHRONIZE, false, D3D10GRABBER_MUTEX_MEM_NAME);
        if(!m_mutex) {
            qCritical() << Q_FUNC_INFO << "couldn't create mutex. error code " << GetLastError();
            freeIPC();
            return false;
        }
    }

    return true;
}

D3D10Grabber::D3D10Grabber(QObject *parent, QList<QRgb> *grabResult, QList<GrabWidget *> *grabWidgets) : GrabberBase(parent, grabResult, grabWidgets) {
    connect(getLightpackApp(), SIGNAL(postInitialization()), SLOT(init()));
}

void D3D10Grabber::init(void) {
    HRESULT hr;
    if(!m_isInited) {
        AcquirePrivileges();
        GetCurrentDirectoryW(sizeof(m_hooksLibPath)/2, m_hooksLibPath);
        wcscat(m_hooksLibPath, L"\\");
        wcscat(m_hooksLibPath, lightpackHooksDllName);

        hr = CoInitialize(0);
        //        hr = InitComSecurity();
        //        hr = CoCreateInstanceAsAdmin(NULL, CLSID_ILibraryInjector, IID_ILibraryInjector, reinterpret_cast<void **>(&m_libraryInjector));
        hr = CoCreateInstance(CLSID_ILibraryInjector, NULL, CLSCTX_INPROC_SERVER, IID_ILibraryInjector, reinterpret_cast<void **>(&m_libraryInjector));

        SECURITY_ATTRIBUTES sa;
        SECURITY_DESCRIPTOR sd;

        PVOID  ptr;

        // build a restricted security descriptor
        ptr = BuildRestrictedSD(&sd);
        if (!ptr) {
    //        syslog(LOG_ERR, "BuildRestrictedSD() failed");
            return;
        }

        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = &sd;
        sa.bInheritHandle = FALSE;

        if (!initIPC(NULL)) {
            freeIPC();
            return;
        }
        m_processesScanAndInfectTimer = new QTimer(this);
        m_processesScanAndInfectTimer->setInterval(5000);
        m_processesScanAndInfectTimer->setSingleShot(false);
        connect(m_processesScanAndInfectTimer, SIGNAL(timeout()), this, SLOT(infectCleanDxProcesses()) );
        m_processesScanAndInfectTimer->start();

        m_workerThread = new QThread();
        m_worker = new D3D10GrabberWorker(NULL, NULL);
        m_worker->moveToThread(m_workerThread);
        m_workerThread->start();
        connect(m_worker, SIGNAL(frameGrabbed()), this, SLOT(grab()));
        QMetaObject::invokeMethod(m_worker, "runLoop", Qt::QueuedConnection);

        FreeRestrictedSD(ptr);

        m_isInited = true;


    }
}

void D3D10Grabber::startGrabbing() {
    m_isStarted = true;
//    grab();
}

void D3D10Grabber::stopGrabbing() {
    m_isStarted = false;
}

void D3D10Grabber::setGrabInterval(int msec) {
    m_fallbackGrabber->setGrabInterval(msec);
}

void D3D10Grabber::grab() {
    if (m_isStarted) {
        m_lastGrabResult = _grab();
        emit frameGrabAttempted(m_lastGrabResult);
    }
}

GrabResult D3D10Grabber::_grab() {
    if (!m_isInited) {
        return GrabResultFrameNotReady;
    }
    GrabResult result;
    memcpy(&m_memDesc, m_memMap, sizeof(m_memDesc));
    if( m_memDesc.frameId != D3D10GRABBER_BLANK_FRAME_ID) {
        if( m_memDesc.frameId != m_lastFrameId) {
            m_lastFrameId = m_memDesc.frameId;
            m_grabResult->clear();
            foreach(QWidget *widget, *m_grabWidgets) {
                QRect widgetRect = widget->frameGeometry();
                m_grabResult->append(getColor(widgetRect));
            }
            result = GrabResultOk;
        } else {
            //            result = GrabResultFrameNotReady;
            result = m_fallbackGrabber->_grab();
        }
    } else {
        result = m_fallbackGrabber->_grab();
    }

    return result;
}

D3D10Grabber::~D3D10Grabber() {
    if(m_isInited) {
        m_libraryInjector->Release();
        CoUninitialize();
        freeIPC();
        m_isInited = false;
        delete m_worker;
        delete m_workerThread;
    }
}

void D3D10Grabber::freeIPC() {
    if(m_memMap) {
        UnmapViewOfFile(m_memMap);
    }

    if(m_sharedMem) {
        CloseHandle(m_sharedMem);
    }

    if(m_mutex) {
        CloseHandle(m_mutex);
    }
}

void D3D10Grabber::updateGrabMonitor(QWidget *widget) {
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    if (m_isInited) {
        HMONITOR hMonitor = MonitorFromWindow( widget->winId(), MONITOR_DEFAULTTONEAREST );

        ZeroMemory( &m_monitorInfo, sizeof(MONITORINFO) );
        m_monitorInfo.cbSize = sizeof(MONITORINFO);

        // Get position and resolution of the monitor
        GetMonitorInfo( hMonitor, &m_monitorInfo );

        m_fallbackGrabber->updateGrabMonitor(widget);
    }
}

void D3D10Grabber::setFallbackGrabber(GrabberBase *grabber) {
    m_fallbackGrabber = grabber;
}

void D3D10Grabber::infectCleanDxProcesses() {
    if (m_isInited) {
        QList<DWORD> processes = QList<DWORD>();
        getDxProcessesIDs(&processes);
        foreach (DWORD procId, processes) {
            m_libraryInjector->Inject(procId, m_hooksLibPath);
        }
    }
}

UINT GetDxgiPresentOffset(HWND hwnd) {
    Q_ASSERT(hwnd != NULL);
    IDXGIFactory1 * factory = NULL;
    IDXGIAdapter * adapter;
    HRESULT hresult = CreateDXGIFactory1(IID_IDXGIFactory, reinterpret_cast<void **>(&factory));
    if (hresult != S_OK) {
        qCritical() << "Can't create DXGIFactory. " << hresult;
        return NULL;
    }
    hresult = factory->EnumAdapters(0, &adapter);
    if (hresult != S_OK) {
        qCritical() << "Can't enumerate adapters. " << hresult;
        return NULL;
    }

    DXGI_MODE_DESC dxgiModeDesc;
    dxgiModeDesc.Width                      = 1;
    dxgiModeDesc.Height                     = 1;
    dxgiModeDesc.RefreshRate.Numerator      = 1;
    dxgiModeDesc.RefreshRate.Denominator    = 1;
    dxgiModeDesc.Format                     = DXGI_FORMAT_R8G8B8A8_UNORM;
    dxgiModeDesc.ScanlineOrdering           = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
    dxgiModeDesc.Scaling                    = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SAMPLE_DESC dxgiSampleDesc = {1, 0};

    DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
    memset(&dxgiSwapChainDesc, 0, sizeof(DXGI_SWAP_CHAIN_DESC));
    dxgiSwapChainDesc.BufferDesc    = dxgiModeDesc;
    dxgiSwapChainDesc.SampleDesc    = dxgiSampleDesc;
    dxgiSwapChainDesc.BufferUsage   = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    dxgiSwapChainDesc.BufferCount   = 2;
    dxgiSwapChainDesc.OutputWindow  = hwnd;
    dxgiSwapChainDesc.Windowed      = true;
    dxgiSwapChainDesc.SwapEffect    = DXGI_SWAP_EFFECT_DISCARD;
    dxgiSwapChainDesc.Flags         = 0;

    IDXGISwapChain * pSc;
    ID3D10Device1  * pDev;
    hresult = D3D10CreateDeviceAndSwapChain1(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_FEATURE_LEVEL_9_1, D3D10_1_SDK_VERSION, &dxgiSwapChainDesc, &pSc, &pDev);
    if (S_OK != hresult) {
        qCritical() << Q_FUNC_INFO << QString("Can't create D3D10Device and SwapChain. hresult = 0x%1").arg(QString::number(hresult, 16));
        return NULL;
    }

    intptr_t *pvtbl = reinterpret_cast<intptr_t *>(pSc);

    intptr_t presentFuncPtr = *pvtbl + sizeof(void *) * DXGI_PRESENT_FUNC_ORD;

    char buf[100];
    sprintf(buf, "presentFuncPtr=%x", presentFuncPtr);
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << buf;

    intptr_t hDxgi = reinterpret_cast<intptr_t>(GetModuleHandle(L"dxgi.dll"));
    int presentFuncOffset = static_cast<UINT>(presentFuncPtr - hDxgi);

    sprintf(buf, "presentFuncOffset=%x", presentFuncOffset);
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << buf;


    pSc->Release();
    pDev->Release();
    adapter->Release();
    factory->Release();

    return presentFuncOffset;
}

QRgb D3D10Grabber::getColor(QRect &widgetRect)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO << Debug::toString(widgetRect);

    unsigned char * pbPixelsBuff = reinterpret_cast<unsigned char *>(m_memMap) + sizeof(D3D10GRABBER_SHARED_MEM_DESC);

    if (pbPixelsBuff == NULL)
    {
        qCritical() << Q_FUNC_INFO << "pbPixelsBuff == NULL";
        return 0;
    }

    RECT rcMonitor = m_monitorInfo.rcMonitor;
    QRect monitorRect = QRect( QPoint(rcMonitor.left, rcMonitor.top), QSize(m_memDesc.width, m_memDesc.height));

    QRect clippedRect = monitorRect.intersected(widgetRect);

    // Checking for the 'grabme' widget position inside the monitor that is used to capture color
    if( !clippedRect.isValid() ){

        DEBUG_MID_LEVEL << "Widget 'grabme' is out of screen:" << Debug::toString(clippedRect);

        // Widget 'grabme' is out of screen
        return 0x000000;
    }

    // Convert coordinates from "Main" desktop coord-system to capture-monitor coord-system
    QRect preparedRect = clippedRect.translated(-monitorRect.x(), -monitorRect.y());

    // Align width by 4 for accelerated calculations
    preparedRect.setWidth(preparedRect.width() - (preparedRect.width() % 4));

    if( !preparedRect.isValid() ){
        qWarning() << Q_FUNC_INFO << " preparedRect is not valid:" << Debug::toString(preparedRect);

        // width and height can't be negative
        return 0x000000;
    }

    QRgb result;

    if (Calculations::calculateAvgColor(&result, pbPixelsBuff, BufferFormatAbgr, m_memDesc.rowPitch, preparedRect) == 0) {
        return result;
    } else {
        return qRgb(0,0,0);
    }

#if 0
    if (screenWidth < 1920 && (r > 120 || g > 120 || b > 120)) {
        int monitorWidth = screenWidth;
        int monitorHeight = screenHeight;
        const int BytesPerPixel = 4;
        // Save image of screen:
        QImage * im = new QImage( monitorWidth, monitorHeight, QImage::Format_RGB32 );
        for(int i=0; i<monitorWidth; i++){
            for(int j=0; j<monitorHeight; j++){
                index = (BytesPerPixel * j * monitorWidth) + (BytesPerPixel * i);
                QRgb rgb = pbPixelsBuff[index+2] << 16 | pbPixelsBuff[index+1] << 8 | pbPixelsBuff[index];
                im->setPixel(i, j, rgb);
            }
        }
        im->save("screen.jpg");
        delete im;
    }
#endif
}

BOOL SetPrivilege(HANDLE hToken, LPCTSTR szPrivName, BOOL fEnable) {

    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    LookupPrivilegeValue(NULL, szPrivName, &tp.Privileges[0].Luid);
    tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
    AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof (tp), NULL, NULL);
    return ((GetLastError() == ERROR_SUCCESS));

}

BOOL AcquirePrivileges() {

    HANDLE hCurrentProc = GetCurrentProcess();
    HANDLE hCurrentProcToken;

    if (!OpenProcessToken(hCurrentProc, TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hCurrentProcToken)) {
//        syslog(LOG_ERR, "OpenProcessToken Error %u", GetLastError());
    } else {
        if (!SetPrivilege(hCurrentProcToken, SE_DEBUG_NAME, TRUE)) {
//            syslog(LOG_ERR, "SetPrivleges SE_DEBUG_NAME Error %u", GetLastError());
        } else {
            return TRUE;
        }
    }
    return FALSE;
}

QList<DWORD> * getDxProcessesIDs(QList<DWORD> * processes) {

    DWORD aProcesses[1024];
    HMODULE hMods[1024];
    DWORD cbNeeded;
    DWORD cProcesses;
    char debug_buf[255];
    unsigned int i;

    //     Get the list of process identifiers.
    processes->clear();

    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
        return NULL;

    // Calculate how many process identifiers were returned.

    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the names of the modules for each process.

    for ( i = 0; i < cProcesses; i++ )
    {
        if (aProcesses[i] != GetCurrentProcessId()) {
            HANDLE hProcess;
            hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                    PROCESS_VM_READ,
                                    FALSE, aProcesses[i] );
            if (NULL == hProcess)
                goto nextProcess;

            // Get a list of all the modules in this process.

            if( EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
            {
                bool isDxgiPresent = false;
                for ( int j = 0; j < (cbNeeded / sizeof(HMODULE)); j++ )
                {
                    TCHAR szModName[MAX_PATH];

                    // Get the full path to the module's file.

                    if ( GetModuleFileNameEx( hProcess, hMods[j], szModName,
                                              sizeof(szModName) / sizeof(TCHAR)))
                    {
                        PathStripPathW(szModName);
                        ::WideCharToMultiByte(CP_ACP, 0, szModName, -1, debug_buf, 255, NULL, NULL);
                        DEBUG_HIGH_LEVEL << Q_FUNC_INFO << debug_buf;

                        if(wcsicmp(szModName, lightpackHooksDllName) == 0) {
                            goto nextProcess;
                        } else {
                            if (wcsicmp(szModName, L"dxgi.dll") == 0)
                                isDxgiPresent = true;
                        }
                    }
                }
                if (isDxgiPresent)
                    processes->append(aProcesses[i]);

            }
nextProcess:
            // Release the handle to the process.
            CloseHandle( hProcess );
        }
    }

    return processes;
}
PVOID BuildRestrictedSD(PSECURITY_DESCRIPTOR pSD) {

    DWORD  dwAclLength;

    PSID   pAuthenticatedUsersSID = NULL;

    PACL   pDACL   = NULL;
    BOOL   bResult = FALSE;

    PACCESS_ALLOWED_ACE pACE = NULL;

    SID_IDENTIFIER_AUTHORITY siaNT = SECURITY_NT_AUTHORITY;

    SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;

    // initialize the security descriptor
    if (!InitializeSecurityDescriptor(pSD,
                                      SECURITY_DESCRIPTOR_REVISION)) {
//        syslog(LOG_ERR, "InitializeSecurityDescriptor() failed with error %d\n",
//               GetLastError());
        goto end;
    }

    // obtain a sid for the Authenticated Users Group
    if (!AllocateAndInitializeSid(&siaNT, 1,
                                  SECURITY_AUTHENTICATED_USER_RID, 0, 0, 0, 0, 0, 0, 0,
                                  &pAuthenticatedUsersSID)) {
//        syslog(LOG_ERR, "AllocateAndInitializeSid() failed with error %d\n",
//               GetLastError());
        goto end;
    }

    // NOTE:
    //
    // The Authenticated Users group includes all user accounts that
    // have been successfully authenticated by the system. If access
    // must be restricted to a specific user or group other than
    // Authenticated Users, the SID can be constructed using the
    // LookupAccountSid() API based on a user or group name.

    // calculate the DACL length
    dwAclLength = sizeof(ACL)
            // add space for Authenticated Users group ACE
            + sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD)
            + GetLengthSid(pAuthenticatedUsersSID);

    // allocate memory for the DACL
    pDACL = (PACL) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                             dwAclLength);
    if (!pDACL) {
//        syslog(LOG_ERR, "HeapAlloc() failed with error %d\n", GetLastError());
        goto end;
    }

    // initialize the DACL
    if (!InitializeAcl(pDACL, dwAclLength, ACL_REVISION)) {
//        syslog(LOG_ERR, "InitializeAcl() failed with error %d\n",
//               GetLastError());
        goto end;
    }

    // add the Authenticated Users group ACE to the DACL with
    // GENERIC_READ, GENERIC_WRITE, and GENERIC_EXECUTE access

    if (!AddAccessAllowedAce(pDACL, ACL_REVISION,
                             MAXIMUM_ALLOWED ,
                             pAuthenticatedUsersSID)) {
//        syslog(LOG_ERR, "AddAccessAllowedAce() failed with error %d\n",
//               GetLastError());
        goto end;
    }

    // set the DACL in the security descriptor
    if (!SetSecurityDescriptorDacl(pSD, TRUE, pDACL, FALSE)) {
//        syslog(LOG_ERR, "SetSecurityDescriptorDacl() failed with error %d\n",
//               GetLastError());
        goto end;
    }

    bResult = TRUE;

end:
    if (pAuthenticatedUsersSID) FreeSid(pAuthenticatedUsersSID);

    if (bResult == FALSE) {
        if (pDACL) HeapFree(GetProcessHeap(), 0, pDACL);
        pDACL = NULL;
    }

    return (PVOID) pDACL;
}

VOID FreeRestrictedSD(PVOID ptr) {

    if (ptr) HeapFree(GetProcessHeap(), 0, ptr);

    return;
}

#endif
