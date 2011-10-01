/*
 * Settings.cpp
 *
 *  Created on: 22.02.2011
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

#include "Settings.hpp"

#include <QtDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QSize>
#include <QPoint>
#include <QFileInfo>
#include <QDir>
#include <QUuid>

#include "../../CommonHeaders/LEDS_COUNT.h"

#include "debug.h"

using namespace SettingsScope;

//
// This strings keys and values must be accessible only in current file
//
namespace SettingsScope
{
namespace Main
{
namespace Key
{
// [General]
static const QString ProfileLast = "ProfileLast";
static const QString Language = "Language";
static const QString DebugLevel = "DebugLevel";
static const QString IsExpertModeEnabled = "IsExpertModeEnabled";
static const QString ConnectedDevice = "ConnectedDevice";
static const QString SupportedDevices = "SupportedDevices";
// [API]
namespace Api
{
static const QString IsEnabled = "API/IsEnabled";
static const QString Port = "API/Port";
static const QString IsAuthEnabled = "API/IsAuthEnabled";
static const QString AuthKey = "API/AuthKey";
}
} /*Key*/
} /*Main*/

namespace Profile
{
namespace Key
{
// [General]
static const QString LightpackMode = "LightpackMode";
static const QString IsBacklightOn = "IsBacklightOn";
// [Grab]
namespace Grab
{
static const QString Mode = "Grab/Mode";
static const QString IsAvgColorsOn = "Grab/IsAvgColorsOn";
static const QString Slowdown = "Grab/Slowdown";
static const QString MinimumLevelOfSensitivity = "Grab/MinimumLevelOfSensitivity";
}
// [MoodLamp]
namespace MoodLamp
{
static const QString IsLiquidMode = "MoodLamp/LiquidMode";
static const QString Color = "MoodLamp/Color";
static const QString Speed = "MoodLamp/Speed";
}
// [Device]
namespace Device
{
static const QString RefreshDelay = "Device/RefreshDelay";
static const QString Smooth = "Device/Smooth";
static const QString Brightness = "Device/Brightness";
static const QString Gamma = "Device/Gamma";
}
// [LED_i]
namespace Led
{
static const QString Prefix = "LED_";
static const QString IsEnabled = "IsEnabled";
static const QString Size = "Size";
static const QString Position = "Position";
static const QString CoefRed = "CoefRed";
static const QString CoefGreen = "CoefGreen";
static const QString CoefBlue = "CoefBlue";
}
} /*Key*/

namespace Value
{

namespace LightpackMode
{
static const QString Ambilight = "Ambilight";
static const QString MoodLamp = "MoodLamp";
} /*LightpackMode*/

namespace GrabMode
{
static const QString Qt = "Qt";
static const QString WinAPI = "WinAPI";
static const QString X11 = "X11";
static const QString D3D9 = "D3D9";
} /*LightpackMode*/

} /*Value*/
} /*Profile*/
} /*SettingsScope*/

QMutex Settings::m_mutex;
QSettings * Settings::m_currentProfile;
QSettings * Settings::m_mainConfig; // LightpackMain.conf contains last profile

// Path to directory there store application generated stuff
QString Settings::m_applicationDirPath = "";

