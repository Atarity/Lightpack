/*
 * RGB.h
 *
 *  Created on: 23.07.2010
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
 

#ifndef RGB_H_
#define RGB_H_

// Number of leds, using to define colors and colors_new arrays
#define LEDS_COUNT  8


enum LEDS{
	LED1 = 0, /* this is indexes in colors and colors_new arrays */
	LED2,
	LED3,
	LED4,
	LED5,
	LED6,
	LED7,
	LED8,
};


// Define indexes for colors and colors_new arrays
#define R	1
#define G	0
#define B	2

// Be sure that this is defined
#ifndef TRUE
#	define TRUE 	0xff
#	define FALSE	0x00
#endif


#endif /* RGB_H_ */
