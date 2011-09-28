/*
 * enums.hpp
 *
 *  Created on: 11.09.2011
 *     Project: Lightpack
 *
 *  Copyright (c) 2011 Mike Shatohin (MikeShatohin@gmail.com)
 *
 *  Lightpack a USB content-driving ambient lighting system
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

#include "defs.h"

namespace Backlight
{
enum Status {
    StatusOff = 0,
    StatusOn = -1,
    StatusDeviceError = -2,
    // Just for API
    StatusUnknown = -93
};
}

namespace Api
{
enum DeviceLockStatus {
    DeviceUnlocked,
    DeviceLocked
};
}

namespace Lightpack
{
enum Mode {
    AmbilightMode,
    MoodLampMode,
    Default = AmbilightMode
};
}

namespace Grab
{

enum Mode {
    QtGrabMode
#ifdef WINAPI_GRAB_SUPPORT
    ,WinAPIGrabMode
#endif
#ifdef D3D9_GRAB_SUPPORT
    ,D3D9GrabMode
#endif
#ifdef X11_GRAB_SUPPORT
    ,X11GrabMode
#endif
};
}

namespace SupportedDevices
{
enum DeviceType {
    LightpackDevice,
    AlienFxDevice,
    VirtualDevice,
    DefaultDevice = LightpackDevice
};
}

namespace LedDeviceCommands
{
enum Cmd {
    OffLeds,
    SetColors,
    SetTimerOptions,
    SetColorDepth,
    SetSmoothSlowdown,
    RequestFirmwareVersion
};
}
