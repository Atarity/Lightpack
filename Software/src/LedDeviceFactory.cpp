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
#include "LightFx.hpp"
#include "LightpackMock.hpp"
#include "LightpackDevice.hpp"

ILedDevice * LedDeviceFactory::create(QObject *parent, bool isAlienFx)
{
#   ifdef Q_WS_WIN

//
//    use LightpackMock to run software without lightpack device, as following:
//    return new LightpackMock(parent);
//

    return isAlienFx ? (ILedDevice *)new LightFx(parent) : (ILedDevice *)new LightpackDevice(parent);

#   else

    if (isAlienFx)
        qWarning() << "AlienFx not supported on current platform";

    return (ILedDevice *)new LightpackDevice(parent);

#   endif /* Q_WS_WIN */
}
