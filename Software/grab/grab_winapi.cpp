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



HWND hWnd;

HDC hScreenDC;         // The HDC of the screen
HDC hMemDC;            // Temporarily hold HDC
DISPLAY_DEVICE dd;

BYTE* pbPixels;        // Pointer to screen pixels
HBITMAP hBitmap;       // Bitmap handle of screen
HGDIOBJ hOldBitmap;    // Handle to old bitmap
BITMAP bmp;            // Actual bitmap of screen
DWORD cbBuffer;        // Pixels from bmp

int ScreenWidth = 0, ScreenHeight = 0;
int BytesPerPixel;     // The amount of bytes of one pixel

int Grab::step = 1;
void Grab::setStep(int value)
{
    qDebug() << Q_FUNC_INFO;

    if(value > 1){
        step = value;
    }else{
        qWarning() << "bad value 'step' =" << step;
    }
}


void Grab::Initialize()
{
    qDebug() << Q_FUNC_INFO;

    ZeroMemory(&dd, sizeof(dd));
    dd.cb = sizeof(dd);

    int displayIndex = 0;

    // TODO: multimonitor configuration
    int result = EnumDisplayDevices(NULL, displayIndex, &dd, NULL);
    if (result) {
        hScreenDC = CreateDC(NULL,dd.DeviceName,NULL,NULL);

        // Get the screen resolution
        ScreenWidth = GetDeviceCaps(hScreenDC, HORZRES);
        ScreenHeight = GetDeviceCaps(hScreenDC, VERTRES);

        // Create a bitmap compatible with the screen DC
        hBitmap = CreateCompatibleBitmap(hScreenDC, ScreenWidth, ScreenHeight);

        // Create a memory DC compatible to screen DC
        hMemDC = CreateCompatibleDC(hScreenDC);



        // select new bitmap into memory DC
        hOldBitmap = SelectObject (hMemDC, hBitmap);

        // BitBlt screen DC to memory DC
        BitBlt(hMemDC, 0, 0, ScreenWidth, ScreenHeight, hScreenDC, 0, 0, SRCCOPY);

        // Select old bitmap back into memory DC and get handle to
        // bitmap of the screen
        hBitmap = (HBITMAP) SelectObject(hMemDC, hOldBitmap);

        // Now get the actual Bitmap
        GetObject(hBitmap, sizeof(BITMAP), &bmp);

        // Calculate the size the buffer needs to be
        cbBuffer = bmp.bmWidthBytes * bmp.bmHeight;
        // Allocate
        pbPixels = new BYTE[cbBuffer];

        // The amount of bytes per pixel is the amount of bits divided by 8
        BytesPerPixel = bmp.bmBitsPixel / 8;

    }else{
        qWarning() << "EnumDisplayDevices() fail";
    }
}

void Grab::DeInitialize()
{
    qDebug() << Q_FUNC_INFO;

    DeleteDC(hScreenDC);
    DeleteDC(hMemDC);
    if (pbPixels) delete pbPixels;
}


void Grab::copyScreen()
{
    // Copy the screen into our Bitmap handle

    // select new bitmap into memory DC
    hOldBitmap = SelectObject (hMemDC, hBitmap);

    // BitBlt screen DC to memory DC
    BitBlt(hMemDC, 0, 0, ScreenWidth, ScreenHeight, hScreenDC, 0, 0, SRCCOPY);

    // Select old bitmap back into memory DC and get handle to
    // bitmap of the screen
    hBitmap = (HBITMAP) SelectObject(hMemDC, hOldBitmap);


    // Now get the actual Bitmap
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    // Get the actual RGB data and put it into pbPixels
    GetBitmapBits(hBitmap, cbBuffer, pbPixels);
}

QColor Grab::getColor(int x, int y, int width, int height)
{
    if( x < 0 ) x = 0;
    if( y < 0 ) y = 0;
    if( x > ScreenWidth || y > ScreenHeight ){
        qDebug() << "bad values of 'x' or 'y' ( x > ScreenWidth || y > ScreenHeight ); x =" << x << "y =" << y;
        return QColor();
    }

    int index = 0; // Index of the selected pixel in pbPixels

    int r = 0, g = 0, b = 0; // Values to hold the RGB averages
    int count = 0; // count the amount of pixels taken into account

    // This is where all the magic happens: calculate the average RGB
    for (int j = y; j < y + height && j < ScreenHeight; j += step){
        for (int i = x; i < x + width && i < ScreenWidth; i += step){
            // Calculate new index value
            index = (BytesPerPixel * j * ScreenWidth) + (BytesPerPixel * i);

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

    return QColor(r, g, b);
}
