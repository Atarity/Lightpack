#include "DxgiFrameGrabber.hpp"

#include "hooksutils.h"
#include "../common/msvcstub.h"
#include <initguid.h>
#include "D3D11.h"
#include "DXGI.h"
#include "D3D10.h"
#include "D3DX10tex.h"

#include "ProxyFuncVFTable.hpp"
#include "IPCContext.hpp"

DxgiFrameGrabber *DxgiFrameGrabber::m_this = NULL;

enum DxgiDevice {
    DxgiDeviceD3D10,
    DxgiDeviceD3D11,
    DxgiDeviceUnknown
};

DxgiDevice dxgiDevice = DxgiDeviceUnknown;

typedef HRESULT (WINAPI *DXGISCPresentFunc)(IDXGISwapChain *, UINT, UINT);

HRESULT WINAPI DXGIPresent(IDXGISwapChain * sc, UINT b, UINT c);

DxgiFrameGrabber::DxgiFrameGrabber(Logger *logger): GAPIProxyFrameGrabber(NULL), LoggableTrait(logger) {
    m_isInited = false;
    m_ipcContext = NULL;
}

bool DxgiFrameGrabber::init() {
    if (!m_isInited) {
        if (m_ipcContext) {
            m_dxgiPresentProxyFuncVFTable = new ProxyFuncVFTable(this->calcDxgiPresentPointer(), reinterpret_cast<void*>(DXGIPresent), m_logger);
            m_isInited = m_dxgiPresentProxyFuncVFTable->init();
        }
    }
    return m_isInited;
}

bool DxgiFrameGrabber::isGAPILoaded() {
    return GetModuleHandleA("dxgi.dll") != NULL;
}

bool DxgiFrameGrabber::installHooks() {
    return m_isInited && m_dxgiPresentProxyFuncVFTable->installHook();
}

bool DxgiFrameGrabber::isHooksInstalled() {
    return m_isInited && m_dxgiPresentProxyFuncVFTable->isHookInstalled();
}

bool DxgiFrameGrabber::removeHooks() {
    return m_isInited && m_dxgiPresentProxyFuncVFTable->removeHook();
}

void DxgiFrameGrabber::free() {
    if (m_isInited) {
        m_isInited = false;
        delete m_dxgiPresentProxyFuncVFTable;
        m_dxgiPresentProxyFuncVFTable = NULL;
    }
}

