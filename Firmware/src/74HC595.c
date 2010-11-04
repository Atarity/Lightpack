/*
 * 74HC595.c
 *
 *  Created on: 18.07.2010
 *      Author: brunql
 */

#include "74HC595.h"

void HC595_PutUInt16(uint16_t data)
{
	char i;
	for(i=0; i<16; i++){
		HC595_CLK_DOWN;
		if(data & _BV(i)){
			HC595_PORT |= HC595_DATA_PIN;
		}else{
			HC595_PORT &= (uint8_t)~HC595_DATA_PIN;
		}
		HC595_CLK_UP;
	}
	HC595_LATCH_PULSE;
}

void HC595_Init(void)
{
	HC595_LATCH_DOWN;
	HC595_CLK_DOWN;
	HC595_DDR |= HC595_DATA_PIN | HC595_CLK_PIN | HC595_LATCH_PIN;
	HC595_PutUInt16(0xFFFF); // All off
}
