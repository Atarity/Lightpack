/*
 * ambilightusb.h
 *
 *  Created on: 26.07.2010
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


#ifndef LIGHTFX_H
#define LIGHTFX_H

#include "ILedDevice.hpp"

class LightFx : public ILedDevice
{
    Q_OBJECT

public:
    LightFx(QObject *parent);
    ~LightFx();

    bool openDevice();
    bool deviceOpened();
    QString firmwareVersion();
    void offLeds(); /* send CMD_OFF_ALL to device, it causes rapid shutdown LEDs */

public slots:
    void updateColors(const QList<StructRGB> & colors);
    void setTimerOptions(int prescallerIndex, int outputCompareRegValue);
    void setColorDepth(int colorDepth);
    void smoothChangeColors(bool isSmooth);

signals:
    void openDeviceSuccess(bool isSuccess);
    void ioDeviceSuccess(bool isSuccess);

private:
    HINSTANCE hLfxLibrary;
    bool isInited;

};

#endif // LIGHTFX_H
