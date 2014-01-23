#include "D3D9FrameGrabber.hpp"
#include "IPCContext.hpp"

#include "hooksutils.h"
#include "../common/msvcstub.h"
#include <initguid.h>
#include "d3d9types.h"
#include "d3d9.h"

#include "ProxyFuncJmp.hpp"

D3D9FrameGrabber *D3D9FrameGrabber::m_this = NULL;

HRESULT WINAPI D3D9Present(IDirect3DDevice9 *, CONST RECT*,CONST RECT*,HWND,CONST RGNDATA*);
HRESULT WINAPI D3D9SCPresent(IDirect3DSwapChain9 *, CONST RECT*,CONST RECT*,HWND,CONST RGNDATA*, DWORD);

BufferFormat getCompatibleBufferFormat(D3DFORMAT format);

D3D9FrameGrabber::D3D9FrameGrabber(HANDLE syncRunMutex, Logger *logger): GAPIProxyFrameGrabber(syncRunMutex), LoggableTrait(logger) {
    m_isInited = false;
    m_ipcContext = NULL;
}

bool D3D9FrameGrabber::init() {
    if (!m_isInited) {
        if (m_ipcContext) {

            m_d3d9PresentProxyFuncJmp = new ProxyFuncJmp(this->calcD3d9PresentPointer(), reinterpret_cast<void*>(D3D9Present), m_logger);
            m_d3d9SCPresentProxyFuncJmp = new ProxyFuncJmp(this->calcD3d9SCPresentPointer(), reinterpret_cast<void*>(D3D9SCPresent), m_logger);
            m_isInited = m_d3d9PresentProxyFuncJmp->init() && m_d3d9SCPresentProxyFuncJmp->init();
        }
    }
    return m_isInited;
}

bool D3D9FrameGrabber::isGAPILoaded() {
    return GetModuleHandleA("d3d9.dll") != NULL;
}

bool D3D9FrameGrabber::installHooks() {
    m_d3d9PresentProxyFuncJmp->installHook();
    m_d3d9SCPresentProxyFuncJmp->installHook();
    return this->isHooksInstalled();
}

bool D3D9FrameGrabber::isHooksInstalled() {
    return m_d3d9PresentProxyFuncJmp->isHookInstalled() && m_d3d9SCPresentProxyFuncJmp->isHookInstalled();
}

bool D3D9FrameGrabber::removeHooks() {
    m_d3d9PresentProxyFuncJmp->removeHook();
    m_d3d9SCPresentProxyFuncJmp->removeHook();

    return !m_d3d9PresentProxyFuncJmp->isHookInstalled() && !m_d3d9SCPresentProxyFuncJmp->isHookInstalled();
}

void D3D9FrameGrabber::free() {
    delete m_d3d9PresentProxyFuncJmp;
    delete m_d3d9SCPresentProxyFuncJmp;
    m_d3d9PresentProxyFuncJmp = NULL;
    m_d3d9SCPresentProxyFuncJmp = NULL;
    m_isInited = false;
}

void ** D3D9FrameGrabber::calcD3d9PresentPointer() {
    void * hD3d9 = reinterpret_cast<void *>(GetModuleHandleA("d3d9.dll"));
    m_logger->reportLogDebug(L"m_ipcContext->m_memDesc.d3d9PresentFuncOffset = 0x%x, hDxgi = 0x%x", m_ipcContext->m_memDesc.d3d9PresentFuncOffset, hD3d9);
    void ** result = static_cast<void ** >(incPtr(hD3d9 , m_ipcContext->m_memDesc.d3d9PresentFuncOffset));
    m_logger->reportLogDebug(L"d3d9.dll = 0x%x, swapchain::present location = 0x%x", hD3d9, result);
    return result;
}

void ** D3D9FrameGrabber::calcD3d9SCPresentPointer() {
    void * hD3d9 = reinterpret_cast<void *>(GetModuleHandleA("d3d9.dll"));
    m_logger->reportLogDebug(L"m_ipcContext->m_memDesc.d3d9SCPresentFuncOffset = 0x%x, hDxgi = 0x%x", m_ipcContext->m_memDesc.d3d9SCPresentFuncOffset, hD3d9);
    void ** result = static_cast<void ** >(incPtr(hD3d9, m_ipcContext->m_memDesc.d3d9SCPresentFuncOffset));
    m_logger->reportLogDebug(L"d3d9.dll = 0x%x, swapchain::present location = 0x%x", hD3d9, result);
    return result;
}

