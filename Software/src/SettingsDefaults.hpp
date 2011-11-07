/*
 * SettingsDefaults.hpp
 *
 *  Created on: 01.10.2011
 *     Project: Lightpack
 *
 *  Copyright (c) 2011 Mike Shatohin, mikeshatohin [at] gmail.com
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

#include <QSize>
#include <QString>
#include "debug.h"
#include "defs.h"
#include "enums.hpp"

#ifdef ALIEN_FX_SUPPORTED
#   define SUPPORTED_DEVICES            "Lightpack,AlienFx,Adalight,Virtual"
#else
#   define SUPPORTED_DEVICES            "Lightpack,Adalight,Virtual"
#endif

#ifdef WINAPI_GRAB_SUPPORT
#   define GRABMODE_DEFAULT         ::Grab::WinAPIGrabMode
#   define GRABMODE_DEFAULT_STR     "WinAPI"
#elif defined(X11_GRAB_SUPPORT)
#   define GRABMODE_DEFAULT         ::Grab::X11GrabMode
#   define GRABMODE_DEFAULT_STR     "X11"
#elif defined(MAC_OS_CG_GRAB_SUPPORT)
#   define GRABMODE_DEFAULT         ::Grab::MacCoreGraphicsGrabMode
#   define GRABMODE_DEFAULT_STR     "MacCoreGraphics"
#else
#   define GRABMODE_DEFAULT         ::Grab::QtGrabMode
#   define GRABMODE_DEFAULT_STR     "Qt"
#endif

#ifdef Q_OS_UNIX
#   define SERIAL_PORT_DEFAULT    "/dev/ttyUSB0" /* For UART over USB on FT232RL */
#elif defined(Q_OS_WIN)
#   define SERIAL_PORT_DEFAULT    "COM1"
#endif

namespace SettingsScope
{

// LightpackMain.conf
namespace Main
{
// [General]
static const QString ProfileNameDefault = "Lightpack";
static const QString LanguageDefault = "<System>";
static const Debug::DebugLevels DebugLevelDefault = Debug::LowLevel;
static const bool IsExpertModeEnabledDefault = false;
static const QString ConnectedDeviceDefault = "Lightpack";
static const QString SupportedDevices = SUPPORTED_DEVICES; /* comma separated values! */
// [API]
namespace Api
{
static const bool IsEnabledDefault = true;
static const int PortDefault = 3636;
static const bool IsAuthEnabledDefault = true;
// See ApiKey generation in Settings initialization
}
// [SerialPort]
namespace SerialPort
{
static const QString PortDefault = SERIAL_PORT_DEFAULT;
static const QString BaudRateDefault = "115200";
}
}

// ProfileName.ini
namespace Profile
{
// [General]
static const bool IsBacklightOnDefault = true;
static const QString LightpackModeDefault = "Ambilight";

// [Grab]
namespace Grab
{
static const ::Grab::Mode ModeDefault = GRABMODE_DEFAULT;
static const QString ModeDefaultString = GRABMODE_DEFAULT_STR;
static const bool IsAvgColorsOnDefault = false;
static const int SlowdownMin = 1;
static const int SlowdownDefault = 50;
static const int SlowdownMax = 1000;
static const int MinimumLevelOfSensitivityMin = 1;
static const int MinimumLevelOfSensitivityDefault = 3;
static const int MinimumLevelOfSensitivityMax = 50;
}
// [MoodLamp]
namespace MoodLamp
{
static const int SpeedMin = 1;
static const int SpeedDefault = 50;
static const int SpeedMax = 100;
static const QString ColorDefault = "#00FF00";
static const bool IsLiquidMode = true;
}
// [Device]
namespace Device
{
static const int RefreshDelayMin = 10;
static const int RefreshDelayDefault = 100;
static const int RefreshDelayMax = 1023;

static const int BrightnessMin = 0;
static const int BrightnessDefault = 100;
static const int BrightnessMax = 100;

static const int SmoothMin = 0;
static const int SmoothDefault = 100;
static const int SmoothMax = 255;

static const double GammaMin = 0.01;
static const double GammaDefault = 2.0;
static const double GammaMax = 10.0;
}
// [LED_i]
namespace Led
{
static const bool IsEnabledDefault = true;
static const double CoefMin = 0.1;
static const double CoefDefault = 1.0;
static const double CoefMax = 3;
static const QSize SizeDefault = QSize(150, 150);
}
} /*Profile*/

} /*SettingsScope*/
