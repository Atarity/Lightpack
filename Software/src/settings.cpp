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

#include "desktop.h" // Desktop width and height
#include "../../CommonHeaders/RGB.h"        /* Led defines */


QSettings * Settings::settingsNow;


void Settings::Initialize()
{
    Settings::settingsNow = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Lightpack", "Lightpack");

    qDebug() << "Settings file: " << settingsNow->fileName();

    settingsInit();
}


void Settings::setValue(const QString & key, const QVariant & value)
{
    settingsNow->setValue(key, value);
}

QVariant Settings::value( const QString & key)
{
    return settingsNow->value(key);
}

QString Settings::fileName()
{
    return settingsNow->fileName();
}

void Settings::loadOrCreateConfig(const QString & configName)
{
    if(settingsNow != NULL) delete settingsNow;

    settingsNow = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Lightpack", configName);
    qDebug() << "Settings file: " << settingsNow->fileName();
    settingsInit();
}

void Settings::removeCurrentConfig()
{
    if(QFile::remove( settingsNow->fileName() ) == false){
        qWarning() << "void Settings::removeCurrentConfig() fail";
        return;
    }

    delete settingsNow;
    settingsNow = NULL;
}




// private

void Settings::setDefaultSettingIfNotFound(const QString & name, const QVariant & value)
{
    if(!settingsNow->contains(name)){
        if(value.canConvert<QSize>()){
            qDebug() << "Settings:" << name << "not found. Set it to default value: " << value.toSize().width() << "x" << value.toSize().height();
        }else if(value.canConvert<QPoint>()){
            qDebug() << "Settings:"<< name << "not found. Set it to default value: " << value.toPoint().x() << "x" << value.toPoint().y();
        }else{
            qDebug() << "Settings:"<< name << "not found. Set it to default value: " << value.toString();
        }

        settingsNow->setValue(name, value);
    }
}

//
//  Check and/or initialize settings
//
void Settings::settingsInit()
{
    setDefaultSettingIfNotFound("RefreshAmbilightDelayMs",             REFRESH_AMBILIGHT_MS_DEFAULT_VALUE);
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
            ledPosition.setX(Desktop::WidthAvailable - LED_FIELD_WIDTH_DEFAULT_VALUE);
        }

        switch( ledIndex ){
        case LED1:
        case LED5: ledPosition.setY(Desktop::HeightFull / 2 - 2*LED_FIELD_HEIGHT_DEFAULT_VALUE);  break;
        case LED2:
        case LED6: ledPosition.setY(Desktop::HeightFull / 2 - LED_FIELD_HEIGHT_DEFAULT_VALUE);  break;
        case LED3:
        case LED7: ledPosition.setY(Desktop::HeightFull / 2 );  break;
        case LED4:
        case LED8: ledPosition.setY(Desktop::HeightFull / 2 + LED_FIELD_HEIGHT_DEFAULT_VALUE);  break;
        }

        setDefaultSettingIfNotFound("LED_" + QString::number(ledIndex+1) + "/Size",      LED_FIELD_SIZE_DEFAULT_VALUE);
        setDefaultSettingIfNotFound("LED_" + QString::number(ledIndex+1) + "/Position",  ledPosition);
        setDefaultSettingIfNotFound("LED_" + QString::number(ledIndex+1) + "/IsEnabled", LED_IS_ENABLED_DEFAULT_VALUE);
    }

    settingsNow->sync();
}