HRESULT WINAPI D3D9Present(IDirect3DDevice9 *pDev, CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion) {
    D3D9FrameGrabber *d3d9FrameGrabber = D3D9FrameGrabber::getInstance();
    Logger *logger = d3d9FrameGrabber->m_logger;
    DWORD errorcode;
    if (WAIT_OBJECT_0 == (errorcode = WaitForSingleObject(d3d9FrameGrabber->m_syncRunMutex, 0))) {
        IPCContext *ipcContext = d3d9FrameGrabber->m_ipcContext;
        logger->reportLogDebug(L"D3D9Present");
        HRESULT hRes;

        RECT newRect = RECT();
        IDirect3DSurface9 *pBackBuffer = NULL;
        IDirect3DSurface9 *pDemultisampledSurf = NULL;
        IDirect3DSurface9 *pOffscreenSurf = NULL;
        IDirect3DSwapChain9 *pSc = NULL;
        D3DPRESENT_PARAMETERS params;

        if(FAILED( hRes = pDev->GetSwapChain(0, &pSc))) {
            logger->reportLogError(L"d3d9present couldn't get swapchain. result 0x%x", hRes);
            goto end;
        }

        hRes = pSc->GetPresentParameters(&params);
        if (FAILED(hRes) || params.Windowed) {
            goto end;
        }

        if(FAILED( hRes = pDev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer))) {
            goto end;
        }

        D3DSURFACE_DESC surfDesc;
        pBackBuffer->GetDesc(&surfDesc);

        hRes = pDev->CreateRenderTarget(
            surfDesc.Width, surfDesc.Height,
            surfDesc.Format, D3DMULTISAMPLE_NONE, 0, false,
            &pDemultisampledSurf, NULL );
        if (FAILED(hRes))
        {
            logger->reportLogError(L"GetFramePrep: FAILED to create demultisampled render target. 0x%x, width=%u, height=%u, format=%x", hRes, surfDesc.Width, surfDesc.Height, surfDesc.Format );
            goto end;
        }

        hRes = pDev->StretchRect(pBackBuffer, NULL, pDemultisampledSurf, NULL, D3DTEXF_LINEAR );
        if (FAILED(hRes))
        {
            logger->reportLogError(L"GetFramePrep: StretchRect FAILED for image surfacee. 0x%x, width=%u, height=%u, format=%x", hRes, surfDesc.Width, surfDesc.Height, surfDesc.Format );
            goto end;
        }

        hRes = pDev->CreateOffscreenPlainSurface(
            surfDesc.Width, surfDesc.Height,
            surfDesc.Format, D3DPOOL_SYSTEMMEM,
            &pOffscreenSurf, NULL );
        if (FAILED(hRes))
        {
            logger->reportLogError(L"GetFramePrep: FAILED to create image surface. 0x%x, width=%u, height=%u, format=%x", hRes, surfDesc.Width, surfDesc.Height, surfDesc.Format );
            goto end;
        }

        hRes = pDev->GetRenderTargetData(pDemultisampledSurf, pOffscreenSurf );
        if (FAILED(hRes))
        {
            logger->reportLogError(L"GetFramePrep: GetRenderTargetData() FAILED for image surfacee. 0x%x, width=%u, height=%u, format=%x", hRes, surfDesc.Width, surfDesc.Height, surfDesc.Format );
            goto end;
        }

        D3DLOCKED_RECT lockedSrcRect;
        newRect.right = surfDesc.Width;
        newRect.bottom = surfDesc.Height;
        hRes = pOffscreenSurf->LockRect( &lockedSrcRect, &newRect, 0);
        if (FAILED(hRes))
        {
            logger->reportLogError(L"GetFramePrep: FAILED to lock source rect. (0x%x)", hRes );
            goto end;
        }

        ipcContext->m_memDesc.width = surfDesc.Width;
        ipcContext->m_memDesc.height = surfDesc.Height;
        ipcContext->m_memDesc.rowPitch = lockedSrcRect.Pitch;
        ipcContext->m_memDesc.frameId++;
        ipcContext->m_memDesc.format = getCompatibleBufferFormat(surfDesc.Format);

        DWORD errorcode;
        if (WAIT_OBJECT_0 == (errorcode = WaitForSingleObject(ipcContext->m_hMutex, 0))) {
    //        reportLog(EVENTLOG_INFORMATION_TYPE, L"d3d9 writing description to mem mapped file");
            memcpy(ipcContext->m_pMemMap, &(ipcContext->m_memDesc), sizeof (ipcContext->m_memDesc));
    //        reportLog(EVENTLOG_INFORMATION_TYPE, L"d3d9 writing data to mem mapped file");
            PVOID pMemDataMap = incPtr(ipcContext->m_pMemMap, sizeof (ipcContext->m_memDesc));
            if (static_cast<UINT>(lockedSrcRect.Pitch) == surfDesc.Width * 4) {
                memcpy(pMemDataMap, lockedSrcRect.pBits, surfDesc.Width * surfDesc.Height * 4);
            } else {
                UINT i = 0, cleanOffset = 0, pitchOffset = 0;
                while (i < surfDesc.Height) {
                    memcpy(incPtr(pMemDataMap, cleanOffset), incPtr(lockedSrcRect.pBits, pitchOffset), surfDesc.Width * 4);
                    cleanOffset += surfDesc.Width * 4;
                    pitchOffset += lockedSrcRect.Pitch;
                    i++;
                }
            }
            ReleaseMutex(ipcContext->m_hMutex);
            SetEvent(ipcContext->m_hFrameGrabbedEvent);
        } else {
            logger->reportLogError(L"d3d9 couldn't wait mutex. errocode = 0x%x", errorcode);
        }
        end:
        if(pOffscreenSurf) pOffscreenSurf->Release();
        if(pDemultisampledSurf) pDemultisampledSurf->Release();
        if(pBackBuffer) pBackBuffer->Release();
        if(pSc) pSc->Release();

        ProxyFuncJmp *d3d9PresentProxyFuncJmp = d3d9FrameGrabber->m_d3d9PresentProxyFuncJmp;
        if(!d3d9PresentProxyFuncJmp->removeHook()) {
            int i = GetLastError();
            logger->reportLogError(L"d3d9 error occured while trying to removeHook before original call0x%x", i);
        }
        HRESULT result = pDev->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);

        if(!d3d9PresentProxyFuncJmp->installHook()) {
            int i = GetLastError();
            logger->reportLogError(L"d3d9 error occured while trying to installHook after original call0x%x", i);
        }
        ReleaseMutex(d3d9FrameGrabber->m_syncRunMutex);

        return result;
    } else {
        logger->reportLogError(L"d3d9sc present is skipped because mutex is busy");
        return S_FALSE;
    }
}