void ** DxgiFrameGrabber::calcDxgiPresentPointer() {
    void * hDxgi = reinterpret_cast<void *>(GetModuleHandleA("dxgi.dll"));
    if (m_logger)
        m_logger->reportLogDebug(L"gIpcContext.m_memDesc.dxgiPresentFuncOffset = 0x%x, hDxgi = 0x%x", m_ipcContext->m_memDesc.dxgiPresentFuncOffset, hDxgi);
    void ** result = static_cast<void ** >(incPtr(hDxgi, m_ipcContext->m_memDesc.dxgiPresentFuncOffset));
    if (m_logger)
        m_logger->reportLogDebug(L"dxgi.dll = 0x%x, swapchain::present location = 0x%x", hDxgi, result);
    return result;
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
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage = D3D10_USAGE_STAGING;
    tex_desc.MiscFlags = 0;

    HRESULT hr = pDev->CreateTexture2D(&tex_desc, NULL, &pTexture);
#ifdef DEBUG
    reportLog(EVENTLOG_INFORMATION_TYPE, L"pDev->CreateTexture2D 0x%x", hr);
#endif
    pDev->CopyResource(pTexture, pBackBuffer);
    D3D10_BOX box = {0, 0, tex_desc.Width, tex_desc.Height, 0, 1};
    pDev->CopySubresourceRegion(pTexture, 0, 0, 0, 0, pBackBuffer, 0, &box);

    DxgiFrameGrabber *dxgiFrameGrabber = DxgiFrameGrabber::getInstance();
    IPCContext *ipcContext = dxgiFrameGrabber->m_ipcContext;
    Logger *logger = dxgiFrameGrabber->m_logger;

    if (S_OK != (hr = pTexture->Map(D3D10CalcSubresource(0, 0, 1), D3D10_MAP_READ, 0, &mappedTexture))) {
        logger->reportLogError(L"d3d10 couldn't map texture, hresult = 0x%x", hr);
        goto end;
    }

    ipcContext->m_memDesc.width = tex_desc.Width;
    ipcContext->m_memDesc.height = tex_desc.Height;
    ipcContext->m_memDesc.rowPitch = mappedTexture.RowPitch;
    ipcContext->m_memDesc.format = BufferFormatAbgr;
    ipcContext->m_memDesc.frameId++;

//    reportLog(EVENTLOG_INFORMATION_TYPE, L"d3d10 texture description. width: %u, height: %u, pitch: %u", tex_desc.Width, tex_desc.Height, mappedTexture.RowPitch);

    DWORD errorcode;
    if (WAIT_OBJECT_0 == (errorcode = WaitForSingleObject(ipcContext->m_hMutex, 0))) {
        //            __asm__("int $3");
//        reportLog(EVENTLOG_INFORMATION_TYPE, L"d3d10 writing description to mem mapped file");
        memcpy(ipcContext->m_pMemMap, &ipcContext->m_memDesc, sizeof (ipcContext->m_memDesc));
//        reportLog(EVENTLOG_INFORMATION_TYPE, L"d3d10 writing data to mem mapped file");
        PVOID pMemDataMap = incPtr(ipcContext->m_pMemMap, sizeof (ipcContext->m_memDesc));
        if (mappedTexture.RowPitch == tex_desc.Width * 4) {
            memcpy(pMemDataMap, mappedTexture.pData, tex_desc.Width * tex_desc.Height * 4);
        } else {
            UINT cleanOffset = 0, pitchOffset = 0, i = 0;
            while (i < tex_desc.Height) {
                memcpy(incPtr(pMemDataMap, cleanOffset), incPtr(mappedTexture.pData, pitchOffset), tex_desc.Width * 4);
                cleanOffset += tex_desc.Width * 4;
                pitchOffset += mappedTexture.RowPitch;
                i++;
            }
        }
        ReleaseMutex(ipcContext->m_hMutex);
        SetEvent(ipcContext->m_hFrameGrabbedEvent);
    } else {
        logger->reportLogError(L"d3d10 couldn't wait mutex. errocode = 0x%x", errorcode);
    }

    pTexture->Unmap(D3D10CalcSubresource(0, 0, 1));

end:
    pTexture->Release();
    pDev->Release();
}

