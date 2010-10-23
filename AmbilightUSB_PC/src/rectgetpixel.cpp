/*
 * rectgetpixel.cpp
 *
 *  Created on: 27.07.2010
 *      Author: brunql
 *
 *      Project: AmbilightUSB
 */

#include "rectgetpixel.h"


RectGetPixel::RectGetPixel(QWidget *parent)
    : QGraphicsView(parent, Qt::FramelessWindowHint | Qt::Window)
{
    this->setScene(new QGraphicsScene(this));

    QPalette pal = this->palette();
    pal.setBrush(this->backgroundRole(), QBrush(Qt::black, Qt::SolidPattern));
    this->setPalette(pal);

    QIcon settingsIcon(":/icons/settings.png");
    this->setWindowIcon(settingsIcon);

    settingsChangedUpdateImage();
}

void RectGetPixel::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

void RectGetPixel::readSettings()
{
//    step_x = settings->value("StepX").toInt();
//    step_y = settings->value("StepY").toInt();
    ambilight_width = settings->value("WidthAmbilight").toInt();
    ambilight_height = settings->value("HeightAmbilight").toInt();
}

void RectGetPixel::setTransparent(bool transparent)
{
    if(transparent){
        this->setMask(this->pixmap()->mask());
    }else{
        this->clearMask();
    }
}

void RectGetPixel::settingsChangedUpdateImage()
{
    readSettings();

    int desktop_width = QApplication::desktop()->width();
    int desktop_height = QApplication::desktop()->height();
    this->resize(desktop_width, desktop_height);


//    QImage im(desktop_width, desktop_height, QImage::Format_ARGB32);
//    im
//
//    for(int x=0; x < ambilight_width; x+=step_x){
//        for(int y=0; y < ambilight_height; y+=step_y){
//            // LEFT_UP - red
//            drawBigPoint(im, x, (desktop_height / 2) - y, 0xffff0000);
//            // LEFT_DOWN - green
//            drawBigPoint(im, x, (desktop_height / 2) + y, 0xff00ff00);
//
//            // RIGHT_UP - blue
//            drawBigPoint(im, desktop_width-1 - x, (desktop_height / 2) - y, 0xff0000ff);
//            // RIGHT_DOWN - white
//            drawBigPoint(im, desktop_width-1 - x, (desktop_height / 2) + y, 0xffffffff);
//        }
//    }
//
//    QPixmap pix = QPixmap::fromImage(im);
//    this->setPixmap(pix);
}


void RectGetPixel::drawBigPoint(QImage &im, int x, int y, int color)
{
    if(x <= im.width()-1  && y <= im.height()-1){
        im.setPixel(x, y, color);
    }
    if(x+1 <= im.width()-1  && y <= im.height()-1){
        im.setPixel(x+1, y, color);
    }
    if(x <= im.width()-1  && y+1 <= im.height()-1){
        im.setPixel(x, y+1, color);
    }
    if(x+1 <= im.width()-1  && y+1 <= im.height()-1){
        im.setPixel(x+1, y+1, color);
    }
}


