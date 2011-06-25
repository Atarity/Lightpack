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

#include "debug.h"

GrabManager::GrabManager(QWidget *parent) : QWidget(parent)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    timerGrab = new QTimer(this);
    timeEval = new TimeEvaluations();

    fpsMs = 0;
    isGrabWinAPI = true;

    timerUpdateFPS = new QTimer(this);
    connect(timerUpdateFPS, SIGNAL(timeout()), this, SLOT(updateFpsOnMainWindow()));
    timerUpdateFPS->setSingleShot( false );
    timerUpdateFPS->start( 500 );

    // TODO: add me to settings
    this->updateColorsOnlyIfChanges = true; // default value

    this->isResizeOrMoving = false;

    initColorLists();
    initLedWidgets();

    firstWidgetPositionChanged();

    connect(timerGrab, SIGNAL(timeout()), this, SLOT(updateLedsColorsIfChanged()));
    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(scaleLedWidgets(int)));

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "initialized";
}

GrabManager::~GrabManager()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    delete timerGrab;
    delete timeEval;

    for(int i=0; i<ledWidgets.count(); i++){
        ledWidgets[i]->close();
    }

    ledWidgets.clear();
}

void GrabManager::initColorLists()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){       
        colorsCurrent << StructRGB();
        colorsNew     << StructRGB();
    }
}

void GrabManager::clearColorsCurrent()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        colorsCurrent[ledIndex].rgb = 0;
        colorsCurrent[ledIndex].steps = 0;
    }
}

void GrabManager::clearColorsNew()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        colorsNew[ledIndex].rgb = 0;
        colorsNew[ledIndex].steps = 0;
    }
}

void GrabManager::initLedWidgets()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    ledWidgets.clear();

    for(int i=0; i<LEDS_COUNT; i++){
        ledWidgets << new MoveMeWidget(i, this);
    }

    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        connect(ledWidgets[ledIndex], SIGNAL(resizeOrMoveCompleted(int)), this, SLOT(setResizeOrMovingFalse()));
        connect(ledWidgets[ledIndex], SIGNAL(resizeOrMoveStarted()), this, SLOT(setResizeOrMovingTrue()));
    }

    firstWidgetPositionChanged();

    // First LED widget using to determine grabbing-monitor in WinAPI version of Grab
    connect(ledWidgets[0], SIGNAL(resizeOrMoveCompleted(int)), this, SLOT(firstWidgetPositionChanged()));
}

void GrabManager::firstWidgetPositionChanged()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    screenSavedIndex = QApplication::desktop()->screenNumber( ledWidgets[0] );
    screenSaved = QApplication::desktop()->screenGeometry( screenSavedIndex );

    if(isGrabWinAPI){
        #ifdef Q_WS_WIN
            GrabWinAPI::findScreenOnNextCapture( ledWidgets[0]->winId() );
        #endif
        #ifdef Q_WS_X11
            GrabQt::setScreenOnNextCapture(screenSavedIndex);
        #endif

    }
    else
    {
        GrabQt::setScreenOnNextCapture(screenSavedIndex);
    }

}


void GrabManager::scaleLedWidgets(int screenIndexResized)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "screenIndexResized:" << screenIndexResized;

    int screenIndexOfFirstLedWidget = QApplication::desktop()->screenNumber( ledWidgets[0] );

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "LedWidgets[0] index of screen:" << screenIndexOfFirstLedWidget;

    QRect screen = QApplication::desktop()->screenGeometry( screenSavedIndex );

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "LedWidgets[0] screen:" << screen;
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "LedWidgets[0] screenSaved:" << screenSaved;
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "LedWidgets[0] screenSavedIndex:" << screenSavedIndex;

    if(screenIndexResized != screenIndexOfFirstLedWidget){
        return;
    }

    // Move LedWidgets
    int deltaX = screenSaved.x() - screen.x();
    int deltaY = screenSaved.y() - screen.y();

    double scaleX = (double) screen.width() / screenSaved.width();
    double scaleY = (double) screen.height() / screenSaved.height();

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "deltaX =" << deltaX << "deltaY =" << deltaY;
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "scaleX =" << scaleX << "scaleY =" << scaleY;

    screenSaved = screen;
    screenSavedIndex = screenIndexOfFirstLedWidget;

    for(int i=0; i<ledWidgets.count(); i++){

        int width  = round( scaleX * ledWidgets[i]->width() );
        int height = round( scaleY * ledWidgets[i]->height() );

        int x = ledWidgets[i]->x();
        int y = ledWidgets[i]->y();

        x -= screen.x();
        y -= screen.y();

        x = round( scaleX * x );
        y = round( scaleY * y );

        x += screen.x();
        y += screen.y();

        x -= deltaX;
        y -= deltaY;

        ledWidgets[i]->move(x,y);
        ledWidgets[i]->resize(width, height);

        ledWidgets[i]->saveSizeAndPosition();

        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "new values [" << i << "]" << "x =" << x << "y =" << y << "w =" << width << "h =" << height;
    }

    // Update grab buffer if screen resized
    firstWidgetPositionChanged();
}


