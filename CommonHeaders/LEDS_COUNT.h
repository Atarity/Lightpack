/*
 * LEDS_COUNT.h
 *
 *  Created on: 23.07.2010
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Lightpack is a content-appropriate ambient lighting system for any computer
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
 

#ifndef LEDS_COUNT_H_INCLUDED
#define LEDS_COUNT_H_INCLUDED

#include "LIGHTPACK_HW.h"

#if (LIGHTPACK_HW == 6)

#	define LEDS_COUNT  10

#elif (LIGHTPACK_HW == 5)

#	define LEDS_COUNT  10

#elif (LIGHTPACK_HW == 4)

#	define LEDS_COUNT  8

#else
#   error "LIGHTPACK_HW must be defined in 'CommonHeaders/LIGHTPACK_HW.h' to major number of the hardware revision"
#endif

#endif /* LEDS_COUNT_H_INCLUDED */