void D3D11Grab(ID3D11Texture2D *pBackBuffer) {
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
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage = D3D11_USAGE_STAGING;
    tex_desc.MiscFlags = 0;

    HRESULT hr = pDev->CreateTexture2D(&tex_desc, NULL, &pTexture);
//    reportLog(EVENTLOG_INFORMATION_TYPE, L"d3d11 pDev->CreateTexture2D 0x%x", hr);

    pDevContext->CopyResource(pTexture, pBackBuffer);
    D3D11_BOX box = {0, 0, tex_desc.Width, tex_desc.Height, 0, 1};
    pDevContext->CopySubresourceRegion(pTexture, 0, 0, 0, 0, pBackBuffer, 0, &box);

    DxgiFrameGrabber *dxgiFrameGrabber = DxgiFrameGrabber::getInstance();
    IPCContext *ipcContext = dxgiFrameGrabber->m_ipcContext;
    Logger *logger = dxgiFrameGrabber->m_logger;

    //        __asm__("int $3");
    if (S_OK != (hr = pDevContext->Map(pTexture, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_READ, 0, &mappedTexture))) {
        logger->reportLogError(L"d3d11 couldn't map texture, hresult = 0x%x", hr);
        goto end;
    }

    ipcContext->m_memDesc.width = tex_desc.Width;
    ipcContext->m_memDesc.height = tex_desc.Height;
    ipcContext->m_memDesc.rowPitch = mappedTexture.RowPitch;
    ipcContext->m_memDesc.frameId++;

//    reportLog(EVENTLOG_INFORMATION_TYPE, L"d3d11 texture description. width: %u, height: %u, pitch: %u", tex_desc.Width, tex_desc.Height, mappedTexture.RowPitch);

    DWORD errorcode;
    if (WAIT_OBJECT_0 == (errorcode = WaitForSingleObject(ipcContext->m_hMutex, 0))) {
        //            __asm__("int $3");
//        reportLog(EVENTLOG_INFORMATION_TYPE, L"d3d11 writing description to mem mapped file");
        memcpy(ipcContext->m_pMemMap, &ipcContext->m_memDesc, sizeof (ipcContext->m_memDesc));
//        reportLog(EVENTLOG_INFORMATION_TYPE, L"d3d11 writing data to mem mapped file");
        PVOID pMemDataMap = incPtr(ipcContext->m_pMemMap, sizeof (ipcContext->m_memDesc));
        if (mappedTexture.RowPitch == tex_desc.Width * 4) {
            memcpy(pMemDataMap, mappedTexture.pData, tex_desc.Width * tex_desc.Height * 4);
        } else {
            UINT i = 0, cleanOffset = 0, pitchOffset = 0;
            while (i < tex_desc.Height) {
                memcpy(incPtr(pMemDataMap, cleanOffset), incPtr(mappedTexture.pData, pitchOffset), tex_desc.Width * 4);
                cleanOffset += tex_desc.Width * 4;
                pitchOffset += mappedTexture.RowPitch;
                i++;
            }
        }
        ReleaseMutex(ipcContext->m_hMutex);
        SetEvent(ipcContext->m_hFrameGrabbedEvent);
    } else {
        logger->reportLogError(L"d3d11 couldn't wait mutex. errocode = 0x%x", errorcode);
    }

    pDevContext->Unmap(pTexture, D3D10CalcSubresource(0, 0, 1));
end:
    pTexture->Release();
    pDevContext->Release();
    pDev->Release();
}

HRESULT WINAPI DXGIPresent(IDXGISwapChain * sc, UINT b, UINT c) {

    DxgiFrameGrabber *dxgiFrameGrabber = DxgiFrameGrabber::getInstance();

    Logger *logger = dxgiFrameGrabber->m_logger;

    DXGI_SWAP_CHAIN_DESC desc;
    sc->GetDesc(&desc);
    logger->reportLogDebug(L"d3d10 Buffers count: %u, Output hwnd: %u, %s", desc.BufferCount, desc.OutputWindow, desc.Windowed ? "windowed" : "fullscreen");

    if (!desc.Windowed) {
        if (dxgiDevice == DxgiDeviceUnknown || dxgiDevice == DxgiDeviceD3D10 ) {

            ID3D10Texture2D *pBackBuffer;
            HRESULT hr = sc->GetBuffer(0, IID_ID3D10Texture2D, reinterpret_cast<LPVOID*> (&pBackBuffer));

            if (hr == S_OK) {
                if (dxgiDevice == DxgiDeviceUnknown) dxgiDevice = DxgiDeviceD3D10;
                D3D10Grab(pBackBuffer);
                pBackBuffer->Release();
            } else {
                if (dxgiDevice != DxgiDeviceUnknown)
                    logger->reportLogError(L"couldn't get d3d10 buffer. returned 0x%x", hr);
            }
        }
        if (dxgiDevice == DxgiDeviceUnknown || dxgiDevice == DxgiDeviceD3D11 ) {
            ID3D11Texture2D *pBackBuffer;
            HRESULT hr = sc->GetBuffer(0, IID_ID3D11Texture2D, reinterpret_cast<LPVOID*> (&pBackBuffer));
            if(hr == S_OK) {
                if (dxgiDevice == DxgiDeviceUnknown) dxgiDevice = DxgiDeviceD3D11;
                D3D11Grab(pBackBuffer);
                pBackBuffer->Release();
            } else {
                logger->reportLogError(L"couldn't get d3d11 buffer. returned 0x%x", hr);
            }
        }
    }

    DXGISCPresentFunc originalFunc = reinterpret_cast<DXGISCPresentFunc>(dxgiFrameGrabber->m_dxgiPresentProxyFuncVFTable->getOriginalFunc());
    return originalFunc(sc,b,c);
}
