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
#include <QSize>
#include <QPoint>
#include <QFileInfo>
#include <QDir>

#include "desktop.h" // Desktop width and height
#include "../../CommonHeaders/RGB.h"        /* Led defines */

#include "debug.h"

QSettings * Settings::settingsNow;
QSettings * Settings::settingsMain; // LightpackMain.conf contains last profile

// Path to directory there store application generated stuff
QString Settings::appDirPath = "";

// Desktop should be initialized before call Settings::Initialize()
void Settings::Initialize( const QString & applicationDirPath)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    appDirPath = applicationDirPath;

    // Append to the end of dir path '/'
    if(appDirPath.lastIndexOf('/') != appDirPath.length() - 1){
        appDirPath += "/";
    }

    settingsMain = new QSettings(appDirPath + "LightpackMain.conf", QSettings::IniFormat);
    settingsMain->setIniCodec("UTF-8");

    setDefaultSettingIfNotFound(settingsMain, "ProfileLast",    PROFILE_DEFAULT_NAME);
    setDefaultSettingIfNotFound(settingsMain, "Language",       LANGUAGE_DEFAULT_NAME);    
    setDefaultSettingIfNotFound(settingsMain, "GuiShowSwitchQtWinAPI", GUI_SHOW_SWITCH_GRAB_QT_WINAPI);

    QString profileLast = settingsMain->value("ProfileLast").toString();

    // Load last profile
    settingsNow = new QSettings(appDirPath + "Profiles/" + profileLast + ".ini", QSettings::IniFormat);
    settingsNow->setIniCodec("UTF-8");

    qDebug() << "Settings file:" << settingsNow->fileName();

    settingsInit();
}


void Settings::setValue(const QString & key, const QVariant & value)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << key;

    settingsNow->setValue(key, value);
}

QVariant Settings::value( const QString & key)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << key;

    return settingsNow->value(key);
}

QString Settings::fileName()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    return settingsNow->fileName();
}


void Settings::setValueMain(const QString & key, const QVariant & value)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << key;

    settingsMain->setValue(key, value);
}

QVariant Settings::valueMain( const QString & key)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << key;

    return settingsMain->value(key);
}


void Settings::loadOrCreateConfig(const QString & configName)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << configName;

    if(settingsNow != NULL){
        // Copy current settings to new one
        QString settingsDir = QFileInfo(settingsNow->fileName()).absoluteDir().absolutePath();
        QString settingsNewFileName = settingsDir + "/" + configName + ".ini";

        if(settingsNow->fileName() != settingsNewFileName){
            QFile::copy(settingsNow->fileName(), settingsNewFileName);
        }
        delete settingsNow;
    }


    settingsNow = new QSettings(appDirPath + "Profiles/" + configName + ".ini", QSettings::IniFormat );
    settingsNow->setIniCodec("UTF-8");
    settingsInit();
    qDebug() << "Settings file:" << settingsNow->fileName();

    settingsMain->setValue("ProfileLast", configName);
}

void Settings::renameCurrentConfig(const QString & configName)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << configName;

    if(settingsNow == NULL){
        qWarning() << "void Settings::renameCurrentConfig(): fail, settingsNow not initialized";
        return;
    }

    // Copy current settings to new one
    QString settingsDir = QFileInfo( settingsNow->fileName() ).absoluteDir().absolutePath();
    QString settingsNewFileName = settingsDir + "/" + configName + ".ini";

    if(settingsNow->fileName() != settingsNewFileName){
        QFile::rename(settingsNow->fileName(), settingsNewFileName);

        delete settingsNow;

        // Update settingsNow point to new QSettings with configName
        settingsNow = new QSettings(appDirPath + "Profiles/" + configName + ".ini", QSettings::IniFormat );
        settingsNow->setIniCodec("UTF-8");

        qDebug() << "Settings file renamed:" << settingsNow->fileName();

        settingsMain->setValue("ProfileLast", configName);
    }

    settingsNow->sync();
}

