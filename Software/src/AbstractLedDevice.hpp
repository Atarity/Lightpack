/*
 * AbstractLedDevice.hpp
 *
 *  Created on: 17.04.2011
 *      Author: Timur Sattarov && Mike Shatohin
 *     Project: Lightpack
 *
 *  Lightpack is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2011 Mike Shatohin, mikeshatohin [at] gmail.com
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

#include <QtGui>
#include "colorspace_types.h"
#include "types.h"

/*!
    Abstract class representing any LED device.
    \a LedDeviceManager
*/
class AbstractLedDevice : public QObject
{
    Q_OBJECT
public:
    AbstractLedDevice(QObject * parent) : QObject(parent) {}
    virtual ~AbstractLedDevice(){}

signals:
    void openDeviceSuccess(bool isSuccess);
    void ioDeviceSuccess(bool isSuccess);
    void firmwareVersion(const QString & fwVersion);

    /*!
      This signal must be sent at the completion of each command
      (setColors, setTimerOptions, setColorDepth, setSmoothSlowdown, etc.)
      \param ok is command completed successfully
    */
    void commandCompleted(bool ok);
    void colorsUpdated(QList<QRgb> colors);

public slots:
    virtual const QString name() const = 0;
    virtual void open() = 0;
    virtual void close() = 0;
    virtual void setColors(const QList<QRgb> & colors) = 0;
    virtual void switchOffLeds() = 0;

    /*!
      \obsolete only form compatibility with Lightpack ver.<=5.5 hardware
      PWM timer period.
     \param value in millis
    */
    virtual void setRefreshDelay(int value) = 0;
    virtual void setSmoothSlowdown(int value) = 0;
    virtual void setGamma(double value);
    virtual void setBrightness(int value);
    virtual void setColorSequence(QString value) = 0;
    virtual void setLuminosityThreshold(int value);
    virtual void setMinimumLuminosityThresholdEnabled(bool value);
    virtual void updateWBAdjustments(const QList<WBAdjustment> &coefs);
    virtual void requestFirmwareVersion() = 0;
    virtual void updateDeviceSettings();

    virtual size_t maxLedsCount() = 0;
    virtual size_t defaultLedsCount() = 0;

    /*!
      \obsolete only form compatibility with Lightpack ver.<=5.5 hardware
     \param value bits per channel
    */
    virtual void setColorDepth(int value) = 0;

protected:
    virtual void applyColorModifications(const QList<QRgb> & inColors, QList<StructRgb> & outColors);

protected:
    QString m_colorSequence;
    double m_gamma;
    int m_brightness;
    int m_luminosityThreshold;
    bool m_isMinimumLuminosityEnabled;

    QList<WBAdjustment> m_wbAdjustments;

    QList<QRgb> m_colorsSaved;
    QList<StructRgb> m_colorsBuffer;
};
