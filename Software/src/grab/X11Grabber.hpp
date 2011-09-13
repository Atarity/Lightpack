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

struct X11GrabberData;

class X11Grabber : public IGrabber
{
public:
    X11Grabber();
    ~X11Grabber();
    virtual const char * getName();
    virtual void updateGrabScreenFromWidget( QWidget * widget );
    virtual QList<QRgb> grabWidgetsColors(QList<GrabWidget *> &widgets);
private:
    void captureScreen();
    QRgb getColor(const QWidget * grabme);
    QRgb getColor(int x, int y, int width, int height);

private:
    bool updateScreenAndAllocateMemory;
    int screen;
    QRect screenres;

    X11GrabberData *d;
//    int w, h;
};
#endif // X11_GRAB_SUPPORT
#endif // X11GRABBER_H
