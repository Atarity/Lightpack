#include"hooks.h"
#include"stdio.h"
#include"shlwapi.h"
//#include"D3D10_1.h"
#include"stdarg.h"
//#include <cstdlib>
#include <windows.h>
//#include <tlhelp32.h>


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
#define WINAPI_INLINE WINAPI
#define UINT8 unsigned char
#include<initguid.h>
#include "D3D11.h"
#include "D3DX11tex.h"
#include "D3D10.h"
#include "D3DX10tex.h"
#include "DXGI.h"
#include "../common/D3D10GrabberDefs.hpp"

//#define SHARED __attribute__((section ("shared"), shared))

HINSTANCE hDLL;
char executableName[255];

HANDLE hSharedMem;
HANDLE hMutex;
HANDLE hFrameGrabbedEvent;

D3D10GRABBER_SHARED_MEM_DESC memDesc;

PVOID pMemMap;

//#define SWAPCHAIN_PRESENT_OFFSET 0x000019a0
//#define SWAPCHAIN_PRESENT_OFFSET 0x00001a38

typedef HRESULT WINAPI(*PresentType)(IDXGISwapChain *, UINT, UINT);
PresentType savedPresentFunc;

//#define INITGUID
#define PRESENT_FUNC_ORD 8

void ** presentFunc;

#define REPORT_LOG_BUF_SIZE 2048
static HANDLE hEventSrc = NULL;

void reportLog(WORD logType, const LPWSTR message, ...) {
    va_list ap;

    WCHAR *reportLogBuf = (WCHAR *)malloc(REPORT_LOG_BUF_SIZE);
    memset(reportLogBuf, 0, REPORT_LOG_BUF_SIZE);

    va_start( ap, message );
    int sprintfResult = wvsprintfW(reportLogBuf, message, ap);
    va_end( ap );
    if (sprintfResult > -1)
        ReportEventW(hEventSrc, logType, 0, 0x100, NULL, 1, 0, const_cast<const WCHAR **>(&reportLogBuf), NULL);

    free(reportLogBuf);
}


void ** GetDxgiPresentPointer() {
    void * hDxgi = reinterpret_cast<void *>(GetModuleHandleA("dxgi.dll"));
    reportLog(EVENTLOG_INFORMATION_TYPE, L"memDesc.dxgiPresentFuncOffset = 0x%x, hDxgi = 0x%x", memDesc.dxgiPresentFuncOffset, hDxgi);
    void ** result = static_cast<void ** >(hDxgi + memDesc.dxgiPresentFuncOffset);
    reportLog(EVENTLOG_INFORMATION_TYPE, L"swapchain::present location = 0x%x", result);
    return result;
//    return reinterpret_cast<void **> (SWAPCHAIN_PRESENT_OFFSET + reinterpret_cast<UINT> (GetModuleHandle("dxgi.dll")));
}

