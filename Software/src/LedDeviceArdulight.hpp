/*
 * LedDeviceArdulight.hpp
 *
 *  Created on: 08.11.2011
 *      Author: Andrei Isupov
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
#include "StructRgb.hpp"
#include "abstractserial.h"

class LedDeviceArdulight : public ILedDevice
{
    Q_OBJECT
public:
    LedDeviceArdulight(QObject * parent = 0);
    ~LedDeviceArdulight();

public slots:
    void setColors(const QList<QRgb> & /*colors*/);
    void offLeds();
    void setRefreshDelay(int /*value*/);
    void setColorDepth(int /*value*/);
    void setSmoothSlowdown(int /*value*/);
    void setGamma(double /*value*/);
    void setBrightness(int /*value*/);
    void requestFirmwareVersion();
public:
    void open();

private:
    bool writeBuffer(const QByteArray & buff);

private:
    AbstractSerial *m_ArdulightDevice;

    QByteArray m_writeBufferHeader;
    QByteArray m_writeBuffer;

    double m_gamma;
    int m_brightness;

    QList<QRgb> m_colorsSaved;
    QList<StructRgb> m_colorsBuffer;
};
