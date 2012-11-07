/*
 * SettingsWindowLittleVersion.hpp
 *
 *  Created on: 04.11.2011
 *     Project: Lightpack
 *
 *  Copyright (c) 2011 Mike Shatohin, mikeshatohin [at] gmail.com
 *
 *  Lightpack a USB content-driving ambient lighting system
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

#include <QObject>
#include <QRgb>
#include "enums.hpp"

class SettingsWindowMockup : public QObject
{
    Q_OBJECT
public:
    SettingsWindowMockup(QObject *parent) : QObject(parent)
    {
        m_status = Backlight::StatusOff;
        m_smooth = -1;
        m_isErrorCallbackWorksFine = false;
    }
signals:
    void resultBacklightStatus(Backlight::Status status);
    void enableApiServer(bool isEnabled);
    void updateApiPort(int port);
    void updateApiKey(QString key);
public slots:
    void requestBacklightStatus();
    void setLedColors(QList<QRgb> colors);
    void setSmooth(int value);
    void setGamma(double value);
    void setBrightness(int value);
    void setProfile(QString profile);
    void setStatus(Backlight::Status status);
    void onApiServer_ErrorOnStartListening(QString errorMessage);
public:
    void setIsEnabledApiAuth(bool isEnabled);
    void setApiKey(const QString & apiKey);

    Backlight::Status m_status;
    bool m_isDone;
    QList<QRgb> m_colors;
    int m_smooth;
    double m_gamma;
    int m_brightness;
    QString m_profile;
    bool m_isErrorCallbackWorksFine;
};
