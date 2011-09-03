/*
 * LightpackDevice.hpp
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

#pragma once

#include <QtGui>

#include "ILedDevice.hpp"
#include "timeevaluations.h"

#include "../../CommonHeaders/USB_ID.h"     /* For device VID, PID, vendor name and product name */
#include "hidapi.h" /* USB HID API */

#include "../../CommonHeaders/COMMANDS.h"   /* CMD defines */
#include "../../CommonHeaders/LEDS_COUNT.h"

// This defines using in all data transfers to determine indexes in write_buffer[]
// In device COMMAND have index 0, data 1 and so on, report id isn't using
#define WRITE_BUFFER_INDEX_REPORT_ID    0
#define WRITE_BUFFER_INDEX_COMMAND      1
#define WRITE_BUFFER_INDEX_DATA_START   2


class LightpackDevice : public ILedDevice
{
    Q_OBJECT

public:
    LightpackDevice(QObject *parent = 0);
    ~LightpackDevice();

public:
    bool openDevice();
    bool deviceOpened();
    QString firmwareVersion();
    void offLeds(); /* send CMD_OFF_ALL to device, it causes rapid shutdown LEDs */

public slots:
    void updateColors(const QList<StructRGB> & colors);
    void setTimerOptions(int prescallerIndex, int outputCompareRegValue);
    void setColorDepth(int value);
    void setSmoothSlowdown(int value);
    void setBrightness(int value);

private:
    bool readDataFromDevice();
    bool readDataFromDeviceWithCheck();
    bool writeBufferToDevice(int command);
    bool writeBufferToDeviceWithCheck(int command);
    bool tryToReopenDevice();

    hid_device *hidDevice;

    unsigned char read_buffer[65];    /* 0-ReportID, 1..65-data */
    unsigned char write_buffer[65];   /* 0-ReportID, 1..65-data */
};