void D3D10Grab(ID3D10Texture2D* pBackBuffer) {

    D3D10_TEXTURE2D_DESC tex_desc;
    pBackBuffer->GetDesc(&tex_desc);

    ID3D10Device *pDev;
    pBackBuffer->GetDevice(&pDev);

    ID3D10Texture2D * pTexture;
    D3D10_MAPPED_TEXTURE2D mappedTexture;
    tex_desc.CPUAccessFlags = D3D10_CPU_ACCESS_READ;
    tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    tex_desc.ArraySize = 1;
    tex_desc.MipLevels = 1;
    tex_desc.BindFlags = 0;
    tex_desc.SampleDesc = (DXGI_SAMPLE_DESC){1, 0};
    tex_desc.Usage = D3D10_USAGE_STAGING;
    tex_desc.MiscFlags = 0;

    HRESULT hr = pDev->CreateTexture2D(&tex_desc, NULL, &pTexture);
#ifdef DEBUG
    reportLog(EVENTLOG_INFORMATION_TYPE, L"pDev->CreateTexture2D 0x%x", hr);
#endif
    pDev->CopyResource(pTexture, pBackBuffer);
    D3D10_BOX box = {0, 0, tex_desc.Width, tex_desc.Height, 0, 1};
    pDev->CopySubresourceRegion(pTexture, 0, 0, 0, 0, pBackBuffer, 0, &box);

    //        __asm__("int $3");
    if (S_OK != (hr = pTexture->Map(D3D10CalcSubresource(0, 0, 1), D3D10_MAP_READ, 0, &mappedTexture))) {
        reportLog(EVENTLOG_ERROR_TYPE, L"couldn't map texture, hresult = 0x%x", hr);
        goto end;
    }

    memDesc.width = tex_desc.Width;
    memDesc.height = tex_desc.Height;
    memDesc.rowPitch = mappedTexture.RowPitch;
    memDesc.frameId++;

//    reportLog(EVENTLOG_INFORMATION_TYPE, L"texture description. width: %u, height: %u, pitch: %u", tex_desc.Width, tex_desc.Height, mappedTexture.RowPitch);

    DWORD errorcode;
    if (WAIT_OBJECT_0 == (errorcode = WaitForSingleObject(hMutex, 0))) {
        //            __asm__("int $3");
//        reportLog(EVENTLOG_INFORMATION_TYPE, L"writing description to mem mapped file");
        memcpy(pMemMap, &memDesc, sizeof (memDesc));
//        reportLog(EVENTLOG_INFORMATION_TYPE, L"writing data to mem mapped file");
        PVOID pMemDataMap = pMemMap + sizeof (memDesc);
        if (mappedTexture.RowPitch == tex_desc.Width * 4) {
            memcpy(pMemDataMap, mappedTexture.pData, tex_desc.Width * tex_desc.Height * 4);
        } else {
            int i = 0, cleanOffset = 0, pitchOffset = 0;
            while (i < tex_desc.Height) {
                memcpy(pMemDataMap + cleanOffset, mappedTexture.pData + pitchOffset, tex_desc.Width * 4);
                cleanOffset += tex_desc.Width * 4;
                pitchOffset += mappedTexture.RowPitch;
                i++;
            }
        }
        ReleaseMutex(hMutex);
        SetEvent(hFrameGrabbedEvent);
    } else {
        reportLog(EVENTLOG_ERROR_TYPE, L"couldn't wait mutex. errocode = 0x%x", errorcode);
    }


    pTexture->Unmap(D3D10CalcSubresource(0, 0, 1));

end:
    pTexture->Release();
    pDev->Release();
    pBackBuffer->Release();
}


void D3D11Grab(ID3D11Texture2D* pBackBuffer) {

    D3D11_TEXTURE2D_DESC tex_desc;
    pBackBuffer->GetDesc(&tex_desc);

    ID3D11Device *pDev;
    pBackBuffer->GetDevice(&pDev);
    ID3D11DeviceContext * pDevContext;
    pDev->GetImmediateContext(&pDevContext);

    ID3D11Texture2D * pTexture;
    D3D11_MAPPED_SUBRESOURCE mappedTexture;
    tex_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    tex_desc.ArraySize = 1;
    tex_desc.MipLevels = 1;
    tex_desc.BindFlags = 0;
    tex_desc.SampleDesc = (DXGI_SAMPLE_DESC){1, 0};
    tex_desc.Usage = D3D11_USAGE_STAGING;
    tex_desc.MiscFlags = 0;

    HRESULT hr = pDev->CreateTexture2D(&tex_desc, NULL, &pTexture);
//    reportLog(EVENTLOG_INFORMATION_TYPE, L"d3d11 pDev->CreateTexture2D 0x%x", hr);

    pDevContext->CopyResource(pTexture, pBackBuffer);
    D3D11_BOX box = {0, 0, tex_desc.Width, tex_desc.Height, 0, 1};
    pDevContext->CopySubresourceRegion(pTexture, 0, 0, 0, 0, pBackBuffer, 0, &box);

    //        __asm__("int $3");
    if (S_OK != (hr = pDevContext->Map(pTexture, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_READ, 0, &mappedTexture))) {
        reportLog(EVENTLOG_ERROR_TYPE, L"d3d11 couldn't map texture, hresult = 0x%x", hr);
        goto end;
    }

    memDesc.width = tex_desc.Width;
    memDesc.height = tex_desc.Height;
    memDesc.rowPitch = mappedTexture.RowPitch;
    memDesc.frameId++;

//    reportLog(EVENTLOG_INFORMATION_TYPE, L"d3d11 texture description. width: %u, height: %u, pitch: %u", tex_desc.Width, tex_desc.Height, mappedTexture.RowPitch);

    DWORD errorcode;
    if (WAIT_OBJECT_0 == (errorcode = WaitForSingleObject(hMutex, 0))) {
        //            __asm__("int $3");
//        reportLog(EVENTLOG_INFORMATION_TYPE, L"d3d11 writing description to mem mapped file");
        memcpy(pMemMap, &memDesc, sizeof (memDesc));
//        reportLog(EVENTLOG_INFORMATION_TYPE, L"d3d11 writing data to mem mapped file");
        PVOID pMemDataMap = pMemMap + sizeof (memDesc);
        if (mappedTexture.RowPitch == tex_desc.Width * 4) {
            memcpy(pMemDataMap, mappedTexture.pData, tex_desc.Width * tex_desc.Height * 4);
        } else {
            int i = 0, cleanOffset = 0, pitchOffset = 0;
            while (i < tex_desc.Height) {
                memcpy(pMemDataMap + cleanOffset, mappedTexture.pData + pitchOffset, tex_desc.Width * 4);
                cleanOffset += tex_desc.Width * 4;
                pitchOffset += mappedTexture.RowPitch;
                i++;
            }
        }
        ReleaseMutex(hMutex);
        SetEvent(hFrameGrabbedEvent);
    } else {
        reportLog(EVENTLOG_ERROR_TYPE, L"d3d11 couldn't wait mutex. errocode = 0x%x", errorcode);
    }


    pDevContext->Unmap(pTexture, D3D10CalcSubresource(0, 0, 1));
end:
    pTexture->Release();
    pBackBuffer->Release();
    pDevContext->Release();
    pDev->Release();
}

