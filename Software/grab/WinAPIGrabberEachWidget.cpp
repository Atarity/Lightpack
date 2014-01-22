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

#include "WinAPIGrabberEachWidget.hpp"

#ifdef WINAPI_GRAB_SUPPORT
#include "../src/debug.h"
#include <cmath>

#if defined _MSC_VER
#include "PrismatikMath.hpp"

using PrismatikMath::round;
#endif // _MSC_VER

WinAPIGrabberEachWidget::WinAPIGrabberEachWidget(QObject * parent, QList<QRgb> *grabResult, QList<GrabWidget *> *grabAreasGeometry)
    : TimeredGrabber(parent, grabResult, grabAreasGeometry),
      isBufferNeedsResize(true),
      hScreenDC(NULL),
      hMemDC(NULL),
      hBitmap(NULL)
{
}

WinAPIGrabberEachWidget::~WinAPIGrabberEachWidget()
{
    freeDCs();
}

const char * WinAPIGrabberEachWidget::getName()
{
    return "WinAPIGrabberEachWidget";
}

void WinAPIGrabberEachWidget::updateGrabMonitor(QWidget *widget)
{
    hMonitor = MonitorFromWindow( reinterpret_cast<HWND>(widget->winId()), MONITOR_DEFAULTTONEAREST );
    isBufferNeedsResize = true;
}

GrabResult WinAPIGrabberEachWidget::_grab()
{
    m_grabResult->clear();
    foreach(GrabWidget * widget, *m_grabWidgets) {
        m_grabResult->append( widget->isAreaEnabled() ? getColor(widget) : qRgb(0,0,0) );
    }
    return GrabResultOk;
}

void WinAPIGrabberEachWidget::captureWidget(const QWidget * w)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    if( isBufferNeedsResize ){

        ZeroMemory( &monitorInfo, sizeof(MONITORINFO) );
        monitorInfo.cbSize = sizeof(MONITORINFO);

        // Get position and resolution of the monitor
        GetMonitorInfo( hMonitor, &monitorInfo );

        screenWidth  = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
        screenHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "screenWidth x screenHeight" << screenWidth << "x" << screenHeight;

        // Release previous resorces.
        freeDCs();

        // CreateDC for multiple monitors
        hScreenDC = CreateDC( TEXT("DISPLAY"), NULL, NULL, NULL );

        // Create a bitmap compatible with the screen DC
        hBitmap = CreateCompatibleBitmap( hScreenDC, screenWidth, screenHeight );

        // Create a memory DC compatible to screen DC
        hMemDC = CreateCompatibleDC( hScreenDC );

        // Select new bitmap into memory DC
        SelectObject( hMemDC, hBitmap );
    }

    // Copy screen
    BitBlt( hMemDC, w->x(), w->y(), w->width(), w->height(), hScreenDC,
            w->x(), w->y(), SRCCOPY );

    if( isBufferNeedsResize ){

        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Allocate memory for pbPixelsBuff and update pixelsBuffSize, bytesPerPixel";

        BITMAP bmp;
        memset(&bmp, 0, sizeof(BITMAP));

        // Now get the actual Bitmap
        GetObject( hBitmap, sizeof(BITMAP), &bmp );

        // Calculate the size the buffer needs to be
        unsigned pixelsBuffSizeNew = bmp.bmWidthBytes * bmp.bmHeight;

        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "pixelsBuffSize =" << pixelsBuffSizeNew;

        // ReAllocate memory for new buffer size
        pbPixelsBuff.resize(pixelsBuffSizeNew);

        // The amount of bytes per pixel is the amount of bits divided by 8
        bytesPerPixel = bmp.bmBitsPixel / 8;

        if( bytesPerPixel != 4 ){
            qDebug() << "Not 32-bit mode is not supported!" << bytesPerPixel;
        }

        isBufferNeedsResize = false;
    }

    // Get the actual RGB data and put it into pbPixelsBuff
    GetBitmapBits( hBitmap, pbPixelsBuff.size(), &pbPixelsBuff[0] );
}

QRgb WinAPIGrabberEachWidget::getColor(const QWidget * grabme)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    captureWidget(grabme);

    return getColor(grabme->x(),
                    grabme->y(),
                    grabme->width(),
                    grabme->height());
}

