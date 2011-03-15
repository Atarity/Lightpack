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

#include "grabmanager.h"
#include "grab_api.h"

GrabManager::GrabManager(QWidget *parent) : QWidget(parent)
{
    timer = new QTimer(this);
    timeEval = new TimeEvaluations();    

    // TODO: add me to settings
    this->updateColorsOnlyIfChanges = true; // default value

    this->isResizeOrMoving = false;

    qDebug() << "GrabDesktopWindowLeds(): initColorLists()";
    initColorLists();

    qDebug() << "GrabDesktopWindowLeds(): createLedWidgets()";
    initLedWidgets();

    connect(timer, SIGNAL(timeout()), this, SLOT(updateLedsColorsIfChanged()));
    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(scaleLedWidgets()));

    timer->start(10);
    qDebug() << "GrabDesktopWindowLeds(): initialized";
}

GrabManager::~GrabManager()
{
    delete timer;
    delete timeEval;

    for(int i=0; i<ledWidgets.count(); i++){
        ledWidgets[i]->close();
    }

    ledWidgets.clear();
}

void GrabManager::initColorLists()
{
    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){       
        colors    << StructRGB();
        colorsNew << StructRGB();
    }
}

void GrabManager::clearColors()
{
    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        colors[ledIndex].rgb = 0;
        colors[ledIndex].steps = 0;
        colorsNew[ledIndex].rgb = 0;
        colorsNew[ledIndex].steps = 0;
    }
}


void GrabManager::initLedWidgets()
{
    ledWidgets.clear();

    for(int i=0; i<LEDS_COUNT; i++){
        ledWidgets.append(new MoveMeWidget(i, this));
    }

    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        connect(ledWidgets[ledIndex], SIGNAL(resizeOrMoveCompleted(int)), this, SLOT(setAmbilightON()));
        connect(ledWidgets[ledIndex], SIGNAL(resizeOrMoveStarted()), this, SLOT(setAmbilightOFF()));
    }

    firstWidgetPositionChanged();

    // First LED widget using to determine grabbing-monitor in WinAPI version of Grab
    connect(ledWidgets[0], SIGNAL(resizeOrMoveCompleted(int)), this, SLOT(firstWidgetPositionChanged()));
}

void GrabManager::firstWidgetPositionChanged()
{
    Grab::findScreenOnNextCapture( ledWidgets[0]->winId() );
}


void GrabManager::scaleLedWidgets()
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

        qDebug() << "scaleLedWidgets(): new values [" << i << "]" << "x =" << x << "y =" << y << "w =" << width << "h =" << height;
    }
}


void GrabManager::updateLedsColorsIfChanged()
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

    int avgR = 0, avgG = 0, avgB = 0;
    int countGrabEnabled = 0;

    // Clear colorsNew
    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        colorsNew[ledIndex].rgb = 0;
        colorsNew[ledIndex].steps = 0;
    }


//#define PRINT_TIME_SPENT_ON_GRAB
#ifdef PRINT_TIME_SPENT_ON_GRAB
    QTime t; t.start();
#endif    

    // Capture screen what contains first LED widgets
    Grab::captureScreen();

    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        if(ledWidgets[ledIndex]->isGrabEnabled()){
            QRgb rgb = Grab::getColor( ledWidgets[ledIndex] );

            if( avgColorsOnAllLeds ){
                avgR += qRed(rgb);
                avgG += qGreen(rgb);
                avgB += qBlue(rgb);
                countGrabEnabled++;
            }else{
                colorsNew[ledIndex].rgb = rgb;
            }
        }else{
            colorsNew[ledIndex].rgb = 0; // off led
        }
    }

#ifdef PRINT_TIME_SPENT_ON_GRAB
    qDebug() << "Time spent on grab:" << t.elapsed() << "ms";
#endif

    if( avgColorsOnAllLeds ){
        if( countGrabEnabled != 0 ){
            avgR /= countGrabEnabled;
            avgG /= countGrabEnabled;
            avgB /= countGrabEnabled;
        }
        // Set one AVG color to all LEDs
        for(int ledIndex = 0; ledIndex < LEDS_COUNT; ledIndex++){
            colorsNew[ledIndex].rgb = qRgb(avgR, avgG, avgB);
        }
    }

#if 0
    // 0 <= color <= ambilight_color_depth
    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        colorsNew[ledIndex]->r = (int)((double)colorsNew[ledIndex]->r / (256.0 / ambilight_color_depth));
        colorsNew[ledIndex]->g = (int)((double)colorsNew[ledIndex]->g / (256.0 / ambilight_color_depth));
        colorsNew[ledIndex]->b = (int)((double)colorsNew[ledIndex]->b / (256.0 / ambilight_color_depth));
    }