HRESULT WINAPI MyPresent(IDXGISwapChain * sc, UINT b, UINT c) {
    //    __asm__("int $3");
    DXGI_SWAP_CHAIN_DESC desc;
    sc->GetDesc(&desc);
    reportLog(EVENTLOG_INFORMATION_TYPE, L"MyPresent. Buffers count: %u, Output hwnd: %u, %s", desc.BufferCount, desc.OutputWindow, desc.Windowed ? "windowed" : "fullscreen");

    if (!desc.Windowed) {
        ID3D10Texture2D* pD3D10BackBuffer;
        //        GUID iid_texture2d = {0x9B7E4C04, 0x342C, 0x4106,
        //            {0xA1, 0x9F, 0x4F, 0x27, 0x04, 0xF6, 0x89, 0xF0}};
        HRESULT hr = sc->GetBuffer(0, IID_ID3D10Texture2D, reinterpret_cast<LPVOID*> (&pD3D10BackBuffer));
//        reportLog(EVENTLOG_INFORMATION_TYPE, L"sc->GetBuffer returned %x", hr);

        if (hr == S_OK) {
            D3D10Grab(pD3D10BackBuffer);
        } else {
            ID3D11Texture2D * pD3D11BackBuffer;
            hr = sc->GetBuffer(0, IID_ID3D11Texture2D, reinterpret_cast<LPVOID*> (&pD3D11BackBuffer));
//            reportLog(EVENTLOG_INFORMATION_TYPE, L"d3d11 sc->GetBuffer returned 0x%x", hr);
            if(hr == S_OK) {
                D3D11Grab(pD3D11BackBuffer);
            } else {
                reportLog(EVENTLOG_ERROR_TYPE, L"couldn't get d3d11 buffer. returned 0x%x", hr);
            }
        }
    }

    return savedPresentFunc(sc, b, c);
}

void writeBlankFrame(PVOID dest) {
    D3D10GRABBER_SHARED_MEM_DESC lmemDesc;
    memset(&lmemDesc, 0, sizeof (lmemDesc));
    lmemDesc.frameId = D3D10GRABBER_BLANK_FRAME_ID;
    lmemDesc.dxgiPresentFuncOffset = memDesc.dxgiPresentFuncOffset; // keep presentFuncOffset for other instances of dll
    DWORD errorcode;
    if (WAIT_OBJECT_0 == (errorcode = WaitForSingleObject(hMutex, 10))) {
        memcpy(dest, &lmemDesc, sizeof (lmemDesc));
        ReleaseMutex(hMutex);
    } else {
        reportLog(EVENTLOG_ERROR_TYPE, L"couldn't wait mutex while writing blank frame. errocode = 0x%x", errorcode);
    }
}