// Desktop should be initialized before call Settings::Initialize()
void Settings::Initialize( const QString & applicationDirPath, bool isDebugLevelObtainedFromCmdArgs)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    m_applicationDirPath = applicationDirPath;

    // Append to the end of dir path '/'
    if (m_applicationDirPath.lastIndexOf('/') != m_applicationDirPath.length() - 1)
        m_applicationDirPath += "/";

    m_mainConfig = new QSettings(m_applicationDirPath + "LightpackMain.conf", QSettings::IniFormat);
    m_mainConfig->setIniCodec("UTF-8");

    setNewOptionMain(Main::Key::ProfileLast,            Main::ProfileNameDefault);
    setNewOptionMain(Main::Key::Language,               Main::LanguageDefault);
    setNewOptionMain(Main::Key::DebugLevel,             Main::DebugLevelDefault);
    setNewOptionMain(Main::Key::IsExpertModeEnabled,    Main::IsExpertModeEnabledDefault);
    setNewOptionMain(Main::Key::ConnectedDevice,        Main::ConnectedDeviceDefault);
    setNewOptionMain(Main::Key::SupportedDevices,       Main::SupportedDevices, true /* always rewrite this information to main config */);
    setNewOptionMain(Main::Key::Api::IsEnabled,         Main::Api::IsEnabledDefault);
    setNewOptionMain(Main::Key::Api::Port,              Main::Api::PortDefault);
    setNewOptionMain(Main::Key::Api::IsAuthEnabled,     Main::Api::IsAuthEnabledDefault);
    // Generation AuthKey as new UUID
    setNewOptionMain(Main::Key::Api::AuthKey,           QUuid::createUuid().toString());

    if (isDebugLevelObtainedFromCmdArgs == false)
    {
        bool ok = false;
        int sDebugLevel = valueMain(Main::Key::DebugLevel).toInt(&ok);

        if (ok && sDebugLevel >= 0)
        {
            g_debugLevel = sDebugLevel;
            DEBUG_LOW_LEVEL << Q_FUNC_INFO << "debugLevel =" << g_debugLevel;
        } else {
            qWarning() << "DebugLevel in config has an invalid value, set the default" << Main::DebugLevelDefault;
            setValueMain(Main::Key::DebugLevel, Main::DebugLevelDefault);
            g_debugLevel = Main::DebugLevelDefault;
        }
    }

    QString profileLast = getLastProfileName();

    // Load last profile
    m_currentProfile = new QSettings(m_applicationDirPath + "Profiles/" + profileLast + ".ini", QSettings::IniFormat);
    m_currentProfile->setIniCodec("UTF-8");

    qDebug() << "Settings file:" << m_currentProfile->fileName();

    settingsInit(false);
}

//
//  Set all settings in current config to default values
//
void Settings::resetDefaults()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    settingsInit(true /* = reset to default values */);
}

QStringList Settings::findAllProfiles()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QFileInfo setsFile(Settings::getCurrentProfilePath());
    QFileInfoList iniFiles = setsFile.absoluteDir().entryInfoList(QStringList("*.ini"));

    QStringList settingsFiles;
    for(int i=0; i<iniFiles.count(); i++){
        QString compBaseName = iniFiles.at(i).completeBaseName();
        settingsFiles.append(compBaseName);
    }

    return settingsFiles;
}

void Settings::loadOrCreateProfile(const QString & profileName)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << profileName;

    QMutexLocker locker(&m_mutex);

    if (m_currentProfile != NULL)
    {
        // Copy current settings to new one
        QString profilesDir = QFileInfo(m_currentProfile->fileName()).absoluteDir().absolutePath();
        QString profileNewPath = profilesDir + "/" + profileName + ".ini";

        if (m_currentProfile->fileName() != profileNewPath)
            QFile::copy(m_currentProfile->fileName(), profileNewPath);

        delete m_currentProfile;
    }


    m_currentProfile = new QSettings(m_applicationDirPath + "Profiles/" + profileName + ".ini", QSettings::IniFormat );
    m_currentProfile->setIniCodec("UTF-8");

    locker.unlock();
    settingsInit(false);
    locker.relock();

    qDebug() << "Settings file:" << m_currentProfile->fileName();

    m_mainConfig->setValue(Main::Key::ProfileLast, profileName);
}

void Settings::renameCurrentProfile(const QString & profileName)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << profileName;

    QMutexLocker locker(&m_mutex);

    if (m_currentProfile == NULL)
    {
        qWarning() << "void Settings::renameCurrentConfig(): fail, m_currentProfile not initialized";
        return;
    }

    // Copy current settings to new one
    QString profilesDir = QFileInfo( m_currentProfile->fileName() ).absoluteDir().absolutePath();
    QString profileNewPath = profilesDir + "/" + profileName + ".ini";

    if (m_currentProfile->fileName() != profileNewPath)
    {
        QFile::rename(m_currentProfile->fileName(), profileNewPath);

        delete m_currentProfile;

        // Update m_currentProfile point to new QSettings with configName
        m_currentProfile = new QSettings(m_applicationDirPath + "Profiles/" + profileName + ".ini", QSettings::IniFormat );
        m_currentProfile->setIniCodec("UTF-8");

        qDebug() << "Settings file renamed:" << m_currentProfile->fileName();

        m_mainConfig->setValue(Main::Key::ProfileLast, profileName);
    }

    m_currentProfile->sync();
}

