#ifndef GRABDESKTOPWINDOWLEDS_H
#define GRABDESKTOPWINDOWLEDS_H

#include <QtGui>
#include "RGB.h"        /* Led defines */

class GrabDesktopWindowLeds : public QWidget
{
    Q_OBJECT

public:
    GrabDesktopWindowLeds();
    ~GrabDesktopWindowLeds();

public:
    void showGrabPixelsRects();
    void hideGrabPixelsRects();

public slots:
    void setAmbilightWidth(int w);
    void setAmbilightHeight(int h);
    void setAmbilightRefreshDelayMs(int ms);

signals:
    void updateLedsColors(const int colorsNew[][]);

private:
    void updateLedsColorsIfChanged();
    void createLabelsGrabPixelsRects();

private: // variables
    QTimer *timer;
    QList<QLabel *> labelGrabPixelsRects;

    //  colors[LED_INDEX][COLOR] contains colors that already written to device
    int colors[LEDS_COUNT][3];

    int ambilight_width;
    int ambilight_height;
    int ambilight_refresh_delay_ms;
    int ambilight_color_depth;

    int desktop_width;
    int desktop_height;

};

#endif // GRABDESKTOPWINDOWLEDS_H
