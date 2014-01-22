/*
 * WinAPIGrabberEachWidget.hpp
 *
 *  Created on: 22.11.11
 *     Project: Lightpack
 *
 *  Copyright (c) 2011 Timur Sattarov, Mike Shatohin
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

#ifdef WINAPI_GRAB_SUPPORT

#if defined WINVER && WINVER < 0x0500
#undef WINVER
#endif

#if !defined WINVER
#define WINVER 0x0500 /* Windows2000 for MonitorFromWindow(..) func */
#endif
#if !defined NOMINMAX
#define NOMINMAX
#endif

#if !defined WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

using namespace Grab;

class WinAPIGrabberEachWidget : public TimeredGrabber
{
public:
    WinAPIGrabberEachWidget(QObject * parent, QList<QRgb> *grabResult, QList<GrabWidget *> *grabAreasGeometry);
    ~WinAPIGrabberEachWidget();
    virtual const char * getName();
    virtual void updateGrabMonitor( QWidget * widget );

protected:
    virtual GrabResult _grab();

private:
    void captureWidget(const QWidget * w);
    QRgb getColor(const QWidget * grabme);
    QRgb getColor(int x, int y, int width, int height);
    void freeDCs();

private:
    HMONITOR hMonitor;
    bool isBufferNeedsResize;
    MONITORINFO monitorInfo;

    // Size of screen in pixels, initialize in captureScreen() using in getColor()
    unsigned screenWidth;
    unsigned screenHeight;

    // Captured screen buffer, contains actual RGB data in reversed order
    QVector<BYTE> pbPixelsBuff;
    unsigned bytesPerPixel;

    HDC hScreenDC;
    HDC hMemDC;
    HBITMAP hBitmap;

};
#endif // WINAPI_GRAB_SUPPORT
