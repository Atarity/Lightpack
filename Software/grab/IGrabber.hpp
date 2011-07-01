#ifndef IGRABBER_HPP
#define IGRABBER_HPP

#include <movemewidget.h>
#include <QColor>
#include "struct_rgb.h"

class IGrabber : public QObject
{
    Q_OBJECT
public:
    IGrabber(QObject * parent) : QObject(parent) { }
    virtual const char * getName() = 0;
    virtual void updateGrabScreenFromWidget( QWidget * widget ) = 0;
    virtual void grabWidgets(QList<MoveMeWidget *> &widgets, QList<StructRGB> &colors, int widgets_count) = 0;
//    virtual void captureScreen() = 0;
//    virtual QRgb getColor(const QWidget * grabme) = 0;
//    virtual QRgb getColor(int x, int y, int width, int height) = 0;
//    virtual void releaseScreenBuffer() = 0;

};

#endif // IGRABBER_HPP
