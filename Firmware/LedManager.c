/*
 * LedManager.c
 *
 *  Created on: 28.09.2011
 *     Project: Lightpack
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

#include "Lightpack.h"
#include "LedDriver.h"

void LedManager_FillImages(const uint8_t red, const uint8_t green, const uint8_t blue)
{
    for (uint8_t i = 0; i < LEDS_COUNT; i++)
    {
        g_Images.start[i].r = red;
        g_Images.start[i].g = green;
        g_Images.start[i].b = blue;

        g_Images.current[i].r = red;
        g_Images.current[i].g = green;
        g_Images.current[i].b = blue;

        g_Images.end[i].r = red;
        g_Images.end[i].g = green;
        g_Images.end[i].b = blue;
    }
}

#if (LIGHTPACK_HW == 6 || LIGHTPACK_HW == 7)

void EvalCurrentImage_SmoothlyAlg(void)
{
    for (uint8_t i = 0; i < LEDS_COUNT; i++)
    {
        if (g_Images.smoothIndex[i] >= g_Settings.smoothSlowdown)
        {
            // Smooth change colors complete, rewrite start image
            g_Images.current[i].r = g_Images.start[i].r = g_Images.end[i].r;
            g_Images.current[i].g = g_Images.start[i].g = g_Images.end[i].g;
            g_Images.current[i].b = g_Images.start[i].b = g_Images.end[i].b;

        } else {
            uint32_t coefEnd = ((uint32_t)g_Images.smoothIndex[i] << 16) / g_Settings.smoothSlowdown;
            uint32_t coefStart = (1UL << 16) - coefEnd;

            g_Images.current[i].r = (
                    coefStart * g_Images.start[i].r +
                    coefEnd   * g_Images.end  [i].r) >> 16;

            g_Images.current[i].g = (
                    coefStart * g_Images.start[i].g +
                    coefEnd   * g_Images.end  [i].g) >> 16;

            g_Images.current[i].b = (
                    coefStart * g_Images.start[i].b +
                    coefEnd   * g_Images.end  [i].b) >> 16;

            g_Images.smoothIndex[i]++;
        }
    }
}

void LedManager_UpdateColors(void)
{
    EvalCurrentImage_SmoothlyAlg();

    if (g_Settings.isSmoothEnabled)
    	LedDriver_Update(g_Images.current);
    else
    	LedDriver_Update(g_Images.end);
}


#elif (LIGHTPACK_HW == 5 || LIGHTPACK_HW == 4)


static inline void _StartConstantTime(void)
{
    TCNT1 = 0;
}

static inline void _EndConstantTime(const uint8_t time)
{
    while(TCNT1 < time * 256UL) { }
}

void EvalCurrentImage_SmoothlyAlg(void)
{
    for (uint8_t i = 0; i < LEDS_COUNT; i++)
    {
        if (g_Images.smoothIndex[i] >= g_Settings.smoothSlowdown)
        {
            // Smooth change colors complete, rewrite start image
            g_Images.current[i].r = g_Images.start[i].r = g_Images.end[i].r;
            g_Images.current[i].g = g_Images.start[i].g = g_Images.end[i].g;
            g_Images.current[i].b = g_Images.start[i].b = g_Images.end[i].b;

        } else {
            uint16_t coefEnd = ((uint16_t)g_Images.smoothIndex[i] << 8) / g_Settings.smoothSlowdown;
            uint16_t coefStart = (1UL << 8) - coefEnd;

            g_Images.current[i].r = (
                    coefStart * g_Images.start[i].r +
                    coefEnd   * g_Images.end  [i].r) >> 8;

            g_Images.current[i].g = (
                    coefStart * g_Images.start[i].g +
                    coefEnd   * g_Images.end  [i].g) >> 8;

            g_Images.current[i].b = (
                    coefStart * g_Images.start[i].b +
                    coefEnd   * g_Images.end  [i].b) >> 8;

            g_Images.smoothIndex[i]++;
        }
    }
}

static inline void _PulseWidthModulation(void)
{
    static uint8_t s_pwmIndex = 0; // index of current PWM level

    if (s_pwmIndex == g_Settings.maxPwmValue)
    {
        s_pwmIndex = 0;

        _StartConstantTime();

        // Switch OFF LEDs on time sets in g_Settings.brightness
        LedDriver_OffLeds();

        // Also eval current image
        if (g_Settings.isSmoothEnabled)
        {
            EvalCurrentImage_SmoothlyAlg();
        }

        _EndConstantTime(g_Settings.brightness);
    }


    if (g_Settings.isSmoothEnabled)
    {
        LedDriver_UpdatePWM(g_Images.current, s_pwmIndex);
    } else {
        LedDriver_UpdatePWM(g_Images.end, s_pwmIndex);
    }

    s_pwmIndex++;

    // Clear timer counter
    TCNT1 = 0x0000;
}

void LedManager_UpdateColors(void)
{
	_PulseWidthModulation();
}

#endif
