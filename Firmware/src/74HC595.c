/*
 * 74HC595.c
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

#include "74HC595.h"

//void HC595_PutUInt16(uint16_t data)
//{
//	char i;
//	for(i=0; i<16; i++){
//		HC595_CLK_DOWN;
//		if(data & _BV(i)){
//			HC595_PORT |= HC595_DATA_PIN;
//		}else{
//			HC595_PORT &= (uint8_t)~HC595_DATA_PIN;
//		}
//		HC595_CLK_UP;
//	}
//	HC595_LATCH_PULSE;
//}

void HC595_Init(void)
{
	HC595_LATCH_DOWN;
	HC595_CLK_DOWN;
	HC595_DDR |= HC595_CLK_PIN | HC595_LATCH_PIN;
	HC595_DATA_PORT = 0x00;
	HC595_DATA_DDR = HC595_DATA0_PIN | HC595_DATA1_PIN | HC595_DATA2_PIN | HC595_DATA3_PIN;
}
