/*
 * grab_x11.cpp
 *
 *  Created on: 29.06.2011
 *      Author: Andrei Isupov (Eraser)
 *     Project: Lightpack
 *
 *  Lightpack is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2011 Andrei Isupov , eraser1981 [at] googlemail.com
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

#include <QApplication>
#include <QDesktopWidget>
#include <QPixmap>
#include <QImage>

#include "grab_api.h"

#include <qtextstream.h>

//need libxv-dev package

// X Server
 #include <X11/Xlib.h>
 #include <X11/Xutil.h>

// X shared-mem extension
 #include <sys/shm.h>
 #include <X11/extensions/XShm.h>

#include "debug.h"

#include <cmath>

namespace GrabX11
{
 bool updateScreenAndAllocateMemory = true;
 int screen = 0;
 QRect screenres;
 Display *display;
 Screen *Xscreen;
 XImage *image;
 XShmSegmentInfo shminfo;
 int w, h;

  void setScreenOnNextCapture( int screenId )
 {
             DEBUG_HIGH_LEVEL << Q_FUNC_INFO;
            updateScreenAndAllocateMemory = true;
            screen = screenId;
 }

 void captureScreen()
 {
           DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

            if( updateScreenAndAllocateMemory ){
            //screenres = QApplication::desktop()->screenGeometry(screen);
            updateScreenAndAllocateMemory = false;

            display = XOpenDisplay(NULL);
            // todo test and fix dual monitor configuration
            Xscreen = DefaultScreenOfDisplay(display);

            long width=DisplayWidth(display,screen);
            long height=DisplayHeight(display,screen);

            screenres = QRect(0,0,width,height);

            image = XShmCreateImage(display,   DefaultVisualOfScreen(Xscreen),
                                    DefaultDepthOfScreen(Xscreen),
                                    ZPixmap, NULL, &shminfo,
                                    screenres.width(), screenres.height() );
            uint imagesize;
            imagesize = image->bytes_per_line * image->height;
            shminfo.shmid = shmget(    IPC_PRIVATE,
                                 imagesize,
                                    IPC_CREAT|0777
                                        );

            char* mem = (char*)shmat(shminfo.shmid, 0, 0);
                 shminfo.shmaddr = mem;
                 image->data = mem;
                 shminfo.readOnly = False;

                 XShmAttach(display, &shminfo);
            }
             // DEBUG_LOW_LEVEL << "XShmGetImage";
              XShmGetImage(display,
                               RootWindow(display, screen),
                               image,
                               0,
                               0,
                               0x00FFFFFF
                           );

//              DEBUG_LOW_LEVEL << "QImage";
//              QImage *pic = new QImage(w,h,QImage::Format_RGB32);
//               DEBUG_LOW_LEVEL << "format";
//              unsigned long pixel;
//              for (int y = 0; y < h; y++)
//                  {
//                    for (int x = 0; x < w; x++)
//                    {
//                      pixel = XGetPixel(image, x, y);
//                      int r = (pixel >> 16) & 0xff;
//                      int g = (pixel >>  8) & 0xff;
//                      int b = (pixel >>  0) & 0xff;
//                      //QRgb rgb = QRgb() (r,g,b);
//                      pic->setPixel(x,y,pixel);
//                    }
//                  }
//               DEBUG_LOW_LEVEL << "save";
//              pic->save("/home/eraser/test.bmp");

}


QRgb getColor(const QWidget * grabme)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    return getColor(grabme->x(),
                    grabme->y(),
                    grabme->width(),
                    grabme->height());
}

QRgb getColor(int x, int y, int width, int height)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO
            << "x y w h:" << x << y << width << height;

    // Checking for the 'grabme' widget position inside the monitor that is used to capture color
    if( x + width  < screenres.left()  ||
        x               > screenres.right()  ||
        y + height < screenres.top()    ||
        y               > screenres.bottom() ){

        DEBUG_MID_LEVEL << "Widget 'grabme' is out of screen, x y w h:" << x << y << width << height;

        // Widget 'grabme' is out of screen
        return 0x000000;
    }

    // Convert coordinates from "Main" desktop coord-system to capture-monitor coord-system
    x -= screenres.left() ;
    y -= screenres.top();

    // Ignore part of LED widget which out of screen
    if( x < 0 ) {
        width  += x;  /* reduce width  */
        x = 0;
    }
    if( y < 0 ) {
        height += y;  /* reduce height */
        y = 0;
    }
    if( x + width  > (int)screenres.width()  ) width  -= (x + width ) - screenres.width();
    if( y + height > (int)screenres.height() ) height -= (y + height) - screenres.height();

    //calculate aligned width (align by 4 pixels)
    width = width - (width % 4);

    if(width < 0 || height < 0){
        qWarning() << Q_FUNC_INFO << "width < 0 || height < 0:" << width << height;

        // width and height can't be negative
        return 0x000000;
    }

    register unsigned r=0,g=0,b=0;
// old metod
//    for (int y1 = y; y1 < y+height; y1++)
//        {
//          for (int x1 = x; x1 < x+width; x1++)
//          {
//            unsigned long pixel = XGetPixel(image, x1, y1);
//            r += (pixel >> 16) & 0xff;
//            g += (pixel >>  8) & 0xff;
//            b += (pixel >>  0) & 0xff;
//          }
//        }
//    r = r / (width*height);
//    g = g / (width*height);
//    b = b / (width*height);

    unsigned char *pbPixelsBuff;
    int bytesPerPixel = image->bits_per_pixel / 8;
    pbPixelsBuff = (unsigned char *)image->data;
    int count = 0; // count the amount of pixels taken into account
    for(int j = 0; j < height; j++) {
        int index = image->bytes_per_line * (y+j) + x * bytesPerPixel;
        for(int i = 0; i < width; i+=4) {
            b += pbPixelsBuff[index] + pbPixelsBuff[index + 4] + pbPixelsBuff[index + 8 ] + pbPixelsBuff[index + 12];
            g += pbPixelsBuff[index+1] + pbPixelsBuff[index + 5] + pbPixelsBuff[index + 9 ] + pbPixelsBuff[index + 13];
            r += pbPixelsBuff[index+2] + pbPixelsBuff[index + 6] + pbPixelsBuff[index + 10] + pbPixelsBuff[index + 14];
            count+=4;
            index += bytesPerPixel * 4;
        }

    }

    if( count != 0 ){
        r = (unsigned)round((double) r / count) & 0xff;
        g = (unsigned)round((double) g / count) & 0xff;
        b = (unsigned)round((double) b / count) & 0xff;
    }

    QRgb result = qRgb(r,g,b);// im.pixel(0,0);

    DEBUG_HIGH_LEVEL << "QRgb result =" << hex << result;

    return result;
}

}


