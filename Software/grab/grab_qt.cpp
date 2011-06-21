/*
 * grab_qt.cpp
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

#include <QApplication>
#include <QDesktopWidget>
#include <QPixmap>
#include <QImage>

#include "grab_api.h"

#include "debug.h"

#ifndef Q_WS_WIN
namespace GrabWinAPI
{
    void findScreenOnNextCapture( WId ) { }
    void captureScreen() { }
    QRgb getColor(const QWidget * ) { return 0; }
    QRgb getColor(int, int, int, int) { return 0; }
};
#endif

namespace GrabQt
{
 QPixmap capture = NULL;
 QRect screenres;
 bool updateScreenAndAllocateMemory = true;
 int screen = 0;

  void setScreenOnNextCapture( int screenId )
 {
             DEBUG_HIGH_LEVEL << Q_FUNC_INFO;
            updateScreenAndAllocateMemory = true;
            screen = screenId;
 }

 void captureScreen()
 {
           DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

            if( updateScreenAndAllocateMemory ){
            capture =  QPixmap();
            screenres = QApplication::desktop()->screenGeometry(screen);
            updateScreenAndAllocateMemory = false;
            }
            capture = QPixmap::grabWindow(QApplication::desktop()->screen(screen) ->winId(),
                                                screenres.x(), //!
                                                screenres.y(), //!
                                                screenres.width(),
                                                screenres.height());
}


QRgb getColor(const QWidget * grabme)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    return getColor(grabme->x(),
                    grabme->y(),
                    grabme->width(),
                    grabme->height());
}

QRgb getColor(int x, int y, int width, int height)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO
            << "x y w h:" << x << y << width << height;

    QPixmap pix = capture.copy(x,y, width, height);
    QPixmap scaledPix = pix.scaled(1,1, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage im = scaledPix.toImage();
    QRgb result = im.pixel(0,0);

    DEBUG_HIGH_LEVEL << "QRgb result =" << hex << result;

    return result;
}

};

