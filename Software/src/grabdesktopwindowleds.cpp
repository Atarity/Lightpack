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

const QColor GrabDesktopWindowLeds::backgroundAndTextColors[LEDS_COUNT][2] = {
    { Qt::red,        Qt::black }, /* LED1 */
    { Qt::green,      Qt::black }, /* LED2 */
    { Qt::blue,       Qt::white }, /* LED3 */
    { Qt::yellow,     Qt::black }, /* LED4 */
    { Qt::darkRed,    Qt::white }, /* LED5 */
    { Qt::darkGreen,  Qt::white }, /* LED6 */
    { Qt::darkBlue,   Qt::white }, /* LED7 */
    { Qt::darkYellow, Qt::white }, /* LED8 */
};

GrabDesktopWindowLeds::GrabDesktopWindowLeds(QWidget *parent) : QWidget(parent)
{
    timer = new QTimer(this);
    timeEval = new TimeEvaluations();


    // Read settings once
    qDebug() << "GrabDesktopWindowLeds(): read settings";
    this->ambilight_refresh_delay_ms = Settings::value("RefreshAmbilightDelayMs").toInt();
    this->avgColorsOnAllLeds = Settings::value("IsAvgColorsOn").toBool();
    this->minLevelOfSensivity = Settings::value("MinimumLevelOfSensitivity").toInt();

    this->ambilight_color_depth = Settings::value("Firmware/ColorDepth").toInt();

    // TODO: add me to settings
    this->updateColorsOnlyIfChanges = true; // default value

    this->isResizeOrMoving = false;

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
    }

    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        QSize ledWidgetSize = Settings::value("LED_" + QString::number(ledIndex+1) + "/Size").toSize();
        QPoint ledWidgetPosition = Settings::value("LED_" + QString::number(ledIndex+1) + "/Position").toPoint();

        ledWidgets[ledIndex]->resize(ledWidgetSize);
        ledWidgets[ledIndex]->move(ledWidgetPosition);

        // Connect signal resizeCompleted for save new size and position
        connect(ledWidgets[ledIndex], SIGNAL(resizeOrMoveCompleted(int)), this, SLOT(ledWidgetResizeCompleted(int)));
        connect(ledWidgets[ledIndex], SIGNAL(resizeOrMoveCompleted(int)), this, SLOT(setAmbilightON()));
        connect(ledWidgets[ledIndex], SIGNAL(resizeOrMoveStarted()), this, SLOT(setAmbilightOFF()));
    }

    setColoredLedWidgets(true);
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
    if(isResizeOrMoving){
        return;
    }

    timeEval->howLongItStart();

    if(isAmbilightOn) {
        if(timer->isActive() == false){
            timer->start(ambilight_refresh_delay_ms);
        }
    }


    bool needToUpdate = false;

    int r = 0, g = 0, b = 0;

    LedColors colorsNew;

    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        QPixmap pix = QPixmap::grabWindow(QApplication::desktop()->winId(),
                                          ledWidgets[ledIndex]->x(),
                                          ledWidgets[ledIndex]->y(),
                                          ledWidgets[ledIndex]->width(),
                                          ledWidgets[ledIndex]->height());
        QPixmap scaledPix = pix.scaled(1,1, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QImage im = scaledPix.toImage();


        if (avgColorsOnAllLeds){
            r += (im.pixel(0,0) >> 0x10) & 0xff;
            g += (im.pixel(0,0) >> 0x08) & 0xff;
            b += (im.pixel(0,0) >> 0x00) & 0xff;
        }else{
            colorsNew[ledIndex]->r = (im.pixel(0,0) >> 0x10) & 0xff;
            colorsNew[ledIndex]->g = (im.pixel(0,0) >> 0x08) & 0xff;
            colorsNew[ledIndex]->b = (im.pixel(0,0) >> 0x00) & 0xff;
        }
    }


    if(avgColorsOnAllLeds){
        r /= LEDS_COUNT;
        g /= LEDS_COUNT;
        b /= LEDS_COUNT;

        // Set all LEDs one AVG color
        for(int i=0; i<LEDS_COUNT; i++){
            colorsNew[i]->r = (unsigned char)(r & 0xff);
            colorsNew[i]->g = (unsigned char)(g & 0xff);
            colorsNew[i]->b = (unsigned char)(b & 0xff);
        }
    }


    // Find average for each led color
    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        colorsNew[ledIndex]->r = (int)((double)colorsNew[ledIndex]->r / (256.0 / ambilight_color_depth)); // now pwm_value_max==64
        colorsNew[ledIndex]->g = (int)((double)colorsNew[ledIndex]->g / (256.0 / ambilight_color_depth)); // now pwm_value_max==64
        colorsNew[ledIndex]->b = (int)((double)colorsNew[ledIndex]->b / (256.0 / ambilight_color_depth)); // now pwm_value_max==64
    }

    // White balance
    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        colorsNew[ledIndex]->r *= ledWidgets[ledIndex]->getCoefRed();
        colorsNew[ledIndex]->g *= ledWidgets[ledIndex]->getCoefGreen();
        colorsNew[ledIndex]->b *= ledWidgets[ledIndex]->getCoefBlue();
    }

    // Check minimum level of sensivity
    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        int avg = colorsNew[ledIndex]->r + colorsNew[ledIndex]->g + colorsNew[ledIndex]->b;
        avg /= 3;
        if(avg <= minLevelOfSensivity){
            colorsNew[ledIndex]->r = 0;
            colorsNew[ledIndex]->g = 0;
            colorsNew[ledIndex]->b = 0;
        }
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

