#ifndef X11GRABBER_H
#define X11GRABBER_H

#include<qglobal.h>
#ifdef Q_WS_X11
#include "IGrabber.hpp"

class X11Grabber : public IGrabber
{
public:
    X11Grabber(){}
    virtual const char * getName() = 0;
    virtual void updateGrabScreenFromWidget( QWidget * widget ) = 0;
    virtual QList<QRgb> grabWidgetsColors(QList<MoveMeWidget *> &widgets) = 0;
private:
    QRgb getColor(const QWidget * grabme);
    QRgb getColor(int x, int y, int width, int height);

private:
    bool updateScreenAndAllocateMemory;
    int screen = 0;
    QRect screenres;
    Display *display;
    Screen *Xscreen;
    XImage *image;
    XShmSegmentInfo shminfo;
    int w, h;
};
#endif

#endif // X11GRABBER_H
