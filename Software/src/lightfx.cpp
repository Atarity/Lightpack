/*
 * ambilightusb.cpp
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


#include "lightfx.h"

#include <unistd.h>

#include <QtDebug>
#include "debug.h"
#include "lfxapi.h"

LightFx::LightFx(QObject *parent) :
        QObject(parent)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "initialized";
}

LightFx::~LightFx()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "hid_close(lightFxDevice);";
}

bool LightFx::deviceOpened()
{
    return true;
}

bool LightFx::openDevice()
{
    return true;
}



QString LightFx::firmwareVersion()
{
    QString firmwareVer = QString::number(0) + "." + QString::number(0);

    return firmwareVer;
}

void LightFx::offLeds()
{
    return;
}

void LightFx::smoothChangeColors(bool isSmooth)
{
    return;
}


void LightFx::setTimerOptions(int prescallerIndex, int outputCompareRegValue)
{
    return;
}

void LightFx::setColorDepth(int colorDepth)
{
    return;
}


void LightFx::updateColors(const QList<StructRGB> & colors)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    unsigned int numDevs = 0;
    LFX_RESULT result = lfxGetNumDevicesFunction(&numDevs);

    for(unsigned int devIndex = 0; devIndex < numDevs; devIndex++)
    {
        unsigned int numLights = 0;
        result = lfxGetNumLightsFunction(devIndex, &numLights);

        LFX_COLOR lfxColor;

        lfxColor.red   = qRed   ( colors[0].rgb );
        lfxColor.green = qGreen ( colors[0].rgb );
        lfxColor.blue  = qBlue  ( colors[0].rgb );
        lfxColor.brightness = 255;

        for(unsigned int lightIndex = 0; lightIndex < numLights; lightIndex++)
                lfxSetLightColorFunction(devIndex, lightIndex, &lfxColor);
        lfxUpdateFunction();
    }

    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    return;
}
