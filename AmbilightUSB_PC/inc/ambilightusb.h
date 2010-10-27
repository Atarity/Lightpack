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


class AmbilightUsb : public QThread, public QRunnable
{
    Q_OBJECT

public:
    AmbilightUsb(QObject *parent = 0);
    ~AmbilightUsb();        
    bool openDevice();
    bool deviceOpened();

    // Start updateColorsIfChanges algorithm
    void run();

    QString hardwareVersion();

    bool offLeds();
    bool setTimerOptions(int prescallerIndex, int outputCompareRegValue);
    bool setColorDepth(int colorDepth);
    double updateColorsIfChanges();

    void clearColorSave();
    void readSettings();

signals:
    void openDeviceError();
    void openDeviceSuccess();
    void writeBufferToDeviceError();
    void writeBufferToDeviceSuccess();
    void readBufferFromDeviceError();
    void readBufferFromDeviceSuccess();
    void ambilightTimeOfUpdatingColors(double ms);


private:
    bool readDataFromDevice();
    bool readDataFromDeviceWithCheck();
    bool writeBufferToDevice();
    bool writeBufferToDeviceWithCheck();
    bool tryToReopenDevice();
    QString usbErrorMessage(int errCode);

    usbDevice_t *dev;
    TimeEvaluations *timeEval;

    char read_buffer[1 + 7];    /* 0-system, 1..7-data */
    char write_buffer[1 + 7];   /* 0-system, 1..7-data */

    //  colors_save[LED_INDEX][COLOR]
    int colors_save[LEDS_COUNT][3];

    // Settings:
    int update_delay_ms;

    int ambilight_width;
    int ambilight_height;

    double white_balance_r;
    double white_balance_g;
    double white_balance_b;

    int color_depth;

    int usb_send_data_timeout;    
};

#endif // AMBILIGHTUSB_H
