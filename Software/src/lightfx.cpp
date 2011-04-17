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


#include "LightFx.hpp"

#include <unistd.h>

#include <QtDebug>
#include "debug.h"
#include "../alienfx/LFX2.h"
#include <windows.h>

LFX2INITIALIZE lfxInitFunction;
LFX2RELEASE lfxReleaseFunction;
LFX2RESET lfxResetFunction;
LFX2UPDATE lfxUpdateFunction;
LFX2GETNUMDEVICES lfxGetNumDevicesFunction;
LFX2GETDEVDESC lfxGetDeviceDescriptionFunction;
LFX2GETNUMLIGHTS lfxGetNumLightsFunction;
LFX2SETLIGHTCOL lfxSetLightColorFunction;
LFX2GETLIGHTCOL lfxGetLightColorFunction;
LFX2GETLIGHTDESC lfxGetLightDescriptionFunction;
LFX2LIGHT lfxLightFunction;

LightFx::LightFx(QObject *parent) :
        ILedDevice(parent)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    hLfxLibrary = LoadLibrary(L"LightFX.dll");
    if (hLfxLibrary)
    {
        lfxInitFunction = (LFX2INITIALIZE)GetProcAddress(hLfxLibrary, LFX_DLL_INITIALIZE);
        lfxReleaseFunction = (LFX2RELEASE)GetProcAddress(hLfxLibrary, LFX_DLL_RELEASE);
        lfxResetFunction = (LFX2RESET)GetProcAddress(hLfxLibrary, LFX_DLL_RESET);
        lfxUpdateFunction = (LFX2UPDATE)GetProcAddress(hLfxLibrary, LFX_DLL_UPDATE);
        lfxGetNumDevicesFunction = (LFX2GETNUMDEVICES)GetProcAddress(hLfxLibrary, LFX_DLL_GETNUMDEVICES);
        lfxGetDeviceDescriptionFunction = (LFX2GETDEVDESC)GetProcAddress(hLfxLibrary, LFX_DLL_GETDEVDESC);
        lfxGetNumLightsFunction = (LFX2GETNUMLIGHTS)GetProcAddress(hLfxLibrary, LFX_DLL_GETNUMLIGHTS);
        lfxSetLightColorFunction = (LFX2SETLIGHTCOL)GetProcAddress(hLfxLibrary, LFX_DLL_SETLIGHTCOL);
        lfxGetLightColorFunction = (LFX2GETLIGHTCOL)GetProcAddress(hLfxLibrary, LFX_DLL_GETLIGHTCOL);
        lfxGetLightDescriptionFunction = (LFX2GETLIGHTDESC)GetProcAddress(hLfxLibrary, LFX_DLL_GETLIGHTDESC);
        lfxLightFunction = (LFX2LIGHT)GetProcAddress(hLfxLibrary, LFX_DLL_LIGHT);

        LFX_RESULT result = lfxInitFunction();
        if (result == LFX_SUCCESS)
        {
            isInited = true;
            result = lfxResetFunction();
        } else {
            emit ioDeviceSuccess(false);
        }

    } else {
        qWarning() << "couldn't load LightFX.dll";
    }

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "initialized";
}

LightFx::~LightFx()
{
    if(isInited)
        lfxReleaseFunction();
    if (hLfxLibrary)
        FreeLibrary(hLfxLibrary);

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
    return "not supported";
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
    if(isInited)
    {
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
        emit ioDeviceSuccess(true);
    }
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    return;
}
