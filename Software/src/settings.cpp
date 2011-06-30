/*
 * settings.cpp
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

#include "settings.h"

#include <QtDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QSize>
#include <QPoint>
#include <QFileInfo>
#include <QDir>

#include "../../CommonHeaders/LEDS_COUNT.h"

#include "debug.h"

#define KEY_MOOD_LAMP_COLOR       "MoodLampColor"
#define KEY_MOOD_LAMP_SPEED       "MoodLampSpeed"
#define KEY_MOOD_LAMP_LIQUID_MODE "MoodLampLiquidMode"
#define KEY_MODE                  "Mode"

QSettings * Settings::m_currentProfile;
QSettings * Settings::m_mainConfig; // LightpackMain.conf contains last profile

// Path to directory there store application generated stuff
QString Settings::m_applicationDirPath = "";

// Desktop should be initialized before call Settings::Initialize()
void Settings::Initialize( const QString & applicationDirPath, bool isSetDebugLevelFromConfig)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    m_applicationDirPath = applicationDirPath;

    // Append to the end of dir path '/'
    if (m_applicationDirPath.lastIndexOf('/') != m_applicationDirPath.length() - 1)
        m_applicationDirPath += "/";

    m_mainConfig = new QSettings(m_applicationDirPath + "LightpackMain.conf", QSettings::IniFormat);
    m_mainConfig->setIniCodec("UTF-8");

    setNewOptionMain("ProfileLast",    PROFILE_DEFAULT_NAME);
    setNewOptionMain("Language",       LANGUAGE_DEFAULT_NAME);
    setNewOptionMain("ShowAnotherGui", SHOW_ANOTHER_GUI);
    setNewOptionMain("DebugLevel",     DEBUG_LEVEL_DEFAULT);

    if (isSetDebugLevelFromConfig)
    {
        bool ok = false;
        int sDebugLevel = Settings::valueMain("DebugLevel").toInt(&ok);

        if (ok && sDebugLevel >= 0)
        {
            debugLevel = sDebugLevel;
            DEBUG_LOW_LEVEL << Q_FUNC_INFO << "debugLevel =" << debugLevel;
        } else {
            qWarning() << "DebugLevel in config has an invalid value, set the default" << DEBUG_LEVEL_DEFAULT;
            Settings::setValueMain("DebugLevel", DEBUG_LEVEL_DEFAULT);
            debugLevel = DEBUG_LEVEL_DEFAULT;
        }
    }

    QString profileLast = m_mainConfig->value("ProfileLast").toString();

    // Load last profile
    m_currentProfile = new QSettings(m_applicationDirPath + "Profiles/" + profileLast + ".ini", QSettings::IniFormat);
    m_currentProfile->setIniCodec("UTF-8");

    qDebug() << "Settings file:" << m_currentProfile->fileName();

    settingsInit(false);
}


void Settings::setValue(const QString & key, const QVariant & value)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << key;

    m_currentProfile->setValue(key, value);
}

QVariant Settings::value( const QString & key)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << key;

    return m_currentProfile->value(key);
}

QString Settings::fileName()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    return m_currentProfile->fileName();
}


void Settings::setValueMain(const QString & key, const QVariant & value)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << key;

    m_mainConfig->setValue(key, value);
}

QVariant Settings::valueMain( const QString & key)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << key;

    return m_mainConfig->value(key);
}


void Settings::loadOrCreateConfig(const QString & configName)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << configName;

    if (m_currentProfile != NULL)
    {
        // Copy current settings to new one
        QString settingsDir = QFileInfo(m_currentProfile->fileName()).absoluteDir().absolutePath();
        QString settingsNewFileName = settingsDir + "/" + configName + ".ini";

        if (m_currentProfile->fileName() != settingsNewFileName)
            QFile::copy(m_currentProfile->fileName(), settingsNewFileName);

        delete m_currentProfile;
    }


    m_currentProfile = new QSettings(m_applicationDirPath + "Profiles/" + configName + ".ini", QSettings::IniFormat );
    m_currentProfile->setIniCodec("UTF-8");

    settingsInit(false);

    qDebug() << "Settings file:" << m_currentProfile->fileName();

    m_mainConfig->setValue("ProfileLast", configName);
}

void Settings::renameCurrentConfig(const QString & configName)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << configName;

    if (m_currentProfile == NULL)
    {
        qWarning() << "void Settings::renameCurrentConfig(): fail, m_currentProfile not initialized";
        return;
    }

    // Copy current settings to new one
    QString settingsDir = QFileInfo( m_currentProfile->fileName() ).absoluteDir().absolutePath();
    QString settingsNewFileName = settingsDir + "/" + configName + ".ini";

    if (m_currentProfile->fileName() != settingsNewFileName)
    {
        QFile::rename(m_currentProfile->fileName(), settingsNewFileName);

        delete m_currentProfile;

        // Update m_currentProfile point to new QSettings with configName
        m_currentProfile = new QSettings(m_applicationDirPath + "Profiles/" + configName + ".ini", QSettings::IniFormat );
        m_currentProfile->setIniCodec("UTF-8");

        qDebug() << "Settings file renamed:" << m_currentProfile->fileName();

        m_mainConfig->setValue("ProfileLast", configName);
    }

    m_currentProfile->sync();
}

void Settings::removeCurrentConfig()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    if (m_currentProfile == NULL)
    {
        qWarning() << "void Settings::removeCurrentConfig() nothing to remove";
        return;
    }

    bool result = QFile::remove( m_currentProfile->fileName() );

    if (result == false)
    {
        qWarning() << "void Settings::removeCurrentConfig() QFile::remove() fail";
        return;
    }

    delete m_currentProfile;
    m_currentProfile = NULL;

    m_mainConfig->setValue("ProfileLast", PROFILE_DEFAULT_NAME);
}

QString Settings::lastProfileName()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    return m_mainConfig->value("ProfileLast").toString();
}

QString Settings::getApplicationDirPath()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

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

static int getValidGrabSlowdownMs(int value)
{
    if (value < 1)
        value = 1;
    else if (value > 1000)
        value = 1000;
    return value;
}

static int getValidMoodLampSpeed(int value)
{
    if (value < 0)
        value = 0;
    else if (value > 255)
        value = 255;
    return value;
}

LightpackMode Settings::getMode()
{
    QString strMode = m_currentProfile->value(KEY_MODE).toString().toLower();
    if (strMode == "grab")
        return Grab;
    else
        return MoodLamp;
}

void Settings::setMode(LightpackMode mode)
{
    QString strMode;
    if(mode == Grab)
        strMode = "Grab";
    else
        strMode = "MoodLamp";
    m_currentProfile->setValue(KEY_MODE, strMode);
}

bool Settings::isMoodLampLiquidMode()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    return m_currentProfile->value(KEY_MOOD_LAMP_LIQUID_MODE).toBool();
}

void Settings::setMoodLampLiquidMode(bool value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    m_currentProfile->setValue(KEY_MOOD_LAMP_LIQUID_MODE, value );
}

QColor Settings::getMoodLampColor()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    return QColor(m_currentProfile->value(KEY_MOOD_LAMP_COLOR).toString());
}

void Settings::setMoodLampColor(QColor value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    m_currentProfile->setValue(KEY_MOOD_LAMP_COLOR, value.name() );
}

int Settings::getMoodLampSpeed()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    return getValidMoodLampSpeed(m_currentProfile->value(KEY_MOOD_LAMP_SPEED).toInt());
}

void Settings::setMoodLampSpeed(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    m_currentProfile->setValue(KEY_MOOD_LAMP_SPEED, getValidMoodLampSpeed(value));
}


int Settings::getGrabSlowdownMs()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    return getValidGrabSlowdownMs(m_currentProfile->value("GrabSlowdownMs").toInt());
}

void Settings::setGrabSlowdownMs(int value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    m_currentProfile->setValue("GrabSlowdownMs", getValidGrabSlowdownMs(value));
}

//
//  Set all settings in current config to default values
//
void Settings::resetDefaults()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    settingsInit(true);
}


//
//  Check and/or initialize settings
//
void Settings::settingsInit(bool isResetDefault)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << isResetDefault;

    setNewOption(KEY_MODE,                         MODE_DEFAULT,
                 isResetDefault);
    setNewOption("GrabSlowdownMs",                 GRAB_SLOWDOWN_MS_DEFAULT_VALUE,
                 isResetDefault);
    setNewOption("IsAmbilightOn",                  IS_AMBILIGHT_ON_DEFAULT_VALUE,
                 isResetDefault);
    setNewOption("IsAvgColorsOn",                  IS_AVG_COLORS_ON_DEFAULT_VALUE,
                 isResetDefault);
    setNewOption("MinimumLevelOfSensitivity",      MINIMUM_LEVEL_OF_SENSITIVITY_DEFAULT,
                 isResetDefault);
    setNewOption("GammaCorrection",                GAMMA_CORRECTION_DEFAULT_VALUE,
                 isResetDefault);
    setNewOption(KEY_MOOD_LAMP_LIQUID_MODE,        MOOD_LAMP_MODE_DEFAULT_VALUE,
                 isResetDefault);
    setNewOption(KEY_MOOD_LAMP_COLOR,              MOOD_LAMP_COLOR_DEFAULT_VALUE,
                 isResetDefault);
    setNewOption(KEY_MOOD_LAMP_SPEED,              SPEED_MOOD_LAMP_DEFAULT_VALUE,
                 isResetDefault);
    setNewOption("Brightness",                     BRIGHTNESS_DEFAULT_VALUE,
                 isResetDefault);

    setNewOption("Firmware/TimerPrescallerIndex",  FW_TIMER_PRESCALLER_INDEX_DEFAULT_VALUE,
                 isResetDefault);
    setNewOption("Firmware/TimerOCR",              FW_TIMER_OCR_DEFAULT_VALUE,
                 isResetDefault);
    setNewOption("Firmware/ColorDepth",            FW_COLOR_DEPTH_DEFAULT_VALUE,
                 isResetDefault);
    setNewOption("Firmware/SmoothSlowdown",        FW_SMOOTH_SLOWDOWN_DEFAULT,
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

    m_currentProfile->sync();
}


void Settings::setNewOption(const QString & name, const QVariant & value,
                                        bool isForceSetOption, QSettings * settings)
{
    if (isForceSetOption)
    {
        m_currentProfile->setValue(name, value);
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

void Settings::setNewOptionMain(const QString & name, const QVariant & value, bool isForceSetOption)
{
    setNewOption(name, value, isForceSetOption, m_mainConfig);
}