void GrabManager::updateLedsColorsIfChanged()
{    
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

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
        #ifdef Q_WS_WIN
            GrabWinAPI::captureScreen();
        #endif
        #ifdef Q_WS_X11
            GrabX11::captureScreen();
        #endif
    }
    else
    {
        GrabQt::captureScreen();
    }

    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        if(ledWidgets[ledIndex]->isGrabEnabled()){
            QRgb rgb;
            if(isGrabWinAPI){
                #ifdef Q_WS_WIN
                    rgb = GrabWinAPI::getColor( ledWidgets[ledIndex] );
                #endif
                #ifdef Q_WS_X11
                    rgb = GrabX11::getColor( ledWidgets[ledIndex] );
                #endif
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

        unsigned r = qRed(rgb)   * ledWidgets[ledIndex]->getCoefRed();
        unsigned g = qGreen(rgb) * ledWidgets[ledIndex]->getCoefGreen();
        unsigned b = qBlue(rgb)  * ledWidgets[ledIndex]->getCoefBlue();

        if(r > 0xff) r = 0xff;
        if(g > 0xff) g = 0xff;
        if(b > 0xff) b = 0xff;

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


    //--------------------------------------------------------------------------
    // Gamma correction
    // TODO: move this code to capturemath.cpp after merge
    for (int i = 0; i < LEDS_COUNT; i++)
    {
        QRgb rgb = colorsNew[i].rgb;

        unsigned r = 256.0 * pow( qRed(rgb)   / 256.0, m_gammaCorrection );
        unsigned g = 256.0 * pow( qGreen(rgb) / 256.0, m_gammaCorrection );
        unsigned b = 256.0 * pow( qBlue(rgb)  / 256.0, m_gammaCorrection );

        colorsNew[i].rgb = qRgb(r, g, b);
    }
    //--------------------------------------------------------------------------


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
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

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
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    emit ambilightTimeOfUpdatingColors( fpsMs );
}

void GrabManager::setAmbilightOn(bool isAmbilightOn, bool isErrorState)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << isAmbilightOn << isErrorState;

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

void GrabManager::setResizeOrMovingFalse()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    isResizeOrMoving = false;
}

void GrabManager::setResizeOrMovingTrue()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    isResizeOrMoving = true;
}

void GrabManager::settingsProfileChanged()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    this->avgColorsOnAllLeds = Settings::value("IsAvgColorsOn").toBool();
    this->minLevelOfSensivity = Settings::value("MinimumLevelOfSensitivity").toInt();
    this->m_gammaCorrection = Settings::value("GammaCorrection").toDouble();

    this->ambilightDelayMs = Settings::getGrabSlowdownMs();
    this->colorDepth = Settings::value("Firmware/ColorDepth").toInt();

    for(int i=0; i<ledWidgets.count(); i++){
        ledWidgets[i]->settingsProfileChanged();
    }
}


void GrabManager::switchQtWinApi(bool isWinApi)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << isWinApi;

    this->isGrabWinAPI = isWinApi;
}


void GrabManager::setAmbilightSlowdownMs(int ms)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << ms;

    this->ambilightDelayMs = ms;
    Settings::setGrabSlowdownMs(ms);
}

// TODO: remove unused colorDepth
void GrabManager::setAmbilightColorDepth(int depth)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << depth;

    this->colorDepth = depth;
}

void GrabManager::setVisibleLedWidgets(bool state)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << state;

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
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << state;

    if(state){
        for(int i=0; i<ledWidgets.count(); i++){
            // Fill label with labelColors[i] color
            ledWidgets[i]->setColors(i);
        }
    }
}

void GrabManager::setWhiteLedWidgets(bool state)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << state;

    if(state){
        for(int i=0; i<ledWidgets.count(); i++){
            // Fill labels white
            ledWidgets[i]->setColors(MoveMeWidget::ColorIndexWhite);
        }
    }
}


void GrabManager::setUpdateColorsOnlyIfChanges(bool state)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << state;

    this->updateColorsOnlyIfChanges = state;
}


void GrabManager::setAvgColorsOnAllLeds(bool state)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << state;

    this->avgColorsOnAllLeds = state;
    Settings::setValue("IsAvgColorsOn", state);
}

void GrabManager::setMinLevelOfSensivity(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    this->minLevelOfSensivity = value;
    Settings::setValue("MinimumLevelOfSensitivity", value);
}

void GrabManager::setGrabGammaCorrection(double value)
{
    m_gammaCorrection = value;
    Settings::setValue("GammaCorrection", value);
}



