/*
 * grabmanager.cpp
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
    timerGrab = new QTimer(this);
    timeEval = new TimeEvaluations();

    fpsMs = 0;
    isGrabWinAPI = true;

    timerUpdateFPS = new QTimer(this);
    connect(timerUpdateFPS, SIGNAL(timeout()), this, SLOT(updateFpsOnMainWindow()));
    timerUpdateFPS->setSingleShot( false );
    timerUpdateFPS->start( 1000 );

    // TODO: add me to settings
    this->updateColorsOnlyIfChanges = true; // default value

    this->isResizeOrMoving = false;

    qDebug() << "GrabManager(): initColorLists()";
    initColorLists();

    qDebug() << "GrabManager(): createLedWidgets()";
    initLedWidgets();

    connect(timerGrab, SIGNAL(timeout()), this, SLOT(updateLedsColorsIfChanged()));
    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(scaleLedWidgets()));

    qDebug() << "GrabManager(): initialized";
}

GrabManager::~GrabManager()
{
    delete timerGrab;
    delete timeEval;

    for(int i=0; i<ledWidgets.count(); i++){
        ledWidgets[i]->close();
    }

    ledWidgets.clear();
}

void GrabManager::initColorLists()
{
    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){       
        colorsCurrent << StructRGB();
        colorsNew     << StructRGB();
    }
}

void GrabManager::clearColorsCurrent()
{
    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        colorsCurrent[ledIndex].rgb = 0;
        colorsCurrent[ledIndex].steps = 0;
    }
}

void GrabManager::clearColorsNew()
{
    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        colorsNew[ledIndex].rgb = 0;
        colorsNew[ledIndex].steps = 0;
    }
}

void GrabManager::initLedWidgets()
{
    ledWidgets.clear();

    for(int i=0; i<LEDS_COUNT; i++){
        ledWidgets << new MoveMeWidget(i, this);
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
    if(isGrabWinAPI){
        GrabWinAPI::findScreenOnNextCapture( ledWidgets[0]->winId() );
    }
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
    // Temporary switch off updating colors
    // if one of LED widgets resizing or moving
    if(isResizeOrMoving){
        timerGrab->start(50); // check in 50 ms
        return;
    }

    bool needToUpdate = false;

    int avgR = 0, avgG = 0, avgB = 0;
    int countGrabEnabled = 0;

    clearColorsNew();


//#define PRINT_TIME_SPENT_ON_GRAB
#ifdef PRINT_TIME_SPENT_ON_GRAB
    QTime t; t.start();
#endif    

    // Capture screen what contains first LED widgets
    if(isGrabWinAPI){
        GrabWinAPI::captureScreen();
    }

    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        if(ledWidgets[ledIndex]->isGrabEnabled()){
            QRgb rgb;
            if(isGrabWinAPI){
                rgb = GrabWinAPI::getColor( ledWidgets[ledIndex] );
            } else {
                rgb = GrabQt::getColor( ledWidgets[ledIndex] );
            }

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
            if(ledWidgets[ledIndex]->isGrabEnabled()){
                colorsNew[ledIndex].rgb = qRgb(avgR, avgG, avgB);
            }
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

    updateSmoothSteps();

    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        if( colorsCurrent[ledIndex].rgb != colorsNew[ledIndex].rgb ){
            colorsCurrent[ledIndex].rgb  = colorsNew[ledIndex].rgb;
            needToUpdate = true;
        }
        colorsCurrent[ledIndex].steps = colorsNew[ledIndex].steps;
    }

    if((updateColorsOnlyIfChanges == false) || needToUpdate){
        // if updateColorsOnlyIfChanges == false, then update colors (not depending on needToUpdate flag)
        emit updateLedsColors( colorsCurrent );
    }

    fpsMs = timeEval->howLongItEnd();
    timeEval->howLongItStart();

    if(isAmbilightOn){
        timerGrab->start( ambilightDelayMs );
    }
}

//
// Update steps for smooth change colors
// Using for linear interpolation from 'colors' to 'colorsNew'
//
void GrabManager::updateSmoothSteps()
{
    double maxDiff = 0, diff = 0;

    // First find MAX diff between old and new colors, and save all diffs in each smooth_step
    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        QRgb rgbCurrent = colorsCurrent[ledIndex].rgb;
        QRgb rgbNew = colorsNew[ledIndex].rgb;
        int stepR, stepG, stepB;

        diff = qRed(rgbCurrent) - qRed(rgbNew);
        if(diff < 0) diff *= -1;
        if(diff > maxDiff) maxDiff = diff;
        stepR = (diff != 0) ? diff : 1;

        diff = qGreen(rgbCurrent) - qGreen(rgbNew);
        if(diff < 0) diff *= -1;
        if(diff > maxDiff) maxDiff = diff;
        stepG = (diff != 0) ? diff : 1;

        diff = qBlue(rgbCurrent) - qBlue(rgbNew);
        if(diff < 0) diff *= -1;
        if(diff > maxDiff) maxDiff = diff;
        stepB = (diff != 0) ? diff : 1;

        colorsNew[ledIndex].steps = qRgb(stepR, stepG, stepB);
    }

    // To find smooth_step which will be using max_diff divide on each smooth_step
    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        QRgb steps = colorsNew[ledIndex].steps;
        int stepR, stepG, stepB;
        stepR = round( maxDiff / qRed(steps)   );
        stepG = round( maxDiff / qGreen(steps) );
        stepB = round( maxDiff / qBlue(steps)  );
        if(stepR == 0) stepR = 1;
        if(stepG == 0) stepG = 1;
        if(stepB == 0) stepB = 1;
        colorsNew[ledIndex].steps = qRgb(stepR, stepG, stepB);
    }
}

// Send each second new grabbing time in ms to main window
void GrabManager::updateFpsOnMainWindow()
{
    emit ambilightTimeOfUpdatingColors( fpsMs );
}

void GrabManager::setAmbilightOn(bool isAmbilightOn, bool isErrorState)
{
    this->isAmbilightOn = isAmbilightOn;

    clearColorsNew();

    if( isAmbilightOn ){
        // Restart ambilight timer
        timerGrab->start( 0 );
    }else{
        // Switch ambilight off
        timerGrab->stop();
        updateSmoothSteps();
        clearColorsCurrent();

        if(isErrorState == false){
            emit updateLedsColors( colorsNew );
        }
    }
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
    qDebug() << "GrabManager::settingsProfileChanged()";

    this->avgColorsOnAllLeds = Settings::value("IsAvgColorsOn").toBool();
    this->minLevelOfSensivity = Settings::value("MinimumLevelOfSensitivity").toInt();

    this->ambilightDelayMs = Settings::value("GrabSlowdownMs").toInt();
    this->colorDepth = Settings::value("Firmware/ColorDepth").toInt();

    for(int i=0; i<ledWidgets.count(); i++){
        ledWidgets[i]->settingsProfileChanged();
    }
}


void GrabManager::switchQtWinApi(bool isWinApi)
{
    this->isGrabWinAPI = isWinApi;
}


void GrabManager::setAmbilightSlowdownMs(int ms)
{
    this->ambilightDelayMs = ms;
    Settings::setValue("GrabSlowdownMs", ms);
}

void GrabManager::setAmbilightColorDepth(int depth)
{
    this->colorDepth = depth;
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



//void GrabManager::moveMeLabelRightClicked(int id)
//{
//    int index = this->moveMeGroup.indexOf(labelGrabPixelsRects[id]);
//    if(index == -1){
//        this->moveMeGroup.append(labelGrabPixelsRects[id]);
//    }else{
//        this->moveMeGroup.removeAt(index);
//    }
//}

