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

#define LEDS_COUNT       4

// On this "screen", digit is number of led (leds behind of screen)
//   ╔═════════╗
//  3╠  Linux  ╣0
//  2╠  brunql ╣1
//   ╚═════════╝
#define RIGHT_UP	0
#define RIGHT_DOWN	1
#define LEFT_DOWN	2
#define LEFT_UP		3

// Define indexes for colors and colors_new arrays
#define R	2
#define G	1
#define B	0

// Hardware RGB bits definitions:

// Right side
#define RED_RIGHT 		0b0100ul
#define GREEN_RIGHT 	0b0010ul
#define BLUE_RIGHT 		0b0001ul

#define RIGHT_UP_RED_LS		(RED_RIGHT 		<< 12)
#define RIGHT_UP_GREEN_LS	(GREEN_RIGHT 	<< 12)
#define RIGHT_UP_BLUE_LS	(BLUE_RIGHT 	<< 12)

#define RIGHT_DOWN_RED_LS	(RED_RIGHT 		<< 8)
#define RIGHT_DOWN_GREEN_LS	(GREEN_RIGHT 	<< 8)
#define RIGHT_DOWN_BLUE_LS	(BLUE_RIGHT 	<< 8)

// Left side
#define RED_LEFT 	0b1000ul
#define GREEN_LEFT 	0b0100ul
#define BLUE_LEFT 	0b0010ul

#define LEFT_DOWN_RED_LS	(RED_LEFT 	<< 4)
#define LEFT_DOWN_GREEN_LS	(GREEN_LEFT << 4)
#define LEFT_DOWN_BLUE_LS	(BLUE_LEFT 	<< 4)

#define LEFT_UP_RED_LS		(RED_LEFT 	<< 0)
#define LEFT_UP_GREEN_LS	(GREEN_LEFT << 0)
#define LEFT_UP_BLUE_LS		(BLUE_LEFT 	<< 0)

// Be sure that this is defined
#ifndef TRUE
#	define TRUE 	0xff
#	define FALSE	0x00
#endif


#endif /* RGB_H_ */
