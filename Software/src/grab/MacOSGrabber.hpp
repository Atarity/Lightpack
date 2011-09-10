#ifndef MACOSGRABBER_HPP
#define MACOSGRABBER_HPP

#include"defs.h"
#ifdef MAC_OS_GRAB_SUPPORT
#include"IGrabber.hpp"
#include"ApplicationServices/ApplicationServices.h"
class MacOSGrabber : public IGrabber
{
public:
    MacOSGrabber();
    ~MacOSGrabber();
    virtual void updateGrabScreenFromWidget(QWidget *widget);
    virtual QList<QRgb>grabWidgetsColors(QList<MoveMeWidget *> &widgets);
    virtual const char * getName();
    CGImageRef openGlGrab();
private:
    QRgb getColor(QPixmap pixmap, const QWidget * grabme);
    QRgb getColor(QPixmap pixmap, int x, int y, int width, int height);
};
#endif
#endif // MACOSGRABBER_HPP