void Settings::removeCurrentConfig()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    if(settingsNow == NULL){
        qWarning() << "void Settings::removeCurrentConfig() nothing to remove";
        return;
    }

    bool result = QFile::remove( settingsNow->fileName() );

    if(result == false){
        qWarning() << "void Settings::removeCurrentConfig() QFile::remove() fail";
        return;
    }

    delete settingsNow;
    settingsNow = NULL;

    settingsMain->setValue("ProfileLast", PROFILE_DEFAULT_NAME);
}

QString Settings::lastProfileName()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    return settingsMain->value("ProfileLast").toString();
}


// private

void Settings::setDefaultSettingIfNotFound(const QString & name, const QVariant & value)
{
    setDefaultSettingIfNotFound(settingsNow, name, value);
}

void Settings::setDefaultSettingIfNotFound(QSettings *settings, const QString & name, const QVariant & value)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO << name;

    if(!settings->contains(name)){
        if(value.canConvert<QSize>()){
            qDebug() << "Settings:" << name << "not found. Set it to default value: " << value.toSize().width() << "x" << value.toSize().height();
        }else if(value.canConvert<QPoint>()){
            qDebug() << "Settings:"<< name << "not found. Set it to default value: " << value.toPoint().x() << "x" << value.toPoint().y();
        }else{
            qDebug() << "Settings:"<< name << "not found. Set it to default value: " << value.toString();
        }

        settings->setValue(name, value);
    }
}

//
//  Check and/or initialize settings
//
void Settings::settingsInit()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    setDefaultSettingIfNotFound("GrabSlowdownMs",                      GRAB_SLOWDOWN_MS_DEFAULT_VALUE);
    setDefaultSettingIfNotFound("IsAmbilightOn",                       IS_AMBILIGHT_ON_DEFAULT_VALUE);
    setDefaultSettingIfNotFound("IsAvgColorsOn",                       IS_AVG_COLORS_ON_DEFAULT_VALUE);
    setDefaultSettingIfNotFound("MinimumLevelOfSensitivity",           MINIMUM_LEVEL_OF_SENSITIVITY_DEFAULT);

    setDefaultSettingIfNotFound("Firmware/TimerPrescallerIndex",       FW_TIMER_PRESCALLER_INDEX_DEFAULT_VALUE);
    setDefaultSettingIfNotFound("Firmware/TimerOCR",                   FW_TIMER_OCR_DEFAULT_VALUE);
    setDefaultSettingIfNotFound("Firmware/ColorDepth",                 FW_COLOR_DEPTH_DEFAULT_VALUE);
    setDefaultSettingIfNotFound("Firmware/IsSmoothChangeColors",       FW_IS_SMOOTH_CHANGE_COLORS_DEFAULT_VALUE);

    QPoint ledPosition;

    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        setDefaultSettingIfNotFound("LED_" + QString::number(ledIndex+1) + "/CoefRed",   LED_COEF_RGB_DEFAULT_VALUE);
        setDefaultSettingIfNotFound("LED_" + QString::number(ledIndex+1) + "/CoefGreen", LED_COEF_RGB_DEFAULT_VALUE);
        setDefaultSettingIfNotFound("LED_" + QString::number(ledIndex+1) + "/CoefBlue",  LED_COEF_RGB_DEFAULT_VALUE);

        if(ledIndex < 4){
            ledPosition.setX(0);
        }else{
            ledPosition.setX(Desktop::Width - LED_FIELD_WIDTH_DEFAULT_VALUE);
        }

        switch( ledIndex ){
        case LED1:
        case LED5: ledPosition.setY(Desktop::Height / 2 - 2*LED_FIELD_HEIGHT_DEFAULT_VALUE);  break;
        case LED2:
        case LED6: ledPosition.setY(Desktop::Height / 2 - LED_FIELD_HEIGHT_DEFAULT_VALUE);  break;
        case LED3:
        case LED7: ledPosition.setY(Desktop::Height / 2 );  break;
        case LED4:
        case LED8: ledPosition.setY(Desktop::Height / 2 + LED_FIELD_HEIGHT_DEFAULT_VALUE);  break;
        }

        setDefaultSettingIfNotFound("LED_" + QString::number(ledIndex+1) + "/Size",      LED_FIELD_SIZE_DEFAULT_VALUE);
        setDefaultSettingIfNotFound("LED_" + QString::number(ledIndex+1) + "/Position",  ledPosition);
        setDefaultSettingIfNotFound("LED_" + QString::number(ledIndex+1) + "/IsEnabled", LED_IS_ENABLED_DEFAULT_VALUE);
    }

    settingsNow->sync();
}

