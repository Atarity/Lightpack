/*
 * X11Grabber.hpp
 *
 *  Created on: 25.06.11
 *     Project: Lightpack
 *
 *  Copyright (c) 2011 Andrey Isupov, Timur Sattarov, Mike Shatohin
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

#ifdef X11_GRAB_SUPPORT

#include <QScopedPointer>
#include "../src/debug.h"

struct X11GrabberData;

using namespace Grab;

class X11Grabber : public TimeredGrabber
{
public:
    X11Grabber(QObject *parent, QList<QRgb> *grabResult, QList<GrabWidget *> *grabAreasGeometry);
    ~X11Grabber();
    virtual const char * getName();
    virtual void updateGrabMonitor( QWidget * widget );
    virtual QList<QRgb> grabWidgetsColors(QList<GrabWidget *> &widgets);

protected:
    virtual GrabResult _grab();

private:
    void captureScreen();
    QRgb getColor(const QWidget * grabme);
    QRgb getColor(int x, int y, int width, int height);

private:
    bool updateScreenAndAllocateMemory;
    int screen;
    QRect screenres;

    QScopedPointer<X11GrabberData> data;
};
#endif // X11_GRAB_SUPPORT
