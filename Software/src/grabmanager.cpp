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

GrabManager::GrabManager(QWidget *parent) : QObject(parent)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    m_parentWidget = parent;

    for (int i = 0; i < Grab::GrabbersCount; i++)
        m_grabbers.append(NULL);

    m_timerGrab = new QTimer(this);
    m_timeEval = new TimeEvaluations();

    m_fpsMs = 0;
    m_isGrabEnabled = false;

    m_isSendDataOnlyIfColorsChanged = Settings::isSendDataOnlyIfColorsChanges();

    m_grabber = createGrabber(Settings::getGrabberType());

    m_timerUpdateFPS = new QTimer(this);
    connect(m_timerUpdateFPS, SIGNAL(timeout()), this, SLOT(timeoutUpdateFPS()));
    m_timerUpdateFPS->setSingleShot(false);
    m_timerUpdateFPS->start(500);

    m_isPauseGrabWhileResizeOrMoving = false;
    m_isGrabWidgetsVisible = false;

    initColorLists(MaximumNumberOfLeds::Default);
    initLedWidgets(MaximumNumberOfLeds::Default);

    connect(m_timerGrab, SIGNAL(timeout()), this, SLOT(timeoutUpdateColors()));
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

    for (int i = 0; i < m_ledWidgets.size(); i++)
    {
        m_ledWidgets[i]->deleteLater();
    }
    m_ledWidgets.clear();

    for (int i = 0; i < Grab::GrabbersCount; i++)
        delete m_grabbers[i];
}

void GrabManager::start(bool isGrabEnabled)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << isGrabEnabled;

    m_isGrabEnabled = isGrabEnabled;

    clearColorsNew();

    if (m_isGrabEnabled)
    {
        m_timerGrab->start(0);
    } else {
        m_timerGrab->stop();
        clearColorsCurrent();
    }
}

void GrabManager::onGrabberTypeChanged(const Grab::GrabberType grabberType)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << grabberType;

    if (m_grabbers[grabberType] == NULL)
    {
        m_grabbers[grabberType] = createGrabber(grabberType);
    }

    m_grabber = m_grabbers[grabberType];

    firstWidgetPositionChanged();
}

void GrabManager::onGrabSlowdownChanged(int ms)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << ms;
    m_slowdownTime = ms;
}

void GrabManager::onThresholdOfBlackChanged(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;
    m_minLevelOfSensivity = value;
}

void GrabManager::onGrabAvgColorsEnabledChanged(bool state)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << state;
    m_avgColorsOnAllLeds = state;
}

void GrabManager::onSendDataOnlyIfColorsEnabledChanged(bool state)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << state;
    m_isSendDataOnlyIfColorsChanged = state;
}

void GrabManager::setNumberOfLeds(int numberOfLeds)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << numberOfLeds;    

    initColorLists(numberOfLeds);
    initLedWidgets(numberOfLeds);

    for (int i = 0; i < m_ledWidgets.size(); i++)
    {
        m_ledWidgets[i]->settingsProfileChanged();
        m_ledWidgets[i]->setVisible(m_isGrabWidgetsVisible);
    }
}

void GrabManager::reset()
{
    clearColorsCurrent();
}

void GrabManager::settingsProfileChanged(const QString &profileName)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    m_isSendDataOnlyIfColorsChanged = Settings::isSendDataOnlyIfColorsChanges();
    m_avgColorsOnAllLeds = Settings::isGrabAvgColorsEnabled();
    m_minLevelOfSensivity = Settings::getThresholdOfBlack();
    m_slowdownTime = Settings::getGrabSlowdown();

    setNumberOfLeds(Settings::getNumberOfLeds(Settings::getConnectedDevice()));

    for (int i = 0; i < m_ledWidgets.size(); i++)
    {
        m_ledWidgets[i]->settingsProfileChanged();
    }
}

void GrabManager::setVisibleLedWidgets(bool state)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << state;

    m_isGrabWidgetsVisible = state;

    for (int i = 0; i < m_ledWidgets.size(); i++)
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
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    // This slot is directly connected to radioButton toggled(bool) signal
    if (state)
    {
        for (int i = 0; i < m_ledWidgets.size(); i++)
            m_ledWidgets[i]->fillBackgroundColored();
    }
}

void GrabManager::setWhiteLedWidgets(bool state)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    // This slot is directly connected to radioButton toggled(bool) signal
    if (state)
    {
        for (int i = 0; i < m_ledWidgets.size(); i++)
            m_ledWidgets[i]->fillBackgroundWhite();
    }
}

