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

#include <QtGui>

#include "timeevaluations.h"

#include "settings.h"

#include "usbconfig.h"  /* For device VID, PID, vendor name and product name */
#include "hiddata.h"    /* USB HID */

#include "commands.h"   /* CMD defines */
#include "RGB.h"        /* Led defines */


class AmbilightUsb : public QObject
{
    Q_OBJECT

public:
    AmbilightUsb(QObject *parent = 0);
    ~AmbilightUsb();        

public:
    bool openDevice();
    bool deviceOpened();
    QString hardwareVersion();
    void offLeds();

public slots:
    void updateColors(int colors[LEDS_COUNT][3]);
    void setUsbSendDataTimeoutMs(double usb_send_data_timeout_secs);
    void setTimerOptions(int prescallerIndex, int outputCompareRegValue);
    void setColorDepth(int colorDepth);

signals:
    void openDeviceError();
    void openDeviceSuccess();
    void writeBufferToDeviceError();
    void writeBufferToDeviceSuccess();
    void readBufferFromDeviceError();
    void readBufferFromDeviceSuccess();


private:
    bool readDataFromDevice();
    bool readDataFromDeviceWithCheck();
    bool writeBufferToDevice();
    bool writeBufferToDeviceWithCheck();
    bool tryToReopenDevice();
    QString usbErrorMessage(int errCode);

    usbDevice_t *ambilightDevice;


    char read_buffer[1 + 7];    /* 0-system, 1..7-data */
    char write_buffer[1 + 7];   /* 0-system, 1..7-data */


    // Settings:
    int usb_send_data_timeout_ms;
};

#endif // AMBILIGHTUSB_H
