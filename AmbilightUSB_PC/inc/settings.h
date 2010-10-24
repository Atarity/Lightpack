/*
 * settings.h
 *
 *  Created on: 29.07.2010
 *      Author: brunql
 *
 *      Project: AmbilightUSB
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

extern QSettings *settings;

#endif // SETTINGS_H
