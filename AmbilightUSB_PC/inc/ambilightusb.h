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

#include <QtGui> // QApplication::desktop()->width();

#ifndef WIN32
#   include <X11/Xutil.h>
#endif

#include "timeevaluations.h"

#include "settings.h"

#include "usbconfig.h"  /* For device VID, PID, vendor name and product name */
#include "hiddata.h"    /* USB HID */

#include "commands.h"   /* CMD defines */
#include "RGB.h"        /* Led defines */


class ambilightUsb
{
public:
    ambilightUsb();
    ~ambilightUsb();
    bool deviceOpened();
    bool openDevice();
    double updateColorsIfChanges();
    void offLeds();
    void clearColorSave();
    void readSettings();
    QString hardwareVersion();

private:
    bool openX11Display();

    bool readDataFromDevice();
    bool writeBufferToDevice();
    bool writeBufferToDeviceWithCheck();
    bool tryToReopenDevice();
    QString usbErrorMessage(int errCode);

    usbDevice_t *dev;
#ifndef WIN32
    Display *display;
    Colormap cmap;
    Window root_window;
#endif

    char read_buffer[1 + 7];    /* 0-system, 1..7-data */
    char write_buffer[1 + 7];   /* 0-system, 1..7-data */

    //  colors_save[LED_INDEX][COLOR]
    int colors_save[4][3];

    int step_x;
    int step_y;
    int ambilight_width;
    int ambilight_height;

    int usb_send_data_timeout;

    int pixels_count_for_each_led;

    TimeEvaluations *timeEval;
};

#endif // AMBILIGHTUSB_H
