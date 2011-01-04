/*
 * main.h
 *
 *  Created on: 21.07.2010
 *      Author: Mike Shatohin (brunql)
 *     Project: AmbilightUSB
 *
 *  AmbilightUSB is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2010, 2011 Mike Shatohin, mikeshatohin [at] gmail.com
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
 

#ifndef MAIN_H_
#define MAIN_H_

#include "RGB.h"

// Test pin on PORTA.7 using for debugging and evaluation time intervals
#define TEST_PIN_DDR_INIT()		{ DDRD |= _BV(PD0); }
#define TEST_UP()				{ PORTD |= _BV(PD0); }
#define TEST_DOWN()				{ PORTD &= (uint8_t)~_BV(PD0); }
#define TEST_TOGGLE()			{ PORTD ^= _BV(PD0); }

#define ABS( x ) ((x)<0?(-x):(x))

// If this TRUE we need to update our colors[] array (with smooth if it is used)
extern volatile uint8_t update_colors;

// Save new colors comes from PC
extern volatile uint8_t colors_new[LEDS_COUNT][3];

#endif /* MAIN_H_ */
