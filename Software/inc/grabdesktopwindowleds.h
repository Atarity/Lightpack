/*
 * grabdesktopwindowleds.h
 *
 *  Created on: 26.07.2010
 *      Author: Mike Shatohin (brunql)
 *     Project: AmbilightUSB
 *
 *  AmbilightUSB is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2010, 2011 Mike Shatohin, mikeshatohin [at] gmail.com
 *
 *  AmbilightUSB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  AmbilightUSB is distributed in the hope that it will be useful,
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
#include "RGB.h"        /* Led defines */
#include "settings.h"
#include "timeevaluations.h"
#include "ledcolors.h"

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
    void setAmbilightWidth(int w);
    void setAmbilightHeight(int h);
    void setAmbilightRefreshDelayMs(int ms);
    void setAmbilightColorDepth(int color_depth);
    void setAmbilightWhiteBalanceR(double r);
    void setAmbilightWhiteBalanceG(double g);
    void setAmbilightWhiteBalanceB(double b);
    void setVisibleGrabPixelsRects(bool state);
    void setColoredGrabPixelsRects(bool state);
    void setWhiteGrabPixelsRects(bool state);
    void setUpdateColorsOnlyIfChanges(bool state);


private slots:
    void updateLedsColorsIfChanged();

private:
    void createLabelsGrabPixelsRects();
    void updateSizesLabelsGrabPixelsRects();

private: // variables
    QTimer *timer;
    QList<QLabel *> labelGrabPixelsRects;
    const static QColor labelsColors[LEDS_COUNT];
    TimeEvaluations *timeEval;

    LedColors colors;

    int desktop_width;
    int desktop_height;

    bool isAmbilightOn;    
    bool updateColorsOnlyIfChanges;

    // Settings:
    int ambilight_width;
    int ambilight_height;
    int ambilight_refresh_delay_ms;
    int ambilight_color_depth;

    double ambilight_white_balance_r;
    double ambilight_white_balance_g;
    double ambilight_white_balance_b;
};

#endif // GRABDESKTOPWINDOWLEDS_H
