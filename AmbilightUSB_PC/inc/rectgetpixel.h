/*
 * rectgetpixel.h
 *
 *  Created on: 27.07.2010
 *      Author: brunql
 *
 *      Project: AmbilightUSB
 */

#ifndef RECTGETPIXEL_H
#define RECTGETPIXEL_H

#include <QtGui>

class RectGetPixel : public QLabel {

public:
    RectGetPixel(QWidget *parent = 0);
    void setTransparent(bool transparent);
};

#endif // RECTGETPIXEL_H
