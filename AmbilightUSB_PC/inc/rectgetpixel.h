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
#include "settings.h"
#include "ambilightusb.h"


class RectGetPixel : public QGraphicsView {

public:
    RectGetPixel(QWidget *parent = 0);
    void setTransparent(bool transparent);
    void settingsChangedUpdateImage();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void readSettings();
    void drawBigPoint(QImage &im, int x, int y, int color);

    int step_x;
    int step_y;
    int ambilight_width;
    int ambilight_height;
};

#endif // RECTGETPIXEL_H
