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

//
// This strings keys must be accessible only in current file
//
// Main keys
#define KEY_PROFILE_LAST            "ProfileLast"
#define KEY_LANGUAGE                "Language"
#define KEY_DEBUG_LEVEL             "DebugLevel"
#define KEY_API_PORT                "ApiPort"
#define KEY_ENABLE_API              "EnableApi"
#define KEY_API_KEY                 "ApiKey"
#define KEY_EXPERT_MODE_ENABLED     "ExpertModeEnabled"
#define KEY_CONNECTED_DEVICE        "ConnectedDevice"
#define KEY_SUPPORTED_DEVICES       "SupportedDevices"

// Profile keys
#define KEY_GRAB_MODE               "GrabMode"
#define KEY_MODE                    "Mode"

#define KEY_GRAB_SLOWDOWN_MS        "GrabSlowdownMs"
#define KEY_IS_BACKLIGHT_ON         "IsBacklightOn"
#define KEY_IS_AVG_COLORS_ON        "IsAvgColorsOn"
#define KEY_MIN_LVL_SENSITIVITY     "MinimumLevelOfSensitivity"
#define KEY_GAMMA_CORRECTION        "GammaCorrection"
#define KEY_MOOD_LAMP_LIQUID_MODE   "MoodLampLiquidMode"
#define KEY_MOOD_LAMP_COLOR         "MoodLampColor"
#define KEY_MOOD_LAMP_SPEED         "MoodLampSpeed"
#define KEY_BRIGHTNESS              "Brightness"
#define KEY_FW_TIM_PRESCALLER       "Firmware/TimerPrescallerIndex"
#define KEY_FW_TIM_OCR              "Firmware/TimerOCR"
#define KEY_FW_COLOR_DEPTH          "Firmware/ColorDepth"
#define KEY_FW_SMOOTH_SLOWDOWN      "Firmware/SmoothSlowdown"
#define KEY_LED_PREFIX              "LED_"
#define KEY_LED_CRED                "CoefRed"
#define KEY_LED_CGREEN              "CoefGreen"
#define KEY_LED_CBLUE               "CoefBlue"
#define KEY_LED_SIZE                "Size"
#define KEY_LED_POSITION            "Position"
#define KEY_LED_IS_ENABLED          "IsEnabled"

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

    setNewOptionMain(KEY_PROFILE_LAST,          PROFILE_DEFAULT_NAME);
    setNewOptionMain(KEY_LANGUAGE,              LANGUAGE_DEFAULT_NAME);
    setNewOptionMain(KEY_DEBUG_LEVEL,           DEBUG_LEVEL_DEFAULT);
    setNewOptionMain(KEY_API_PORT,              API_PORT_DEFAULT);
    setNewOptionMain(KEY_ENABLE_API,            ENABLE_API_DEFAULT);
    setNewOptionMain(KEY_API_KEY,               QUuid::createUuid().toString());
    setNewOptionMain(KEY_EXPERT_MODE_ENABLED,   EXPERT_MODE_ENABLED_DEFAULT);
    setNewOptionMain(KEY_CONNECTED_DEVICE,      CONNECTED_DEVICE_DEFAULT);
    setNewOptionMain(KEY_SUPPORTED_DEVICES,     SUPPORTED_DEVICES, true /* always rewrite this information to main config */);

    if (isDebugLevelObtainedFromCmdArgs == false)
    {
        bool ok = false;
        int sDebugLevel = valueMain(KEY_DEBUG_LEVEL).toInt(&ok);

        if (ok && sDebugLevel >= 0)
        {
            g_debugLevel = sDebugLevel;
            DEBUG_LOW_LEVEL << Q_FUNC_INFO << "debugLevel =" << g_debugLevel;
        } else {
            qWarning() << "DebugLevel in config has an invalid value, set the default" << DEBUG_LEVEL_DEFAULT;
            setValueMain(KEY_DEBUG_LEVEL, DEBUG_LEVEL_DEFAULT);
            g_debugLevel = DEBUG_LEVEL_DEFAULT;
        }
    }

    QString profileLast = valueMain(KEY_PROFILE_LAST).toString();

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

    m_mainConfig->setValue(KEY_PROFILE_LAST, profileName);
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

        m_mainConfig->setValue(KEY_PROFILE_LAST, profileName);
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

    m_mainConfig->setValue(KEY_PROFILE_LAST, PROFILE_DEFAULT_NAME);
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
        result.setX(screen.width() - LED_FIELD_WIDTH_DEFAULT_VALUE);
    }

    int height = ledsCountDiv2 * LED_FIELD_HEIGHT_DEFAULT_VALUE;

    int y = screen.height() / 2 - height / 2;

    result.setY(y + (ledIndex % ledsCountDiv2) * LED_FIELD_HEIGHT_DEFAULT_VALUE);

    return result;
}

