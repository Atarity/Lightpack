/*
 * LightpackVirtual.cpp
 *
 *  Created on: 06.09.2011
 *      Author: Mike Shatohin (brunql)
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

#include "LedDeviceVirtual.hpp"
#include "debug.h"

LedDeviceVirtual::LedDeviceVirtual(QObject * parent) : ILedDevice(parent)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
}

void LedDeviceVirtual::setColors(const QList<QRgb> & /*colors*/)
{
    emit commandCompleted(true);
}

void LedDeviceVirtual::offLeds()
{
    emit commandCompleted(true);
}

void LedDeviceVirtual::setTimerOptions(int /*prescallerIndex*/, int /*outputCompareRegValue*/)
{
    emit commandCompleted(true);
}

void LedDeviceVirtual::setColorDepth(int /*value*/)
{
    emit commandCompleted(true);
}

void LedDeviceVirtual::setSmoothSlowdown(int /*value*/)
{
    emit commandCompleted(true);
}

void LedDeviceVirtual::requestFirmwareVersion()
{
    emit firmwareVersion("1.0 (virtual device)");
    emit commandCompleted(true);
}

void LedDeviceVirtual::open()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    emit openDeviceSuccess(true);
}
