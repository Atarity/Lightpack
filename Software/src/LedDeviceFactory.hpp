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

#include "enums.hpp"
#include "ILedDevice.hpp"

class LedDeviceFactory : public QObject
{
    Q_OBJECT

public:
    explicit LedDeviceFactory(QObject *parent = 0);

signals:
    void openDeviceSuccess(bool isSuccess);
    void ioDeviceSuccess(bool isSuccess);
    void firmwareVersion(const QString & fwVersion);

    // This signals are directly connected to ILedDevice. Don't use outside.
    void ledDeviceSetColors(const QList<QRgb> & colors);
    void ledDeviceOffLeds();
    void ledDeviceSetTimerOptions(int prescallerIndex, int outputCompareRegValue);
    void ledDeviceSetColorDepth(int value);
    void ledDeviceSetSmoothSlowdown(int value);
    void ledDeviceSetGamma(double value);
    void ledDeviceSetBrightness(int value);
    void ledDeviceRequestFirmwareVersion();

public slots:
    void recreateLedDevice();

    // This slots are protected from the overflow of queries
    void setColors(const QList<QRgb> & colors);
    void offLeds();
    void setTimerOptions(int prescallerIndex, int outputCompareRegValue);
    void setColorDepth(int value);
    void setSmoothSlowdown(int value);
    void setGamma(double value);
    void setBrightness(int value);
    void requestFirmwareVersion();

private slots:
    void ledDeviceCommandCompleted(bool ok);

private:
    void initLedDevice();
    ILedDevice * createLedDevice();
    void connectSignalSlotsLedDevice();
    void disconnectSignalSlotsLedDevice();
    void cmdQueueAppend(LedDeviceCommands::Cmd);
    void cmdQueueProcessNext();

private:
    bool m_isLastCommandCompleted;

    QList<LedDeviceCommands::Cmd> m_cmdQueue;

    QList<QRgb> m_savedColors;
    int m_savedTimerPrescallerIndex;
    int m_savedTimerOCR;
    int m_savedColorDepth;
    int m_savedSmoothSlowdown;
    double m_savedGamma;
    int m_savedBrightness;

    ILedDevice *m_ledDevice;
    QThread *m_ledDeviceThread;
};
