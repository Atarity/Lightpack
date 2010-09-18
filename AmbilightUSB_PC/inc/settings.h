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

#define RECONNECT_USB_MS_DEFAULT_VALUE          2000
#define REFRESH_AMBILIGHT_MS_DEFAULT_VALUE      50
#define STEP_X_DEFAULT_VALUE                    30
#define STEP_Y_DEFAULT_VALUE                    30
#define WIDTH_AMBILIGHT_DEFAULT_VALUE           300
#define HEIGHT_AMBILIGHT_DEFAULT_VALUE          300
#define IS_AMBILIGHT_ON_DEFAULT_VALUE           true
#define USB_SEND_DATA_TIMEOUT                   1500

extern QSettings *settings;

#endif // SETTINGS_H
