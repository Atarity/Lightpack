/*
 * 74HC595.h
 *
 *  Created on: 18.07.2010
 *      Author: Mike Shatohin (brunql)
 *     Project: AmbilightUSB
 *
 *  AmbilightUSB is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2010 Mike Shatohin, mikeshatohin [at] gmail.com
 *
 *  AmbilightUSB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  AmbilightUSB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
 

#ifndef HC595_H_INCLUDED
#define HC595_H_INCLUDED 1

#include <avr/io.h>

#define HC595_PORT 			(PORTB)
#define HC595_DDR 			(DDRB)
#define HC595_CLK_PIN 		_BV(PB0)
#define HC595_LATCH_PIN		_BV(PB1)
#define HC595_OUT_EN_PIN 	_BV(PB2)

#define HC595_DATA_PORT 	(PORTC)
#define HC595_DATA_DDR 		(DDRC)
#define HC595_DATA0_PIN 	_BV(PC0)
#define HC595_DATA1_PIN 	_BV(PC1)
#define HC595_DATA2_PIN 	_BV(PC2)
#define HC595_DATA3_PIN 	_BV(PC3)


#define HC595_LATCH_UP		{ HC595_PORT |= HC595_LATCH_PIN; }
#define HC595_LATCH_DOWN	{ HC595_PORT &= (uint8_t)~HC595_LATCH_PIN; }
#define HC595_LATCH_PULSE	{ HC595_LATCH_UP; __asm__ __volatile__("nop"::); HC595_LATCH_DOWN; }

#define HC595_CLK_DOWN		{ HC595_PORT &= (uint8_t)~HC595_CLK_PIN; }
#define HC595_CLK_UP		{ HC595_PORT |= HC595_CLK_PIN; }

// Inverse logic
#define HC595_OUT_ENABLE	{ HC595_PORT &= (uint8_t)~HC595_OUT_EN_PIN; }
#define HC595_OUT_DISABLE	{ HC595_PORT |= HC595_OUT_EN_PIN; }

extern void HC595_Init(void);
extern void HC595_PutUInt16(uint16_t);

#endif /* HC595_H_INCLUDED */
