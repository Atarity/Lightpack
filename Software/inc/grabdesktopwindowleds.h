/*
 * grabdesktopwindowleds.h
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


#ifndef GRABDESKTOPWINDOWLEDS_H
#define GRABDESKTOPWINDOWLEDS_H

#include <QtGui>
#include "../../CommonHeaders/RGB.h"        /* Led defines */
#include "settings.h"
#include "timeevaluations.h"
#include "ledcolors.h"
#include "movemewidget.h"
#include "desktop.h"

class GrabDesktopWindowLeds : public QWidget
{
    Q_OBJECT

public:
    GrabDesktopWindowLeds(QWidget *parent = 0);
    ~GrabDesktopWindowLeds();

public:
    void clearColors();

signals:
    void updateLedsColors(LedColors colorsNew);
    void ambilightTimeOfUpdatingColors(double ms);


public slots:
    void setAmbilightOn(bool state);
    void setAmbilightRefreshDelayMs(int ms);
    void setAmbilightColorDepth(int color_depth);
    void setVisibleLedWidgets(bool state);
    void setColoredLedWidgets(bool state);
    void setWhiteLedWidgets(bool state);
    void setUpdateColorsOnlyIfChanges(bool state);
    void setAvgColorsOnAllLeds(bool state);
    void setMinLevelOfSensivity(int value);

    void setAmbilightON();
    void setAmbilightOFF();

    void settingsProfileChanged();

private slots:
    void scaleLedWidgets();
    void updateLedsColorsIfChanged();

private:
    void createLedWidgets();
    void updateSizesLabelsGrabPixelsRects();

private: // variables
    QTimer *timer;
    QList<MoveMeWidget *> ledWidgets;
    //QList<MoveMeWidget *> moveMeGroup; // move and resize labels together
    const static QColor backgroundAndTextColors[LEDS_COUNT][2];
    TimeEvaluations *timeEval;

    LedColors colors;

    bool isAmbilightOn;    
    bool isResizeOrMoving;
    bool updateColorsOnlyIfChanges;
    bool avgColorsOnAllLeds;
    int minLevelOfSensivity;

    // Settings:
    int ambilight_refresh_delay_ms;
    int ambilight_color_depth;
};

#endif // GRABDESKTOPWINDOWLEDS_H
