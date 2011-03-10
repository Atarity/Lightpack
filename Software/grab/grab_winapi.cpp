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

#include <windows.h>
#include <cmath>

#include "grab_api.h"

QColor Grab::getColor(const QWidget * grabme)
{    
    BITMAP *bmp = new BITMAP;

    int width =  grabme->width();
    int height = grabme->height();

    HDC hWidgetDC = GetWindowDC( grabme->winId() );

    // Create a bitmap compatible with the screen DC
    HBITMAP hBitmap = CreateCompatibleBitmap(hWidgetDC, width, height);

    // Create a memory DC compatible to screen DC
    HDC hMemDC = CreateCompatibleDC(hWidgetDC);

    // select new bitmap into memory DC
    HGDIOBJ hOldBitmap = SelectObject (hMemDC, hBitmap);

    // BitBlt screen DC to memory DC
    BitBlt(hMemDC, 0, 0, width, height, hWidgetDC, 0, 0, SRCCOPY);

    // Select old bitmap back into memory DC and get handle to
    // bitmap of the screen
    hBitmap = (HBITMAP) SelectObject(hMemDC, hOldBitmap);


    // Now get the actual Bitmap
    GetObject(hBitmap, sizeof(BITMAP), bmp);

    // Calculate the size the buffer needs to be
    DWORD cbBuffer = bmp->bmWidthBytes * bmp->bmHeight;
    // Allocate
    BYTE * pbPixels = new BYTE[cbBuffer];

    // The amount of bytes per pixel is the amount of bits divided by 8
    int BytesPerPixel = bmp->bmBitsPixel / 8;


    // Copy the screen into our Bitmap handle

    // select new bitmap into memory DC
    hOldBitmap = SelectObject (hMemDC, hBitmap);

    // BitBlt screen DC to memory DC
    BitBlt(hMemDC, 0, 0, width, height, hWidgetDC, 0, 0, SRCCOPY);

    // Select old bitmap back into memory DC and get handle to
    // bitmap of the screen
    hBitmap = (HBITMAP) SelectObject(hMemDC, hOldBitmap);



    // Now get the actual Bitmap
    GetObject(hBitmap, sizeof(BITMAP), bmp);

    // Get the actual RGB data and put it into pbPixels
    GetBitmapBits(hBitmap, cbBuffer, pbPixels);


    int index = 0; // Index of the selected pixel in pbPixels
    int r = 0, g = 0, b = 0; // Values to hold the RGB averages
    int count = 0; // count the amount of pixels taken into account


    // This is where all the magic happens: calculate the average RGB
    for (int j = 0; j < height; j++){
        for (int i = 0; i < width; i++){
            // Calculate new index value
            index = (BytesPerPixel * j * width) + (BytesPerPixel * i);

            // Get RGB values (stored in reversed order)
            b += pbPixels[index];
            g += pbPixels[index+1];
            r += pbPixels[index+2];

            count++;
        }
    }

    r = round((double) r / count);
    g = round((double) g / count);
    b = round((double) b / count);


    ReleaseDC(grabme->winId(), hWidgetDC);
    ReleaseDC(grabme->winId(), hMemDC);

    DeleteObject(hBitmap);
    DeleteObject(hOldBitmap);

    DeleteDC(hWidgetDC);
    DeleteDC(hMemDC);

    delete pbPixels;
    delete bmp;

    return QColor(r,g,b);
}
