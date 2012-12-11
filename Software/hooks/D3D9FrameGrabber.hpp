#ifndef D3D9FRAMEGRABBER_HPP
#define D3D9FRAMEGRABBER_HPP

#include "GAPIProxyFrameGrabber.hpp"
#include "LoggableTrait.hpp"

class Logger;
class ProxyFuncJmp;
struct IDirect3DDevice9;
struct IDirect3DSwapChain9;

class D3D9FrameGrabber : public GAPIProxyFrameGrabber, LoggableTrait
{
public:
    static D3D9FrameGrabber *m_this;
    static D3D9FrameGrabber *getInstance(HANDLE syncRunMutex) {
        if (!m_this)
            m_this = new D3D9FrameGrabber(syncRunMutex, Logger::getInstance());
        return m_this;
    }

    static D3D9FrameGrabber *getInstance() {
        return m_this;
    }

    virtual bool init();
    virtual bool isGAPILoaded();
    virtual bool isHooksInstalled();
    virtual bool installHooks();
    virtual bool removeHooks();
    virtual void free();
    friend HRESULT WINAPI D3D9Present(IDirect3DDevice9 *, CONST RECT*,CONST RECT*,HWND,CONST RGNDATA*);
    friend HRESULT WINAPI D3D9SCPresent(IDirect3DSwapChain9*, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*, DWORD);

protected:
    ProxyFuncJmp *m_d3d9PresentProxyFuncJmp;
    ProxyFuncJmp *m_d3d9SCPresentProxyFuncJmp;
    D3D9FrameGrabber(HANDLE syncRunMutex, Logger *logger);
    void ** calcD3d9PresentPointer();
    void ** calcD3d9SCPresentPointer();
};

#endif // D3D9FRAMEGRABBER_HPP
