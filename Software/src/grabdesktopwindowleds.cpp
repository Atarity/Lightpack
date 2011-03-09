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
#include "grab_api.h"


// Uncomment this line for see in debug output time spent on grab
//#define PRINT_TIME_SPENT_ON_GRAB


GrabDesktopWindowLeds::GrabDesktopWindowLeds(QWidget *parent) : QWidget(parent)
{
    timer = new QTimer(this);
    timeEval = new TimeEvaluations();    

    Grab::Initialize();

    // TODO: add me to settings
    this->updateColorsOnlyIfChanges = true; // default value

    this->isResizeOrMoving = false;

    qDebug() << "GrabDesktopWindowLeds(): createLedWidgets()";
    createLedWidgets();

    connect(timer, SIGNAL(timeout()), this, SLOT(updateLedsColorsIfChanged()));
    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(scaleLedWidgets()));

    clearColors();

    timer->start(10);
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

    Grab::DeInitialize();
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
        connect(ledWidgets[ledIndex], SIGNAL(resizeOrMoveCompleted(int)), this, SLOT(setAmbilightON()));
        connect(ledWidgets[ledIndex], SIGNAL(resizeOrMoveStarted()), this, SLOT(setAmbilightOFF()));
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

void GrabDesktopWindowLeds::scaleLedWidgets()
{
    double scaleX = (double) Desktop::Width / Desktop::WidthSaved;
    double scaleY = (double) Desktop::Height / Desktop::HeightSaved;

    for(int i=0; i<ledWidgets.count(); i++){
        int width  = round( scaleX * ledWidgets[i]->width() );
        int height = round( scaleY * ledWidgets[i]->height() );
        int x = round( scaleX * ledWidgets[i]->x() );
        int y = round( scaleY * ledWidgets[i]->y() );

        ledWidgets[i]->move(x,y);
        ledWidgets[i]->resize(width, height);

        ledWidgets[i]->saveSizeAndPosition();
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
    int countGrabEnabled = 0;

    LedColors colorsNew;    

#ifdef PRINT_TIME_SPENT_ON_GRAB
    QTime t; t.start();
#endif

    // Copy screen only in WinAPI version of Grab
    Grab::copyScreen();

    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        if(ledWidgets[ledIndex]->isGrabEnabled()){
            QColor color = Grab::getColor( ledWidgets[ledIndex]->x(),
                                           ledWidgets[ledIndex]->y(),
                                           ledWidgets[ledIndex]->width(),
                                           ledWidgets[ledIndex]->height());
            if (avgColorsOnAllLeds){
                countGrabEnabled++;
                r += color.red();
                g += color.green();
                b += color.blue();
            }else{
                colorsNew[ledIndex]->r = color.red();
                colorsNew[ledIndex]->g = color.green();
                colorsNew[ledIndex]->b = color.blue();
            }
        }
    }

#ifdef PRINT_TIME_SPENT_ON_GRAB
    qDebug() << "Time spent on grab:" << t.elapsed() << "ms";
#endif

    if(avgColorsOnAllLeds){
        if(countGrabEnabled == 0) countGrabEnabled = 1;

        r /= countGrabEnabled;
        g /= countGrabEnabled;
        b /= countGrabEnabled;

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

    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        if(ledWidgets[ledIndex]->isGrabEnabled() == false){
            colorsNew[ledIndex]->r = 0x00;
            colorsNew[ledIndex]->g = 0x00;
            colorsNew[ledIndex]->b = 0x00;
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
    qDebug() << "GrabDesktopWindowLeds::settingsProfileChanged()";

    this->avgColorsOnAllLeds = Settings::value("IsAvgColorsOn").toBool();
    this->minLevelOfSensivity = Settings::value("MinimumLevelOfSensitivity").toInt();

    this->ambilight_refresh_delay_ms = Settings::value("RefreshAmbilightDelayMs").toInt();
    this->ambilight_color_depth = Settings::value("Firmware/ColorDepth").toInt();

    for(int i=0; i<ledWidgets.count(); i++){
        ledWidgets[i]->settingsProfileChanged();
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
            ledWidgets[i]->setColors(i);
        }
    }
}

void GrabDesktopWindowLeds::setWhiteLedWidgets(bool state)
{
    if(state){
        for(int i=0; i<ledWidgets.count(); i++){
            // Fill labels white
            ledWidgets[i]->setColors(MoveMeWidget::ColorIndexWhite);
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



//void GrabDesktopWindowLeds::moveMeLabelRightClicked(int id)
//{
//    int index = this->moveMeGroup.indexOf(labelGrabPixelsRects[id]);
//    if(index == -1){
//        this->moveMeGroup.append(labelGrabPixelsRects[id]);
//    }else{
//        this->moveMeGroup.removeAt(index);
//    }
//}