HRESULT WINAPI D3D9SCPresent(IDirect3DSwapChain9 *pSc, CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion, DWORD dwFlags) {

    D3D9FrameGrabber *d3d9FrameGrabber = D3D9FrameGrabber::getInstance();
    Logger *logger = d3d9FrameGrabber->m_logger;
    DWORD errorcode;
    if (WAIT_OBJECT_0 == (errorcode = WaitForSingleObject(d3d9FrameGrabber->m_syncRunMutex, 0))) {
        IPCContext *ipcContext = d3d9FrameGrabber->m_ipcContext;
        logger->reportLogDebug(L"D3D9SCPresent");
        IDirect3DSurface9 *pBackBuffer = NULL;
        D3DPRESENT_PARAMETERS params;
        RECT newRect = RECT();
        IDirect3DSurface9 *pDemultisampledSurf = NULL;
        IDirect3DSurface9 *pOffscreenSurf = NULL;
        IDirect3DDevice9 *pDev = NULL;

        HRESULT hRes = pSc->GetPresentParameters(&params);

        if (FAILED(hRes) || params.Windowed) {
            goto end;
        }

        if (FAILED(hRes = pSc->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer))) {
            logger->reportLogError(L"d3d9sc couldn't get backbuffer. errorcode = 0x%x", hRes);
            goto end;
        }
        D3DSURFACE_DESC surfDesc;
        pBackBuffer->GetDesc(&surfDesc);

        hRes = pSc->GetDevice(&pDev);
        if (FAILED(hRes))
        {
            logger->reportLogError(L"GetFramePrep: FAILED to get pDev. 0x%x, width=%u, height=%u, format=%x", hRes, surfDesc.Width, surfDesc.Height, surfDesc.Format );
            goto end;
        }
        hRes = pDev->CreateRenderTarget(
            surfDesc.Width, surfDesc.Height,
            surfDesc.Format, D3DMULTISAMPLE_NONE, 0, false,
            &pDemultisampledSurf, NULL );
        if (FAILED(hRes))
        {
            logger->reportLogError(L"GetFramePrep: FAILED to create demultisampled render target. 0x%x, width=%u, height=%u, format=%x", hRes, surfDesc.Width, surfDesc.Height, surfDesc.Format );
            goto end;
        }

        hRes = pDev->StretchRect(pBackBuffer, NULL, pDemultisampledSurf, NULL, D3DTEXF_LINEAR );
        if (FAILED(hRes))
        {
            logger->reportLogError(L"GetFramePrep: StretchRect FAILED for image surfacee. 0x%x, width=%u, height=%u, format=%x", hRes, surfDesc.Width, surfDesc.Height, surfDesc.Format );
            goto end;
        }

        hRes = pDev->CreateOffscreenPlainSurface(
            surfDesc.Width, surfDesc.Height,
            surfDesc.Format, D3DPOOL_SYSTEMMEM,
            &pOffscreenSurf, NULL );
        if (FAILED(hRes))
        {
            logger->reportLogError(L"GetFramePrep: FAILED to create image surface. 0x%x, width=%u, height=%u, format=%x", hRes, surfDesc.Width, surfDesc.Height, surfDesc.Format );
            goto end;
        }

        hRes = pDev->GetRenderTargetData(pDemultisampledSurf, pOffscreenSurf );
        if (FAILED(hRes))
        {
            logger->reportLogError(L"GetFramePrep: GetRenderTargetData() FAILED for image surfacee. 0x%x, width=%u, height=%u, format=%x", hRes, surfDesc.Width, surfDesc.Height, surfDesc.Format );
            goto end;
        }

        D3DLOCKED_RECT lockedSrcRect;
        newRect.right = surfDesc.Width;
        newRect.bottom = surfDesc.Height;

        hRes = pOffscreenSurf->LockRect( &lockedSrcRect, &newRect, 0);
        if (FAILED(hRes))
        {
            logger->reportLogError(L"GetFramePrep: FAILED to lock source rect. (0x%x)", hRes );
            goto end;
        }

        ipcContext->m_memDesc.width = surfDesc.Width;
        ipcContext->m_memDesc.height = surfDesc.Height;
        ipcContext->m_memDesc.rowPitch = lockedSrcRect.Pitch;
        ipcContext->m_memDesc.frameId++;
        ipcContext->m_memDesc.format = getCompatibleBufferFormat(surfDesc.Format);

        if (WAIT_OBJECT_0 == (errorcode = WaitForSingleObject(ipcContext->m_hMutex, 0))) {
            //            __asm__("int $3");
    //        reportLog(EVENTLOG_INFORMATION_TYPE, L"d3d9sc writing description to mem mapped file");
            memcpy(ipcContext->m_pMemMap, &ipcContext->m_memDesc, sizeof (ipcContext->m_memDesc));
    //        reportLog(EVENTLOG_INFORMATION_TYPE, L"d3d9sc writing data to mem mapped file");
            PVOID pMemDataMap = incPtr(ipcContext->m_pMemMap, sizeof (ipcContext->m_memDesc));
            if (static_cast<UINT>(lockedSrcRect.Pitch) == surfDesc.Width * 4) {
                memcpy(pMemDataMap, lockedSrcRect.pBits, surfDesc.Width * surfDesc.Height * 4);
            } else {
                UINT i = 0, cleanOffset = 0, pitchOffset = 0;
                while (i < surfDesc.Height) {
                    memcpy(incPtr(pMemDataMap, cleanOffset), incPtr(lockedSrcRect.pBits, pitchOffset), surfDesc.Width * 4);
                    cleanOffset += surfDesc.Width * 4;
                    pitchOffset += lockedSrcRect.Pitch;
                    i++;
                }
            }
            ReleaseMutex(ipcContext->m_hMutex);
            SetEvent(ipcContext->m_hFrameGrabbedEvent);
        } else {
            logger->reportLogError(L"d3d9sc couldn't wait mutex. errocode = 0x%x", errorcode);
        }
end:
        if(pOffscreenSurf) pOffscreenSurf->Release();
        if(pDemultisampledSurf) pDemultisampledSurf->Release();
        if(pBackBuffer) pBackBuffer->Release();
        if(pDev) pDev->Release();

        ProxyFuncJmp *d3d9SCPresentProxyFuncJmp = d3d9FrameGrabber->m_d3d9SCPresentProxyFuncJmp;

        if(d3d9SCPresentProxyFuncJmp->removeHook()) {
            int i = GetLastError();
            logger->reportLogError(L"d3d9sc error occured while trying to removeHook before original call0x%x", i);
        }
        HRESULT result = pSc->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);

        if(d3d9SCPresentProxyFuncJmp->installHook()) {
            int i = GetLastError();
            logger->reportLogError(L"d3d9sc error occured while trying to installHook after original call0x%x", i);
        }

        ReleaseMutex(d3d9FrameGrabber->m_syncRunMutex);

        return result;
    } else {
        logger->reportLogError(L"d3d9sc present is skipped because mutex is busy");
        return S_FALSE;
    }
}

BufferFormat getCompatibleBufferFormat(D3DFORMAT format) {
    switch(format) {
        case D3DFMT_A8B8G8R8:
            return BufferFormatAbgr;
        case D3DFMT_X8B8G8R8:
            return BufferFormatAbgr;
        case D3DFMT_R8G8_B8G8:
            return BufferFormatRgbg;
        case D3DFMT_A8R8G8B8:
            return BufferFormatArgb;
        case D3DFMT_X8R8G8B8:
            return BufferFormatArgb;
        default:
            return BufferFormatArgb;
    }
}
