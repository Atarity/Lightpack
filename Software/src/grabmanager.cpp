/*
 * GrabManager.cpp
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

#include "GrabManager.hpp"
#include <QtCore/qmath.h>
#include "debug.h"

using namespace SettingsScope;

GrabManager::GrabManager(IGrabber *grabber, QWidget *parent) : QWidget(parent)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    m_timerGrab = new QTimer(this);
    m_timeEval = new TimeEvaluations();

    m_fpsMs = 0;

    m_isMoodLampLiquidMode = Settings::isMoodLampLiquidMode();
    m_moodLampSpeed = Settings::getMoodLampSpeed();
    m_backlightColor = Settings::getMoodLampColor();

    m_grabber = grabber;

    m_timerUpdateFPS = new QTimer(this);
    connect(m_timerUpdateFPS, SIGNAL(timeout()), this, SLOT(updateFpsOnMainWindow()));
    m_timerUpdateFPS->setSingleShot( false );
    m_timerUpdateFPS->start( 500 );

    // TODO: add me to settings
    m_updateColorsOnlyIfChanges = true; // default value

    m_isResizeOrMoving = false;

    m_isGrabWidgetsVisible = false;

    initColorLists(MaximumNumberOfLeds::Default);
    initLedWidgets(MaximumNumberOfLeds::Default);

    connect(m_timerGrab, SIGNAL(timeout()), this, SLOT(updateLedsColorsIfChanged()));
    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(scaleLedWidgets(int)));

    firstWidgetPositionChanged();

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "initialized";
}

GrabManager::~GrabManager()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    delete m_timerGrab;
    delete m_timeEval;
    delete m_grabber;

    for (int i = 0; i < m_ledWidgets.count(); i++)
    {
        m_ledWidgets[i]->close();
    }
    m_ledWidgets.clear();
}

void GrabManager::initColorLists(int numberOfLeds)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << numberOfLeds;

    m_colorsCurrent.clear();
    m_colorsNew.clear();

    for (int i = 0; i < numberOfLeds; i++)
    {
        m_colorsCurrent << 0;
        m_colorsNew     << 0;
    }
}

void GrabManager::clearColorsCurrent()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    for (int i = 0; i < m_colorsCurrent.count(); i++)
    {
        m_colorsCurrent[i] = 0;
    }
}

void GrabManager::clearColorsNew()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    for (int i = 0; i < m_colorsNew.count(); i++)
    {
        m_colorsNew[i] = 0;
    }
}

void GrabManager::initLedWidgets(int numberOfLeds)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << numberOfLeds;

    if (m_ledWidgets.count() == 0)
    {
        DEBUG_LOW_LEVEL << "First widget initialization";

        GrabWidget * ledWidget = new GrabWidget(m_ledWidgets.count(), this);

        // First LED widget using to determine grabbing-monitor in WinAPI version of Grab
        connect(ledWidget, SIGNAL(resizeOrMoveCompleted(int)), this, SLOT(firstWidgetPositionChanged()));

        m_ledWidgets << ledWidget;

        // TODO: Check out this line!
        // firstWidgetPositionChanged();
    }

    int diff = numberOfLeds - m_ledWidgets.count();

    if (diff > 0)
    {
        DEBUG_LOW_LEVEL << "Append" << diff << "grab widgets";

        for (int i = 0; i < diff; i++)
        {
            GrabWidget * ledWidget = new GrabWidget(m_ledWidgets.count(), this);

            connect(ledWidget, SIGNAL(resizeOrMoveCompleted(int)), this, SLOT(setResizeOrMovingFalse()));
            connect(ledWidget, SIGNAL(resizeOrMoveStarted()), this, SLOT(setResizeOrMovingTrue()));

            m_ledWidgets << ledWidget;
        }
    } else {
        diff *= -1;
        DEBUG_LOW_LEVEL << "Remove last" << diff << "grab widgets";

        while (diff --> 0)
        {
            m_ledWidgets.last()->deleteLater();
            m_ledWidgets.removeLast();
        }
    }

    if (m_ledWidgets.count() != numberOfLeds)
        qCritical() << "Fail: m_ledWidgets.count()" << m_ledWidgets.count() << " != numberOfLeds" << numberOfLeds;
}

void GrabManager::firstWidgetPositionChanged()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    m_screenSavedIndex = QApplication::desktop()->screenNumber( m_ledWidgets[0] );
    m_screenSavedRect = QApplication::desktop()->screenGeometry( m_screenSavedIndex );

    m_grabber->updateGrabScreenFromWidget(m_ledWidgets[0]);
}


void GrabManager::scaleLedWidgets(int screenIndexResized)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "screenIndexResized:" << screenIndexResized;

    int screenIndexOfFirstLedWidget = QApplication::desktop()->screenNumber( m_ledWidgets[0] );

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "LedWidgets[0] index of screen:" << screenIndexOfFirstLedWidget;

    QRect screen = QApplication::desktop()->screenGeometry( m_screenSavedIndex );

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "LedWidgets[0] screen:" << screen;
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "LedWidgets[0] screenSaved:" << m_screenSavedRect;
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "LedWidgets[0] screenSavedIndex:" << m_screenSavedIndex;
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "LedWidgets[0] screenIndexOfFirstLedWidget:" << screenIndexOfFirstLedWidget;

    if(screenIndexResized != -1 && screenIndexOfFirstLedWidget != -1 && screenIndexResized != screenIndexOfFirstLedWidget ) {
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "not interesting monitor has been resized";
        return;
    }

    // Move LedWidgets
    int deltaX = m_screenSavedRect.x() - screen.x();
    int deltaY = m_screenSavedRect.y() - screen.y();

    double scaleX = (double) screen.width() / m_screenSavedRect.width();
    double scaleY = (double) screen.height() / m_screenSavedRect.height();

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "deltaX =" << deltaX << "deltaY =" << deltaY;
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "scaleX =" << scaleX << "scaleY =" << scaleY;

    m_screenSavedRect = screen;
    m_screenSavedIndex = screenIndexOfFirstLedWidget;

    for(int i=0; i<m_ledWidgets.count(); i++){

        int width  = round( scaleX * m_ledWidgets[i]->width() );
        int height = round( scaleY * m_ledWidgets[i]->height() );

        int x = m_ledWidgets[i]->x();
        int y = m_ledWidgets[i]->y();

        x -= screen.x();
        y -= screen.y();

        x = round( scaleX * x );
        y = round( scaleY * y );

        x += screen.x();
        y += screen.y();

        x -= deltaX;
        y -= deltaY;

        m_ledWidgets[i]->move(x,y);
        m_ledWidgets[i]->resize(width, height);

        m_ledWidgets[i]->saveSizeAndPosition();

        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "new values [" << i << "]" << "x =" << x << "y =" << y << "w =" << width << "h =" << height;
    }

    // Update grab buffer if screen resized
    firstWidgetPositionChanged();
}

////**** FOR MOOD LAMP ****************************************************

// Facepalm...

int newRed=0;
int newGreen=0;
int newBlue=0;
int Red=0;
int Green=0;
int Blue=0;
int speed=1000;
QColor prevColor=Qt::black;
int checkInd = 0;
int GrabManager::m_checkColors[GrabManager::ColorsMoodLampCount];
// Colors for Moodlamp
const QColor GrabManager::m_colorsMoodLamp[GrabManager::ColorsMoodLampCount] = {
    Qt::white, Qt::black ,
    Qt::red, qRgb(255,128,0) , Qt::yellow, Qt::green, qRgb(128,255,255), Qt::blue, qRgb(128,0,255), //rainbow
    Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkYellow,
    Qt::magenta, Qt::cyan,
};
/////////////////////////

void GrabManager::updateLedsColorsIfChanged()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << "m_lightpackMode =" << m_lightpackMode;

    int timer = m_grabSlowdown;
    switch (m_lightpackMode)
    {
    case Lightpack::AmbilightMode:
        ambilight();
        break;
    case Lightpack::MoodLampMode:
        moodlamp();
        timer = speed;
        break;
    }

    if(m_isGrabOn){
        m_timerGrab->start( timer );
    }
}

int random(int val)
{
    return qrand()%val;
}

int GrabManager::genNewSpeed(int value)
{
    //speed = 1000 /  ( random(m_SpeedMoodLamp)+1);
    return  speed = 1000 /  ( value+random(25)+1);
}

QColor GrabManager::genNewColor()
{
    QColor newColor;
#if 0
    // not repeat previous color
    do {
        newColor = colorsMoodLamp[random( ColorsMoodLampCount)];
    } while (newColor == prevColor);
     prevColor = newColor;
#endif

     // once per cycle
     bool fl=false;
     do {
         int ind = random( ColorsMoodLampCount);
         newColor =  m_colorsMoodLamp[ind];
         fl=false;
         if (checkInd<ColorsMoodLampCount)
         {
             for (int i=0;i<checkInd;i++)
                 if (m_checkColors[i]==ind)
                     fl=true;
             if (!fl)
             {
                 m_checkColors[checkInd]=ind;
                 checkInd++;
             }
         }
         else
         {
             checkInd=0;
             m_checkColors[checkInd]=ind;
         }
     } while (fl);
     return newColor;
}

void GrabManager::moodlamp()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << m_moodLampSpeed;

    if (m_isMoodLampLiquidMode)
    {

        if ((Red==newRed) && (Green==newGreen) && (Blue==newBlue))
        {
            speed = genNewSpeed(m_moodLampSpeed);
            QColor newColor = genNewColor();
            newRed = newColor.red();
            newGreen = newColor.green();
            newBlue = newColor.blue();
            DEBUG_HIGH_LEVEL << Q_FUNC_INFO << newColor;
        }

        if(newRed!=Red) { if (Red>newRed) --Red; else ++Red;}
        if(newGreen!=Green)  {if (Green>newGreen) --Green; else ++Green;}
        if(newBlue!=Blue)  {if (Blue>newBlue) --Blue; else ++Blue;}

        for (int i = 0; i < m_ledWidgets.count(); i++)
        {
            if(m_ledWidgets[i]->isGrabEnabled())
                m_colorsCurrent[i] = qRgb(Red, Green, Blue);
            else
                m_colorsCurrent[i] = 0; // off led
        }
    }
    else
    {
        for (int i = 0; i < m_ledWidgets.count(); i++)
        {
            if(m_ledWidgets[i]->isGrabEnabled())
                m_colorsCurrent[i] = qRgb(m_backlightColor.red(),m_backlightColor.green(),m_backlightColor.blue());
            else
                m_colorsCurrent[i] = 0; // off led
        }
    }
    emit updateLedsColors( m_colorsCurrent );
}


void GrabManager::ambilight()
{    
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    // Temporary switch off updating colors
    // if one of LED widgets resizing or moving
    if(m_isResizeOrMoving){
        m_timerGrab->start(50); // check in 50 ms
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

    QList<QRgb> widgetsColors = m_grabber->grabWidgetsColors(m_ledWidgets);

    for (int i = 0; i < m_ledWidgets.count(); i++)
    {
        if (m_ledWidgets[i]->isGrabEnabled())
        {
            QRgb rgb = widgetsColors[i];

            if (m_avgColorsOnAllLeds)
            {
                avgR += qRed(rgb);
                avgG += qGreen(rgb);
                avgB += qBlue(rgb);
                countGrabEnabled++;
            } else {
                m_colorsNew[i] = rgb;
            }
        }else{
            m_colorsNew[i] = 0; // off led
        }
    }

#ifdef PRINT_TIME_SPENT_ON_GRAB
    qDebug() << "Time spent on grab:" << t.elapsed() << "ms";
#endif

    if( m_avgColorsOnAllLeds ){
        if( countGrabEnabled != 0 ){
            avgR /= countGrabEnabled;
            avgG /= countGrabEnabled;
            avgB /= countGrabEnabled;
        }
        // Set one AVG color to all LEDs
        for(int ledIndex = 0; ledIndex < m_ledWidgets.count(); ledIndex++){
            if(m_ledWidgets[ledIndex]->isGrabEnabled()){
                m_colorsNew[ledIndex] = qRgb(avgR, avgG, avgB);
            }
        }
    }

    // White balance
    for(int ledIndex=0; ledIndex < m_ledWidgets.count(); ledIndex++){
        QRgb rgb = m_colorsNew[ledIndex];

        unsigned r = qRed(rgb)   * m_ledWidgets[ledIndex]->getCoefRed();
        unsigned g = qGreen(rgb) * m_ledWidgets[ledIndex]->getCoefGreen();
        unsigned b = qBlue(rgb)  * m_ledWidgets[ledIndex]->getCoefBlue();

        if(r > 0xff) r = 0xff;
        if(g > 0xff) g = 0xff;
        if(b > 0xff) b = 0xff;

        m_colorsNew[ledIndex] = qRgb(r, g, b);
    }

    // Check minimum level of sensivity
    for(int ledIndex=0; ledIndex < m_ledWidgets.count(); ledIndex++){
        QRgb rgb = m_colorsNew[ledIndex];
        int avg = round( (qRed(rgb) + qGreen(rgb) + qBlue(rgb)) / 3.0 );
        if(avg <= m_minLevelOfSensivity){
            m_colorsNew[ledIndex] = 0;
        }
    }

    for(int ledIndex=0; ledIndex < m_ledWidgets.count(); ledIndex++){
        if( m_colorsCurrent[ledIndex] != m_colorsNew[ledIndex] ){
            m_colorsCurrent[ledIndex]  = m_colorsNew[ledIndex];
            needToUpdate = true;
        }
    }

    if((m_updateColorsOnlyIfChanges == false) || needToUpdate){
        // if updateColorsOnlyIfChanges == false, then update colors (not depending on needToUpdate flag)
        emit updateLedsColors( m_colorsCurrent );
    }

    m_fpsMs = m_timeEval->howLongItEnd();
    m_timeEval->howLongItStart();

}

// Send each second new grabbing time in ms to main window
void GrabManager::updateFpsOnMainWindow()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    emit ambilightTimeOfUpdatingColors( m_fpsMs );
}

void GrabManager::setMoodLampLiquidMode(bool state)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << state;
    m_isMoodLampLiquidMode = state;
}

void GrabManager::setNumberOfLeds(int numberOfLeds)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << numberOfLeds;

    initColorLists(numberOfLeds);
    initLedWidgets(numberOfLeds);

    for (int i = 0; i < m_ledWidgets.count(); i++)
    {
        m_ledWidgets[i]->settingsProfileChanged();
        m_ledWidgets[i]->setVisible(m_isGrabWidgetsVisible);
    }
}

void GrabManager::updateBacklightState(Backlight::Status backlightStatus, Api::DeviceLockStatus deviceLockStatus)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << backlightStatus << deviceLockStatus;

    m_isGrabOn = (backlightStatus == Backlight::StatusOn) && (deviceLockStatus == Api::DeviceUnlocked);

    clearColorsNew();

    if (m_isGrabOn)
    {
        // Start grabbing colors and sending signals
        m_timerGrab->start( 0 );
    } else {
        // Backlight is off or in an error state and we need to turn off the grab and stop sending signals
        m_timerGrab->stop();

        if (backlightStatus == Backlight::StatusOff)
        {
            // Send signal to switch off leds if led device isn't in error state
            clearColorsCurrent();
            if (backlightStatus != Backlight::StatusDeviceError)
            {
                emit updateLedsColors(m_colorsCurrent);
            }
        }
    }
}

void GrabManager::setResizeOrMovingFalse()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    m_isResizeOrMoving = false;
}

void GrabManager::setResizeOrMovingTrue()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    m_isResizeOrMoving = true;
}

void GrabManager::settingsProfileChanged()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    m_avgColorsOnAllLeds = Settings::isGrabAvgColorsOn();
    m_minLevelOfSensivity = Settings::getGrabMinimumLevelOfSensitivity();
    m_grabSlowdown = Settings::getGrabSlowdown();
    m_lightpackMode = Settings::getLightpackMode();

    m_isMoodLampLiquidMode = Settings::isMoodLampLiquidMode();
    m_moodLampSpeed = Settings::getMoodLampSpeed();
    m_backlightColor = Settings::getMoodLampColor();

    for(int i=0; i<m_ledWidgets.count(); i++){
        m_ledWidgets[i]->settingsProfileChanged();
    }
}


void GrabManager::setGrabber(IGrabber * newGrabber)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    if (m_grabber)
        delete m_grabber;
    m_grabber = newGrabber;
    firstWidgetPositionChanged();
}

void GrabManager::setMoodLampSpeed(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;
    m_moodLampSpeed = value;
    speed = genNewSpeed(value);
}

void GrabManager::setBackLightColor(QColor color)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << color;
    m_backlightColor = color;
    moodlamp();
    Settings::setMoodLampColor(color);
}

 void GrabManager::switchMode(Lightpack::Mode mode)
 {
     DEBUG_LOW_LEVEL << Q_FUNC_INFO << mode;

     m_lightpackMode = mode;
 }

void GrabManager::setGrabSlowdownMs(int ms)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << ms;

    m_grabSlowdown = ms;
    Settings::setGrabSlowdown(ms);
}

void GrabManager::setVisibleLedWidgets(bool state)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << state;

    m_isGrabWidgetsVisible = state;

    for (int i = 0; i < m_ledWidgets.count(); i++)
    {
        if (state)
        {
            m_ledWidgets[i]->show();
        } else {
            m_ledWidgets[i]->hide();
        }
    }
}

void GrabManager::setColoredLedWidgets(bool state)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << state;

    if(state){
        for(int i=0; i<m_ledWidgets.count(); i++){
            // Fill label with labelColors[i] color
            m_ledWidgets[i]->setColors(i);
        }
    }
}

void GrabManager::setWhiteLedWidgets(bool state)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << state;

    if(state){
        for(int i=0; i<m_ledWidgets.count(); i++){
            // Fill labels white
            m_ledWidgets[i]->setColors(GrabWidget::ColorIndexWhite);
        }
    }
}


void GrabManager::setUpdateColorsOnlyIfChanges(bool state)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << state;

    m_updateColorsOnlyIfChanges = state;
}


void GrabManager::setAvgColorsOnAllLeds(bool state)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << state;

    m_avgColorsOnAllLeds = state;
    Settings::setAvgColorsOn(state);
}

void GrabManager::setMinLevelOfSensivity(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;

    m_minLevelOfSensivity = value;
    Settings::setMinimumLevelOfSensitivity(value);
}