QString Settings::getLastProfileName()
{
    return valueMain(KEY_PROFILE_LAST).toString();
}

QString Settings::getLanguage()
{
    return valueMain(KEY_LANGUAGE).toString();
}

void Settings::setLanguage(const QString & language)
{
    setValueMain(KEY_LANGUAGE, language);
}

int Settings::getDebugLevel()
{
    return valueMain(KEY_DEBUG_LEVEL).toInt();
}

void Settings::setDebugLevel(int debugLvl)
{
    setValueMain(KEY_DEBUG_LEVEL, debugLvl);
}

bool Settings::isEnabledApi()
{
    return valueMain(KEY_ENABLE_API).toBool();
}

void Settings::setEnableApi(bool isEnabled)
{
    setValueMain(KEY_ENABLE_API, isEnabled);
}

int Settings::getApiPort()
{
    return valueMain(KEY_API_PORT).toInt();
}

void Settings::setApiPort(int apiPort)
{
    setValueMain(KEY_API_PORT, apiPort);
}

QString Settings::getApiKey()
{
    return valueMain(KEY_API_KEY).toString();
}

void Settings::setApiKey(const QString & apiKey)
{
    setValueMain(KEY_API_KEY, apiKey);
}

bool Settings::isExpertModeEnabled()
{   
    return valueMain(KEY_EXPERT_MODE_ENABLED).toBool();
}

void Settings::setExpertModeEnabled(bool isEnabled)
{
    setValueMain(KEY_EXPERT_MODE_ENABLED, isEnabled);
}

SupportedDevices::DeviceType Settings::getConnectedDevice()
{
    QString deviceName = valueMain(KEY_CONNECTED_DEVICE).toString();

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
        deviceName = CONNECTED_DEVICE_DEFAULT;
        return;
    }

    setValueMain(KEY_CONNECTED_DEVICE, deviceName);
}

int Settings::getGrabSlowdownMs()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    return getValidGrabSlowdownMs(value(KEY_GRAB_SLOWDOWN_MS).toInt());
}

void Settings::setGrabSlowdownMs(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    setValue(KEY_GRAB_SLOWDOWN_MS, getValidGrabSlowdownMs(value));
}

bool Settings::isBacklightOn()
{
    return value(KEY_IS_BACKLIGHT_ON).toBool();
}

void Settings::setIsBacklightOn(bool isEnabled)
{
    setValue(KEY_IS_BACKLIGHT_ON, isEnabled);
}

bool Settings::isAvgColorsOn()
{
    return value(KEY_IS_AVG_COLORS_ON).toBool();
}

void Settings::setAvgColorsOn(bool isEnabled)
{
    setValue(KEY_IS_AVG_COLORS_ON, isEnabled);
}

int Settings::getMinimumLevelOfSensitivity()
{
    return value(KEY_MIN_LVL_SENSITIVITY).toInt();
}

void Settings::setMinimumLevelOfSensitivity(int value)
{
    setValue(KEY_MIN_LVL_SENSITIVITY, value);
}

double Settings::getGammaCorrection()
{
    return getValidGammaCorrection(value(KEY_GAMMA_CORRECTION).toDouble());
}

void Settings::setGammaCorrection(double gamma)
{
    setValue(KEY_GAMMA_CORRECTION, getValidGammaCorrection(gamma));
}

