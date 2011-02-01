/*
 * settings.h
 *
 *  Created on: 29.07.2010
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Lightpack is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2010, 2011 Mike Shatohin, mikeshatohin [at] gmail.com
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


#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

// [General]
#define REFRESH_AMBILIGHT_MS_DEFAULT_VALUE      50
#define IS_AMBILIGHT_ON_DEFAULT_VALUE           true

// [Firmware]
#define FW_TIMER_PRESCALLER_INDEX_DEFAULT_VALUE     1 /* prescaller == 8 */
#define FW_TIMER_OCR_DEFAULT_VALUE                  200
#define FW_COLOR_DEPTH_DEFAULT_VALUE                64
#define FW_CHANGE_COLORS_SMOOTH_DELAY_VALUE         32

// [LED_i]
#define LED_FIELD_WIDTH_DEFAULT_VALUE       150
#define LED_FIELD_HEIGHT_DEFAULT_VALUE      150
#define LED_FIELD_SIZE_DEFAULT_VALUE        QSize(LED_FIELD_WIDTH_DEFAULT_VALUE, LED_FIELD_HEIGHT_DEFAULT_VALUE)
#define LED_COEF_RED_DEFAULT_VALUE          1
#define LED_COEF_GREEN_DEFAULT_VALUE        1
#define LED_COEF_BLUE_DEFAULT_VALUE         1

extern QSettings *settings;

#endif // SETTINGS_H
