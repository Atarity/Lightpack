/*
 * Lightpack.h
 *
 *  Created on: 11.01.2011
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

#ifndef LIGHTPACK_H_INCLUDED
#define LIGHTPACK_H_INCLUDED

/* Includes: */
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>

#include "Descriptors.h"

#include <LUFA/Version.h>
#include <LUFA/Drivers/Board/LEDs.h>
#include <LUFA/Drivers/USB/USB.h>

#include "flags.h"
#include "iodefs.h"
#include "datatypes.h"
#include "version.h"

#include "../CommonHeaders/LEDS_COUNT.h"

/* LED use with I/O manipulations macroses from iodefs.h */
#define PINRX   (D,2)
#define PINTX   (D,3)
#define USBLED  (C,5)

// Global variables
extern Settings_t g_Settings;
extern Images_t g_Images;


static inline void _BlinkUsbLed(const uint8_t times, const uint8_t ms)
{
    for (uint8_t t = 0; t < times; t++)
    {
        SET(USBLED);
        for (uint8_t i = 0; i < ms; i++)
            _delay_ms(1);

        CLR(USBLED);
        for (uint8_t i = 0; i < ms; i++)
            _delay_ms(1);
    }
    SET(USBLED);
}

static inline void _SmoothSwitchOnUsbLed(const uint8_t num)
{
    for (uint8_t pwm = 0; pwm < num; pwm++)
    {
        SET(USBLED);
        for (uint8_t i = 0; i < pwm; i++)
            _delay_us(50);

        CLR(USBLED);
        for (uint8_t i = 0; i < num - pwm; i++)
            _delay_us(50);
    }
    SET(USBLED);
}

#endif /* LIGHTPACK_H_INCLUDED */