int Settings::getBrightness()
{
    return value(KEY_BRIGHTNESS).toInt();
}

void Settings::setBrightness(int value)
{
    setValue(KEY_BRIGHTNESS, value);
}

Grab::Mode Settings::getGrabMode()
{
    QString strGrabMode = value(KEY_GRAB_MODE).toString().toLower();
#ifdef WINAPI_GRAB_SUPPORT
    if (strGrabMode == "winapi")
        return Grab::WinAPIGrabMode;
#endif
#ifdef X11_GRAB_SUPPORT
    if (strGrabMode == "x11")
        return Grab::X11GrabMode;
#endif
    return Grab::QtGrabMode;
}

void Settings::setGrabMode(Grab::Mode grabMode)
{
    QString strGrabMode;
    switch (grabMode)
    {
#ifdef WINAPI_GRAB_SUPPORT
    case Grab::WinAPIGrabMode:
        strGrabMode = "Winapi";
        break;
#endif
#ifdef X11_GRAB_SUPPORT
    case Grab::X11GrabMode:
        strGrabMode = "X11";
        break;
#endif
    default:
        strGrabMode = "Qt";
    }
    setValue(KEY_GRAB_MODE, strGrabMode);
}

Lightpack::Mode Settings::getMode()
{
    QString strMode = value(KEY_MODE).toString();
    if (strMode == "grab")
        return Lightpack::GrabScreenMode;
    else
        return Lightpack::MoodLampMode;
}

void Settings::setMode(Lightpack::Mode mode)
{
    QString strMode;
    if(mode == Lightpack::GrabScreenMode)
        strMode = "Grab";
    else
        strMode = "MoodLamp";
    setValue(KEY_MODE, strMode);
}

bool Settings::isMoodLampLiquidMode()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    return value(KEY_MOOD_LAMP_LIQUID_MODE).toBool();
}

void Settings::setMoodLampLiquidMode(bool value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    setValue(KEY_MOOD_LAMP_LIQUID_MODE, value );
}

QColor Settings::getMoodLampColor()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    return QColor(value(KEY_MOOD_LAMP_COLOR).toString());
}

void Settings::setMoodLampColor(QColor value)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << value.name();
    setValue(KEY_MOOD_LAMP_COLOR, value.name() );
}

int Settings::getMoodLampSpeed()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    return getValidMoodLampSpeed(value(KEY_MOOD_LAMP_SPEED).toInt());
}

void Settings::setMoodLampSpeed(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    setValue(KEY_MOOD_LAMP_SPEED, getValidMoodLampSpeed(value));
}

int Settings::getFwTimerPrescallerIndex()
{
    return value(KEY_FW_TIM_PRESCALLER).toInt();
}

void Settings::setFwTimerPrescallerIndex(int value)
{
    setValue(KEY_FW_TIM_PRESCALLER, value);
}

int Settings::getFwTimerOCR()
{
    return value(KEY_FW_TIM_OCR).toInt();
}

void Settings::setFwTimerOCR(int value)
{
    setValue(KEY_FW_TIM_OCR, value);
}

int Settings::getFwColorDepth()
{
    return value(KEY_FW_COLOR_DEPTH).toInt();
}

void Settings::setFwColorDepth(int value)
{
    setValue(KEY_FW_COLOR_DEPTH, value);
}

int Settings::getFwSmoothSlowdown()
{
    return value(KEY_FW_SMOOTH_SLOWDOWN).toInt();
}

void Settings::setFwSmoothSlowdown(int value)
{
    setValue(KEY_FW_SMOOTH_SLOWDOWN, value);
}

double Settings::getLedCoefRed(int ledIndex)
{
    return getValidLedCoef(ledIndex, KEY_LED_CRED);
}

double Settings::getLedCoefGreen(int ledIndex)
{
    return getValidLedCoef(ledIndex, KEY_LED_CGREEN);
}

double Settings::getLedCoefBlue(int ledIndex)
{
    return getValidLedCoef(ledIndex, KEY_LED_CBLUE);
}

