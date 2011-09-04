/*
 * LedDeviceFactory.cpp
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

#include <qglobal.h>

#include "LedDeviceFactory.hpp"
#include "LedDeviceLightpack.hpp"
#include "LedDeviceAlienFx.hpp"
#include "LedDeviceVirtual.hpp"
#include "settings.h"

ILedDevice * LedDeviceFactory::create(QObject *parent)
{
    SupportedDevices connectedDevice = Settings::getConnectedDevice();

    if (connectedDevice == SupportedDevice_AlienFx){
#       if !defined(Q_WS_WIN)
        qWarning() << Q_FUNC_INFO << "AlienFx not supported on current platform";

        Settings::setConnectedDevice(SupportedDevice_Default);
        connectedDevice = Settings::getConnectedDevice();
#       endif /* Q_WS_WIN */
    }

    switch (connectedDevice){

    case SupportedDevice_Lightpack:
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "SupportedDevice_Lightpack";
        return (ILedDevice *)new LedDeviceLightpack(parent);

    case SupportedDevice_AlienFx:
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "SupportedDevice_AlienFx";

#       ifdef Q_WS_WIN
        return (ILedDevice *)new LedDeviceAlienFx(parent);
#       else
        break;
#       endif /* Q_WS_WIN */

    case SupportedDevice_Virtual:
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "SupportedDevice_Virtual";
        return (ILedDevice *)new LedDeviceVirtual(parent);
    }

    qFatal("%s %s%d%s", Q_FUNC_INFO,
           "Create LedDevice fail. connectedDevice = '", connectedDevice,
           "'. Application exit.");

    return NULL; // Avoid compiler warning,
}
