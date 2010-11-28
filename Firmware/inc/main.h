/*
 * main.h
 *
 *  Created on: 21.07.2010
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
 

#ifndef MAIN_H_
#define MAIN_H_

#include "RGB.h"

// Test pin on PORTA.7 using for debugging and evaluation time intervals
#define TEST_PIN_DDR_INIT()		{ DDRA |= _BV(PA7); }
#define TEST_UP()				{ PORTA |= _BV(PA7); }
#define TEST_DOWN()				{ PORTA &= (uint8_t)~_BV(PA7); }



// If this TRUE we need to update our colors[] array (with smooth if it is used)
extern volatile uint8_t update_colors;

// Save new colors comes from PC
extern volatile uint8_t colors_new[4][3];

// Color max value, i.e. 0 <= color[led][c] < pwm_level_max
extern volatile uint8_t pwm_level_max;

extern volatile uint8_t is_smooth_change;

#endif /* MAIN_H_ */
