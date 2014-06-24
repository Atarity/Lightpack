/*
 * defs.h
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

#include <qglobal.h>

//#define QT_GRAB_SUPPORT
#ifdef Q_OS_WIN
//#   define ALIEN_FX_SUPPORTED
#   define WINAPI_GRAB_SUPPORT
//#   define WINAPI_EACH_GRAB_SUPPORT
//#   define D3D9_GRAB_SUPPORT
#   define D3D10_GRAB_SUPPORT
#elif defined(Q_OS_UNIX)
#   define X11_GRAB_SUPPORT
#endif

#if defined(Q_OS_DARWIN) || defined(Q_OS_DARWIN64) || defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
#   define MAC_OS
#   define MAC_OS_CG_GRAB_SUPPORT
#   undef X11_GRAB_SUPPORT
#endif

#define DEBUG_OUT_RGB( RGB_VALUE ) \
    qDebug() << #RGB_VALUE << "=" << qRed(RGB_VALUE) << qGreen(RGB_VALUE) << qBlue(RGB_VALUE)

// http://msdn.microsoft.com/ru-ru/library/b0084kay.aspx
// In MSVS2013 round() function was added to CRT.
#if !defined _MSC_VER || _MSC_VER >= 1700
#define HAVE_PLATFORM_ROUND_FUNC 1
#endif
