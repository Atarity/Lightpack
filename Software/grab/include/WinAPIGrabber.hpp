/*
 * WinAPIGrabber.hpp
 *
 *  Created on: 25.07.11
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

class WinAPIGrabber : public TimeredGrabber
{
    Q_OBJECT
public:
    WinAPIGrabber(QObject * parent, GrabberContext *context);
    virtual ~WinAPIGrabber();

    DECLARE_GRABBER_NAME("WinAPIGrabber")

protected slots:
    virtual GrabResult grabScreens();
    virtual bool reallocate(const QList< ScreenInfo > &grabScreens);

    virtual QList< ScreenInfo > * screensWithWidgets(QList< ScreenInfo > * result, const QList<GrabWidget *> &grabWidgets);

protected:
    void freeScreens();
    void captureScreen();
    QRgb getColor(const QWidget * grabme);
    QRgb getColor(const QRect &widgetRect);
    void updateMonitorInfo();
    void resizePixelsBuffer();

protected:

    // Captured screen buffer, contains actual RGB data in reversed order
    unsigned bytesPerPixel;

};

#endif // WINAPI_GRAB_SUPPORT
