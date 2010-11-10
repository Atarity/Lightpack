/*
 * settings.h
 *
 *  Created on: 29.07.2010
 *      Author: Mike Shatohin (brunql)
 *     Project: AmbilightUSB
 *
 *  AmbilightUSB is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2010 Mike Shatohin, mikeshatohin [at] gmail.com
 *
 *  AmbilightUSB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  AmbilightUSB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

#define REFRESH_AMBILIGHT_MS_DEFAULT_VALUE      50
#define WIDTH_AMBILIGHT_DEFAULT_VALUE           300
#define HEIGHT_AMBILIGHT_DEFAULT_VALUE          300
#define IS_AMBILIGHT_ON_DEFAULT_VALUE           true

#define WHITE_BALANCE_COEF_RED_DEFAULT_VALUE    0.7
#define WHITE_BALANCE_COEF_GREEN_DEFAULT_VALUE  1
#define WHITE_BALANCE_COEF_BLUE_DEFAULT_VALUE   1.2

#define USB_SEND_DATA_TIMEOUT_DEFAULT_VALUE     1500
#define RECONNECT_USB_MS_DEFAULT_VALUE          2000

#define HW_TIMER_PRESCALLER_INDEX_DEFAULT_VALUE     1 /* 8 */
#define HW_TIMER_OCR_DEFAULT_VALUE                  234
#define HW_COLOR_DEPTH_DEFAULT_VALUE                64

extern QSettings *settings;

#endif // SETTINGS_H