void Settings::setLedCoefRed(int ledIndex, double value)
{
    setValidLedCoef(ledIndex, KEY_LED_CRED, value);
}

void Settings::setLedCoefGreen(int ledIndex, double value)
{
    setValidLedCoef(ledIndex, KEY_LED_CGREEN, value);
}

void Settings::setLedCoefBlue(int ledIndex, double value)
{
    setValidLedCoef(ledIndex, KEY_LED_CBLUE, value);
}

QSize Settings::getLedSize(int ledIndex)
{
    return value(KEY_LED_PREFIX + QString::number(ledIndex + 1) + "/" + KEY_LED_SIZE).toSize();
}

void Settings::setLedSize(int ledIndex, QSize size)
{
    setValue(KEY_LED_PREFIX + QString::number(ledIndex + 1) + "/" + KEY_LED_SIZE, size);
}

QPoint Settings::getLedPosition(int ledIndex)
{
    return value(KEY_LED_PREFIX + QString::number(ledIndex + 1) + "/" + KEY_LED_POSITION).toPoint();
}

void Settings::setLedPosition(int ledIndex, QPoint position)
{
    setValue(KEY_LED_PREFIX + QString::number(ledIndex + 1) + "/" + KEY_LED_POSITION, position);
}

bool Settings::isLedEnabled(int ledIndex)
{
    return value(KEY_LED_PREFIX + QString::number(ledIndex + 1) + "/" + KEY_LED_IS_ENABLED).toBool();
}

void Settings::setLedEnabled(int ledIndex, bool isEnabled)
{
    setValue(KEY_LED_PREFIX + QString::number(ledIndex + 1) + "/" + KEY_LED_IS_ENABLED, isEnabled);
}

double Settings::getValidGammaCorrection(double value)
{
    if (value <= GAMMA_MIN_VALUE)
        value = GAMMA_MIN_VALUE;
    else if (value >= GAMMA_MAX_VALUE)
        value = GAMMA_MAX_VALUE;
    return value;
}

int Settings::getValidGrabSlowdownMs(int value)
{
    if (value < SMOOTH_MIN_VALUE)
        value = SMOOTH_MIN_VALUE;
    else if (value > SMOOTH_MAX_VALUE)
        value = SMOOTH_MAX_VALUE;
    return value;
}

int Settings::getValidMoodLampSpeed(int value)
{
    if (value < 0)
        value = 0;
    else if (value > 255)
        value = 255;
    return value;
}

void Settings::setValidLedCoef(int ledIndex, const QString & keyCoef, double coef)
{
    if (coef < LED_COEF_MIN_VALUE || coef > LED_COEF_MAX_VALUE){
        QString error = "Error: outside the valid values (coef < " +
                QString::number(LED_COEF_MIN_VALUE) + " || coef > " + QString::number(LED_COEF_MAX_VALUE) + ").";

        qWarning() << Q_FUNC_INFO << "Settings bad value"
                   << "[" KEY_LED_PREFIX + QString::number(ledIndex + 1) + "]"
                   << keyCoef
                   << error
                   << "Convert to double error. Set it to default value" << keyCoef << "=" << LED_COEF_RGB_DEFAULT_VALUE;
        coef = LED_COEF_RGB_DEFAULT_VALUE;
        Settings::setValue(KEY_LED_PREFIX + QString::number(ledIndex + 1) + "/" + keyCoef, coef);
    }
}

double Settings::getValidLedCoef(int ledIndex, const QString & keyCoef)
{
    bool ok = false;
    double coef = Settings::value(KEY_LED_PREFIX + QString::number(ledIndex + 1) + "/" + keyCoef).toDouble(&ok);
    QString error;
    if (ok == false){
        error = "Error: Convert to double.";
    } else if (coef < LED_COEF_MIN_VALUE || coef > LED_COEF_MAX_VALUE){
        QString error = "Error: outside the valid values (coef < " +
                QString::number(LED_COEF_MIN_VALUE) + " || coef > " + QString::number(LED_COEF_MAX_VALUE) + ").";
    } else {
        // OK
        return coef;
    }
    // Have an error
    qWarning() << Q_FUNC_INFO << "Settings bad value"
               << "[" KEY_LED_PREFIX + QString::number(ledIndex + 1) + "]"
               << keyCoef
               << error
               << "Set it to default value" << keyCoef << "=" << LED_COEF_RGB_DEFAULT_VALUE;
    coef = LED_COEF_RGB_DEFAULT_VALUE;
    Settings::setValue(KEY_LED_PREFIX + QString::number(ledIndex + 1) + "/" + keyCoef, coef);
    return coef;
}

