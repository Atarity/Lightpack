#ifndef X11GRABBER_H
#define X11GRABBER_H

#include<qglobal.h>
#define X11_SUPPORT
#ifdef X11_SUPPORT
#include "IGrabber.hpp"
#include "debug.h"

class X11Grabber : public IGrabber
{
public:
    X11Grabber();
    virtual const char * getName() = 0;
    virtual void updateGrabScreenFromWidget( QWidget * widget ) = 0;
    virtual QList<QRgb> grabWidgetsColors(QList<MoveMeWidget *> &widgets) = 0;
private:
    char * getImage();
    QRgb getColor(const QWidget * grabme);
    QRgb getColor(int x, int y, int width, int height);

private:
    bool updateScreenAndAllocateMemory;
    int screen;
//    QRect screenres;
//    Display *display;
//    Screen *Xscreen;
    XImage *image;
    XShmSegmentInfo shminfo;
//    int w, h;
};
#endif

#endif // X11GRABBER_H
