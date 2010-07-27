/*
 * rectgetpixel.cpp
 *
 *  Created on: 27.07.2010
 *      Author: brunql
 *
 *      Project: AmbilightUSB
 */

#include <QtGui>

#include "ambilightusb.h"
#include "rectgetpixel.h"


RectGetPixel::RectGetPixel(QWidget *parent)
    : QLabel(parent, Qt::FramelessWindowHint | Qt::Window)
{
    this->resize(PIXELS_X, PIXELS_Y);

    QImage im(PIXELS_X, PIXELS_Y, QImage::Format_ARGB32);

    for(int x=0; x < PIXELS_X; x+=X_STEP){
        for(int y=0; y < PIXELS_Y; y+=Y_STEP){
            im.setPixel(x, y, 0xffff0000 /* Black opaque */);
        }
    }

    QPixmap pix = QPixmap::fromImage(im);
    this->setPixmap(pix);
}

void RectGetPixel::setTransparent(bool transparent)
{
    if(transparent){
        this->setMask(this->pixmap()->mask());
    }else{
        this->clearMask();
    }
}
