/*
 * LightpackMath.cpp
 *
 *  Created on: 29.09.2011
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

#include "LightpackMath.hpp"
#include <algorithm>
#include "debug.h"

void LightpackMath::gammaCorrection(double gamma, const QList<QRgb> &colors, QList<StructRgb> &result, int colorDepth /* = 256 */)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO << gamma;

    //
    // If colorDepth == 4096 then this function, after gamma correction,
    // converts 8-bit 'colors' to 12-bit 'result'
    //
    // 256  == 2^8 : This is default color depth
    // 4096 == 2^12: This is color depth for lightpack hw6

    for (int i = 0; i < colors.count(); i++)
    {
        StructRgb rgbResult;

        QRgb rgb = colors[i]; // color depth -- 8-bit

        rgbResult.r = colorDepth * pow(qRed(rgb) / 256.0, gamma);
        rgbResult.g = colorDepth * pow(qGreen(rgb) / 256.0, gamma);
        rgbResult.b = colorDepth * pow(qBlue(rgb) / 256.0, gamma);

        result[i] = rgbResult;
    }
}

void LightpackMath::brightnessCorrection(int brightness, QList<StructRgb> & result)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO << brightness;

    // brightness -- must be in percentage (0..100%)

    for (int i = 0; i < result.count(); i++)
    {
        result[i].r = (brightness / 100.0) * result[i].r;
        result[i].g = (brightness / 100.0) * result[i].g;
        result[i].b = (brightness / 100.0) * result[i].b;
    }
}

void LightpackMath::maxCorrection(int max, QList<StructRgb> & result)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO << max;
    for (int i = 0; i < result.count(); i++)
    {
        if (result[i].r > max) result[i].r = max;
        if (result[i].g > max) result[i].g = max;
        if (result[i].b > max) result[i].b = max;
    }
}

int LightpackMath::getValueHSV(const QRgb rgb)
{
    return LightpackMath::max(rgb);
}

int LightpackMath::max(const QRgb rgb)
{
    return std::max( std::max( qRed(rgb), qGreen(rgb)), qBlue(rgb));
}

int LightpackMath::min(const QRgb rgb)
{
    return std::min( std::min( qRed(rgb), qGreen(rgb)), qBlue(rgb));
}

int LightpackMath::getChromaHSV(const QRgb rgb)
{
    return LightpackMath::max(rgb) - LightpackMath::min(rgb);
}

QRgb LightpackMath::withValueHSV(const QRgb rgb, int value)
{

    int currentValue = LightpackMath::getValueHSV(rgb);

    if (currentValue == 0) {
       return qRgb(value, value, value);
    }

    double m = double(value)/currentValue;
    return qRgb(round(qRed(rgb)*m), round(qGreen(rgb)*m), round(qBlue(rgb)*m));

}

QRgb LightpackMath::withChromaHSV(const QRgb rgb, int chroma)
{
    int currentChroma = LightpackMath::getChromaHSV(rgb);

    if (currentChroma == 0) {
       return rgb;
    }

    if (chroma < 0) chroma = 0;

    int currentMax = LightpackMath::max(rgb);

    double m = double(chroma)/currentChroma;

    int r = qRed(rgb)   + (currentMax - qRed(rgb))*m;
    int g = qGreen(rgb) + (currentMax - qGreen(rgb))*m;
    int b = qBlue(rgb)  + (currentMax - qBlue(rgb))*m;
    return qRgb(r,g,b);
}
