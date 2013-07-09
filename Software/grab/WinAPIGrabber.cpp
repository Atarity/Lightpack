/*
 * WinAPIGrabber.cpp
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

#include"WinAPIGrabber.hpp"
#ifdef WINAPI_GRAB_SUPPORT
#include"../src/debug.h"
#include<cmath>
#include"calculations.hpp"
#include"../src/enums.hpp"

WinAPIGrabber::WinAPIGrabber(QObject * parent, QList<QRgb> *grabResult, QList<GrabWidget *> *grabAreasGeometry)
    : TimeredGrabber(parent, grabResult, grabAreasGeometry)
{
    pbPixelsBuff = NULL;
}

WinAPIGrabber::~WinAPIGrabber()
{
    freeDCs();
    delete[] pbPixelsBuff;
}

void WinAPIGrabber::freeDCs()
{
    if (hScreenDC)
        DeleteObject(hScreenDC);

    if (hBitmap)
        DeleteObject(hBitmap);

    if (hMemDC)
        DeleteObject(hMemDC);
}

void WinAPIGrabber::updateGrabMonitor(QWidget *widget)
{
    HMONITOR hMonitorNew = MonitorFromWindow(reinterpret_cast<HWND>(widget->winId()), MONITOR_DEFAULTTONEAREST);
    if (hMonitor != hMonitorNew) {
        hMonitor = hMonitorNew;

        ZeroMemory( &monitorInfo, sizeof(MONITORINFO) );
        monitorInfo.cbSize = sizeof(MONITORINFO);

        // Get position and resolution of the monitor
        GetMonitorInfo( hMonitor, &monitorInfo );

        screenWidth  = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
        screenHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "screenWidth x screenHeight" << screenWidth << "x" << screenHeight;


        freeDCs();

        // CreateDC for multiple monitors
        hScreenDC = CreateDC( TEXT("DISPLAY"), NULL, NULL, NULL );

        // Create a bitmap compatible with the screen DC
        hBitmap = CreateCompatibleBitmap( hScreenDC, screenWidth, screenHeight );

        // Create a memory DC compatible to screen DC
        hMemDC = CreateCompatibleDC( hScreenDC );

        // Select new bitmap into memory DC
        SelectObject( hMemDC, hBitmap );

        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Allocate memory for pbPixelsBuff and update pixelsBuffSize, bytesPerPixel";

        BITMAP * bmp = new BITMAP;

        // Now get the actual Bitmap
        GetObject( hBitmap, sizeof(BITMAP), bmp );

        // Calculate the size the buffer needs to be
        unsigned pixelsBuffSizeNew = bmp->bmWidthBytes * bmp->bmHeight;

        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "pixelsBuffSize =" << pixelsBuffSizeNew;

        if(pixelsBuffSize != pixelsBuffSizeNew){
            pixelsBuffSize = pixelsBuffSizeNew;

            // ReAllocate memory for new buffer size
            if( pbPixelsBuff ) delete[] pbPixelsBuff;

            // Allocate
            pbPixelsBuff = new BYTE[ pixelsBuffSize ];
        }

        // The amount of bytes per pixel is the amount of bits divided by 8
        bytesPerPixel = bmp->bmBitsPixel / 8;

        if( bytesPerPixel != 4 ){
            qDebug() << "Not 32-bit mode is not supported!" << bytesPerPixel;
        }

        DeleteObject( bmp );
    }

}

GrabResult WinAPIGrabber::_grab()
{
    captureScreen();
    m_grabResult->clear();
    foreach(GrabWidget * widget, *m_grabWidgets) {
        m_grabResult->append( widget->isAreaEnabled() ? getColor(widget) : qRgb(0,0,0) );
    }
    return GrabResultOk;
}

void WinAPIGrabber::captureScreen()
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    // Copy screen
    BitBlt( hMemDC, 0, 0, screenWidth, screenHeight, hScreenDC,
            monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top, SRCCOPY );

    // Get the actual RGB data and put it into pbPixelsBuff
    GetBitmapBits( hBitmap, pixelsBuffSize, pbPixelsBuff );
}

QRgb WinAPIGrabber::getColor(const QWidget * grabme)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    QRect widgetRect = grabme->frameGeometry();
    return getColor(widgetRect);
}

QRgb WinAPIGrabber::getColor(const QRect &widgetRect)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO << Debug::toString(widgetRect);

    if (pbPixelsBuff == NULL)
    {
        qCritical() << Q_FUNC_INFO << "pbPixelsBuff == NULL";
        return 0;
    }

    RECT rcMonitor = monitorInfo.rcMonitor;
    QRect monitorRect = QRect( QPoint(rcMonitor.left, rcMonitor.top), QPoint(rcMonitor.right-1, rcMonitor.bottom-1));

    QRect clippedRect = monitorRect.intersected(widgetRect);

    // Checking for the 'grabme' widget position inside the monitor that is used to capture color
    if( !clippedRect.isValid() ){

        DEBUG_MID_LEVEL << "Widget 'grabme' is out of screen:" << Debug::toString(clippedRect);

        // Widget 'grabme' is out of screen
        return 0x000000;
    }

    // Convert coordinates from "Main" desktop coord-system to capture-monitor coord-system
    QRect preparedRect = clippedRect.translated(-monitorRect.x(), -monitorRect.y());

    // Align width by 4 for accelerated calculations
    preparedRect.setWidth(preparedRect.width() - (preparedRect.width() % 4));

    if( !preparedRect.isValid() ){
        qWarning() << Q_FUNC_INFO << " preparedRect is not valid:" << Debug::toString(preparedRect);

        // width and height can't be negative
        return 0x000000;
    }

    using namespace Grab;
    QRgb avgColor;
    if (Calculations::calculateAvgColor(&avgColor, pbPixelsBuff, BufferFormatArgb, screenWidth * bytesPerPixel, preparedRect ) == 0) {
        return avgColor;
    } else {
        return qRgb(0,0,0);
    }

#if 0
    if (screenWidth < 1920 && (r > 120 || g > 120 || b > 120)) {
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
        im->save("screen.jpg");
        delete im;
    }
#endif

}

#endif // WINAPI_GRAB_SUPPORT
