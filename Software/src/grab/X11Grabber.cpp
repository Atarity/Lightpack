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

X11Grabber::X11Grabber(QObject *parent, QList<QRgb> *grabResult, QList<GrabWidget *> *grabAreasGeometry)
    : TimeredGrabber(parent, grabResult, grabAreasGeometry)
{
    this->updateScreenAndAllocateMemory = true;
    this->screen = 0;
    d = new X11GrabberData();
    d->image = NULL;
    d->display = XOpenDisplay(NULL);
}

X11Grabber::~X11Grabber()
{
    XCloseDisplay(d->display);
    delete d;
}

const char * X11Grabber::getName()
{
    return "X11Grabber";
}
void X11Grabber::updateGrabMonitor(QWidget *widget)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;
//    updateScreenAndAllocateMemory = true;
//    screen = QApplication::desktop()->screenNumber( widget );
}

QList<QRgb> X11Grabber::grabWidgetsColors(QList<GrabWidget *> &widgets)
{
    captureScreen();
    QList<QRgb> result;
    for(int i = 0; i < widgets.size(); i++) {
        result.append(getColor(widgets[i]));
    }
    return result;
}

GrabResult X11Grabber::_grab()
{
    captureScreen();
    m_grabResult->clear();
    foreach(GrabWidget * widget, *m_grabWidgets) {
        m_grabResult->append( widget->isAreaEnabled() ? getColor(widget) : qRgb(0,0,0) );
    }
    return GrabResultOk;
}

void X11Grabber::captureScreen()
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    if( updateScreenAndAllocateMemory ){
        //screenres = QApplication::desktop()->screenGeometry(screen);
        updateScreenAndAllocateMemory = false;

        // todo test and fix dual monitor configuration
        d->Xscreen = DefaultScreenOfDisplay(d->display);

        long width=DisplayWidth(d->display, screen);
        long height=DisplayHeight(d->display, screen);
	
	DEBUG_HIGH_LEVEL << "dimensions " << width << "x" << height << screen;
        screenres = QRect(0,0,width,height);

        if (d->image != NULL) {
            XShmDetach(d->display, &d->shminfo);
            XDestroyImage(d->image);
            shmdt (d->shminfo.shmaddr);
            shmctl(d->shminfo.shmid, IPC_RMID, 0);
        }
        d->image = XShmCreateImage(d->display,   DefaultVisualOfScreen(d->Xscreen),
                                        DefaultDepthOfScreen(d->Xscreen),
                                        ZPixmap, NULL, &d->shminfo,
                                        screenres.width(), screenres.height() );
        uint imagesize;
        imagesize = d->image->bytes_per_line * d->image->height;
        d->shminfo.shmid = shmget(    IPC_PRIVATE,
                                   imagesize,
                                   IPC_CREAT|0777
                                   );

        char* mem = (char*)shmat(d->shminfo.shmid, 0, 0);
        d->shminfo.shmaddr = mem;
        d->image->data = mem;
        d->shminfo.readOnly = False;

        XShmAttach(d->display, &d->shminfo);
    }
    // DEBUG_LOW_LEVEL << "XShmGetImage";
    XShmGetImage(d->display,
                 RootWindow(d->display, screen),
                 d->image,
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

    unsigned char *pbPixelsBuff;
    int bytesPerPixel = d->image->bits_per_pixel / 8;
    pbPixelsBuff = (unsigned char *)d->image->data;
    int count = 0; // count the amount of pixels taken into account
    for(int j = 0; j < height; j++) {
        int index = d->image->bytes_per_line * (y+j) + x * bytesPerPixel;
        for(int i = 0; i < width; i+=4) {
            b += pbPixelsBuff[index]   + pbPixelsBuff[index + 4] + pbPixelsBuff[index + 8 ] + pbPixelsBuff[index + 12];
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
#endif // X11_GRAB_SUPPORT
