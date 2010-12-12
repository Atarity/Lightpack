/*
 * grabdesktopwindowleds.cpp
 *
 *  Created on: 26.07.2010
 *      Author: Mike Shatohin (brunql)
 *     Project: AmbilightUSB
 *
 *  AmbilightUSB is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2010 Mike Shatohin, mikeshatohin [at] gmail.com
 *
 *  AmbilightUSB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  AmbilightUSB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */



#include "grabdesktopwindowleds.h"

const QColor GrabDesktopWindowLeds::labelsColors[LEDS_COUNT] = {
    Qt::red,       /* LED1 */
    Qt::green,     /* LED2 */
    Qt::blue,      /* LED3 */
    Qt::yellow,    /* LED4 */
    Qt::darkRed,   /* LED5 */
    Qt::darkGreen, /* LED6 */
    Qt::darkBlue,  /* LED7 */
    Qt::darkYellow,/* LED8 */
};

GrabDesktopWindowLeds::GrabDesktopWindowLeds(QWidget *parent) : QWidget(parent)
{
    timer = new QTimer(this);
    timeEval = new TimeEvaluations();

    desktop_width = QApplication::desktop()->availableGeometry(QApplication::desktop()->primaryScreen()).width();
    desktop_height = QApplication::desktop()->height(); // need full desktop height to find center of the screen


    // Read settings once
    qDebug() << "GrabDesktopWindowLeds(): read settings";
    this->ambilight_refresh_delay_ms = settings->value("RefreshAmbilightDelayMs").toInt();

    this->ambilight_width = settings->value("WidthAmbilight").toInt();
    this->ambilight_height = settings->value("HeightAmbilight").toInt();

    this->ambilight_white_balance_r = settings->value("WhiteBalanceCoefRed").toDouble();
    this->ambilight_white_balance_g = settings->value("WhiteBalanceCoefGreen").toDouble();
    this->ambilight_white_balance_b = settings->value("WhiteBalanceCoefBlue").toDouble();

    this->ambilight_color_depth = settings->value("HwColorDepth").toInt();

    this->updateColorsOnlyIfChanges = true;


    qDebug() << "GrabDesktopWindowLeds(): createLabelsGrabPixelsRects()";
    createLabelsGrabPixelsRects();

    connect(timer, SIGNAL(timeout()), this, SLOT(updateLedsColorsIfChanged()));

    clearColors();       

    timer->start(ambilight_refresh_delay_ms);
    qDebug() << "GrabDesktopWindowLeds(): initialized";
}

GrabDesktopWindowLeds::~GrabDesktopWindowLeds()
{
    delete timer;
    delete timeEval;

    for(int i=0; i<labelGrabPixelsRects.count(); i++){
        labelGrabPixelsRects[i]->close();
    }

    labelGrabPixelsRects.clear();
}

void GrabDesktopWindowLeds::clearColors()
{
    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        colors[ledIndex]->r = 0;
        colors[ledIndex]->g = 0;
        colors[ledIndex]->b = 0;
    }
}

void GrabDesktopWindowLeds::createLabelsGrabPixelsRects()
{
    labelGrabPixelsRects.clear();

    for(int i=0; i<LEDS_COUNT; i++){
        labelGrabPixelsRects.append(new QLabel(this, Qt::FramelessWindowHint | Qt::SplashScreen));
        labelGrabPixelsRects[i]->setFocusPolicy(Qt::NoFocus);
        labelGrabPixelsRects[i]->setText(QString::number(i+1));
        labelGrabPixelsRects[i]->setAlignment(Qt::AlignCenter);

        // Fill label with labelColors[i] color
        QPalette pal = labelGrabPixelsRects[i]->palette();
        pal.setBrush(labelGrabPixelsRects[i]->backgroundRole(), QBrush(labelsColors[i]));
        labelGrabPixelsRects[i]->setPalette(pal);
    }

    updateSizesLabelsGrabPixelsRects();
}

void GrabDesktopWindowLeds::updateSizesLabelsGrabPixelsRects()
{
    for(int i=0; i<LEDS_COUNT; i++){
        labelGrabPixelsRects[i]->setFixedWidth(ambilight_width);
        labelGrabPixelsRects[i]->setFixedHeight(ambilight_height);
    }

    // TODO: LEFT side 1234, RIGHT side 5678

    // Now, get pixels from monitor:
    //
    // 1    5
    // 2    6
    // 3    7
    // 4    8

    labelGrabPixelsRects[LED1]->move(/* x = */ 0,
                                     /*  y = */ desktop_height / 2 - 2*ambilight_height);
    labelGrabPixelsRects[LED2]->move(/* x = */ 0,
                                     /*  y = */  desktop_height / 2 - ambilight_height);
    labelGrabPixelsRects[LED3]->move(/* x = */ 0,
                                     /*  y = */  desktop_height / 2 );
    labelGrabPixelsRects[LED4]->move(/* x = */ 0,
                                     /*  y = */  desktop_height / 2 + ambilight_height);

    labelGrabPixelsRects[LED5]->move(/* x = */ desktop_width - ambilight_width,
                                     /*  y = */  desktop_height / 2 - 2*ambilight_height);
    labelGrabPixelsRects[LED6]->move(/* x = */ desktop_width - ambilight_width,
                                     /*  y = */  desktop_height / 2 - ambilight_height);
    labelGrabPixelsRects[LED7]->move(/* x = */ desktop_width - ambilight_width,
                                     /*  y = */  desktop_height / 2 );
    labelGrabPixelsRects[LED8]->move(/* x = */ desktop_width - ambilight_width,
                                     /* y = */ desktop_height / 2 + ambilight_height);
}