QRgb WinAPIGrabberEachWidget::getColor(int x, int y, int width, int height)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO
                     << "x y w h:" << x << y << width << height;

    if (pbPixelsBuff.empty())
    {
        qCritical() << Q_FUNC_INFO << "pbPixelsBuff is empty!";
        return 0;
    }

    // Checking for the 'grabme' widget position inside the monitor that is used to capture color
    if(     x + width  < monitorInfo.rcMonitor.left   ||
            x          > monitorInfo.rcMonitor.right  ||
            y + height < monitorInfo.rcMonitor.top    ||
            y          > monitorInfo.rcMonitor.bottom ){

        DEBUG_MID_LEVEL << "Widget 'grabme' is out of screen, x y w h:" << x << y << width << height;

        // Widget 'grabme' is out of screen
        return 0x000000;
    }

    // Convert coordinates from "Main" desktop coord-system to capture-monitor coord-system
    x -= monitorInfo.rcMonitor.left;
    y -= monitorInfo.rcMonitor.top;

    // Ignore part of LED widget which out of screen
    if( x < 0 ) {
        width  += x;  /* reduce width  */
        x = 0;
    }
    if( y < 0 ) {
        height += y;  /* reduce height */
        y = 0;
    }
    if( x + width  > (int)screenWidth  ) width  -= (x + width ) - screenWidth;
    if( y + height > (int)screenHeight ) height -= (y + height) - screenHeight;

    //calculate aligned width (align by 4 pixels)
    width = width - (width % 4);

    if(width < 0 || height < 0){
        qWarning() << Q_FUNC_INFO << "width < 0 || height < 0:" << width << height;

        // width and height can't be negative
        return 0x000000;
    }

    unsigned count = 0; // count the amount of pixels taken into account
    unsigned endIndex = (screenWidth * (y + height) + x + width) * bytesPerPixel;
    register unsigned index = (screenWidth * y + x) * bytesPerPixel; // index of the selected pixel in pbPixelsBuff
    register unsigned r = 0, g = 0, b = 0;
    while (index < endIndex - width * bytesPerPixel) {
        for(int i = 0; i < width; i += 4) {
            b += pbPixelsBuff[index]     + pbPixelsBuff[index + 4] + pbPixelsBuff[index + 8 ] + pbPixelsBuff[index + 12];
            g += pbPixelsBuff[index + 1] + pbPixelsBuff[index + 5] + pbPixelsBuff[index + 9 ] + pbPixelsBuff[index + 13];
            r += pbPixelsBuff[index + 2] + pbPixelsBuff[index + 6] + pbPixelsBuff[index + 10] + pbPixelsBuff[index + 14];

            count+=4;
            index += bytesPerPixel * 4;
        }

        index += (screenWidth - width) * bytesPerPixel;
    }

    if( count != 0 ){
        r = (unsigned)round((double) r / count) & 0xff;
        g = (unsigned)round((double) g / count) & 0xff;
        b = (unsigned)round((double) b / count) & 0xff;
    }

#if 0
	int monitorWidth = screenWidth;
	int monitorHeight = screenHeight;
	const int BytesPerPixel = 4;
	// Save image of screen:
	QImage * im = new QImage( monitorWidth, monitorHeight, QImage::Format_RGB32 );
	for(int i=0; i<monitorWidth; i++){
		for(int j=0; j<monitorHeight; j++){
			index = (BytesPerPixel * j * monitorWidth) + (BytesPerPixel * i);
			QRgb rgb = pbPixelsBuff[index+2] << 16 | pbPixelsBuff[index+1] << 8 | pbPixelsBuff[index];
			im->setPixel(i, j, rgb);
		}
	}
	im->save(QString("screen%1x%2.jpg").arg(x).arg(y));
	delete im;
#endif

    QRgb result = qRgb(r, g, b);

    DEBUG_HIGH_LEVEL << Q_FUNC_INFO << "QRgb result =" << hex << result;

    return result;
}

void WinAPIGrabberEachWidget::freeDCs()
{
    if (hScreenDC)
        DeleteObject( hScreenDC );

    if (hMemDC)
        DeleteObject( hMemDC );

    if (hScreenDC)
        DeleteDC( hScreenDC );
}

#endif // WINAPI_GRAB_SUPPORT
