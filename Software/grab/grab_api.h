/*
 * grab_api.h
 *
 *  Created on: 9.03.2011
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Lightpack is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2011 Mike Shatohin, mikeshatohin [at] gmail.com
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

#ifndef GRAB_API_H
#define GRAB_API_H

#include <QWidget>
#include <QColor>

// Using WinAPI:
//      if grabbing-monitor changed call findScreenOnNextCapture( HWND ) for update buffer size
//      after it just call captureScreen(), getColor(), ..., getColor() repeatedly
// Using Qt:
//      just call getColor(...);


namespace GrabQt
{
    QRgb getColor(const QWidget * grabme);
    QRgb getColor(int x, int y, int width, int height);
};

namespace GrabWinAPI
{    
    void findScreenOnNextCapture( WId winId );
    void captureScreen();
    QRgb getColor(const QWidget * grabme);
    QRgb getColor(int x, int y, int width, int height);
};

#endif // GRAB_API_H
