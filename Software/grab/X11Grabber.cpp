/*
 * X11Grabber.cpp
 *
 *  Created on: 25.06.11
 *     Project: Lightpack
 *
 *  Copyright (c) 2011 Andrey Isupov, Timur Sattarov, Mike Shatohin
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

#include "X11Grabber.hpp"

#ifdef X11_GRAB_SUPPORT

#include <X11/Xutil.h>
// x shared-mem extension
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include <cmath>
#include <sys/ipc.h>

struct X11GrabberData
{
    Display *display;
    Screen *Xscreen;
    XImage *image;
    XShmSegmentInfo shminfo;
};

X11Grabber::X11Grabber(QObject *parent, GrabberContext * context)
    : TimeredGrabber(parent, context)
    , _updateScreenAndAllocateMemory(true)
    , _screen(0)
{
    _data.reset(new X11GrabberData());
    _data->image = NULL;
    _data->display = XOpenDisplay(NULL);
}

X11Grabber::~X11Grabber()
{
    if (_data->image)
        XDestroyImage(_data->image);
    XCloseDisplay(_data->display);
}

GrabResult X11Grabber::_grab(QList<QRgb> &grabResult, const QList<GrabWidget *> &grabWidgets)
{
    captureScreen();
    grabResult.clear();
    foreach(GrabWidget * widget, grabWidgets) {
        grabResult.append( widget->isAreaEnabled() ? getColor(widget) : qRgb(0,0,0) );
    }
    return GrabResultOk;
}

void X11Grabber::captureScreen()
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    if( _updateScreenAndAllocateMemory ){
        //screenres = QApplication::desktop()->screenGeometry(screen);
        _updateScreenAndAllocateMemory = false;

        //TODO: test and fix dual monitor configuration
        _data->Xscreen = DefaultScreenOfDisplay(_data->display);

        long width=DisplayWidth(_data->display, _screen);
        long height=DisplayHeight(_data->display, _screen);

        DEBUG_HIGH_LEVEL << "dimensions " << width << "x" << height << _screen;
        _screenres = QRect(0,0,width,height);

        if (_data->image != NULL) {
            XShmDetach(_data->display, &_data->shminfo);
            XDestroyImage(_data->image);
            shmdt (_data->shminfo.shmaddr);
            shmctl(_data->shminfo.shmid, IPC_RMID, 0);
        }
        _data->image = XShmCreateImage(_data->display,   DefaultVisualOfScreen(_data->Xscreen),
                                        DefaultDepthOfScreen(_data->Xscreen),
                                        ZPixmap, NULL, &_data->shminfo,
                                        _screenres.width(), _screenres.height() );
        uint imagesize;
        imagesize = _data->image->bytes_per_line * _data->image->height;
        _data->shminfo.shmid = shmget(    IPC_PRIVATE,
                                   imagesize,
                                   IPC_CREAT|0777
                                   );

        char* mem = (char*)shmat(_data->shminfo.shmid, 0, 0);
        _data->shminfo.shmaddr = mem;
        _data->image->data = mem;
        _data->shminfo.readOnly = False;

        XShmAttach(_data->display, &_data->shminfo);
    }
    // DEBUG_LOW_LEVEL << "XShmGetImage";
    XShmGetImage(_data->display,
                 RootWindow(_data->display, _screen),
                 _data->image,
                 0,
                 0,
                 0x00FFFFFF
                 );
#if 0
    DEBUG_LOW_LEVEL << "QImage";
    QImage *pic = new QImage(1024,768,QImage::Format_RGB32);
    DEBUG_LOW_LEVEL << "format";
    unsigned long pixel;
    for (int y = 0; y < 768; y++)
    {
        for (int x = 0; x < 1024; x++)
        {
            pixel = XGetPixel(image, x, y);
            int r = (pixel >> 16) & 0xff;
            int g = (pixel >>  8) & 0xff;
            int b = (pixel >>  0) & 0xff;
            //QRgb rgb = QRgb() (r,g,b);
            pic->setPixel(x,y,pixel);
        }
    }
    DEBUG_LOW_LEVEL << "save";
    pic->save("/home/atarity/.Lightpack/test.bmp");
#endif
}

QRgb X11Grabber::getColor(const QWidget * grabme)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    return getColor(grabme->x(),
                    grabme->y(),
                    grabme->width(),
                    grabme->height());
}

QRgb X11Grabber::getColor(int x, int y, int width, int height)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO
            << "x y w h:" << x << y << width << height;

    // Checking for the 'grabme' widget position inside the monitor that is used to capture color
    if( x + width  < _screenres.left()  ||
        x               > _screenres.right()  ||
        y + height < _screenres.top()    ||
        y               > _screenres.bottom() ){

        DEBUG_MID_LEVEL << "Widget 'grabme' is out of screen, x y w h:" << x << y << width << height;

        // Widget 'grabme' is out of screen
        return 0x000000;
    }

    // Convert coordinates from "Main" desktop coord-system to capture-monitor coord-system
    x -= _screenres.left() ;
    y -= _screenres.top();

    // Ignore part of LED widget which out of screen
    if( x < 0 ) {
        width  += x;  /* reduce width  */
        x = 0;
    }
    if( y < 0 ) {
        height += y;  /* reduce height */
        y = 0;
    }
    if( x + width  > (int)_screenres.width()  ) width  -= (x + width ) - _screenres.width();
    if( y + height > (int)_screenres.height() ) height -= (y + height) - _screenres.height();

    //calculate aligned width (align by 4 pixels)
    width = width - (width % 4);

    if(width < 0 || height < 0){
        qWarning() << Q_FUNC_INFO << "width < 0 || height < 0:" << width << height;

        // width and height can't be negative
        return 0x000000;
    }

    register unsigned r=0,g=0,b=0;

    unsigned char *pbPixelsBuff;
    int bytesPerPixel = _data->image->bits_per_pixel / 8;
    pbPixelsBuff = (unsigned char *)_data->image->data;
    int count = 0; // count the amount of pixels taken into account
    for(int j = 0; j < height; j++) {
        int index = _data->image->bytes_per_line * (y+j) + x * bytesPerPixel;
        for(int i = 0; i < width; i+=4) {
            b += pbPixelsBuff[index]   + pbPixelsBuff[index + 4] + pbPixelsBuff[index + 8 ] + pbPixelsBuff[index + 12];
            g += pbPixelsBuff[index+1] + pbPixelsBuff[index + 5] + pbPixelsBuff[index + 9 ] + pbPixelsBuff[index + 13];
            r += pbPixelsBuff[index+2] + pbPixelsBuff[index + 6] + pbPixelsBuff[index + 10] + pbPixelsBuff[index + 14];
            count += 4;
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
#endif // X11_GRAB_SUPPORT