void Settings::removeCurrentProfile()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QMutexLocker locker(&m_mutex);

    if (m_currentProfile == NULL)
    {
        qWarning() << Q_FUNC_INFO << "current profile not loaded, nothing to remove";
        return;
    }

    bool result = QFile::remove( m_currentProfile->fileName() );

    if (result == false)
    {
        qWarning() << Q_FUNC_INFO << "QFile::remove(" << m_currentProfile->fileName() << ") fail";
        return;
    }

    delete m_currentProfile;
    m_currentProfile = NULL;

    m_mainConfig->setValue(Main::Key::ProfileLast, Main::ProfileNameDefault);
}

QString Settings::getCurrentProfileName()
{
    QMutexLocker locker(&m_mutex);

    DEBUG_MID_LEVEL << Q_FUNC_INFO << m_currentProfile->fileName();
    return QFileInfo(m_currentProfile->fileName()).completeBaseName();
}

QString Settings::getCurrentProfilePath()
{
    QMutexLocker locker(&m_mutex);

    DEBUG_MID_LEVEL << Q_FUNC_INFO << m_currentProfile->fileName();
    return m_currentProfile->fileName();
}

QString Settings::getApplicationDirPath()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << m_applicationDirPath;
    return m_applicationDirPath;
}

QPoint Settings::getDefaultPosition(int ledIndex)
{
    QPoint result;

    QRect screen = QApplication::desktop()->screenGeometry();

    int ledsCountDiv2 = LEDS_COUNT / 2;

    if (ledIndex < ledsCountDiv2)
    {
        result.setX(0);
    } else {
        result.setX(screen.width() - Profile::Led::SizeDefault.width());
    }

    int height = ledsCountDiv2 * Profile::Led::SizeDefault.height();

    int y = screen.height() / 2 - height / 2;

    result.setY(y + (ledIndex % ledsCountDiv2) * Profile::Led::SizeDefault.height());

    return result;
}

QString Settings::getLastProfileName()
{
    return valueMain(Main::Key::ProfileLast).toString();
}

QString Settings::getLanguage()
{
    return valueMain(Main::Key::Language).toString();
}

void Settings::setLanguage(const QString & language)
{
    setValueMain(Main::Key::Language, language);
}

int Settings::getDebugLevel()
{
    return valueMain(Main::Key::DebugLevel).toInt();
}

void Settings::setDebugLevel(int debugLvl)
{
    setValueMain(Main::Key::DebugLevel, debugLvl);
}

bool Settings::isApiEnabled()
{
    return valueMain(Main::Key::Api::IsEnabled).toBool();
}

void Settings::setIsApiEnabled(bool isEnabled)
{
    setValueMain(Main::Key::Api::IsEnabled, isEnabled);
}

int Settings::getApiPort()
{
    return valueMain(Main::Key::Api::Port).toInt();
}

void Settings::setApiPort(int apiPort)
{
    setValueMain(Main::Key::Api::Port, apiPort);
}

QString Settings::getApiAuthKey()
{
    return valueMain(Main::Key::Api::AuthKey).toString();
}

void Settings::setApiKey(const QString & apiKey)
{
    setValueMain(Main::Key::Api::AuthKey, apiKey);
}

bool Settings::isApiAuthEnabled()
{
    return valueMain(Main::Key::Api::IsAuthEnabled).toBool();
}

void Settings::setIsApiAuthEnabled(bool isEnabled)
{
    setValueMain(Main::Key::Api::IsAuthEnabled, isEnabled);
}

bool Settings::isExpertModeEnabled()
{   
    return valueMain(Main::Key::IsExpertModeEnabled).toBool();
}

void Settings::setExpertModeEnabled(bool isEnabled)
{
    setValueMain(Main::Key::IsExpertModeEnabled, isEnabled);
}

