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

    qRegisterMetaType<GrabResult>("GrabResult");

    m_parentWidget = parent;

    for (int i = 0; i < Grab::GrabbersCount; i++)
        m_grabbers.append(NULL);

    m_dx1011Grabber = NULL;

    m_timerGrab = new QTimer(this);
    m_timeEval = new TimeEvaluations();

    m_fpsMs = 0;

    m_isSendDataOnlyIfColorsChanged = Settings::isSendDataOnlyIfColorsChanges();

//    m_grabbersThread = new QThread();
    m_grabber = queryGrabber(Settings::getGrabberType());

    m_timerUpdateFPS = new QTimer(this);
    connect(m_timerUpdateFPS, SIGNAL(timeout()), this, SLOT(timeoutUpdateFPS()));
    m_timerUpdateFPS->setSingleShot(false);
    m_timerUpdateFPS->start(500);

    m_isPauseGrabWhileResizeOrMoving = false;
    m_isGrabWidgetsVisible = false;

    initColorLists(MaximumNumberOfLeds::Default);
    initLedWidgets(MaximumNumberOfLeds::Default);

//    connect(m_timerGrab, SIGNAL(timeout()), this, SLOT(handleGrabbedColors()));
    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(scaleLedWidgets(int)));

    firstWidgetPositionChanged();

    settingsProfileChanged(Settings::getCurrentProfileName());


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

//    delete m_grabbersThread;
    delete m_dx1011Grabber;
}

void GrabManager::start(bool isGrabEnabled)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << isGrabEnabled;

    clearColorsNew();

    if (m_grabber != NULL) {
        if (isGrabEnabled) {
            QMetaObject::invokeMethod(m_grabber, "startGrabbing", Qt::QueuedConnection );
//            m_grabber->startGrabbing();
        } else {
            clearColorsCurrent();
            QMetaObject::invokeMethod(m_grabber, "stopGrabbing", Qt::QueuedConnection );
        }
    }
}

void GrabManager::onGrabberTypeChanged(const Grab::GrabberType grabberType)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << grabberType;

    if (m_grabber != NULL)
        m_grabber->stopGrabbing();

    if (Settings::isDx1011GrabberEnabled()) {
        if (m_dx1011Grabber == NULL) {
            m_dx1011Grabber = new D3D10Grabber(static_cast<QObject *>(this), &m_colorsNew, &m_ledWidgets);
            m_dx1011Grabber->init();
        }
        m_dx1011Grabber->setFallbackGrabber(queryGrabber(grabberType));
    }

    m_grabber = Settings::isDx1011GrabberEnabled() ? m_dx1011Grabber : queryGrabber(grabberType);
    m_grabber->startGrabbing();
    firstWidgetPositionChanged();
}

void GrabManager::onGrabSlowdownChanged(int ms)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << ms;
    //emit(slowdownChanged(ms));
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

void GrabManager::handleGrabbedColors()
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

//    clearColorsNew();

#define PRINT_TIME_SPENT_ON_GRAB 0
#if PRINT_TIME_SPENT_ON_GRAB
    QTime t; t.start();
#endif
    QList<QRgb> * widgetsColors = new QList<QRgb>();
//    GrabResult gr = m_grabber->grab();
//    if(gr == GrabResultOk) {
//        Q_ASSERT(m_ledWidgets.size() == widgetsColors->size());
//        for (int i = 0; i < m_ledWidgets.size(); i++)
//        {
//            if (m_ledWidgets[i]->isAreaEnabled())
//            {
//                QRgb rgb = (*widgetsColors)[i];

//                if (m_avgColorsOnAllLeds)
//                {
//                    avgR += qRed(rgb);
//                    avgG += qGreen(rgb);
//                    avgB += qBlue(rgb);
//                    countGrabEnabled++;
//                } else {
//                    m_colorsNew[i] = rgb;
//                }
//            }else{
//                m_colorsNew[i] = 0; // off led
//            }
//        }
//    }
//    delete widgetsColors;

//    if(gr == GrabResultFrameNotReady) {
//        m_timerGrab->start(5); // check in 5 ms
//        return;
//    }


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
    int newScreenNum = QApplication::desktop()->screenNumber(m_ledWidgets[0]);
    QRect newScreenRect = QApplication::desktop()->screenGeometry(newScreenNum);
    if (m_screenSavedIndex != newScreenNum || m_screenSavedRect == newScreenRect) {
        m_screenSavedIndex = newScreenNum;
        m_screenSavedRect = newScreenRect;
        emit changeScreen(m_screenSavedRect);

        if (m_grabber == NULL)
        {
            qCritical() << Q_FUNC_INFO << "m_grabber == NULL";
            return;
        }

        m_grabber->updateGrabMonitor(m_ledWidgets[0]);
    }
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

GrabberBase * GrabManager::queryGrabber(Grab::GrabberType grabberType)
{
    if (m_grabbers[grabberType] != NULL) {
        return m_grabbers[grabberType];
    } else {
        GrabberBase *result = NULL;
        switch (grabberType)
        {
    #ifdef Q_WS_X11
        case Grab::X11Grabber:
            result = new X11Grabber();
            break;
    #endif
    #ifdef Q_WS_WIN
        case Grab::GrabberTypeWinAPI:
//            result = new D3D10Grabber(NULL, &m_grabResult, &m_ledWidgets);
//            break;
            result = new WinAPIGrabber(NULL, &m_colorsNew, &m_ledWidgets);

    //    case Grab::WinAPIEachWidgetGrabber:
    //        result = new WinAPIGrabberEachWidget();

    //    case Grab::D3D9Grabber:
    //        result = new D3D9Grabber();
    #endif

    #ifdef MAC_OS
        case Grab::MacCoreGraphicsGrabber:
            result = new MacOSGrabber();
    #endif

    //    case Grab::QtEachWidgetGrabber:
    //        result new QtGrabberEachWidget();
        default:
            result = new WinAPIGrabber(NULL, &m_colorsNew, &m_ledWidgets);
        }
        m_grabbers[grabberType] = result;
//        result->moveToThread(m_grabbersThread);
//        m_grabbersThread->start();
        QMetaObject::invokeMethod(result, "init", Qt::QueuedConnection);
        QMetaObject::invokeMethod(result, "setGrabInterval", Qt::QueuedConnection, Q_ARG(int, Settings::getGrabSlowdown()));
        QMetaObject::invokeMethod(result, "startGrabbing", Qt::QueuedConnection);
        bool isConnected = connect(result, SIGNAL(frameGrabAttempted(GrabResult)), this, SLOT(onFrameGrabAttempted(GrabResult)), Qt::QueuedConnection);
        Q_ASSERT_X(isConnected, "connecting grabber to grabManager", "failed");

        return result;
    }
}

void GrabManager::onFrameGrabAttempted(GrabResult grabResult) {
    if (grabResult == GrabResultOk) {
        handleGrabbedColors();
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