void GrabDesktopWindowLeds::setAmbilightON()
{
    isResizeOrMoving = false;
}

void GrabDesktopWindowLeds::setAmbilightOFF()
{
    isResizeOrMoving = true;
}

void GrabDesktopWindowLeds::settingsProfileChanged()
{
    this->avgColorsOnAllLeds = Settings::value("IsAvgColorsOn").toBool();
    this->minLevelOfSensivity = Settings::value("MinimumLevelOfSensitivity").toInt();

    for(int id=0; id<ledWidgets.count(); id++){
        ledWidgets[id]->move( Settings::value("LED_" + QString::number(id+1) + "/Position").toPoint() );
        ledWidgets[id]->resize( Settings::value("LED_" + QString::number(id+1) + "/Size").toSize() );
    }
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
            ledWidgets[i]->setBackgroundColor(backgroundAndTextColors[i][0]);
            ledWidgets[i]->setTextColor(backgroundAndTextColors[i][1]);
        }
    }
}

void GrabDesktopWindowLeds::setWhiteLedWidgets(bool state)
{
    if(state){
        for(int i=0; i<ledWidgets.count(); i++){
            // Fill labels white
            ledWidgets[i]->setBackgroundColor(Qt::white);
            ledWidgets[i]->setTextColor(Qt::black);
        }
    }
}


void GrabDesktopWindowLeds::setUpdateColorsOnlyIfChanges(bool state)
{
    this->updateColorsOnlyIfChanges = state;
}


void GrabDesktopWindowLeds::setAvgColorsOnAllLeds(bool state)
{
    this->avgColorsOnAllLeds = state;
    Settings::setValue("IsAvgColorsOn", state);
}

void GrabDesktopWindowLeds::setMinLevelOfSensivity(int value)
{
    this->minLevelOfSensivity = value;
    Settings::setValue("MinimumLevelOfSensitivity", value);
}


void GrabDesktopWindowLeds::ledWidgetResizeCompleted(int id)
{
    // Save size and position
    Settings::setValue("LED_" + QString::number(id+1) + "/Size",      ledWidgets[id]->size());
    Settings::setValue("LED_" + QString::number(id+1) + "/Position",  ledWidgets[id]->pos());

    // TODO: save coefs
//    Settings::setValue("LED_" + QString::number(ledIndex+1) + "/CoefRed",   LED_COEF_RED_DEFAULT_VALUE);
//    Settings::setValue("LED_" + QString::number(ledIndex+1) + "/CoefGreen", LED_COEF_GREEN_DEFAULT_VALUE);
//    Settings::setValue("LED_" + QString::number(ledIndex+1) + "/CoefBlue",  LED_COEF_BLUE);
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

