/*
 * ILedDevice.hpp
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
#include "struct_rgb.h"


class ILedDevice : public QObject
{
    Q_OBJECT
public:
    ILedDevice(QObject * parent) : QObject(parent){}
    virtual bool openDevice() = 0;
    virtual bool deviceOpened() = 0;
    virtual QString firmwareVersion() = 0;
    virtual void offLeds() = 0;

public slots:
    virtual void updateColors(const QList<StructRGB> & colors) = 0;
    virtual void setTimerOptions(int prescallerIndex, int outputCompareRegValue) = 0;
    virtual void setColorDepth(int value) = 0;
    virtual void setSmoothSlowdown(int value) = 0;
    virtual void setBrightness(int value) = 0;

signals:
    void openDeviceSuccess(bool isSuccess);
    void ioDeviceSuccess(bool isSuccess);

};