//
//  Check and/or initialize settings
//
void Settings::settingsInit(bool isResetDefault)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << isResetDefault;

    setNewOption(KEY_GRAB_MODE,                     GRAB_MODE_DEFAULT,
                 isResetDefault);
    setNewOption(KEY_MODE,                          MODE_DEFAULT,
                 isResetDefault);
    setNewOption(KEY_GRAB_SLOWDOWN_MS,              GRAB_SLOWDOWN_MS_DEFAULT_VALUE,
                 isResetDefault);
    setNewOption(KEY_IS_BACKLIGHT_ON,               IS_BACKLIGHT_ON_DEFAULT_VALUE,
                 isResetDefault);
    setNewOption(KEY_IS_AVG_COLORS_ON,              IS_AVG_COLORS_ON_DEFAULT_VALUE,
                 isResetDefault);
    setNewOption(KEY_MIN_LVL_SENSITIVITY,           MINIMUM_LEVEL_OF_SENSITIVITY_DEFAULT,
                 isResetDefault);
    setNewOption(KEY_GAMMA_CORRECTION,              GAMMA_CORRECTION_DEFAULT_VALUE,
                 isResetDefault);
    setNewOption(KEY_BRIGHTNESS,                    BRIGHTNESS_DEFAULT_VALUE,
                 isResetDefault);
    setNewOption(KEY_MOOD_LAMP_LIQUID_MODE,         MOOD_LAMP_MODE_DEFAULT_VALUE,
                 isResetDefault);
    setNewOption(KEY_MOOD_LAMP_COLOR,               MOOD_LAMP_COLOR_DEFAULT_VALUE,
                 isResetDefault);
    setNewOption(KEY_MOOD_LAMP_SPEED,               SPEED_MOOD_LAMP_DEFAULT_VALUE,
                 isResetDefault);

    setNewOption(KEY_FW_TIM_PRESCALLER,             FW_TIMER_PRESCALLER_INDEX_DEFAULT_VALUE,
                 isResetDefault);
    setNewOption(KEY_FW_TIM_OCR,                    FW_TIMER_OCR_DEFAULT_VALUE,
                 isResetDefault);
    setNewOption(KEY_FW_COLOR_DEPTH,                FW_COLOR_DEPTH_DEFAULT_VALUE,
                 isResetDefault);
    setNewOption(KEY_FW_SMOOTH_SLOWDOWN,            FW_SMOOTH_SLOWDOWN_DEFAULT,
                 isResetDefault);

    QPoint ledPosition;

    for (int i = 0; i < LEDS_COUNT; i++)
    {
        setNewOption("LED_" + QString::number(i +1) + "/CoefRed",   LED_COEF_RGB_DEFAULT_VALUE,
                     isResetDefault);
        setNewOption("LED_" + QString::number(i +1) + "/CoefGreen", LED_COEF_RGB_DEFAULT_VALUE,
                     isResetDefault);
        setNewOption("LED_" + QString::number(i +1) + "/CoefBlue",  LED_COEF_RGB_DEFAULT_VALUE,
                     isResetDefault);
        setNewOption("LED_" + QString::number(i +1) + "/Size",      LED_FIELD_SIZE_DEFAULT_VALUE,
                     isResetDefault);

        ledPosition = getDefaultPosition(i);

        setNewOption("LED_" + QString::number(i +1) + "/Position",  ledPosition,
                     isResetDefault);
        setNewOption("LED_" + QString::number(i +1) + "/IsEnabled", LED_IS_ENABLED_DEFAULT_VALUE,
                     isResetDefault);
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
