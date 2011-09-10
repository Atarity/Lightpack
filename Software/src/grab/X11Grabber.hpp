#ifndef X11GRABBER_H
#define X11GRABBER_H

#include "IGrabber.hpp"

#ifdef X11_GRAB_SUPPORT

#include <QApplication>
#include <QDesktopWidget>
#include <QPixmap>
#include <QImage>

#include <qtextstream.h>

#include "debug.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

// x shared-mem extension
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

class X11Grabber : public IGrabber
{
public:
    X11Grabber();
    ~X11Grabber();
    virtual const char * getName();
    virtual void updateGrabScreenFromWidget( QWidget * widget );
    virtual QList<QRgb> grabWidgetsColors(QList<MoveMeWidget *> &widgets);
private:
    void captureScreen();
    QRgb getColor(const QWidget * grabme);
    QRgb getColor(int x, int y, int width, int height);

private:
    bool updateScreenAndAllocateMemory;
    int screen;
    QRect screenres;
    Display *display;
    Screen *Xscreen;
    XImage *image;
    XShmSegmentInfo shminfo;
//    int w, h;
};
#endif // X11_GRAB_SUPPORT
#endif // X11GRABBER_H
