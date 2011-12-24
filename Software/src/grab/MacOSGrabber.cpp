/*
 * MacOSGrabber.cpp
 *
 *  Created on: 01.10.11
 *     Project: Lightpack
 *
 *  Copyright (c) 2011 Timur Sattarov, Nikolay Kudashov, Mike Shatohin
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

#include"MacOSGrabber.hpp"

#ifdef MAC_OS_CG_GRAB_SUPPORT

#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#include "debug.h"

MacOSGrabber::MacOSGrabber()
{
}

MacOSGrabber::~MacOSGrabber()
{
}

const char * MacOSGrabber::getName()
{
    return "MacOSGrabber";
}

void MacOSGrabber::updateGrabScreenFromWidget(QWidget *widget)
{
    Q_UNUSED(widget);
}

QList<QRgb> MacOSGrabber::grabWidgetsColors(QList<GrabWidget *> &widgets)
{
    CGImageRef image = CGDisplayCreateImage(kCGDirectMainDisplay);
    QList<QRgb> result;

    if (image != NULL)
    {
        QPixmap pixmap = QPixmap::fromMacCGImageRef(image);

        for(int i = 0; i < widgets.size(); i++) {
            result.append(getColor(pixmap, widgets[i]));
        }

        CGImageRelease(image);

    } else {        

        qCritical() << Q_FUNC_INFO << "CGDisplayCreateImage(..) returned NULL";

        for(int i = 0; i < widgets.size(); i++)
            result.append(0);
    }
    return result;
}

QRgb MacOSGrabber::getColor(QPixmap pixmap, const QWidget * grabme)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    return getColor(pixmap,
                    grabme->x(),
                    grabme->y(),
                    grabme->width(),
                    grabme->height());
}

QRgb MacOSGrabber::getColor(QPixmap pixmap, int x, int y, int width, int height)
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
#endif // MAC_OS_CG_GRAB_SUPPORT
