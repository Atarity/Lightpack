/*
 * QtGrabber.cpp
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

#include "QtGrabber.hpp"

#ifdef QT_GRAB_SUPPORT

#include "debug.h"
#include <QtGui>

QtGrabber::QtGrabber()
{
    screen = 0;
}

QtGrabber::~QtGrabber()
{
}

const char * QtGrabber::getName()
{
    return "QtGrabber";
}

void QtGrabber::updateGrabScreenFromWidget(QWidget *widget)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;
    screen = QApplication::desktop()->screenNumber( widget );
    screenres = QApplication::desktop()->screenGeometry( screen );
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "screenWidth x screenHeight" << screenres.width() << "x" << screenres.height();
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "screen " << screen;
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "screenres " << screenres;
}

GrabResult QtGrabber::grabWidgetsColors(QList<GrabWidget *> &widgets, QList<QRgb> * widgetsColors)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;
    QPixmap pixmap = QPixmap::grabWindow(QApplication::desktop()->screen(-1) ->winId(),
                                  screenres.x(), //!
                                  screenres.y(), //!
                                  screenres.width(),
                                  screenres.height());
    for(int i = 0; i < widgets.size(); i++) {
        widgetsColors->append(getColor(pixmap, widgets[i]));
    }
#if 0
    if (screenres.width() < 1920)
        pixmap.toImage().save("screen.jpg");
#endif
    return GrabResultOk;
}

QRgb QtGrabber::getColor(QPixmap pixmap, const QWidget * grabme)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    return getColor(pixmap,
                    grabme->x(),
                    grabme->y(),
                    grabme->width(),
                    grabme->height());
}

QRgb QtGrabber::getColor(QPixmap pixmap, int x, int y, int width, int height)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO
                     << "x y w h:" << x << y << width << height;

    QPixmap pix = pixmap.copy(x,y, width, height);
    QPixmap scaledPix = pix.scaled(1,1, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage im = scaledPix.toImage();
    QRgb result = im.pixel(0,0);

    DEBUG_HIGH_LEVEL << "QRgb result =" << hex << result;

    return result;
}
#endif // QT_GRAB_SUPPORT