SupportedDevices::DeviceType Settings::getConnectedDevice()
{
    QString deviceName = valueMain(Main::Key::ConnectedDevice).toString();

    if (deviceName == "Lightpack")
        return SupportedDevices::LightpackDevice;
    else if (deviceName == "AlienFx")
        return SupportedDevices::AlienFxDevice;
    else if (deviceName == "Virtual")
        return SupportedDevices::VirtualDevice;
    else {
        qWarning() << Q_FUNC_INFO << "ConnectedDevice in lightpack main config file contains crap. Reset to default.";

        setConnectedDevice(SupportedDevices::DefaultDevice);
        return SupportedDevices::DefaultDevice;
    }
}

void Settings::setConnectedDevice(SupportedDevices::DeviceType device)
{
    QString deviceName;

    switch (device){
    case SupportedDevices::LightpackDevice:
        deviceName = "Lightpack";
        break;
    case SupportedDevices::AlienFxDevice:
        deviceName = "AlienFx";
        break;
    case SupportedDevices::VirtualDevice:
        deviceName = "Virtual";
        break;
    default:
        qWarning() << Q_FUNC_INFO << "'device' not found in SupportedDevices. Reset to default.";
        deviceName = Main::ConnectedDeviceDefault;
        return;
    }

    setValueMain(Main::Key::ConnectedDevice, deviceName);
}

int Settings::getGrabSlowdown()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    return getValidGrabSlowdown(value(Profile::Key::Grab::Slowdown).toInt());
}

void Settings::setGrabSlowdown(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    setValue(Profile::Key::Grab::Slowdown, getValidGrabSlowdown(value));
}

bool Settings::isBacklightOn()
{
    return value(Profile::Key::IsBacklightOn).toBool();
}

void Settings::setIsBacklightOn(bool isEnabled)
{
    setValue(Profile::Key::IsBacklightOn, isEnabled);
}

bool Settings::isGrabAvgColorsOn()
{
    return value(Profile::Key::Grab::IsAvgColorsOn).toBool();
}

void Settings::setAvgColorsOn(bool isEnabled)
{
    setValue(Profile::Key::Grab::IsAvgColorsOn, isEnabled);
}

int Settings::getGrabMinimumLevelOfSensitivity()
{
    return value(Profile::Key::Grab::MinimumLevelOfSensitivity).toInt();
}

void Settings::setMinimumLevelOfSensitivity(int value)
{
    setValue(Profile::Key::Grab::MinimumLevelOfSensitivity, value);
}

int Settings::getDeviceRefreshDelay()
{
    return value(Profile::Key::Device::RefreshDelay).toInt();
}

void Settings::setDeviceRefreshDelay(int value)
{
    setValue(Profile::Key::Device::RefreshDelay, value);
}

int Settings::getDeviceBrightness()
{
    return value(Profile::Key::Device::Brightness).toInt();
}

void Settings::setDeviceBrightness(int value)
{
    setValue(Profile::Key::Device::Brightness, value);
}

int Settings::getDeviceSmooth()
{
    return value(Profile::Key::Device::Smooth).toInt();
}

void Settings::setDeviceSmooth(int value)
{
    setValue(Profile::Key::Device::Smooth, value);
}

double Settings::getDeviceGamma()
{
    return getValidGammaCorrection(value(Profile::Key::Device::Gamma).toDouble());
}

void Settings::setDeviceGamma(double gamma)
{
    setValue(Profile::Key::Device::Gamma, getValidGammaCorrection(gamma));
}

Grab::Mode Settings::getGrabMode()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QString strGrabMode = value(Profile::Key::Grab::Mode).toString();

    if (strGrabMode == Profile::Value::GrabMode::Qt)
        return Grab::QtGrabMode;

#ifdef WINAPI_GRAB_SUPPORT
    if (strGrabMode == Profile::Value::GrabMode::WinAPI)
        return Grab::WinAPIGrabMode;
#endif

#ifdef D3D9_GRAB_SUPPORT
    if (strGrabMode == Profile::Value::GrabMode::D3D9)
        return Grab::D3D9GrabMode;
#endif

#ifdef X11_GRAB_SUPPORT
    if (strGrabMode == Profile::Value::GrabMode::X11)
        return Grab::X11GrabMode;
#endif

    qWarning() << Q_FUNC_INFO << "GrabMode contains invalid value, reset it to default.";
    setGrabMode(Profile::Grab::ModeDefault);

    return Profile::Grab::ModeDefault;
}

