/*
 * Settings.hpp
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

#pragma once

#include <QSettings>
#include <QVariant>
#include <QMutex>

#include "defs.h"
#include "debug.h"

// Default values:

// LightpackMain.conf
// [General]
#define PROFILE_DEFAULT_NAME            "Lightpack"
#define LANGUAGE_DEFAULT_NAME           "<System>" /* system lang */
#define DEBUG_LEVEL_DEFAULT             Debug::LowLevel
#define ENABLE_API_DEFAULT              true
#define API_PORT_DEFAULT                3636
#define EXPERT_MODE_ENABLED_DEFAULT     false
#define CONNECTED_DEVICE_DEFAULT        "Lightpack"
#ifdef ALIEN_FX_SUPPORTED
#   define SUPPORTED_DEVICES            "Lightpack,AlienFx,Virtual"
#else
#   define SUPPORTED_DEVICES            "Lightpack,Virtual"
#endif

// ProfileName.ini
// [General]
#define GRAB_SLOWDOWN_MS_DEFAULT_VALUE          50
#define IS_AMBILIGHT_ON_DEFAULT_VALUE           true
#define IS_AVG_COLORS_ON_DEFAULT_VALUE          false
#define MINIMUM_LEVEL_OF_SENSITIVITY_DEFAULT    3
#define GAMMA_CORRECTION_DEFAULT_VALUE          2.0

#ifdef WINAPI_GRAB_SUPPORT
#   define GRAB_MODE_DEFAULT    "Winapi"
#elif defined(X11_GRAB_SUPPORT)
#   define GRAB_MODE_DEFAULT    "X11"
#else
#   define GRAB_MODE_DEFAULT    "Qt"
#endif

#define MODE_DEFAULT                        "Grab"
#define SPEED_MOOD_LAMP_DEFAULT_VALUE       50
#define MOOD_LAMP_MODE_DEFAULT_VALUE        true
#define MOOD_LAMP_COLOR_DEFAULT_VALUE       "#00FF00"
#define BRIGHTNESS_DEFAULT_VALUE            200

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

#define LED_COEF_MIN_VALUE  0.1
#define LED_COEF_MAX_VALUE  3

enum LightpackMode { Grab, MoodLamp };

enum GrabMode {
    QtGrabMode
#ifdef WINAPI_GRAB_SUPPORT
    ,WinAPIGrabMode
#endif
#ifdef X11_GRAB_SUPPORT
    ,X11GrabMode
#endif
};

enum SupportedDevices {
    SupportedDevice_Lightpack,
    SupportedDevice_AlienFx,
    SupportedDevice_Virtual,
    SupportedDevice_Default = SupportedDevice_Lightpack
};

class Settings : public QObject
{
    Q_OBJECT

public:
    static void Initialize(const QString & applicationDirPath, bool isSetDebugLevelFromConfig);
    static void resetDefaults();

    static QStringList findAllProfiles();
    static void loadOrCreateProfile(const QString & configName);
    static void renameCurrentProfile(const QString & configName);
    static void removeCurrentProfile();

    static QString getFileName();
    static QString getApplicationDirPath();
    static QPoint getDefaultPosition(int ledIndex);

    // Main
    static QString getLastProfileName();
    static QString getLanguage();
    static void setLanguage(const QString & language);
    static int getDebugLevel();
    static void setDebugLevel(int debugLvl);
    static bool isEnabledApi();
    static void setEnableApi(bool isEnabled);
    static int getApiPort();
    static void setApiPort(int apiPort);
    static QString getApiKey();
    static void setApiKey(const QString & apiKey);
    static bool isExpertModeEnabled();
    static void setExpertModeEnabled(bool isEnabled);
    static SupportedDevices getConnectedDevice();
    static void setConnectedDevice(SupportedDevices device);

    // Profile
    static int getGrabSlowdownMs();
    static void setGrabSlowdownMs(int value);
    static bool isAmbilightOn();
    static void setAmbilightOn(bool isEnabled);
    static bool isAvgColorsOn();
    static void setAvgColorsOn(bool isEnabled);
    static int getMinimumLevelOfSensitivity();
    static void setMinimumLevelOfSensitivity(int value);
    static double getGammaCorrection();
    static void setGammaCorrection(double gamma);
    static int getBrightness();
    static void setBrightness(int value);

    static GrabMode getGrabMode();
    static void setGrabMode(GrabMode grabMode);
    static LightpackMode getMode();
    static void setMode(LightpackMode mode);
    static bool isMoodLampLiquidMode();
    static void setMoodLampLiquidMode(bool isLiquidMode);
    static QColor getMoodLampColor();
    static void setMoodLampColor(QColor color);
    static int getMoodLampSpeed();
    static void setMoodLampSpeed(int value);
    static int getFwTimerPrescallerIndex();
    static void setFwTimerPrescallerIndex(int value);
    static int getFwTimerOCR();
    static void setFwTimerOCR(int value);
    static int getFwColorDepth();
    static void setFwColorDepth(int value);
    static int getFwSmoothSlowdown();
    static void setFwSmoothSlowdown(int value);

    static double getLedCoefRed(int ledIndex);
    static double getLedCoefGreen(int ledIndex);
    static double getLedCoefBlue(int ledIndex);

    static void setLedCoefRed(int ledIndex, double value);
    static void setLedCoefGreen(int ledIndex, double value);
    static void setLedCoefBlue(int ledIndex, double value);

    static QSize getLedSize(int ledIndex);
    static void setLedSize(int ledIndex, QSize size);
    static QPoint getLedPosition(int ledIndex);
    static void setLedPosition(int ledIndex, QPoint position);
    static bool isLedEnabled(int ledIndex);
    static void setLedEnabled(int ledIndex, bool isEnabled);

private:    
    static int getValidGrabSlowdownMs(int value);
    static int getValidMoodLampSpeed(int value);
    static void setValidLedCoef(int ledIndex, const QString & keyCoef, double coef);
    static double getValidLedCoef(int ledIndex, const QString & keyCoef);

    static void settingsInit(bool isResetDefault);
    static void setNewOption(const QString & name, const QVariant & value,
                            bool isForceSetOption = false, QSettings * settings = m_currentProfile);
    static void setNewOptionMain(const QString & name, const QVariant & value,
                                bool isForceSetOption = false);
    // forwarding to m_mainConfig object
    static void setValueMain(const QString & key, const QVariant & value);
    static QVariant valueMain(const QString & key);
    // forwarding to m_currentProfile object
    static void setValue(const QString & key, const QVariant & value);
    static QVariant value(const QString & key);



private:
    static QMutex m_mutex; // for thread-safe access to QSettings* variables
    static QSettings * m_currentProfile; // using profile
    static QSettings * m_mainConfig;     // store last used profile name, locale and so on
    static QString m_applicationDirPath; // path to store app generated stuff
};
