/*
 * main.c
 *
 *   Created on: 21.07.2010
 *       Author: Mike Shatohin (brunql)
 *
 *      Project: AmbilightUSB
 *      	MCU: ATmega8
 *        Clock: 12MHz
 *
 * Command to fill flash and set fuses:
 * MCU ATmega8 (hw_v3.*):
 * avrdude  -pm8 -cusbasp -u -Uflash:w:AmbilightUSB.hex:a -Ulfuse:w:0x9f:m -Uhfuse:w:0xc9:m
 *
 * MCU ATtiny44 (hw_v2.*):
 * avrdude  -pt44 -cusbasp -u -Uflash:w:AmbilightUSB.hex:a -Ulfuse:w:0xee:m -Uhfuse:w:0xdf:m -Uefuse:w:0xff:m
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

#define LED1 	LEFT_DOWN
#define LED2 	LEFT_UP
#define LED3 	RIGHT_DOWN
#define LED4 	RIGHT_UP



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

// If smooth changing colors ON, is_smooth_change == 1, else == 0
volatile uint8_t is_smooth_change = 0;

// Smoothly changing colors index
volatile uint8_t smooth = 0x00;
volatile uint8_t smooth_delay = 0x00;
volatile uint8_t smooth_step[4][3] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} };;


void SmoothlyUpdateColors(void)
{
//	smooth_step evaluated when new color comes from PC

	if(++smooth >= smooth_delay){
		update_colors = FALSE;
		for(uint8_t color=0; color < 3; color++){
			for(uint8_t led_index=0; led_index < 4; led_index++){
				int8_t diff = colors[led_index][color] - colors_new[led_index][color];
				if(((uint8_t)ABS(diff) / SMOOTH_MAX_STEP) > 0){
					colors[led_index][color] -= smooth_step[led_index][color];
					update_colors = TRUE;
				}else if(ABS(diff) % SMOOTH_MAX_STEP > 0){
					colors[led_index][color] = colors_new[led_index][color];
				}
			}
		}
		smooth = 0x00;
	}
}



#define CHECK_PWM_LEVEL_AND_SET_HC595_OUT( LED_INDEX, COLOR_INDEX ) {\
		HC595_CLK_DOWN; \
		if(colors[LED_INDEX][COLOR_INDEX] > pwm_level){ \
			HC595_DATA_PORT &= (uint8_t)~HC595_DATA0_PIN; \
		}else{ \
			HC595_DATA_PORT |= HC595_DATA0_PIN; \
		} \
		HC595_CLK_UP;\
	}


static inline void PWM()
{
	if(++pwm_level >= pwm_level_max){
		pwm_level = 0x00;

		if(update_colors){
			if(smooth_delay != 0){
				SmoothlyUpdateColors();
			}
		}
	}


	// Skip I/O - QH of 74HC595 (IC5, IC6)
	HC595_CLK_DOWN;
	HC595_DATA_PORT = 0x00;
	HC595_CLK_UP;


	if(smooth_delay == 0){
		// Set I/O - QH, QF, QE of 74HC595 (IC5, IC6)
		for(uint8_t color=0; color<3; color++){
			uint8_t hc595_data = 0x00;

			HC595_CLK_DOWN;
			if(colors_new[LED2][color] > pwm_level)  hc595_data |= HC595_DATA0_PIN;
			if(colors_new[LED4][color] > pwm_level)  hc595_data |= HC595_DATA1_PIN;
			HC595_DATA_PORT = hc595_data;
			HC595_CLK_UP;
		}

		// Set I/O - QD, QC, QB of 74HC595 (IC5, IC6)
		for(uint8_t color=0; color<3; color++){
			uint8_t hc595_data = 0x00;

			HC595_CLK_DOWN;
			if(colors_new[LED1][color] > pwm_level)  hc595_data |= HC595_DATA0_PIN;
			if(colors_new[LED3][color] > pwm_level)  hc595_data |= HC595_DATA1_PIN;
			HC595_DATA_PORT = hc595_data;
			HC595_CLK_UP;
		}
	}else{
		// Set I/O - QH, QF, QE of 74HC595 (IC5, IC6)
		for(uint8_t color=0; color<3; color++){
			uint8_t hc595_data = 0x00;

			HC595_CLK_DOWN;
			if(colors[LED2][color] > pwm_level)  hc595_data |= HC595_DATA0_PIN;
			if(colors[LED4][color] > pwm_level)  hc595_data |= HC595_DATA1_PIN;
			HC595_DATA_PORT = hc595_data;
			HC595_CLK_UP;
		}

		// Set I/O - QD, QC, QB of 74HC595 (IC5, IC6)
		for(uint8_t color=0; color<3; color++){
			uint8_t hc595_data = 0x00;

			HC595_CLK_DOWN;
			if(colors[LED1][color] > pwm_level)  hc595_data |= HC595_DATA0_PIN;
			if(colors[LED3][color] > pwm_level)  hc595_data |= HC595_DATA1_PIN;
			HC595_DATA_PORT = hc595_data;
			HC595_CLK_UP;
		}
	}

	// Skip I/O - QA of 74HC595 (IC5, IC6)
	HC595_CLK_DOWN;
	HC595_DATA_PORT = 0x00;
	HC595_CLK_UP;

	HC595_LATCH_PULSE;
}


//
// Interrupts of the timer that generates PWM
//
ISR( TIMER1_COMPA_vect )
{
	// Enable interrupts for usbPoll();
	sei();

	// Set next PWM states for all channels
	PWM();

	// Clear timer counter
	TCNT1 = 0x0000;
}




void SetAllLedsColors(uint8_t red, uint8_t green, uint8_t blue)
{
	for(uint8_t i=0; i<4; i++){
		colors[i][R] = red;
		colors[i][G] = green;
		colors[i][B] = blue;
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

	// Default values of timer prescaller and output compare register
	TCCR1B = _BV(CS11); // 8
	OCR1A = 100;

	TCNT1 = 0x0000;
	TIMSK = _BV(OCIE1A);
}

//
//	Initializing I/O, USB and starts PWM generation
//
int main(void)
{
	// Input/Output Ports initialization
	PORTB = 0x00;
	DDRB = 0x00;

    PORTC = 0x00;
    DDRC = 0x00;

    PORTD = 0x00;
    DDRD = 0x00;

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