void GrabManager::timeoutUpdateColors()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    if (m_grabber == NULL)
    {
        qCritical() << Q_FUNC_INFO << "m_grabber == NULL";
        return;
    }

    // Temporary switch off updating colors
    // if one of LED widgets resizing or moving
    if (m_isPauseGrabWhileResizeOrMoving)
    {
        m_timerGrab->start(50); // check in 50 ms
        return;
    }    

    bool isColorsChanged = false;

    int avgR = 0, avgG = 0, avgB = 0;
    int countGrabEnabled = 0;

    clearColorsNew();

#define PRINT_TIME_SPENT_ON_GRAB 0
#if PRINT_TIME_SPENT_ON_GRAB
    QTime t; t.start();
#endif

    QList<QRgb> widgetsColors = m_grabber->grabWidgetsColors(m_ledWidgets);

    for (int i = 0; i < m_ledWidgets.size(); i++)
    {
        if (m_ledWidgets[i]->isAreaEnabled())
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

#if PRINT_TIME_SPENT_ON_GRAB
    qDebug() << "Time spent on grab:" << t.elapsed() << "ms";
#endif

    if (m_avgColorsOnAllLeds)
    {
        if (countGrabEnabled != 0)
        {
            avgR /= countGrabEnabled;
            avgG /= countGrabEnabled;
            avgB /= countGrabEnabled;
        }
        // Set one AVG color to all LEDs
        for (int ledIndex = 0; ledIndex < m_ledWidgets.size(); ledIndex++)
        {
            if (m_ledWidgets[ledIndex]->isAreaEnabled())
            {
                m_colorsNew[ledIndex] = qRgb(avgR, avgG, avgB);
            }
        }
    }

    // White balance
    for (int i = 0; i < m_ledWidgets.size(); i++)
    {
        QRgb rgb = m_colorsNew[i];

        unsigned r = qRed(rgb)   * m_ledWidgets[i]->getCoefRed();
        unsigned g = qGreen(rgb) * m_ledWidgets[i]->getCoefGreen();
        unsigned b = qBlue(rgb)  * m_ledWidgets[i]->getCoefBlue();

        if (r > 0xff) r = 0xff;
        if (g > 0xff) g = 0xff;
        if (b > 0xff) b = 0xff;

        m_colorsNew[i] = qRgb(r, g, b);
    }

    // Check minimum level of sensivity
    for (int i = 0; i < m_ledWidgets.size(); i++)
    {
        QRgb rgb = m_colorsNew[i];
        int avg = round((qRed(rgb) + qGreen(rgb) + qBlue(rgb)) / 3.0);

        if (avg <= m_minLevelOfSensivity)
        {
            m_colorsNew[i] = 0;
        }
    }

    for (int i = 0; i < m_ledWidgets.size(); i++)
    {
        if (m_colorsCurrent[i] != m_colorsNew[i])
        {
            m_colorsCurrent[i] = m_colorsNew[i];
            isColorsChanged = true;
        }
    }

    if ((m_isSendDataOnlyIfColorsChanged == false) || isColorsChanged)
    {
        emit updateLedsColors(m_colorsCurrent);
    }

    m_fpsMs = m_timeEval->howLongItEnd();
    m_timeEval->howLongItStart();

    if (m_isGrabEnabled)
        m_timerGrab->start(m_slowdownTime);
}

void GrabManager::timeoutUpdateFPS()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;
    emit ambilightTimeOfUpdatingColors(m_fpsMs);
}

void GrabManager::pauseWhileResizeOrMoving()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;
    m_isPauseGrabWhileResizeOrMoving = true;
}

void GrabManager::resumeAfterResizeOrMoving()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;
    m_isPauseGrabWhileResizeOrMoving = false;
}

void GrabManager::firstWidgetPositionChanged()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    m_screenSavedIndex = QApplication::desktop()->screenNumber(m_ledWidgets[0]);
    m_screenSavedRect = QApplication::desktop()->screenGeometry(m_screenSavedIndex);
    emit changeScreen(m_screenSavedRect);

    if (m_grabber == NULL)
    {
        qCritical() << Q_FUNC_INFO << "m_grabber == NULL";
        return;
    }

    m_grabber->updateGrabScreenFromWidget(m_ledWidgets[0]);
}

