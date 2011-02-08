/*
 * grabdesktopwindowleds.cpp
 *
 *  Created on: 26.07.2010
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Lightpack is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2010, 2011 Mike Shatohin, mikeshatohin [at] gmail.com
 *
 *  Lightpack is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Lightpack is distributed in the hope that it will be useful,
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


    // Read settings once
    qDebug() << "GrabDesktopWindowLeds(): read settings";
    this->ambilight_refresh_delay_ms = settings->value("RefreshAmbilightDelayMs").toInt();

    this->ambilight_color_depth = settings->value("Firmware/ColorDepth").toInt();

    this->updateColorsOnlyIfChanges = true;


    qDebug() << "GrabDesktopWindowLeds(): createLedWidgets()";
    createLedWidgets();

    connect(timer, SIGNAL(timeout()), this, SLOT(updateLedsColorsIfChanged()));

    clearColors();

    timer->start(ambilight_refresh_delay_ms);
    qDebug() << "GrabDesktopWindowLeds(): initialized";
}

GrabDesktopWindowLeds::~GrabDesktopWindowLeds()
{
    delete timer;
    delete timeEval;

    for(int i=0; i<ledWidgets.count(); i++){
        ledWidgets[i]->close();
    }

    ledWidgets.clear();
}

void GrabDesktopWindowLeds::clearColors()
{
    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        colors[ledIndex]->r = 0;
        colors[ledIndex]->g = 0;
        colors[ledIndex]->b = 0;
    }
}

void GrabDesktopWindowLeds::createLedWidgets()
{
    ledWidgets.clear();

    for(int i=0; i<LEDS_COUNT; i++){
        ledWidgets.append(new MoveMeWidget(i, this));

        ledWidgets[i]->setBackgroundColor(labelsColors[i]);
    }

    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        QSize ledWidgetSize = settings->value("LED_" + QString::number(ledIndex+1) + "/Size").toSize();
        QPoint ledWidgetPosition = settings->value("LED_" + QString::number(ledIndex+1) + "/Position").toPoint();

        ledWidgets[ledIndex]->resize(ledWidgetSize);
        ledWidgets[ledIndex]->move(ledWidgetPosition);

        // Connect signal resizeCompleted for save new size and position
        connect(ledWidgets[ledIndex], SIGNAL(resizeCompleted(int)), this, SLOT(ledWidgetResizeCompleted(int)));
    }
}


void GrabDesktopWindowLeds::setVisibleLedWidgets(bool state)
{
    for(int i=0; i<ledWidgets.count(); i++){
        if(state){
            ledWidgets[i]->show();
        }else{
            ledWidgets[i]->hide();
        }
    }
}




void GrabDesktopWindowLeds::updateLedsColorsIfChanged()
{    
    timeEval->howLongItStart();

    if(isAmbilightOn) {
        if(timer->isActive() == false){
            timer->start(ambilight_refresh_delay_ms);
        }
    }


    bool needToUpdate = false;

    LedColors colorsNew;

    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        QPixmap pix = QPixmap::grabWindow(QApplication::desktop()->winId(),
                                          ledWidgets[ledIndex]->x(),
                                          ledWidgets[ledIndex]->y(),
                                          ledWidgets[ledIndex]->width(),
                                          ledWidgets[ledIndex]->height());
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
        colorsNew[ledIndex]->r *= ledWidgets[ledIndex]->getCoefRed();
        colorsNew[ledIndex]->g *= ledWidgets[ledIndex]->getCoefGreen();
        colorsNew[ledIndex]->b *= ledWidgets[ledIndex]->getCoefBlue();
    }


    // Eval smooth steps start

    int maxDiff = 0, diff = 0;

    // First find MAX diff between old and new colors, and save all diffs in each smooth_step
    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        diff = colors[ledIndex]->r - colorsNew[ledIndex]->r;
        if(diff < 0) diff *= -1;
        if(diff > maxDiff) maxDiff = diff;
        colorsNew[ledIndex]->sr = (unsigned char)((diff != 0) ? diff : 1);

        diff = colors[ledIndex]->g - colorsNew[ledIndex]->g;
        if(diff < 0) diff *= -1;
        if(diff > maxDiff) maxDiff = diff;
        colorsNew[ledIndex]->sg = (unsigned char)((diff != 0) ? diff : 1);

        diff = colors[ledIndex]->b - colorsNew[ledIndex]->b;
        if(diff < 0) diff *= -1;
        if(diff > maxDiff) maxDiff = diff;
        colorsNew[ledIndex]->sb = (unsigned char)((diff != 0) ? diff : 1);
    }

    // To find smooth_step which will be using max_diff divide on each smooth_step
    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        colorsNew[ledIndex]->sr = (unsigned char) maxDiff / colorsNew[ledIndex]->sr;
        colorsNew[ledIndex]->sg = (unsigned char) maxDiff / colorsNew[ledIndex]->sg;
        colorsNew[ledIndex]->sb = (unsigned char) maxDiff / colorsNew[ledIndex]->sb;
    }
    // Eval smooth steps end



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

        colors[ledIndex]->sr = colorsNew[ledIndex]->sr;
        colors[ledIndex]->sg = colorsNew[ledIndex]->sg;
        colors[ledIndex]->sb = colorsNew[ledIndex]->sb;
    }


    if((!updateColorsOnlyIfChanges) || needToUpdate){
        // if updateColorsOnlyIfChanges == false, then update colors (not depending on needToUpdate flag)
        emit updateLedsColors( colors );
    }
    emit ambilightTimeOfUpdatingColors(timeEval->howLongItEnd());
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


void GrabDesktopWindowLeds::setAmbilightRefreshDelayMs(int ms)
{
    this->ambilight_refresh_delay_ms = ms;
}

void GrabDesktopWindowLeds::setAmbilightColorDepth(int color_depth)
{
    this->ambilight_color_depth = color_depth;
}

void GrabDesktopWindowLeds::setColoredLedWidgets(bool state)
{
    if(state){
        for(int i=0; i<ledWidgets.count(); i++){
            // Fill label with labelColors[i] color
            ledWidgets[i]->setBackgroundColor(labelsColors[i]);
        }
    }
}

void GrabDesktopWindowLeds::setWhiteLedWidgets(bool state)
{
    if(state){
        for(int i=0; i<ledWidgets.count(); i++){
            // Fill labels white
            ledWidgets[i]->setBackgroundColor(Qt::white);
        }
    }
}


void GrabDesktopWindowLeds::setUpdateColorsOnlyIfChanges(bool state)
{
    this->updateColorsOnlyIfChanges = state;
}


void GrabDesktopWindowLeds::ledWidgetResizeCompleted(int id)
{
    // Save size and position
    settings->setValue("LED_" + QString::number(id+1) + "/Size",      ledWidgets[id]->size());
    settings->setValue("LED_" + QString::number(id+1) + "/Position",  ledWidgets[id]->pos());

    // TODO: save coefs
//    settings->setValue("LED_" + QString::number(ledIndex+1) + "/CoefRed",   LED_COEF_RED_DEFAULT_VALUE);
//    settings->setValue("LED_" + QString::number(ledIndex+1) + "/CoefGreen", LED_COEF_GREEN_DEFAULT_VALUE);
//    settings->setValue("LED_" + QString::number(ledIndex+1) + "/CoefBlue",  LED_COEF_BLUE);
}



//void GrabDesktopWindowLeds::moveMeLabelRightClicked(int id)
//{
//    int index = this->moveMeGroup.indexOf(labelGrabPixelsRects[id]);
//    if(index == -1){
//        this->moveMeGroup.append(labelGrabPixelsRects[id]);
//    }else{
//        this->moveMeGroup.removeAt(index);
//    }
//}

