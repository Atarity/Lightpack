/*
 * main.c
 *
 *   Created on: 21.07.2010
 *       Author: Mike Shatohin (brunql)
 *
 *      Project: AmbilightUSB
 *      	MCU: ATtiny44
 *        Clock: 12MHz
 *
 * Command to fill flash and set fuses:       
 *
 * avrdude	-pt44 -cusbasp -u
 *          -Uflash:w:AmbilightUSB.hex:a
 *          -Ulfuse:w:0xee:m
 *          -Uhfuse:w:0xdf:m
 *          -Uefuse:w:0xff:m
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
 
#include <avr/io.h>
#include <avr/interrupt.h> /* for sei() */
#include <util/delay.h>

#include "main.h"
#include "74HC595.h"    /* RGB leds connects to ATtiny44 through 74HC595 */
#include "vusb.h"       /* Using V-USB library from OBDEV team */


//
// Global variables
//

// If TRUE need to update colors[]
volatile uint8_t update_colors = FALSE;

// Contains colors using now in PWM generation
volatile uint8_t colors[4][3] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} };

// Contains new colors (comes from PC) for update with smooth (if it is used)
volatile uint8_t colors_new[4][3] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} };

// Current PWM generation level
volatile uint8_t pwm_level = 0x00;
volatile uint8_t pwm_level_max = 64;

// Smoothly changing colors index
volatile uint8_t smooth = 0x00;


#define CHECK_PWM_LEVEL_AND_SET_HC595_OUT( LED_INDEX, COLOR_INDEX ) {\
		HC595_CLK_DOWN; \
		if(colors_new[LED_INDEX][COLOR_INDEX] > pwm_level){ \
			HC595_PORT &= (uint8_t)~HC595_DATA_PIN; \
		}else{ \
			HC595_PORT |= HC595_DATA_PIN; \
		} \
		HC595_CLK_UP;\
	}


static inline void PWM()
{
	if(++pwm_level >= pwm_level_max){
		pwm_level = 0x00;
	}

	// LEFT_UP
	HC595_CLK_DOWN;
	HC595_PORT |= HC595_DATA_PIN;
	HC595_CLK_UP;

	for(uint8_t color=0; color<3; color++){
		CHECK_PWM_LEVEL_AND_SET_HC595_OUT(LEFT_UP, color);
	}

	// LEFT_DOWN
	HC595_CLK_DOWN;
	HC595_PORT |= HC595_DATA_PIN;
	HC595_CLK_UP;

	for(uint8_t color=0; color<3; color++){
		CHECK_PWM_LEVEL_AND_SET_HC595_OUT(LEFT_DOWN, color);
	}



	// RIGHT_DOWN
	for(uint8_t color=0; color<3; color++){
		CHECK_PWM_LEVEL_AND_SET_HC595_OUT(RIGHT_DOWN, color);
	}
	HC595_CLK_DOWN;
	HC595_PORT |= HC595_DATA_PIN;
	HC595_CLK_UP;


	// RIGHT_UP
	for(uint8_t color=0; color<3; color++){
		CHECK_PWM_LEVEL_AND_SET_HC595_OUT(RIGHT_UP, color);
	}
	HC595_CLK_DOWN;
	HC595_PORT |= HC595_DATA_PIN;
	HC595_CLK_UP;

	HC595_LATCH_PULSE;
}



//
// Interrupts of the timer that generates PWM
//
ISR( TIM1_COMPA_vect )
{
	// Enable interrupts for usbPoll();
	sei();

	// Generate one PWM on all channels
	PWM();

	// Clear timer counter
	TCNT1 = 0x0000;
}




void SetAllLedsColors(uint8_t red, uint8_t green, uint8_t blue)
{
	for(uint8_t i=0; i<4; i++){
		colors_new[i][R] = red;
		colors_new[i][G] = green;
		colors_new[i][B] = blue;
	}
}


//
// When you connect the device to the USB,
// LEDs are smoothly grow and fade the red, green and blue colors.
//
static inline void SmoothlyShowRGB()
{
	uint8_t fsm = 1, red = 0, green = 0, blue = 0;

	SetAllLedsColors(0, 0, 0);
	_delay_ms(10);

	for(;fsm != 0;){
		switch(fsm){
		case 1:
			SetAllLedsColors(red++, green, blue);
			if(red >= pwm_level_max){
				fsm = 2;
			}
			break;
		case 2:
			SetAllLedsColors(red--, green++, blue);
			if(red == 0){
				fsm = 3;
			}
			break;
		case 3:
			SetAllLedsColors(red, green--, blue++);
			if(green == 0){
				fsm = 4;
			}
			break;
		case 4:
			SetAllLedsColors(red, green, blue--);
			if(blue == 0){
				fsm = 0;
			}
			break;
		default: fsm = 0; break;
		}
		_delay_ms(3);
	}

	SetAllLedsColors(0, 0, 0);
}

static inline void TimerForPWM_Init()
{
	TCCR1A = 0x00;
	TCCR1C = 0x00;

	// Default values of timer prescaller and output compare register
	TCCR1B = _BV(CS11); // 8
	OCR1A = 100;

	TCNT1 = 0x0000;
	TIMSK1 = _BV(OCIE1A);
}

//
//	Initializing I/O, USB and starts PWM generation
//
int main(void)
{
	// Input/Output Ports initialization
	PORTA = 0x00;
	DDRA = 0x00;

    PORTB = 0x00;
    DDRB = 0x00;

    // Low speed USB device initialization
    usbInit_FakeUsbDisconnect();

    // HC595 ports initialization
    HC595_Init();

    // Initialize timer used to for generate the PWM
    TimerForPWM_Init();

    // Enable interrupts
	sei(); // USB using INT0

	SmoothlyShowRGB();


   	for(;;){
   		/* Hey, PC! I'm alive! :) */
   		usbPoll();
	}

	return 0;
}

