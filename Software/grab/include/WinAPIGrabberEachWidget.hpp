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

#include "WinAPIGrabber.hpp"
#include "../src/enums.hpp"

#ifdef WINAPI_EACH_GRAB_SUPPORT

class WinAPIGrabberEachWidget : public WinAPIGrabber
{
public:
    WinAPIGrabberEachWidget(QObject * parent, GrabberContext *context);
    virtual ~WinAPIGrabberEachWidget();

    DECLARE_GRABBER_NAME("WinAPIGrabberEachWidget")

protected slots:
    virtual GrabResult grabScreens();
    virtual bool reallocate(const QList< ScreenInfo > &grabScreens);

    virtual QList< ScreenInfo > * screensWithWidgets(QList< ScreenInfo > * result, const QList<GrabWidget *> &grabWidgets);
    virtual void grab();

private:
    void captureWidget(const QWidget * w);
    QRgb getColor(const QWidget * grabme);
    QRgb getColor(int x, int y, int width, int height);

private:
    bool isBufferNeedsResize;
};

#endif // WINAPI_EACH_GRAB_SUPPORT
