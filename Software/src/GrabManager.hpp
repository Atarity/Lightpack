/*
 * GrabManager.hpp
 *
 *  Created on: 26.07.2010
 *     Project: Lightpack
 *
 *  Copyright (c) 2010, 2011 Mike Shatohin, mikeshatohin [at] gmail.com
 *
 *  Lightpack a USB content-driving ambient lighting system
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

#pragma once

#include <QtGui>
#include "Settings.hpp"
#include "SettingsWindow.hpp"
#include "TimeEvaluations.hpp"
#include "GrabWidget.hpp"
#include "WinAPIGrabber.hpp"
#include "WinAPIGrabberEachWidget.hpp"
#include "QtGrabber.hpp"
#include "QtGrabberEachWidget.hpp"
#include "X11Grabber.hpp"
#include "MacOSGrabber.hpp"
#include "D3D9Grabber.hpp"

#include "enums.hpp"

class GrabManager : public QObject
{
    Q_OBJECT

public:
    GrabManager(QWidget *parent = 0);
    ~GrabManager();

signals:
    void updateLedsColors(const QList<QRgb> & colors);
    void ambilightTimeOfUpdatingColors(double ms);
    void changeScreen(QRect rect);

public:


    // Grab options

    // Common options
    void setNumberOfLeds(int numberOfLeds);
    void reset();

public slots:
    void onGrabberTypeChanged(const Grab::GrabberType &grabberType);
    void onGrabSlowdownChanged(int ms);
    void onThresholdOfBlackChanged(int value);
    void onGrabAvgColorsEnabledChanged(bool state);
    void onSendDataOnlyIfColorsEnabledChanged(bool state);
    void start(bool isGrabEnabled);
    void settingsProfileChanged(const QString &profileName);
    void setVisibleLedWidgets(bool state);
    void setColoredLedWidgets(bool state);
    void setWhiteLedWidgets(bool state);

private slots:
    void timeoutUpdateColors();
    void timeoutUpdateFPS();
    void pauseWhileResizeOrMoving();
    void resumeAfterResizeOrMoving();
    void firstWidgetPositionChanged();
    void scaleLedWidgets(int screenIndexResized);

private:
    IGrabber *createGrabber(Grab::GrabberType grabber);
    void initColorLists(int numberOfLeds);
    void clearColorsNew();
    void clearColorsCurrent();
    void initLedWidgets(int numberOfLeds);

private:
    QList<IGrabber*> m_grabbers;
    IGrabber *m_grabber;
    QTimer *m_timerGrab;
    QTimer *m_timerUpdateFPS;
    QWidget *m_parentWidget;
    QList<GrabWidget *> m_ledWidgets;
    const static QColor m_backgroundAndTextColors[10][2];
    TimeEvaluations *m_timeEval;

    QList<QRgb> m_colorsCurrent;
    QList<QRgb> m_colorsNew;       

    QRect m_screenSavedRect;
    int m_screenSavedIndex;

    bool m_isGrabEnabled;
    bool m_isPauseGrabWhileResizeOrMoving;
    bool m_isSendDataOnlyIfColorsChanged;
    bool m_avgColorsOnAllLeds;
    int m_minLevelOfSensivity;

    // Store last grabbing time in milliseconds
    double m_fpsMs;

    int m_slowdownTime;
    bool m_isGrabWidgetsVisible;
};