void GrabDesktopWindowLeds::setVisibleGrabPixelsRects(bool state)
{
    for(int i=0; i<labelGrabPixelsRects.count(); i++){
        if(state){
            labelGrabPixelsRects[i]->show();
        }else{
            labelGrabPixelsRects[i]->hide();
        }
    }
}




void GrabDesktopWindowLeds::updateLedsColorsIfChanged()
{    
    timeEval->howLongItStart();

    bool needToUpdate = false;

    LedColors colorsNew;

    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        QPixmap pix = QPixmap::grabWindow(QApplication::desktop()->winId(),
                                          labelGrabPixelsRects[ledIndex]->x(),
                                          labelGrabPixelsRects[ledIndex]->y(),
                                          labelGrabPixelsRects[ledIndex]->width(),
                                          labelGrabPixelsRects[ledIndex]->height());
        QPixmap scaledPix = pix.scaled(1,1, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QImage im = scaledPix.toImage();

        colorsNew[ledIndex]->r = (im.pixel(0,0) >> 0x10) & 0xff;
        colorsNew[ledIndex]->g = (im.pixel(0,0) >> 0x08) & 0xff;
        colorsNew[ledIndex]->b = (im.pixel(0,0) >> 0x00) & 0xff;
    }


    // Find average for each led color
    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        colorsNew[ledIndex]->r = (int)((double)colorsNew[ledIndex]->r / (256.0 / ambilight_color_depth)); // now pwm_value_max==64
        colorsNew[ledIndex]->g = (int)((double)colorsNew[ledIndex]->g / (256.0 / ambilight_color_depth)); // now pwm_value_max==64
        colorsNew[ledIndex]->b = (int)((double)colorsNew[ledIndex]->b / (256.0 / ambilight_color_depth)); // now pwm_value_max==64

        // hw_v2.*: 4 SMD RGB LEDs
        //  9.6 mA - all off
        // 90.0 mA - all on
        //colorsNew[led_index][color] = ambilight_color_depth;
    }

    // White balance
    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        colorsNew[ledIndex]->r *= ambilight_white_balance_r;
        colorsNew[ledIndex]->g *= ambilight_white_balance_g;
        colorsNew[ledIndex]->b *= ambilight_white_balance_b;
    }

    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        if(colors[ledIndex]->r != colorsNew[ledIndex]->r){
            colors[ledIndex]->r = colorsNew[ledIndex]->r;
            needToUpdate = true;
        }
        if(colors[ledIndex]->g != colorsNew[ledIndex]->g){
            colors[ledIndex]->g = colorsNew[ledIndex]->g;
            needToUpdate = true;
        }
        if(colors[ledIndex]->b != colorsNew[ledIndex]->b){
            colors[ledIndex]->b = colorsNew[ledIndex]->b;
            needToUpdate = true;
        }
    }

    if((!updateColorsOnlyIfChanges) || needToUpdate){
        // if updateColorsOnlyIfChanges == false, then update colors (not depending on needToUpdate flag)
        emit updateLedsColors( colors );
    }
    emit ambilightTimeOfUpdatingColors(timeEval->howLongItEnd());

    if(isAmbilightOn) {
        timer->start(ambilight_refresh_delay_ms);
    }
}

void GrabDesktopWindowLeds::setAmbilightOn(bool state)
{
    if(timer->isActive()){
        timer->stop();
    }
    if(state){        
        timer->start(ambilight_refresh_delay_ms);
    }
    isAmbilightOn = state;
}

void GrabDesktopWindowLeds::setAmbilightWidth(int w)
{
    this->ambilight_width = w;
    updateSizesLabelsGrabPixelsRects();
}

void GrabDesktopWindowLeds::setAmbilightHeight(int h)
{
    this->ambilight_height = h;
    updateSizesLabelsGrabPixelsRects();
}

void GrabDesktopWindowLeds::setAmbilightRefreshDelayMs(int ms)
{
    this->ambilight_refresh_delay_ms = ms;
}

void GrabDesktopWindowLeds::setAmbilightColorDepth(int color_depth)
{
    this->ambilight_color_depth = color_depth;
}

void GrabDesktopWindowLeds::setAmbilightWhiteBalanceR(double r)
{
    this->ambilight_white_balance_r = r;
}

void GrabDesktopWindowLeds::setAmbilightWhiteBalanceG(double g)
{
    this->ambilight_white_balance_g = g;
}

void GrabDesktopWindowLeds::setAmbilightWhiteBalanceB(double b)
{
    this->ambilight_white_balance_b = b;
}

void GrabDesktopWindowLeds::setColoredGrabPixelsRects(bool state)
{
    if(state){
        for(int i=0; i<labelGrabPixelsRects.count(); i++){
            // Fill label with labelColors[i] color
            QPalette pal = labelGrabPixelsRects[i]->palette();
            pal.setBrush(labelGrabPixelsRects[i]->backgroundRole(), QBrush(labelsColors[i]));
            labelGrabPixelsRects[i]->setPalette(pal);
        }
    }
}

void GrabDesktopWindowLeds::setWhiteGrabPixelsRects(bool state)
{
    if(state){
        for(int i=0; i<labelGrabPixelsRects.count(); i++){
            // Fill labels white
            QPalette pal = labelGrabPixelsRects[i]->palette();
            pal.setBrush(labelGrabPixelsRects[i]->backgroundRole(), QBrush(Qt::white));
            labelGrabPixelsRects[i]->setPalette(pal);
        }
    }
}


void GrabDesktopWindowLeds::setUpdateColorsOnlyIfChanges(bool state)
{
    this->updateColorsOnlyIfChanges = state;
}
