/*
 * D3D9Grabber.cpp
 *
 *  Created on: 19.09.2011
 *     Project: Lightpack
 *
 *  Copyright (c) 2011 Timur Sattarov
 *
 *  Lightpack is a USB content-driving ambient lighting system
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


#include "D3D9Grabber.hpp"


#ifdef D3D9_GRAB_SUPPORT

#include "../debug.h"
#include "cmath"
#define BYTES_PER_PIXEL 4

D3D9Grabber::D3D9Grabber() : m_d3D(NULL), m_d3Device(NULL), m_surface(NULL)
{
    if ((m_d3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
    {
        DEBUG_LOW_LEVEL << "Cannot create Direct3D9 interface (Direct3DCreate9)";
        return;
    }

    if (FAILED(m_d3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &m_displayMode)))
    {
        DEBUG_LOW_LEVEL << "Cannot get display mode for Direct3D9 (GetAdapterDisplayMode)";
        return;
    }

    HWND hWnd = GetDesktopWindow(); // todo

    ZeroMemory(&m_presentParams, sizeof(D3DPRESENT_PARAMETERS));
    m_presentParams.Windowed = true;
    m_presentParams.hDeviceWindow = hWnd;
    m_presentParams.BackBufferFormat = m_displayMode.Format;
    m_presentParams.BackBufferHeight = m_displayMode.Height;
    m_presentParams.BackBufferWidth = m_displayMode.Width;
    m_presentParams.MultiSampleType = D3DMULTISAMPLE_NONE;
    m_presentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    //m_presentParams.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER; todo
    //m_presentParams.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

    if (FAILED(m_d3D->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &m_presentParams,
        &m_d3Device)))
    {
        DEBUG_LOW_LEVEL << "Cannot create device for Direct3D9 (CreateDevice)";
        return;
    }

    if (FAILED(m_d3Device->CreateOffscreenPlainSurface(
        m_displayMode.Width,
        m_displayMode.Height,
        D3DFMT_A8R8G8B8,
        D3DPOOL_SYSTEMMEM,
        &m_surface,
        NULL)))
    {
        DEBUG_LOW_LEVEL << "Cannot create offscreen surface for Direct3D9 (CreateOffscreenPlainSurface)";
        return;
    }
}

// todo move body to clear method
D3D9Grabber::~D3D9Grabber()
{
    if (m_surface != NULL)
    {
        m_surface->Release();
        m_surface = NULL;
    }

    if (m_d3Device != NULL)
    {
        m_d3Device->Release();
        m_d3Device = NULL;
    }

    if (m_d3D != NULL)
    {
        m_d3D->Release();
        m_d3D = NULL;
    }
}

const char * D3D9Grabber::getName()
{
    return "D3D9Grabber";
}

int D3D9Grabber::getBufLength(const RECT &rect)
{
    return (rect.right - rect.left) * (rect.bottom - rect.top) * BYTES_PER_PIXEL;
}

QList<QRgb> D3D9Grabber::grabWidgetsColors(QList<GrabWidget *> &widgets)
{
    QList<QRgb> result;
    m_rect = getEffectiveRect(widgets);
    int bufLengthNeeded = getBufLength(m_rect);
    if (bufLengthNeeded > m_bufLength)
    {
        if(m_buf != NULL)
            free(m_buf);
        m_buf = (BYTE *)malloc(bufLengthNeeded);
        m_bufLength = bufLengthNeeded;
    }
    for(int i = 0; i < widgets.size(); i++)
    {
        QRect rect = widgets[i]->rect();
        result.append(getColor(rect.left(), rect.top(), rect.width(), rect.height()));
    }
    return result;
}

RECT D3D9Grabber::getEffectiveRect(QList<GrabWidget *> &widgets)
{
    RECT result = {0,0,0,0};
    if (widgets.size() > 0)
    {
        result.left   = widgets[0]->rect().left();
        result.right  = widgets[0]->rect().right();
        result.top    = widgets[0]->rect().top();
        result.bottom = widgets[0]->rect().bottom();
        for(int i = 1; i < widgets.size(); i++) {
            if (result.left > widgets[i]->rect().left())
                result.left = widgets[i]->rect().left();
            if (result.right < widgets[i]->rect().right())
                result.right = widgets[i]->rect().right();
            if (result.top > widgets[i]->rect().top())
                result.top = widgets[i]->rect().top();
            if (result.bottom < widgets[i]->rect().bottom())
                result.bottom = widgets[i]->rect().bottom();
        }
    }
    return result;
}

BYTE * D3D9Grabber::getImageData(BYTE * buf, RECT &rect)
{
    D3DLOCKED_RECT blockedRect;

    m_d3Device->GetFrontBufferData(0, m_surface);
    m_surface->LockRect(&blockedRect, &rect, D3DLOCK_READONLY);
    CopyMemory(buf, blockedRect.pBits, getBufLength(rect));
    m_surface->UnlockRect();
    return buf;
}

QRgb D3D9Grabber::getColor(int x, int y, int width, int height)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO
                     << "x y w h:" << x << y << width << height;

    // Convert coordinates from "Main" desktop coord-system to capture-monitor coord-system
    x -= m_rect.left;
    y -= m_rect.top;

    // Ignore part of LED widget which out of screen
    if( x < 0 ) {
        width  += x;  /* reduce width  */
        x = 0;
    }
    if( y < 0 ) {
        height += y;  /* reduce height */
        y = 0;
    }
    int screenWidth = m_rect.right - m_rect.left;
    int screenHeight = m_rect.bottom - m_rect.top;
    if( x + width  > screenWidth  ) width  -= (x + width ) - screenWidth;
    if( y + height > screenHeight ) height -= (y + height) - screenHeight;

    //calculate aligned width (align by 4 pixels)
    width = width - (width % 4);

    if(width < 0 || height < 0){
        qWarning() << Q_FUNC_INFO << "width < 0 || height < 0:" << width << height;

        // width and height can't be negative
        return 0x000000;
    }

    unsigned count = 0; // count the amount of pixels taken into account
    unsigned endIndex = (screenWidth * (y + height) + x + width) * BYTES_PER_PIXEL;
    register unsigned index = (screenWidth * y + x) * BYTES_PER_PIXEL; // index of the selected pixel in pbPixelsBuff
    register unsigned r = 0, g = 0, b = 0;
    while (index < endIndex - width * BYTES_PER_PIXEL) {
        for(int i = 0; i < width; i += 4) {
            b += m_buf[index]     + m_buf[index + 4] + m_buf[index + 8 ] + m_buf[index + 12];
            g += m_buf[index + 1] + m_buf[index + 5] + m_buf[index + 9 ] + m_buf[index + 13];
            r += m_buf[index + 2] + m_buf[index + 6] + m_buf[index + 10] + m_buf[index + 14];

            count+=4;
            index += BYTES_PER_PIXEL * 4;
        }

        index += (screenWidth - width) * BYTES_PER_PIXEL;
    }

    if( count != 0 ){
        r = (unsigned)round((double) r / count) & 0xff;
        g = (unsigned)round((double) g / count) & 0xff;
        b = (unsigned)round((double) b / count) & 0xff;
    }

    QRgb result = qRgb(r, g, b);

    DEBUG_HIGH_LEVEL << Q_FUNC_INFO << "QRgb result =" << hex << result;

    return result;
}
#endif