#endif

    // White balance
    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        QRgb rgb = colorsNew[ledIndex].rgb;

        int r = qRed(rgb)   * ledWidgets[ledIndex]->getCoefRed();
        int g = qGreen(rgb) * ledWidgets[ledIndex]->getCoefGreen();
        int b = qBlue(rgb)  * ledWidgets[ledIndex]->getCoefBlue();

        colorsNew[ledIndex].rgb = qRgb(r, g, b);
    }

    // Check minimum level of sensivity
    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        QRgb rgb = colorsNew[ledIndex].rgb;
        int avg = round( (qRed(rgb) + qGreen(rgb) + qBlue(rgb)) / 3.0 );
        if(avg <= minLevelOfSensivity){
            colorsNew[ledIndex].rgb = 0;
        }
    }


    // Eval smooth steps start

    int maxDiff = 0, diff = 0;

    // First find MAX diff between old and new colors, and save all diffs in each smooth_step
    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        QRgb rgb = colors[ledIndex].rgb;
        QRgb rgbNew = colorsNew[ledIndex].rgb;
        int stepR, stepG, stepB;

        diff = qRed(rgb) - qRed(rgbNew);
        if(diff < 0) diff *= -1;
        if(diff > maxDiff) maxDiff = diff;
        stepR = (diff != 0) ? diff : 1;

        diff = qGreen(rgb) - qGreen(rgbNew);
        if(diff < 0) diff *= -1;
        if(diff > maxDiff) maxDiff = diff;
        stepG = (diff != 0) ? diff : 1;

        diff = qBlue(rgb) - qBlue(rgbNew);
        if(diff < 0) diff *= -1;
        if(diff > maxDiff) maxDiff = diff;
        stepB = (diff != 0) ? diff : 1;

        colorsNew[ledIndex].steps = qRgb(stepR, stepG, stepB);
    }

    // To find smooth_step which will be using max_diff divide on each smooth_step
    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        QRgb steps = colorsNew[ledIndex].steps;
        int stepR, stepG, stepB;
        stepR = maxDiff / qRed(steps);
        stepG = maxDiff / qGreen(steps);
        stepB = maxDiff / qBlue(steps);
        colorsNew[ledIndex].steps = qRgb(stepR, stepG, stepB);
    }
    // Eval smooth steps end



    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        if( colors[ledIndex].rgb != colorsNew[ledIndex].rgb ){
            colors[ledIndex].rgb  = colorsNew[ledIndex].rgb;
            needToUpdate = true;
        }
        colors[ledIndex].steps = colorsNew[ledIndex].steps;
    }

    if((!updateColorsOnlyIfChanges) || needToUpdate){
        // if updateColorsOnlyIfChanges == false, then update colors (not depending on needToUpdate flag)
        emit updateLedsColors( colors );
    }
    emit ambilightTimeOfUpdatingColors( timeEval->howLongItEnd() );
}

void GrabManager::setAmbilightOn(bool state)
{
    if(timer->isActive()){
        timer->stop();
        clearColors();
    }
    if(state){        
        timer->start(ambilight_refresh_delay_ms);
    }
    isAmbilightOn = state;
}

void GrabManager::setAmbilightON()
{
    isResizeOrMoving = false;
}

void GrabManager::setAmbilightOFF()
{
    isResizeOrMoving = true;
}

void GrabManager::settingsProfileChanged()
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


void GrabManager::setAmbilightRefreshDelayMs(int ms)
{
    this->ambilight_refresh_delay_ms = ms;
}

void GrabManager::setAmbilightColorDepth(int color_depth)
{
    this->ambilight_color_depth = color_depth;
}

void GrabManager::setVisibleLedWidgets(bool state)
{
    for(int i=0; i<ledWidgets.count(); i++){
        if(state){
            ledWidgets[i]->show();
        }else{
            ledWidgets[i]->hide();
        }
    }
}

void GrabManager::setColoredLedWidgets(bool state)
{
    if(state){
        for(int i=0; i<ledWidgets.count(); i++){
            // Fill label with labelColors[i] color
            ledWidgets[i]->setColors(i);
        }
    }
}

void GrabManager::setWhiteLedWidgets(bool state)
{
    if(state){
        for(int i=0; i<ledWidgets.count(); i++){
            // Fill labels white
            ledWidgets[i]->setColors(MoveMeWidget::ColorIndexWhite);
        }
    }
}


void GrabManager::setUpdateColorsOnlyIfChanges(bool state)
{
    this->updateColorsOnlyIfChanges = state;
}


void GrabManager::setAvgColorsOnAllLeds(bool state)
{
    this->avgColorsOnAllLeds = state;
    Settings::setValue("IsAvgColorsOn", state);
}

void GrabManager::setMinLevelOfSensivity(int value)
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

