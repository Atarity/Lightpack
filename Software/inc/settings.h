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
#include <QVariant>

#include "debug.h"

// Default values:

// LightpackMain.conf
// [General]
#define PROFILE_DEFAULT_NAME            "Lightpack"
#define LANGUAGE_DEFAULT_NAME           "<System>" /* system lang */
#define SHOW_ANOTHER_GUI                false
#define DEBUG_LEVEL_DEFAULT             Debug::LowLevel

// ProfileName.ini
// [General]
#define GRAB_SLOWDOWN_MS_DEFAULT_VALUE          50
#define IS_AMBILIGHT_ON_DEFAULT_VALUE           true
#define IS_AVG_COLORS_ON_DEFAULT_VALUE          false
#define MINIMUM_LEVEL_OF_SENSITIVITY_DEFAULT    3

// [Firmware]
#define FW_TIMER_PRESCALLER_INDEX_DEFAULT_VALUE     0 /* prescaller == 1 */
#define FW_TIMER_OCR_DEFAULT_VALUE                  100
#define FW_COLOR_DEPTH_DEFAULT_VALUE                128
#define FW_IS_SMOOTH_CHANGE_COLORS_DEFAULT_VALUE    true

// [LED_i]
#define LED_FIELD_WIDTH_DEFAULT_VALUE       150
#define LED_FIELD_HEIGHT_DEFAULT_VALUE      150
#define LED_FIELD_SIZE_DEFAULT_VALUE        QSize(LED_FIELD_WIDTH_DEFAULT_VALUE, LED_FIELD_HEIGHT_DEFAULT_VALUE)
#define LED_COEF_RGB_DEFAULT_VALUE          1
#define LED_IS_ENABLED_DEFAULT_VALUE        true



class Settings : public QObject
{
    Q_OBJECT

public:
    static void Initialize(const QString & applicationDirPath, bool isSetDebugLevelFromConfig);

    // Simple abstraction functions for forwarding to settingsNow object
    static void setValue(const QString & key, const QVariant & value);
    static QVariant value(const QString & key);
    static QString fileName();

    // forwarding to settingsMain object
    static void setValueMain(const QString & key, const QVariant & value);
    static QVariant valueMain(const QString & key);

    static void resetToDefaults();
    static void loadOrCreateConfig(const QString & configName);
    static void renameCurrentConfig(const QString & configName);
    static void removeCurrentConfig();

    static QString lastProfileName();
    static QString getApplicationDirPath();

private:
    static void settingsInit();
    static void setDefaultSettingIfNotFound(const QString & name, const QVariant & value);
    static void setDefaultSettingIfNotFound(QSettings *settings, const QString & name, const QVariant & value);

private:
    static QSettings * settingsNow; // using profile
    static QSettings * settingsMain; // store last used profile name
    static QString appDirPath; // path to store app generated stuff
};

#endif // SETTINGS_H
