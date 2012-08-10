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

namespace DeviceLocked
{
enum DeviceLockStatus {
    Unlocked,
    Api,
    Plugin
};
}

namespace Lightpack
{
enum Mode {
    AmbilightMode,
    MoodLampMode,
    UnknownMode,
    Default = AmbilightMode
};
}

namespace Grab
{
enum GrabberType {
    QtGrabber,
    QtEachWidgetGrabber,
    X11Grabber,
    WinAPIGrabber,
    WinAPIEachWidgetGrabber,
    D3D9Grabber,
    MacCoreGraphicsGrabber,

    GrabbersCount
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

    DevicesCount,
    DefaultDevice = LightpackDevice
};
}

namespace MaximumNumberOfLeds
{
enum Devices
{
    AbsoluteMaximum = 255,

    Adalight    = 255,
    Ardulight   = 50,
    AlienFx     = 1,
    Virtual     = 255,

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
    ColorDepth      = (1 << 4),
    SmoothSlowdown  = (1 << 5),
    SerialPort      = (1 << 6), /* serial port name and baud rate */
    VirtualLeds     = (1 << 7),

    Default         = NumberOfLeds | Brightness | Gamma,

    Adalight        = Default | SerialPort,
    Ardulight       = Default | SerialPort,
    AlienFx         = Default,
    Lightpack       = Default | SmoothSlowdown | RefreshDelay | ColorDepth,
    Virtual         = Default | VirtualLeds
};
}

// Using in commands queue of LedDeviceManager
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
    RequestFirmwareVersion,
    UpdateDeviceSettings
};
}
