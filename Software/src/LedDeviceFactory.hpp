/*
 * LedDeviceFactory.hpp
 *
 *  Created on: 17.04.2011
 *      Author: Timur Sattarov && Mike Shatohin
 *     Project: Lightpack
 *
 *  Lightpack is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2011 Mike Shatohin, mikeshatohin [at] gmail.com
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

#include "ILedDevice.hpp"

class LedDeviceFactory : public QObject
{
    Q_OBJECT

public:
    explicit LedDeviceFactory(QObject *parent = 0);

signals:
    void openDeviceSuccess(bool isSuccess);
    void ioDeviceSuccess(bool isSuccess);
//    void setColorsDone();
    void firmwareVersion(const QString & fwVersion);

    // Do not use signal setLedDeviceColors outside of the class!
    // This signal is directly connected to ILedDevice.
    // Use instead public slot setColorsIfDeviceAvailable, which is protected from the overflow of queries.
    void setLedDeviceColors(const QList<QRgb> & colors);

    // This signals is directly connected to ILedDevice.
    // They are NOT protected from the overflow of queries!
    void offLeds();
    void setTimerOptions(int prescallerIndex, int outputCompareRegValue);
    void setColorDepth(int value);
    void setSmoothSlowdown(int value);
//    void setBrightness(int value);
    void requestFirmwareVersion();

public slots:
    void recreateLedDevice();
    void setColorsDone();
    void setColorsIfDeviceAvailable(const QList<QRgb> & colors);

private:
    void initLedDevice();
    ILedDevice * createLedDevice();
    void connectSignalSlotsLedDevice();
    void disconnectSignalSlotsLedDevice();

private:
    bool isSetColorsDone;
    ILedDevice *m_ledDevice;
    QThread *m_ledDeviceThread;
};
