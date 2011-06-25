/*
 * grab_qt.cpp
 *
 *  Created on: 9.03.2011
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Lightpack is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2011 Mike Shatohin, mikeshatohin [at] gmail.com
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

//#include <X11/extensions/Xrender.h>
//#include <X11/extensions/XShm.h>
//#include <X11/extensions/XShm.h>
//#include <sys/ipc.h>
//#include <sys/shm.h>

#include "debug.h"

namespace GrabX11
{
 QRect screenres;
 bool updateScreenAndAllocateMemory = true;
 int screen = 0;
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
            screenres = QApplication::desktop()->screenGeometry(screen);
            updateScreenAndAllocateMemory = false;

            w = screenres.width();
            h= screenres.height();
            DEBUG_LOW_LEVEL << "opendispaly";
            display = XOpenDisplay(0);
           // DEBUG_LOW_LEVEL << "DefaultScreenOfDisplay";
            Xscreen = DefaultScreenOfDisplay(display);
            //DEBUG_LOW_LEVEL << "XShmCreateImage";

            image = XShmCreateImage(display, DefaultVisualOfScreen(Xscreen),
                                    DefaultDepthOfScreen(Xscreen),
                                    ZPixmap, NULL, &shminfo,
                                    w, h );
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
                // runl << "Shared memory allocated" << endl;


                 XShmAttach(display, &shminfo);

            }
             // DEBUG_LOW_LEVEL << "XShmGetImage";
              XShmGetImage(display,
                               RootWindow(display, DefaultScreen(display)),
                               image,
                               screenres.x(),
                               screenres.y(),
                               0x00FFFFFF
                           );

//              DEBUG_LOW_LEVEL << "QImage";
//              QImage *pic = new QImage(w,h,QImage::Format_RGB32);
//               DEBUG_LOW_LEVEL << "format";
//              unsigned long pixel;
//              for (int y = 0; y < 100; y++)
//                  {
//                    for (int x = 0; x < 100; x++)
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
              //if(image == NULL) return 1;

//              Display *display;
//                      int screen;
//                      Window root;
//                      display = XOpenDisplay(0);
//                      screen = DefaultScreen(display);
//                      root = RootWindow(display, screen);
//                      XImage *img = XGetImage(display,root,0,0,400,400,XAllPlanes(),ZPixmap);
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

    int r=0,g=0,b=0;
    for (int y1 = y; y1 < y+height; y1++)
        {
          for (int x1 = x; x1 < x+width; x1++)
          {
            unsigned long pixel = XGetPixel(image, x1, y1);

            r += (pixel >> 16) & 0xff;
            g += (pixel >>  8) & 0xff;
            b += (pixel >>  0) & 0xff;

            //QRgb rgb = QRgb() (r,g,b);
            //pic->setPixel(x,y,pixel);
          }
        }
    r = r / (width*height);
    g = g / (width*height);
    b = b / (width*height);

    QRgb result = qRgb(r,g,b);// im.pixel(0,0);

    DEBUG_HIGH_LEVEL << "QRgb result =" << hex << result;

    return result;
}

};


