/*
 * LedDriver.h
 *
 *  Created on: 31.01.2011
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
 

#ifndef LEDDRIVER_H_INCLUDED
#define LEDDRIVER_H_INCLUDED 1

#include <avr/io.h>

#define LEDDRIVERs_PORT			(PORTB)
#define LEDDRIVERs_DDR 			(DDRB)

#define LEDDRIVER1_LATCH_PIN	_BV(PB4)
#define LEDDRIVER1_CLK_PIN		_BV(PB5)
#define LEDDRIVER1_DATA_PIN 	_BV(PB6)

#define LEDDRIVER2_LATCH_PIN	_BV(PB0)
#define LEDDRIVER2_CLK_PIN		_BV(PB1)
#define LEDDRIVER2_DATA_PIN 	_BV(PB2)


static inline void LedDrivers_LatchUp(void)
{
	LEDDRIVERs_PORT |= LEDDRIVER1_LATCH_PIN | LEDDRIVER2_LATCH_PIN;
}

static inline void LedDrivers_LatchDown(void)
{
	LEDDRIVERs_PORT &= (uint8_t)~(LEDDRIVER1_LATCH_PIN | LEDDRIVER2_LATCH_PIN);
}

static inline void LedDrivers_LatchPulse(void)
{
	LedDrivers_LatchUp();
	__asm__ __volatile__("nop"::);
	LedDrivers_LatchDown();
}

static inline void LedDrivers_ClkUp(void)
{
	LEDDRIVERs_PORT |= LEDDRIVER1_CLK_PIN | LEDDRIVER2_CLK_PIN;
}

static inline void LedDrivers_ClkDown(void)
{
	LEDDRIVERs_PORT &= (uint8_t)~(LEDDRIVER1_CLK_PIN | LEDDRIVER2_CLK_PIN);
}

static inline void LedDriver1_OutOn(void)
{
	LEDDRIVERs_PORT |= LEDDRIVER1_DATA_PIN;
}

static inline void LedDriver1_OutOff(void)
{
	LEDDRIVERs_PORT &= (uint8_t)~LEDDRIVER1_DATA_PIN;
}

static inline void LedDriver2_OutOn(void)
{
	LEDDRIVERs_PORT |= LEDDRIVER2_DATA_PIN;
}

static inline void LedDriver2_OutOff(void)
{
	LEDDRIVERs_PORT &= (uint8_t)~LEDDRIVER2_DATA_PIN;
}


static inline void LedDriver_Init(void)
{
	LedDrivers_LatchDown();
	LedDrivers_ClkDown();

	LEDDRIVERs_PORT = 0x00;
	LEDDRIVERs_DDR = 0xff;
}


#endif /* LEDDRIVER_H_INCLUDED */
