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
BYTE * pbPixelsBuff = NULL;
unsigned pixelsBuffSize;
unsigned bytesPerPixel;

HWND hWndForFindMonitor = NULL;
bool updateScreenAndAllocateMemory = true;

//
// Save winId for find screen/monitor what will using for full screen capture
//
void Grab::findScreenOnNextCapture( WId winId )
{
    qDebug() << Q_FUNC_INFO;

    // Save HWND of widget for find monitor
    hWndForFindMonitor = winId;

    // Next time captureScreen will allocate mem for pbPixelsBuff and update pixelsBuffSize, bytesPerPixel    
    updateScreenAndAllocateMemory = true;
}


//
// Capture screen what contains firstLedWidget to pbPixelsBuff
//
void Grab::captureScreen()
{    
    if( updateScreenAndAllocateMemory ){
        // Find the monitor, what contains firstLedWidget
        HMONITOR hMonitor = MonitorFromWindow( hWndForFindMonitor, MONITOR_DEFAULTTONEAREST );

        ZeroMemory( &monitorInfo, sizeof(MONITORINFO) );
        monitorInfo.cbSize = sizeof(MONITORINFO);

        // Get position and resolution of the monitor
        GetMonitorInfo( hMonitor, &monitorInfo );

        screenWidth  = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
        screenHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
    }

    // TODO: test GetDesktopWindow()/GetWindowDC() for speed,
    // and if it fast - remove CreateDC/DeleteDC to GetDesktopWindow()/GetWindowDC()/ReleaseDC

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

    if( updateScreenAndAllocateMemory ){

        qDebug() << "Allocate memory for pbPixelsBuff and update pixelsBuffSize, bytesPerPixel";

        BITMAP * bmp = new BITMAP;

        // Now get the actual Bitmap
        GetObject( hBitmap, sizeof(BITMAP), bmp );

        // Calculate the size the buffer needs to be
        unsigned pixelsBuffSizeNew = bmp->bmWidthBytes * bmp->bmHeight;

        if(pixelsBuffSize != pixelsBuffSizeNew){
            pixelsBuffSize = pixelsBuffSizeNew;

            // ReAllocate memory for new buffer size
            if( pbPixelsBuff ) delete pbPixelsBuff;

            // Allocate
            pbPixelsBuff = new BYTE[ pixelsBuffSize ];
        }

        // The amount of bytes per pixel is the amount of bits divided by 8
        bytesPerPixel = bmp->bmBitsPixel / 8;

        if( bytesPerPixel != 4 ){
            qDebug() << "Not 32-bit mode is not supported!" << bytesPerPixel;
        }

        DeleteObject( bmp );

        updateScreenAndAllocateMemory = false;
    }

    // Get the actual RGB data and put it into pbPixelsBuff
    GetBitmapBits( hBitmap, pixelsBuffSize, pbPixelsBuff );

    // CleanUp
    DeleteObject( hBitmap );
    DeleteObject( hOldBitmap );

    DeleteDC( hScreenDC );
    DeleteDC( hMemDC );   
}


//
// Get AVG color of the rect set by 'grabme' widget from captured screen buffer pbPixelsBuff
//
QRgb Grab::getColor(const QWidget * grabme)
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

    if( count != 0 ){
        r = (unsigned)round((double) r / count) & 0xff;
        g = (unsigned)round((double) g / count) & 0xff;
        b = (unsigned)round((double) b / count) & 0xff;
    }

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

    return qRgb(r, g, b);
}
