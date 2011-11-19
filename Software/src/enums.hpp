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
    QtGrabMode,
    X11GrabMode,
    WinAPIGrabMode,
    D3D9GrabMode,
    MacCoreGraphicsGrabMode
};
}

namespace SupportedDevices
{
enum DeviceType {
    LightpackDevice,
    AlienFxDevice,
    AdalightDevice,
    VirtualDevice,
    ArdulightDevice,
    DefaultDevice = LightpackDevice
};
}

namespace MaximumNumberOfLeds
{
enum Devices
{
    AbsoluteMaximum = 50,

    Adalight    = 50,
    Ardulight   = 50,
    AlienFx     = 1,
    Virtual     = 50,

    Lightpack4  = 8,
    Lightpack5  = 10,
    Lightpack6  = 10,

    Default     = 10
};
}

// Configure SettingsWindow Device tab for suitable device options
namespace DeviceTab
{
enum Options
{
    NumberOfLeds    = (1 << 0),
    Brightness      = (1 << 1),
    Gamma           = (1 << 2),

    RefreshDelay    = (1 << 3),
    SmoothSlowdown  = (1 << 4),
    SerialPort      = (1 << 5), /* serial port name and baud rate */
    VirtualLeds     = (1 << 6),

    Default         = NumberOfLeds | Brightness | Gamma,

    Adalight        = Default | SerialPort,
    Ardulight       = Default | SerialPort,
    AlienFx         = Default,
    Lightpack       = Default | RefreshDelay | SmoothSlowdown,
    Virtual         = Default | VirtualLeds
};
}

// Using in commands queue of LedDeviceFactory
namespace LedDeviceCommands
{
enum Cmd {
    OffLeds,
    SetColors,
    SetRefreshDelay,
    SetColorDepth,
    SetSmoothSlowdown,
    SetGamma,
    SetBrightness,
    RequestFirmwareVersion
};
}
