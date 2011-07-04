#ifndef IGRABBER_HPP
#define IGRABBER_HPP

#include <movemewidget.h>
#include <QColor>
#include "struct_rgb.h"

class IGrabber
{
public:
    IGrabber(){}
    virtual const char * getName() = 0;
    virtual void updateGrabScreenFromWidget( QWidget * widget ) = 0;
    virtual QList<QRgb> grabWidgetsColors(QList<MoveMeWidget *> &widgets) = 0;

};

#endif // IGRABBER_HPP