void GrabManager::scaleLedWidgets(int screenIndexResized)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "screenIndexResized:" << screenIndexResized;

    int screenIndexOfFirstLedWidget = QApplication::desktop()->screenNumber(m_ledWidgets[0]);

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "LedWidgets[0] index of screen:" << screenIndexOfFirstLedWidget;

    QRect screen = QApplication::desktop()->screenGeometry(m_screenSavedIndex);

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "LedWidgets[0] screen:" << screen;
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "LedWidgets[0] screenSaved:" << m_screenSavedRect;
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "LedWidgets[0] screenSavedIndex:" << m_screenSavedIndex;
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "LedWidgets[0] screenIndexOfFirstLedWidget:" << screenIndexOfFirstLedWidget;

    if(screenIndexResized != -1 && screenIndexOfFirstLedWidget != -1 && screenIndexResized != screenIndexOfFirstLedWidget) {
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

    for(int i=0; i < m_ledWidgets.size(); i++){

        int width  = round(scaleX * m_ledWidgets[i]->width());
        int height = round(scaleY * m_ledWidgets[i]->height());

        int x = m_ledWidgets[i]->x();
        int y = m_ledWidgets[i]->y();

        x -= screen.x();
        y -= screen.y();

        x = round(scaleX * x);
        y = round(scaleY * y);

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

IGrabber * GrabManager::createGrabber(Grab::GrabberType grabberType)
{
    switch (grabberType)
    {
#ifdef Q_WS_X11
    case Grab::X11Grabber:
        return new X11Grabber();
#endif
#ifdef Q_WS_WIN
    case Grab::WinAPIGrabber:
        return new WinAPIGrabber();

    case Grab::WinAPIEachWidgetGrabber:
        return new WinAPIGrabberEachWidget();

    case Grab::D3D9Grabber:
        return new D3D9Grabber();
#endif

#ifdef MAC_OS
    case Grab::MacCoreGraphicsGrabber:
        return new MacOSGrabber();
#endif

    case Grab::QtEachWidgetGrabber:
        return new QtGrabberEachWidget();

    default:
        return new QtGrabber();
    }   
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

void GrabManager::clearColorsNew()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    for (int i = 0; i < m_colorsNew.size(); i++)
    {
        m_colorsNew[i] = 0;
    }
}

void GrabManager::clearColorsCurrent()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    for (int i = 0; i < m_colorsCurrent.size(); i++)
    {
        m_colorsCurrent[i] = 0;
    }
}

void GrabManager::initLedWidgets(int numberOfLeds)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << numberOfLeds;

    if (m_ledWidgets.size() == 0)
    {
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "First widget initialization";

        GrabWidget * ledWidget = new GrabWidget(m_ledWidgets.size(), m_parentWidget);

        connect(ledWidget, SIGNAL(resizeOrMoveStarted()), this, SLOT(pauseWhileResizeOrMoving()));
        connect(ledWidget, SIGNAL(resizeOrMoveCompleted(int)), this, SLOT(resumeAfterResizeOrMoving()));

        // First LED widget using to determine grabbing-monitor in WinAPI version of Grab
        connect(ledWidget, SIGNAL(resizeOrMoveCompleted(int)), this, SLOT(firstWidgetPositionChanged()));

        m_ledWidgets << ledWidget;

        // TODO: Check out this line!
        // firstWidgetPositionChanged();
    }

    int diff = numberOfLeds - m_ledWidgets.size();

    if (diff > 0)
    {
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Append" << diff << "grab widgets";

        for (int i = 0; i < diff; i++)
        {
            GrabWidget * ledWidget = new GrabWidget(m_ledWidgets.size(), m_parentWidget);

            connect(ledWidget, SIGNAL(resizeOrMoveStarted()), this, SLOT(pauseWhileResizeOrMoving()));
            connect(ledWidget, SIGNAL(resizeOrMoveCompleted(int)), this, SLOT(resumeAfterResizeOrMoving()));

            m_ledWidgets << ledWidget;
        }
    } else {
        diff *= -1;
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "Remove last" << diff << "grab widgets";

        while (diff --> 0)
        {
            m_ledWidgets.last()->deleteLater();
            m_ledWidgets.removeLast();
        }
    }

    if (m_ledWidgets.size() != numberOfLeds)
        qCritical() << Q_FUNC_INFO << "Fail: m_ledWidgets.size()" << m_ledWidgets.size() << " != numberOfLeds" << numberOfLeds;
}
