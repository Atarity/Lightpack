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

#include "SettingsDefaults.hpp"
#include "enums.hpp"
#include "defs.h"
#include "debug.h"

namespace SettingsScope
{

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

    static QString getCurrentProfileName();
    static QString getCurrentProfilePath();
    static QString getApplicationDirPath();
    static QPoint getDefaultPosition(int ledIndex);

    // Main
    static QString getLastProfileName();
    static QString getLanguage();
    static void setLanguage(const QString & language);
    static int getDebugLevel();
    static void setDebugLevel(int debugLvl);
    static bool isApiEnabled();
    static void setIsApiEnabled(bool isEnabled);
    static int getApiPort();
    static void setApiPort(int apiPort);
    static QString getApiAuthKey();
    static void setApiKey(const QString & apiKey);
    static bool isApiAuthEnabled();
    static void setIsApiAuthEnabled(bool isEnabled);
    static bool isExpertModeEnabled();
    static void setExpertModeEnabled(bool isEnabled);
    static SupportedDevices::DeviceType getConnectedDevice();
    static void setConnectedDevice(SupportedDevices::DeviceType device);
    static QString getConnectedDeviceName();
    static void setConnectedDeviceName(const QString & deviceName);
    static QStringList getSupportedDevices();
    // [SerialPort]
    static QString getSerialPortName();
    static void setSerialPortName(const QString & port);
    static QString getSerialPortBaudRate();
    static void setSerialPortBaudRate(const QString & baud);
    static QStringList getSupportedSerialPortBaudRates();
    static bool isConnectedDeviceUsesSerialPort();
    // [Adalight | Ardulight | Lightpack | ... | Virtual]
    static void setNumberOfLeds(SupportedDevices::DeviceType device, int numberOfLeds);
    static int getNumberOfLeds(SupportedDevices::DeviceType device);


    // Profile
    static int getGrabSlowdown();
    static void setGrabSlowdown(int value);
    static bool isBacklightOn();
    static void setIsBacklightOn(bool isEnabled);
    static bool isGrabAvgColorsOn();
    static void setAvgColorsOn(bool isEnabled);
    static bool isUSB_SendDataOnlyIfColorsChanges();
    static void setUSB_SendDataOnlyIfColorsChanges(bool isEnabled);
    static int getGrabMinimumLevelOfSensitivity();
    static void setMinimumLevelOfSensitivity(int value);
    // [Device]
    static int getDeviceRefreshDelay();
    static void setDeviceRefreshDelay(int value);
    static int getDeviceBrightness();
    static void setDeviceBrightness(int value);
    static int getDeviceSmooth();
    static void setDeviceSmooth(int value);
    static double getDeviceGamma();
    static void setDeviceGamma(double gamma);

    static Grab::Mode getGrabMode();
    static void setGrabMode(Grab::Mode grabMode);
    static Lightpack::Mode getLightpackMode();
    static void setLightpackMode(Lightpack::Mode mode);
    static bool isMoodLampLiquidMode();
    static void setMoodLampLiquidMode(bool isLiquidMode);
    static QColor getMoodLampColor();
    static void setMoodLampColor(QColor color);
    static int getMoodLampSpeed();
    static void setMoodLampSpeed(int value);

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
    static int getValidDeviceRefreshDelay(int value);
    static int getValidDeviceBrightness(int value);
    static int getValidDeviceSmooth(int value);
    static double getValidDeviceGamma(double value);
    static int getValidGrabSlowdown(int value);
    static int getValidMoodLampSpeed(int value);
    static void setValidLedCoef(int ledIndex, const QString & keyCoef, double coef);
    static double getValidLedCoef(int ledIndex, const QString & keyCoef);

    static void initCurrentProfile(bool isResetDefault);
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

    static void initDevicesMap();



private:
    static QMutex m_mutex; // for thread-safe access to QSettings* variables
    static QSettings * m_currentProfile; // using profile
    static QSettings * m_mainConfig;     // store last used profile name, locale and so on
    static QString m_applicationDirPath; // path to store app generated stuff
    static QMap<SupportedDevices::DeviceType, QString> m_devicesTypeToNameMap;
    static QMap<SupportedDevices::DeviceType, QString> m_devicesTypeToKeyNumberOfLedsMap;
};
} /*SettingsScope*/