//
//  Set all settings in current config to default values
//
void Settings::resetToDefaults()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    settingsNow->setValue("GrabSlowdownMs",                      GRAB_SLOWDOWN_MS_DEFAULT_VALUE);
    settingsNow->setValue("IsAmbilightOn",                       IS_AMBILIGHT_ON_DEFAULT_VALUE);
    settingsNow->setValue("IsAvgColorsOn",                       IS_AVG_COLORS_ON_DEFAULT_VALUE);
    settingsNow->setValue("MinimumLevelOfSensitivity",           MINIMUM_LEVEL_OF_SENSITIVITY_DEFAULT);

    settingsNow->setValue("Firmware/TimerPrescallerIndex",       FW_TIMER_PRESCALLER_INDEX_DEFAULT_VALUE);
    settingsNow->setValue("Firmware/TimerOCR",                   FW_TIMER_OCR_DEFAULT_VALUE);
    settingsNow->setValue("Firmware/ColorDepth",                 FW_COLOR_DEPTH_DEFAULT_VALUE);
    settingsNow->setValue("Firmware/IsSmoothChangeColors",       FW_IS_SMOOTH_CHANGE_COLORS_DEFAULT_VALUE);

    QPoint ledPosition;

    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        settingsNow->setValue("LED_" + QString::number(ledIndex+1) + "/CoefRed",   LED_COEF_RGB_DEFAULT_VALUE);
        settingsNow->setValue("LED_" + QString::number(ledIndex+1) + "/CoefGreen", LED_COEF_RGB_DEFAULT_VALUE);
        settingsNow->setValue("LED_" + QString::number(ledIndex+1) + "/CoefBlue",  LED_COEF_RGB_DEFAULT_VALUE);

        if(ledIndex < 4){
            ledPosition.setX(0);
        }else{
            ledPosition.setX(Desktop::Width - LED_FIELD_WIDTH_DEFAULT_VALUE);
        }

        switch( ledIndex ){
        case LED1:
        case LED5: ledPosition.setY(Desktop::Height / 2 - 2*LED_FIELD_HEIGHT_DEFAULT_VALUE);  break;
        case LED2:
        case LED6: ledPosition.setY(Desktop::Height / 2 - LED_FIELD_HEIGHT_DEFAULT_VALUE);  break;
        case LED3:
        case LED7: ledPosition.setY(Desktop::Height / 2 );  break;
        case LED4:
        case LED8: ledPosition.setY(Desktop::Height / 2 + LED_FIELD_HEIGHT_DEFAULT_VALUE);  break;
        }

        settingsNow->setValue("LED_" + QString::number(ledIndex+1) + "/Size",      LED_FIELD_SIZE_DEFAULT_VALUE);
        settingsNow->setValue("LED_" + QString::number(ledIndex+1) + "/Position",  ledPosition);
        settingsNow->setValue("LED_" + QString::number(ledIndex+1) + "/IsEnabled", LED_IS_ENABLED_DEFAULT_VALUE);
    }

    settingsNow->sync();
}
