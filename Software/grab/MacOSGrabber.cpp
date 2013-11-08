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
#include "debug.h"

MacOSGrabber::MacOSGrabber(QObject *parent, QList<QRgb> *grabResult, QList<GrabWidget *> *grabAreasGeometry):
    TimeredGrabber(parent, grabResult, grabAreasGeometry)
{
    _imageBuf = NULL;
    _imageBufSize = 0;
    _colorSpace = CGColorSpaceCreateDeviceRGB();
    _context = NULL;
    _contextHeight = 0;
    _contextWidth = 0;
}

MacOSGrabber::~MacOSGrabber()
{
    CGColorSpaceRelease(_colorSpace);
    if(_imageBuf)
        free(_imageBuf);
    if(_context)
        CGContextRelease(_context);
}

const char * MacOSGrabber::getName()
{
    return "MacOSGrabber";
}

void MacOSGrabber::updateGrabMonitor(QWidget *widget)
{
    Q_UNUSED(widget);
}

QImage * MacOSGrabber::toImage(CGImageRef imageRef)
{
    size_t width = CGImageGetWidth(imageRef);
    size_t height = CGImageGetHeight(imageRef);
    size_t new_buf_size = height * width * 4;
    if (new_buf_size > _imageBufSize) {
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "new width = " << width << " new height = " << height;
        if (_imageBuf)
            free(_imageBuf);
        _imageBuf = (unsigned char*) calloc(height * width * 4, sizeof(unsigned char));
        _imageBufSize = new_buf_size;
    }

    size_t bytesPerPixel = 4;
    size_t bytesPerRow = bytesPerPixel * width;
    size_t bitsPerComponent = 8;
    if (width != _contextWidth || height != _contextHeight) {
        if(_context)
            CGContextRelease(_context);

        _context = CGBitmapContextCreate(_imageBuf, width, height,
                    bitsPerComponent, bytesPerRow, _colorSpace,
                    kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Little);
        _contextWidth = width;
        _contextHeight = height;
    }

    CGContextDrawImage(_context, CGRectMake(0, 0, _contextWidth, _contextHeight), imageRef);

    QImage * result = new QImage(_imageBuf, _contextWidth, _contextHeight, QImage::Format_RGB32);

  return result;
}

GrabResult MacOSGrabber::_grab()
{
    CGImageRef imageRef = CGDisplayCreateImage(kCGDirectMainDisplay);

    if (imageRef != NULL)
    {

        QImage *image = toImage(imageRef);
        m_grabResult->clear();
        foreach(GrabWidget * widget, *m_grabWidgets) {
            m_grabResult->append( widget->isAreaEnabled() ? getColor(image, widget) : qRgb(0,0,0) );
        }
        delete image;

        CGImageRelease(imageRef);

    } else {        

        qCritical() << Q_FUNC_INFO << "CGDisplayCreateImage(..) returned NULL";
        return GrabResultError;
    }
    return GrabResultOk;
}

QRgb MacOSGrabber::getColor(QImage * image, const QWidget * grabme)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    return getColor(image,
                    grabme->x(),
                    grabme->y(),
                    grabme->width(),
                    grabme->height());
}

QRgb MacOSGrabber::getColor(QImage * image, int x, int y, int width, int height)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO
                     << "x y w h:" << x << y << width << height;

    QImage img = image->copy(x,y, width, height);
    QImage scaledPix = img.scaled(1,1, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QRgb result = scaledPix.pixel(0,0);

    DEBUG_HIGH_LEVEL << "QRgb result =" << hex << result;

    return result;
}
#endif // MAC_OS_CG_GRAB_SUPPORT