void Settings::setGrabMode(Grab::Mode grabMode)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << grabMode;

    QString strGrabMode;
    switch (grabMode)
    {
    case Grab::QtGrabMode:
        strGrabMode = Profile::Value::GrabMode::Qt;
        break;

#ifdef WINAPI_GRAB_SUPPORT
    case Grab::WinAPIGrabMode:
        strGrabMode = Profile::Value::GrabMode::WinAPI;
        break;
#endif

#ifdef D3D9_GRAB_SUPPORT
    case Grab::D3D9GrabMode:
        strGrabMode = Profile::Value::GrabMode::D3D9;
        break;
#endif

#ifdef X11_GRAB_SUPPORT
    case Grab::X11GrabMode:
        strGrabMode = Profile::Value::GrabMode::X11;
        break;
#endif

    default:
        qWarning() << "Switch on GrabMode =" << grabMode << "failed. Reset to default value.";
        strGrabMode = Profile::Grab::ModeDefault;
    }
    setValue(Profile::Key::Grab::Mode, strGrabMode);
}

Lightpack::Mode Settings::getLightpackMode()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QString strMode = value(Profile::Key::LightpackMode).toString();

    if (strMode == Profile::Value::LightpackMode::Ambilight)
    {
        return Lightpack::AmbilightMode;
    }
    else if (strMode == Profile::Value::LightpackMode::MoodLamp)
    {
        return Lightpack::MoodLampMode;
    }
    else
    {
        qWarning() << Q_FUNC_INFO << "Read fail. Reset to default value = " << Lightpack::Default;

        setLightpackMode(Lightpack::Default);
        return Lightpack::Default;
    }
}

void Settings::setLightpackMode(Lightpack::Mode mode)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << mode;

    if (mode == Lightpack::AmbilightMode)
    {
        setValue(Profile::Key::LightpackMode, Profile::Value::LightpackMode::Ambilight);
    }
    else if (mode == Lightpack::MoodLampMode)
    {
        setValue(Profile::Key::LightpackMode, Profile::Value::LightpackMode::MoodLamp);
    }
    else
    {
        qCritical() << Q_FUNC_INFO << "Invalid value =" << mode;
    }
}

bool Settings::isMoodLampLiquidMode()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    return value(Profile::Key::MoodLamp::IsLiquidMode).toBool();
}

void Settings::setMoodLampLiquidMode(bool value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    setValue(Profile::Key::MoodLamp::IsLiquidMode, value );
}

QColor Settings::getMoodLampColor()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    return QColor(value(Profile::Key::MoodLamp::Color).toString());
}

void Settings::setMoodLampColor(QColor value)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << value.name();
    setValue(Profile::Key::MoodLamp::Color, value.name() );
}

int Settings::getMoodLampSpeed()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    return getValidMoodLampSpeed(value(Profile::Key::MoodLamp::Speed).toInt());
}

void Settings::setMoodLampSpeed(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    setValue(Profile::Key::MoodLamp::Speed, getValidMoodLampSpeed(value));
}

double Settings::getLedCoefRed(int ledIndex)
{
    return getValidLedCoef(ledIndex, Profile::Key::Led::CoefRed);
}

double Settings::getLedCoefGreen(int ledIndex)
{
    return getValidLedCoef(ledIndex, Profile::Key::Led::CoefGreen);
}

double Settings::getLedCoefBlue(int ledIndex)
{
    return getValidLedCoef(ledIndex, Profile::Key::Led::CoefBlue);
}

void Settings::setLedCoefRed(int ledIndex, double value)
{
    setValidLedCoef(ledIndex, Profile::Key::Led::CoefRed, value);
}

void Settings::setLedCoefGreen(int ledIndex, double value)
{
    setValidLedCoef(ledIndex, Profile::Key::Led::CoefGreen, value);
}

void Settings::setLedCoefBlue(int ledIndex, double value)
{
    setValidLedCoef(ledIndex, Profile::Key::Led::CoefBlue, value);
}

QSize Settings::getLedSize(int ledIndex)
{
    return value(Profile::Key::Led::Prefix + QString::number(ledIndex + 1) + "/" + Profile::Key::Led::Size).toSize();
}

