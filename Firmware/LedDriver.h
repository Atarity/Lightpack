/*
 * LedDriver.h
 *
 *  Created on: 31.01.2011
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

#ifndef LEDDRIVER_H_INCLUDED
#define LEDDRIVER_H_INCLUDED

#include <stdint.h>

#include "datatypes.h"
#include "../CommonHeaders/LEDS_COUNT.h"

void LedDriver_Init(void);
void LedDriver_UpdatePWM(const RGB_t imageFrame[LEDS_COUNT], const uint8_t pwmIndex);
void LedDriver_OffLeds(void);

#endif /* LEDDRIVER_H_INCLUDED */
