/*
 * LightpackMath.hpp
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

#pragma once

#include <QList>
#include <QRgb>
#include <cmath>
#include "colorspace_types.h"

class LightpackMath
{
public:
    static void gammaCorrection(double gamma, StructRgb &);
    static void brightnessCorrection(unsigned int brightness, StructRgb &);
    static void maxCorrection(unsigned int max, StructRgb &);
    static int getValueHSV(const QRgb rgb);
    static int getChromaHSV(const QRgb rgb);
    static int max(const QRgb);
    static int min(const QRgb);
    static QRgb withValueHSV(const QRgb, int);
    static QRgb withChromaHSV(const QRgb, int);
    static StructXyz toXyz(const StructRgb &);
    static StructXyz toXyz(const StructLab &);
    static StructLab toLab(const StructRgb &);
    static StructLab toLab(const StructXyz &);
    static StructRgb toRgb(const StructXyz &);
    static StructRgb toRgb(const StructLab &);

    // Convert ASCII char '5' to 5
    static inline char getDigit(const char d)
    {
        if (isdigit(d))
            return (d - '0');
        else
            return -1;
    }

    static inline int rand(int val)
    {
        return qrand() % val;
    }

};

