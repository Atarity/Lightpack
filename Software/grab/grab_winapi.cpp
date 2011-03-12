/*
 * grab_winapi.cpp
 *
 *  Created on: 9.03.2011
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Lightpack is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2011 Mike Shatohin, mikeshatohin [at] gmail.com
 *
 *  Thanks Amblone project for WinAPI grab example: http://amblone.com/
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

#include <QtDebug>

#define WINVER 0x0500 /* Windows2000 for MonitorFromWindow(..) func */
#include <windows.h>
#include <cmath>

#include "grab_api.h"


// Position of monitor, initialize in captureScreen() using in getColor()
MONITORINFO monitorInfo;

// Size of screen in pixels, initialize in captureScreen() using in getColor()
unsigned screenWidth;
unsigned screenHeight;

// Captured screen buffer, contains actual RGB data in reversed order
BYTE * pbPixelsBuff;
unsigned pixelsBuffSize;
unsigned bytesPerPixel;

//
// Capture screen what contains firstLedWidget to pbPixelsBuff
//
void Grab::captureScreen(const QWidget * firstLedWidget)
{    
    // Find the monitor, what contains firstLedWidget
    HMONITOR hMonitor = MonitorFromWindow( firstLedWidget->winId(), MONITOR_DEFAULTTONEAREST );

    ZeroMemory( &monitorInfo, sizeof(MONITORINFO) );
    monitorInfo.cbSize = sizeof(MONITORINFO);

    // Get position and resolution of the monitor
    GetMonitorInfo( hMonitor, &monitorInfo );

    screenWidth  = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
    screenHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

    // CreateDC for multiple monitors
    HDC hScreenDC = CreateDC( TEXT("DISPLAY"), NULL, NULL, NULL );

    // Create a bitmap compatible with the screen DC
    HBITMAP hBitmap = CreateCompatibleBitmap( hScreenDC, screenWidth, screenHeight );

    // Create a memory DC compatible to screen DC
    HDC hMemDC = CreateCompatibleDC( hScreenDC );

    // Select new bitmap into memory DC
    HGDIOBJ hOldBitmap = SelectObject( hMemDC, hBitmap );

    // Copy screen
    BitBlt( hMemDC, 0, 0, screenWidth, screenHeight, hScreenDC,
           monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top, SRCCOPY );

    // Select old bitmap back into memory DC and get handle to bitmap of the screen
    hBitmap = (HBITMAP) SelectObject( hMemDC, hOldBitmap );

    BITMAP * bmp = new BITMAP;
    // Now get the actual Bitmap
    GetObject( hBitmap, sizeof(BITMAP), bmp );

    // Calculate the size the buffer needs to be
    pixelsBuffSize = bmp->bmWidthBytes * bmp->bmHeight;

    // Allocate
    pbPixelsBuff = new BYTE[ pixelsBuffSize ];

    // The amount of bytes per pixel is the amount of bits divided by 8
    bytesPerPixel = bmp->bmBitsPixel / 8;

    if( bytesPerPixel != 4 ){
        qDebug() << "Not 32-bit mode is not supported!";
    }

    // Get the actual RGB data and put it into pbPixels
    GetBitmapBits( hBitmap, pixelsBuffSize, pbPixelsBuff );


    // CleanUp
    DeleteObject( hBitmap );
    DeleteObject( hOldBitmap );

    DeleteDC( hScreenDC );
    DeleteDC( hMemDC );

    if( bmp ) delete bmp;
}

//
// Release pbPixelsBuff memory
//
void Grab::cleanUp()
{
    if( pbPixelsBuff ) delete pbPixelsBuff;
}


//
// Get AVG color of the rect set by 'grabme' widget from captured screen buffer pbPixelsBuff
//
QColor Grab::getColor(const QWidget * grabme)
{    
    int x = grabme->x();
    int y = grabme->y();

    unsigned width  = grabme->width();
    unsigned height = grabme->height();

    unsigned r = 0, g = 0, b = 0;

    // Checking for the 'grabme' widget position inside the monitor that is used to capture color
    if( x + (int)width  < monitorInfo.rcMonitor.left   ||
        x               > monitorInfo.rcMonitor.right  ||
        y + (int)height < monitorInfo.rcMonitor.top    ||
        y               > monitorInfo.rcMonitor.bottom ){

        // Widget 'grabme' is out of screen
        return QColor(0, 0, 0);
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
    if( x + width  > screenWidth  ) width  -= (x + width ) - screenWidth;
    if( y + height > screenHeight ) height -= (y + height) - screenHeight;


    unsigned index = 0; // index of the selected pixel in pbPixelsBuff
    unsigned count = 0; // count the amount of pixels taken into account

    // This is where all the magic happens: calculate the average RGB
    for(unsigned i = x; i < x + width; i++){
        for(unsigned j = y; j < y + height; j++){
            // Calculate new index value
            index = (bytesPerPixel * j * screenWidth) + (bytesPerPixel * i);
            if(index > pixelsBuffSize) {
                qDebug() << "index out of range pbPixelsBuff[]" << index << x << y << width << height;
                break;
            }

            // Get RGB values (stored in reversed order)
            b += pbPixelsBuff[index];
            g += pbPixelsBuff[index+1];
            r += pbPixelsBuff[index+2];

            count++;
        }
    }
    r = round((double) r / count);
    g = round((double) g / count);
    b = round((double) b / count);

#if 0
    // Save image of screen:
    QImage * im = new QImage( monitorWidth, monitorHeight, QImage::Format_RGB32 );
    for(int i=0; i<monitorWidth; i++){
        for(int j=0; j<monitorHeight; j++){
            index = (BytesPerPixel * j * monitorWidth) + (BytesPerPixel * i);
            QRgb rgb = pbPixels[index+2] << 16 | pbPixels[index+1] << 8 | pbPixels[index];
            im->setPixel(i, j, rgb);
        }
    }
    im->save("screen.jpg");
    delete im;
#endif

    return QColor(r, g, b);
}
