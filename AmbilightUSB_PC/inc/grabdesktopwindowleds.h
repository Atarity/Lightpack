/*
 * grabdesktopwindowleds.h
 *
 *  Created on: 26.07.2010
 *      Author: brunql
 *
 *      Project: AmbilightUSB
 */

#ifndef GRABDESKTOPWINDOWLEDS_H
#define GRABDESKTOPWINDOWLEDS_H

#include <QtGui>
#include "RGB.h"        /* Led defines */
#include "settings.h"
#include "timeevaluations.h"

class GrabDesktopWindowLeds : public QWidget
{
    Q_OBJECT

public:
    GrabDesktopWindowLeds(QWidget *parent = 0);
    ~GrabDesktopWindowLeds();

public:
    void clearColors();

signals:
    void updateLedsColors(int colorsNew[LEDS_COUNT][3]);
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


private slots:
    void updateLedsColorsIfChanged();

private:
    void createLabelsGrabPixelsRects();
    void updateSizesLabelsGrabPixelsRects();

private: // variables
    QTimer *timer;
    QList<QLabel *> labelGrabPixelsRects;
    TimeEvaluations *timeEval;

    //  colors[LED_INDEX][COLOR] contains colors that already written to device
    int colors[LEDS_COUNT][3];

    int desktop_width;
    int desktop_height;

    bool isAmbilightOn;

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
