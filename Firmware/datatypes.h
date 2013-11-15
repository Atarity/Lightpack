/*
 * datatypes.h
 *
 *  Created on: 13.04.2011
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Lightpack is a content-appropriate ambient lighting system for any computer
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

#ifndef DATATYPES_H_INCLUDED
#define DATATYPES_H_INCLUDED

#include <stdint.h>
#include "../CommonHeaders/LEDS_COUNT.h"

#if (LIGHTPACK_HW >= 6)
typedef struct
{
    uint16_t r;
    uint16_t g;
    uint16_t b;

} RGB_t;
#else /*(LIGHTPACK_HW >= 6)*/
typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;

} RGB_t;
#endif

typedef struct
{
    RGB_t start[LEDS_COUNT];
    RGB_t current[LEDS_COUNT];
    RGB_t end[LEDS_COUNT];

    uint8_t smoothIndex[LEDS_COUNT];

} Images_t;

typedef struct
{
    uint8_t isSmoothEnabled;
    uint8_t smoothSlowdown;
    uint8_t brightness;
    uint8_t maxPwmValue;
    uint16_t timerOutputCompareRegValue;

} Settings_t;

#endif /* DATATYPES_H_INCLUDED */
