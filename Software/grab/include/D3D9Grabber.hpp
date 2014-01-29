/*
 * D3D9Grabber.hpp
 *
 *  Created on: 19.09.2011
 *     Project: Lightpack
 *
 *  Copyright (c) 2011 Timur Sattarov
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

#pragma once

#include "TimeredGrabber.hpp"
#include "../src/enums.hpp"

#ifdef D3D9_GRAB_SUPPORT

// <d3d9.h> includes <windows.h>
#if !defined NOMINMAX
#define NOMINMAX
#endif

#if !defined WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <d3d9.h>

using namespace Grab;

class D3D9Grabber : public TimeredGrabber
{
public:
    D3D9Grabber(QObject * parent, GrabberContext *context);
    virtual ~D3D9Grabber();
    virtual void updateGrabMonitor( QWidget * ){}

    virtual const char * name() const {
        static char * name = "D3D9Grabber";
        return name;
    }

protected:
    virtual GrabResult _grab(QList<QRgb> &grabResult, const QList<GrabWidget *> &grabWidgets);

protected slots:
    virtual GrabResult grabScreens();
    virtual bool reallocate(const QList< ScreenInfo > &grabScreens);

    virtual QList< ScreenInfo > * screensToGrab(QList< ScreenInfo > * result, const QList<GrabWidget *> &grabWidgets);

private:
    BYTE * expandBuffer(BYTE * buf, int newLength);
    BYTE * getImageData(QVector<BYTE> &, RECT &);
    RECT getEffectiveRect(const QList<GrabWidget *> &widgets);
    int getBufLength(const RECT &rect);
    QRgb getColor(int x, int y, int width, int height);
    void clipRect(RECT *rect, D3DSURFACE_DESC *surfaceDesc);

private:
    LPDIRECT3D9 m_d3D;
    LPDIRECT3DDEVICE9 m_d3Device;
    LPDIRECT3DSURFACE9 m_surface;
    D3DDISPLAYMODE m_displayMode;
    D3DPRESENT_PARAMETERS m_presentParams;
    QVector<BYTE> m_buf;
    RECT m_rect;
};

#endif // D3D9_GRAB_SUPPORT
