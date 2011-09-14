#pragma once

#include <QColor>

#include "defs.h"
#include "GrabWidget.hpp"

class IGrabber
{
public:
    virtual const char * getName() = 0;
    virtual void updateGrabScreenFromWidget( QWidget * widget ) = 0;
    virtual QList<QRgb> grabWidgetsColors(QList<GrabWidget *> &widgets) = 0;
};