bool WriteToProtectedMem(HANDLE process, void ** mem, void * newVal, void ** savedVal) {
    DWORD protection = PAGE_READWRITE;
    DWORD oldProtection;
    if (VirtualProtectEx(process, mem, sizeof (void *), protection, &oldProtection)) {
        *savedVal = (void *) *mem;
        *mem = newVal;

        if (VirtualProtectEx(process, mem, sizeof (void *), oldProtection, &oldProtection))
            return true;
    }
    return false;
}

HOOKSDLL_API BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD fdwReason, LPVOID lpReserved) {
    //    __asm__("int $3");
    if (fdwReason == DLL_PROCESS_ATTACH /*|| fdwReason == DLL_THREAD_ATTACH*/) // When initializing....
    {

        // We don't need thread notifications for what we're doing.  Thus, get
        // rid of them, thereby eliminating some of the overhead of this DLL
        DisableThreadLibraryCalls(hModule);

        HMODULE hProc = GetModuleHandle(NULL);
        GetModuleFileNameA(hProc, executableName, sizeof (executableName));
        PathStripPathA(executableName);

        hEventSrc = RegisterEventSourceW(NULL, L"Lightpack hooks");

        if (NULL == (hMutex = OpenMutexW(SYNCHRONIZE, false, D3D10GRABBER_MUTEX_MEM_NAME))) {
            reportLog(EVENTLOG_ERROR_TYPE, L"error occured while opening mutex 0x%x", GetLastError());
        }

        if (NULL == (hSharedMem = OpenFileMappingW(GENERIC_WRITE | GENERIC_READ, false, D3D10GRABBER_SHARED_MEM_NAME))) {
            reportLog(EVENTLOG_ERROR_TYPE, L"error occured while opening memory-mapped file 0x%x", GetLastError());
        }

        if (NULL == (hFrameGrabbedEvent = CreateEventW(NULL, true, false, D3D10GRABBER_FRAMEGRABBED_EVENT_NAME))) {
            reportLog(EVENTLOG_ERROR_TYPE, L"error occured while opening event 0x%x", GetLastError());
        }


        pMemMap = MapViewOfFile(hSharedMem, FILE_MAP_WRITE, 0, 0, D3D10GRABBER_SHARED_MEM_SIZE);
        if (pMemMap == NULL) {
            reportLog(EVENTLOG_ERROR_TYPE, L"error occured while creating mapview 0x%x", GetLastError());
        }

        PVOID pMemMapReadOnly = MapViewOfFile(hSharedMem, FILE_MAP_READ, 0, 0, D3D10GRABBER_SHARED_MEM_SIZE);
        if (pMemMapReadOnly == NULL) {
            reportLog(EVENTLOG_ERROR_TYPE, L"error occured while creating read-only mapview 0x%x", GetLastError());
        }
        
        memcpy(&memDesc, pMemMapReadOnly, sizeof(memDesc));
        reportLog(EVENTLOG_INFORMATION_TYPE, L"swapchain::present() offset is 0x%x", memDesc.dxgiPresentFuncOffset);
        
        UnmapViewOfFile(pMemMapReadOnly);

        //        __asm__("int $3");
        memDesc.frameId = 0;
        HANDLE process = GetCurrentProcess();
        reportLog(EVENTLOG_INFORMATION_TYPE, L"Current process 0x%x", process);

        void ** presentAdr = GetDxgiPresentPointer();
        if (WriteToProtectedMem(process, presentAdr, reinterpret_cast<void *> (MyPresent), reinterpret_cast<void **> (&savedPresentFunc)));

        if (int i = GetLastError() != 0)
            reportLog(EVENTLOG_ERROR_TYPE, L"error occured while hijacking 0x%x", i);

    } else if (fdwReason == DLL_PROCESS_DETACH) // When initializing....
    {
        writeBlankFrame(pMemMap);
        CloseHandle(hMutex);
        UnmapViewOfFile(pMemMap);
        CloseHandle(hSharedMem);
        DeregisterEventSource(hEventSrc);
    }

    return TRUE;
}
