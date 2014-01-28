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

#include <CoreGraphics/CoreGraphics.h>
#include <ApplicationServices/ApplicationServices.h>
#include "calculations.hpp"
#include "debug.h"

MacOSGrabber::MacOSGrabber(QObject * parent, GrabberContext *context)
    : TimeredGrabber(parent, context)
    , _imageBuf(NULL)
    , _imageBufSize(0)
{
}

MacOSGrabber::~MacOSGrabber()
{
    if(_imageBuf)
        free(_imageBuf);
}

void MacOSGrabber::updateGrabMonitor(QWidget *widget)
{
    Q_UNUSED(widget);
}

QImage * MacOSGrabber::toImage(CGImageRef imageRef)
{
    const size_t bytesPerPixel = 4;
    size_t width = CGImageGetWidth(imageRef);
    size_t height = CGImageGetHeight(imageRef);
    size_t new_buf_size = height * width * bytesPerPixel;
    if (new_buf_size > _imageBufSize) {
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "new width = " << width << " new height = " << height;
        if (_imageBuf)
            free(_imageBuf);
        _imageBuf = (unsigned char*) calloc(height * width * bytesPerPixel, sizeof(unsigned char));
        _imageBufSize = new_buf_size;
    }


    CGDataProviderRef provider = CGImageGetDataProvider(imageRef);
    CFDataRef dataref = CGDataProviderCopyData(provider);
    memcpy(_imageBuf, CFDataGetBytePtr(dataref), width * height * 4);
    CFRelease(dataref);

    QImage * result = new QImage(_imageBuf, width, height, QImage::Format_RGB32);

  return result;
}

GrabResult MacOSGrabber::_grab(QList<QRgb> &grabResult, const QList<GrabWidget *> &grabWidgets)
{
    CGImageRef imageRef = CGDisplayCreateImage(kCGDirectMainDisplay);

    if (imageRef != NULL)
    {

        QImage *image = toImage(imageRef);
        grabResult.clear();
        foreach(GrabWidget * widget, grabWidgets) {
            grabResult.append( widget->isAreaEnabled() ? getColor(image, widget->frameGeometry()) : qRgb(0,0,0) );
        }
        delete image;

        CGImageRelease(imageRef);

    } else {        

        qCritical() << Q_FUNC_INFO << "CGDisplayCreateImage(..) returned NULL";
        return GrabResultError;
    }
    return GrabResultOk;
}

QRgb MacOSGrabber::getColor(QImage * image, const QRect &rect)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO
                     << "x y w h:" << rect.x() << rect.y() << rect.width() << rect.height();

    QRgb result;

    Grab::Calculations::calculateAvgColor(&result, image->constBits(), BufferFormatArgb, image->width()*4, rect);

    DEBUG_HIGH_LEVEL << "QRgb result =" << hex << result;

    return result;
}
#endif // MAC_OS_CG_GRAB_SUPPORT
