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

    QString hardwareVersion();

    bool offLeds();
    bool setTimerOptions(enum PRESCALLERS prescaller, int outputCompareRegValue);
    double updateColorsIfChanges();

    void clearColorSave();
    void readSettings();    



private:
    bool readDataFromDevice();
    bool readDataFromDeviceWithCheck();
    bool writeBufferToDevice();
    bool writeBufferToDeviceWithCheck();
    bool tryToReopenDevice();
    QString usbErrorMessage(int errCode);

    usbDevice_t *dev;

    char read_buffer[1 + 7];    /* 0-system, 1..7-data */
    char write_buffer[1 + 7];   /* 0-system, 1..7-data */

    //  colors_save[LED_INDEX][COLOR]
    int colors_save[LEDS_COUNT][3];

    // Settings:
    int ambilight_width;
    int ambilight_height;

    double white_balance_r;
    double white_balance_g;
    double white_balance_b;

    int usb_send_data_timeout;


    TimeEvaluations *timeEval;
};

#endif // AMBILIGHTUSB_H
