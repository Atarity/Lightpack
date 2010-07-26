/*
 * ambilightusb.h
 *
 *  Created on: 26.07.2010
 *      Author: brunql
 *
 *      Project: AmbilightUSB
 */

#ifndef AMBILIGHTUSB_H
#define AMBILIGHTUSB_H

#include <QtCore>
#include <X11/Xutil.h>

#include "RGB.h"        /* Leds defines, CMD defines */
#include "usbconfig.h"  /* For device VID, PID, vendor name and product name */
#include "hiddata.h"    /* USB HID */


// Take colors from screen:
// - with Y-coordinates: 0..PIXELS_Y and (800-PIXELS_Y)..800
#define PIXELS_Y  400
// - with X-coordinates: 0..PIXELS_X and (1280-PIXELS_X)..1280
#define PIXELS_X  100

// Step to next pixel
#define Y_STEP  20
#define X_STEP  20

#define PIXELS_COUNT_FOR_EACH_LED  (PIXELS_Y / Y_STEP) * (PIXELS_X / X_STEP)


class ambilightUsb
{
public:
    ambilightUsb();
    ~ambilightUsb();
    bool deviceOpened();
    bool openDevice();
    void updateColorsIfChanges();
    void offLeds();

private:
    bool openX11Display();

    void readData();
    void writeBuffer();
    QString usbErrorMessage(int errCode);

    usbDevice_t *dev;
    Display *display;
    Colormap cmap;
    Window root_window;

    char read_buffer[1 + 7];    /* 0-system, 1..7-data */
    char write_buffer[1 + 7];   /* 0-system, 1..7-data */

    //  colors[LED_INDEX][COLOR]
    int colors_save[4][3];
};

#endif // AMBILIGHTUSB_H
