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
#include <errno.h>
#include <inttypes.h>

struct X11GrabberData
{
    X11GrabberData()
        : image(NULL)
    {
        memset(&shminfo, 0, sizeof(shminfo));
    }

    XImage *image;
    XShmSegmentInfo shminfo;
};

X11Grabber::X11Grabber(QObject *parent, GrabberContext * context)
    : TimeredGrabber(parent, context)
    , _updateScreenAndAllocateMemory(true)
    , _screen(0)
{
    _display = XOpenDisplay(NULL);
}

X11Grabber::~X11Grabber()
{
    XCloseDisplay(_display);
}

QList<ScreenInfo> * X11Grabber::screensToGrab(QList<ScreenInfo> *result, const QList<GrabWidget *> &grabWidgets)
{
    result->clear();

    for (int i = 0; i < ScreenCount(_display); ++i) {
        XWindowAttributes xwa;
        XGetWindowAttributes(_display, RootWindow(_display, i), &xwa);
        ScreenInfo screen;
        intptr_t handle = i;
        screen.handle = reinterpret_cast<void *>(handle);
        screen.rect = QRect(xwa.x, xwa.y, xwa.width, xwa.height);
        for (int k = 0; k < grabWidgets.size(); ++k) {
            if (screen.rect.intersects(grabWidgets[k]->rect())) {
                result->append(screen);
                break;
            }
        }
    }

    return result;
}

bool X11Grabber::reallocate(const QList<ScreenInfo> &screens)
{
    for (int i = 0; i < _screens.size(); ++i) {
        X11GrabberData *d = reinterpret_cast<X11GrabberData *>(_screens[i].associatedData);
        XShmDetach(_display, &d->shminfo);
        XDestroyImage(d->image);
        shmdt (d->shminfo.shmaddr);
        shmctl(d->shminfo.shmid, IPC_RMID, 0);
        delete d;
        d = NULL;
    }

    _screens.clear();

    for (int i = 0; i < screens.size(); ++i) {

        long width = screens[i].rect.width();
        long height = screens[i].rect.height();

        DEBUG_HIGH_LEVEL << "dimensions " << width << "x" << height << screens[i].handle;

        X11GrabberData *d = new X11GrabberData();

        int screenid = reinterpret_cast<intptr_t>(screens[i].handle);

        Screen * xscreen = ScreenOfDisplay(_display, screenid);

        d->image = XShmCreateImage(_display, DefaultVisualOfScreen(xscreen),
                                   DefaultDepthOfScreen(xscreen),
                                   ZPixmap, NULL, &d->shminfo,
                                   width, height );
        uint imagesize;
        imagesize = _data->image->bytes_per_line * _data->image->height;

                                      imagesize,
                                      IPC_CREAT|0777
                                      );
        if (d->shminfo.shmid == -1) {
            qCritical() << Q_FUNC_INFO << " error occured while trying to get shared memory: " << strerror(errno);
        }

        char* mem = (char*)shmat(_data->shminfo.shmid, 0, 0);
        _data->shminfo.shmaddr = mem;
        _data->image->data = mem;
        _data->shminfo.readOnly = False;

        XShmAttach(_display, &d->shminfo);

        GrabbedScreen grabScreen;
        grabScreen.imgData = (unsigned char *)mem;
        grabScreen.imgFormat = BufferFormatArgb;
        grabScreen.screenInfo = screens[i];
        grabScreen.associatedData = d;
        _screens.append(grabScreen);
    }


    return true;
}

GrabResult X11Grabber::grabScreens()
{
    for (int i = 0; i < _screens.size(); ++i) {
        XShmGetImage(_display,
                     RootWindow(_display, reinterpret_cast<intptr_t>(_screens[i].screenInfo.handle)),
                     reinterpret_cast<X11GrabberData *>(_screens[i].associatedData)->image,
                     0,
                     0,
                     0x00FFFFFF
                     );
    }
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
    /*
    _context->grabResult->clear();
    foreach(GrabWidget * widget, *_context->grabWidgets) {
        _context->grabResult->append( widget->isAreaEnabled() ? getColor(widget) : qRgb(0,0,0) );
    }
    return GrabResultOk;
    */
    return GrabResultOk;
}

/*
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
        width  += x;  // reduce width
        x = 0;
    }
    if( y < 0 ) {
        height += y;  // reduce height
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
*/
#endif // X11_GRAB_SUPPORT