void Settings::setLedSize(int ledIndex, QSize size)
{
    setValue(Profile::Key::Led::Prefix + QString::number(ledIndex + 1) + "/" + Profile::Key::Led::Size, size);
}

QPoint Settings::getLedPosition(int ledIndex)
{
    return value(Profile::Key::Led::Prefix + QString::number(ledIndex + 1) + "/" + Profile::Key::Led::Position).toPoint();
}

void Settings::setLedPosition(int ledIndex, QPoint position)
{
    setValue(Profile::Key::Led::Prefix + QString::number(ledIndex + 1) + "/" + Profile::Key::Led::Position, position);
}

bool Settings::isLedEnabled(int ledIndex)
{
    return value(Profile::Key::Led::Prefix + QString::number(ledIndex + 1) + "/" + Profile::Key::Led::IsEnabled).toBool();
}

void Settings::setLedEnabled(int ledIndex, bool isEnabled)
{
    setValue(Profile::Key::Led::Prefix + QString::number(ledIndex + 1) + "/" + Profile::Key::Led::IsEnabled, isEnabled);
}

double Settings::getValidGammaCorrection(double value)
{
    if (value <= Profile::Device::GammaMin)
        value = Profile::Device::GammaMin;
    else if (value >= Profile::Device::GammaMax)
        value = Profile::Device::GammaMax;
    return value;
}

int Settings::getValidGrabSlowdown(int value)
{
    if (value < Profile::Device::SmoothMin)
        value = Profile::Device::SmoothMin;
    else if (value > Profile::Device::SmoothMax)
        value = Profile::Device::SmoothMax;
    return value;
}

int Settings::getValidMoodLampSpeed(int value)
{
    if (value < Profile::MoodLamp::SpeedMin)
        value = Profile::MoodLamp::SpeedMin;
    else if (value > Profile::MoodLamp::SpeedMax)
        value = Profile::MoodLamp::SpeedMax;
    return value;
}

void Settings::setValidLedCoef(int ledIndex, const QString & keyCoef, double coef)
{
    if (coef < Profile::Led::CoefMin || coef > Profile::Led::CoefMax){
        QString error = "Error: outside the valid values (coef < " +
                QString::number(Profile::Led::CoefMin) + " || coef > " + QString::number(Profile::Led::CoefMax) + ").";

        qWarning() << Q_FUNC_INFO << "Settings bad value"
                   << "[" + Profile::Key::Led::Prefix + QString::number(ledIndex + 1) + "]"
                   << keyCoef
                   << error
                   << "Convert to double error. Set it to default value" << keyCoef << "=" << Profile::Led::CoefDefault;
        coef = Profile::Led::CoefDefault;
        Settings::setValue(Profile::Key::Led::Prefix + QString::number(ledIndex + 1) + "/" + keyCoef, coef);
    }
}

double Settings::getValidLedCoef(int ledIndex, const QString & keyCoef)
{
    bool ok = false;
    double coef = Settings::value(Profile::Key::Led::Prefix + QString::number(ledIndex + 1) + "/" + keyCoef).toDouble(&ok);
    QString error;
    if (ok == false){
        error = "Error: Convert to double.";
    } else if (coef < Profile::Led::CoefMin || coef > Profile::Led::CoefMax){
        QString error = "Error: outside the valid values (coef < " +
                QString::number(Profile::Led::CoefMin) + " || coef > " + QString::number(Profile::Led::CoefMax) + ").";
    } else {
        // OK
        return coef;
    }
    // Have an error
    qWarning() << Q_FUNC_INFO << "Settings bad value"
               << "[" + Profile::Key::Led::Prefix + QString::number(ledIndex + 1) + "]"
               << keyCoef
               << error
               << "Set it to default value" << keyCoef << "=" << Profile::Led::CoefDefault;
    coef = Profile::Led::CoefDefault;
    Settings::setValue(Profile::Key::Led::Prefix + QString::number(ledIndex + 1) + "/" + keyCoef, coef);
    return coef;
}

