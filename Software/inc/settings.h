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
#define DEBUG_LEVEL_DEFAULT             Debug::LowLevel
#define EXPERT_MODE_ENABLED_DEFAULT     false

// ProfileName.ini
// [General]
#define GRAB_SLOWDOWN_MS_DEFAULT_VALUE          50
#define IS_AMBILIGHT_ON_DEFAULT_VALUE           true
#define IS_AVG_COLORS_ON_DEFAULT_VALUE          false
#define MINIMUM_LEVEL_OF_SENSITIVITY_DEFAULT    3
#define GAMMA_CORRECTION_DEFAULT_VALUE          2.0

#define MODE_DEFAULT    "Grab"
#define SPEED_MOOD_LAMP_DEFAULT_VALUE 50
#define MOOD_LAMP_MODE_DEFAULT_VALUE true
#define MOOD_LAMP_COLOR_DEFAULT_VALUE "#00FF00"
#define BRIGHTNESS_DEFAULT_VALUE  200

// [Firmware]
#define FW_TIMER_PRESCALLER_INDEX_DEFAULT_VALUE     0 /* prescaller == 1 */
#define FW_TIMER_OCR_DEFAULT_VALUE                  100
#define FW_COLOR_DEPTH_DEFAULT_VALUE                128
#define FW_SMOOTH_SLOWDOWN_DEFAULT                  100

// [LED_i]
#define LED_FIELD_WIDTH_DEFAULT_VALUE       150
#define LED_FIELD_HEIGHT_DEFAULT_VALUE      150
#define LED_FIELD_SIZE_DEFAULT_VALUE        QSize(LED_FIELD_WIDTH_DEFAULT_VALUE, LED_FIELD_HEIGHT_DEFAULT_VALUE)
#define LED_COEF_RGB_DEFAULT_VALUE          1
#define LED_IS_ENABLED_DEFAULT_VALUE        true

enum LightpackMode { Grab, MoodLamp };

class Settings : public QObject
{
    Q_OBJECT

public:
    static void Initialize(const QString & applicationDirPath, bool isSetDebugLevelFromConfig);

    // Simple functions forwarding to m_currentProfile object
    static void setValue(const QString & key, const QVariant & value);
    static QVariant value(const QString & key);
    static QString fileName();

    // forwarding to m_mainConfig object
    static void setValueMain(const QString & key, const QVariant & value);
    static QVariant valueMain(const QString & key);

    static void loadOrCreateConfig(const QString & configName);
    static void renameCurrentConfig(const QString & configName);
    static void removeCurrentConfig();

    static QString lastProfileName();
    static QString getApplicationDirPath();
    static QPoint getDefaultPosition(int ledIndex);
    static void resetDefaults();

    //
    static bool isExpertModeEnabled();
    static void setExpertModeEnabled(bool isEnabled);
    static LightpackMode getMode();
    static void setMode(LightpackMode mode);
    static bool isMoodLampLiquidMode();
    static void setMoodLampLiquidMode(bool isLiquidMode);
    static QColor getMoodLampColor();
    static void setMoodLampColor(QColor color);
    static int getMoodLampSpeed();
    static void setMoodLampSpeed(int value);
    static int getGrabSlowdownMs();
    static void setGrabSlowdownMs(int value);
private:
    static void settingsInit(bool isResetDefault);
    static void setNewOption(const QString & name, const QVariant & value,
                            bool isForceSetOption = false, QSettings * settings = m_currentProfile);
    static void setNewOptionMain(const QString & name, const QVariant & value,
                                bool isForceSetOption = false);

private:
    static QSettings * m_currentProfile; // using profile
    static QSettings * m_mainConfig;     // store last used profile name, locale and so on
    static QString m_applicationDirPath; // path to store app generated stuff
};

#endif // SETTINGS_H