//
//  Check and/or initialize settings
//
void Settings::settingsInit(bool isResetDefault)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << isResetDefault;

    // [General]
    setNewOption(Profile::Key::LightpackMode, Profile::LightpackModeDefault, isResetDefault);
    setNewOption(Profile::Key::IsBacklightOn, Profile::IsBacklightOnDefault, isResetDefault);
    // [Grab]
    setNewOption(Profile::Key::Grab::Mode,          Profile::Grab::ModeDefaultString, isResetDefault);
    setNewOption(Profile::Key::Grab::IsAvgColorsOn, Profile::Grab::IsAvgColorsOnDefault, isResetDefault);
    setNewOption(Profile::Key::Grab::Slowdown,      Profile::Grab::SlowdownDefault, isResetDefault);
    setNewOption(Profile::Key::Grab::MinimumLevelOfSensitivity, Profile::Grab::MinimumLevelOfSensitivityDefault, isResetDefault);
    // [MoodLamp]
    setNewOption(Profile::Key::MoodLamp::IsLiquidMode,  Profile::MoodLamp::IsLiquidMode, isResetDefault);
    setNewOption(Profile::Key::MoodLamp::Color,         Profile::MoodLamp::ColorDefault, isResetDefault);
    setNewOption(Profile::Key::MoodLamp::Speed,         Profile::MoodLamp::SpeedDefault, isResetDefault);
    // [Device]
    setNewOption(Profile::Key::Device::Brightness,  Profile::Device::BrightnessDefault, isResetDefault);
    setNewOption(Profile::Key::Device::Smooth,      Profile::Device::SmoothDefault, isResetDefault);
    setNewOption(Profile::Key::Device::Gamma,       Profile::Device::GammaDefault, isResetDefault);

    QPoint ledPosition;

    for (int i = 0; i < LEDS_COUNT; i++)
    {
        ledPosition = getDefaultPosition(i);

        setNewOption(Profile::Key::Led::Prefix + QString::number(i + 1) + "/" + Profile::Key::Led::IsEnabled,
                     Profile::Led::IsEnabledDefault, isResetDefault);
        setNewOption(Profile::Key::Led::Prefix + QString::number(i + 1) + "/" + Profile::Key::Led::Position,
                     ledPosition, isResetDefault);
        setNewOption(Profile::Key::Led::Prefix + QString::number(i + 1) + "/" + Profile::Key::Led::Size,
                     Profile::Led::SizeDefault, isResetDefault);

        setNewOption(Profile::Key::Led::Prefix + QString::number(i + 1) + "/" + Profile::Key::Led::CoefRed,
                     Profile::Led::CoefDefault, isResetDefault);
        setNewOption(Profile::Key::Led::Prefix + QString::number(i + 1) + "/" + Profile::Key::Led::CoefGreen,
                     Profile::Led::CoefDefault, isResetDefault);
        setNewOption(Profile::Key::Led::Prefix + QString::number(i + 1) + "/" + Profile::Key::Led::CoefBlue,
                     Profile::Led::CoefDefault, isResetDefault);
    }

    QMutexLocker locker(&m_mutex);
    m_currentProfile->sync();
}


void Settings::setNewOption(const QString & name, const QVariant & value,
                                        bool isForceSetOption, QSettings * settings /*= m_currentProfile*/)
{
    QMutexLocker locker(&m_mutex);

    if (isForceSetOption)
    {
        settings->setValue(name, value);
    } else {
        if (settings->contains(name) == false)
        {
            qDebug() << "Settings:"<< name << "not found."
                    << "Set it to default value: " << value.toString();

            settings->setValue(name, value);
        }
        // else option exists do nothing
    }
}

void Settings::setNewOptionMain(const QString & name, const QVariant & value, bool isForceSetOption /*= false*/)
{
    setNewOption(name, value, isForceSetOption, m_mainConfig);
}

void Settings::setValueMain(const QString & key, const QVariant & value)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << key;

    QMutexLocker locker(&m_mutex);
    m_mainConfig->setValue(key, value);
}

QVariant Settings::valueMain( const QString & key)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << key;

    QMutexLocker locker(&m_mutex);
    return m_mainConfig->value(key);
}

void Settings::setValue(const QString & key, const QVariant & value)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << key;

     QMutexLocker locker(&m_mutex);
     m_currentProfile->setValue(key, value);
}

QVariant Settings::value( const QString & key)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << key;

    QMutexLocker locker(&m_mutex);
    return m_currentProfile->value(key);
}
