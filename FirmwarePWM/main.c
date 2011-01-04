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
 *  This firmware for the microcontroller, which controls the LEDs (PWM).
 *
 *  Command to fill flash and set fuses:
 
 TODO: Check fuses
 
 *  avrdude  -pm8 -cusbasp -u -Uflash:w:FirmwarePWM.hex:a -Ulfuse:w:0x9f:m -Uhfuse:w:0xc9:m
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
 
#include <avr/io.h>
#include <avr/interrupt.h> /* for sei() */
#include <util/delay.h>

#include "main.h"
#include "74HC595.h"
#include "RGB.h"


//
// Global variables
//

// If TRUE need to update colors[]
volatile uint8_t update_colors = FALSE;

// Contains colors using now in PWM generation
volatile uint8_t colors[LEDS_COUNT][3] = {
		{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
		{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}
};

// Contains new colors (comes from PC) for update with smooth (if it is used)
volatile uint8_t colors_new[LEDS_COUNT][3] = {
		{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
		{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}
};

// Current PWM generation level
volatile uint8_t pwm_level = 0x00;
volatile uint8_t pwm_level_max = 64;

// If smooth changing colors ON, is_smooth_change == 1, else == 0
//volatile uint8_t is_smooth_change = 0;

// Smoothly changing colors index
volatile uint8_t smooth = 0x00;
volatile uint8_t smooth_delay = 0x00;
volatile uint8_t smooth_step[LEDS_COUNT][3] = {
		{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
		{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}
};

volatile int16_t max_diff = 0x00;



void SmoothlyUpdateColors(void)
{
	// Array smooth_step evaluated when new color comes from PC

	for(uint8_t color=0; color < 3; color++){
		for(uint8_t led_index=0; led_index < LEDS_COUNT; led_index++){
			if(smooth % smooth_step[led_index][color] == 0){
				if(colors[led_index][color] < colors_new[led_index][color]){
					colors[led_index][color] += 1;
				}
				if(colors[led_index][color] > colors_new[led_index][color]){
					colors[led_index][color] -= 1;
				}
				update_colors = TRUE;
			}
		}
	}

	if(++smooth >= max_diff){
		smooth = 0x00;
		update_colors = FALSE;
	}
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

    // TODO: Check I/O of the 74HC595, skip QH or not?
	// Skip I/O - QH of 74HC595 (IC5, IC6)
	HC595_CLK_DOWN;
	HC595_DATA_PORT = 0x00;
	HC595_CLK_UP;


	uint8_t hc595_data = 0x00;

	if(smooth_delay == 0){
		// Set I/O - QH, QF, QE of 74HC595 (IC5, IC6)
		for(uint8_t color=0; color<3; color++){
			hc595_data = 0x00;

			HC595_CLK_DOWN;
			if(colors_new[LED2][color] > pwm_level)  hc595_data |= HC595_DATA0_PIN;
			if(colors_new[LED4][color] > pwm_level)  hc595_data |= HC595_DATA1_PIN;
			if(colors_new[LED6][color] > pwm_level)  hc595_data |= HC595_DATA2_PIN;
			if(colors_new[LED8][color] > pwm_level)  hc595_data |= HC595_DATA3_PIN;
			HC595_DATA_PORT = hc595_data;
			HC595_CLK_UP;
		}

		// Set I/O - QD, QC, QB of 74HC595 (IC5, IC6)
		for(uint8_t color=0; color<3; color++){
			hc595_data = 0x00;

			HC595_CLK_DOWN;
			if(colors_new[LED1][color] > pwm_level)  hc595_data |= HC595_DATA0_PIN;
			if(colors_new[LED3][color] > pwm_level)  hc595_data |= HC595_DATA1_PIN;
			if(colors_new[LED5][color] > pwm_level)  hc595_data |= HC595_DATA2_PIN;
			if(colors_new[LED7][color] > pwm_level)  hc595_data |= HC595_DATA3_PIN;
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
			if(colors[LED6][color] > pwm_level)  hc595_data |= HC595_DATA2_PIN;
			if(colors[LED8][color] > pwm_level)  hc595_data |= HC595_DATA3_PIN;
			HC595_DATA_PORT = hc595_data;
			HC595_CLK_UP;
		}

		// Set I/O - QD, QC, QB of 74HC595 (IC5, IC6)
		for(uint8_t color=0; color<3; color++){
			uint8_t hc595_data = 0x00;

			HC595_CLK_DOWN;
			if(colors[LED1][color] > pwm_level)  hc595_data |= HC595_DATA0_PIN;
			if(colors[LED3][color] > pwm_level)  hc595_data |= HC595_DATA1_PIN;
			if(colors[LED5][color] > pwm_level)  hc595_data |= HC595_DATA2_PIN;
			if(colors[LED7][color] > pwm_level)  hc595_data |= HC595_DATA3_PIN;
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
	// Enable interrupts for USB INT0
	sei();

	TEST_TOGGLE();

	// Set next PWM states for all channels
	PWM();

	TEST_TOGGLE();


	// Clear timer counter
	TCNT1 = 0x0000;
}

//ISR( UART0_vect )
//{
//    // TODO: check and save new data here or in main loop, think about it.
//}


void SetAllLedsColors(uint8_t red, uint8_t green, uint8_t blue)
{
	for(uint8_t i=0; i<LEDS_COUNT; i++){
		if(smooth_delay == 0){
			colors_new[i][R] = red;
			colors_new[i][G] = green;
			colors_new[i][B] = blue;
		}else{
			colors[i][R] = red;
			colors[i][G] = green;
			colors[i][B] = blue;
		}
	}
}


//
// When you connect the device to the USB,
// LEDs are smoothly grow and fade the red, green and blue colors.
//
static inline void SmoothlyShowRGB()
{

// TODO: rewrite it on timer interrupts

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

static inline void HC595_Init(void)
{
	HC595_LATCH_DOWN;
	HC595_CLK_DOWN;
	HC595_OUT_ENABLE;
	HC595_DDR |= HC595_CLK_PIN | HC595_LATCH_PIN | HC595_OUT_EN_PIN;
	HC595_DATA_PORT = 0x00;
	HC595_DATA_DDR = HC595_DATA0_PIN | HC595_DATA1_PIN | HC595_DATA2_PIN | HC595_DATA3_PIN;
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

    TEST_PIN_DDR_INIT();
    TEST_DOWN();


    // HC595 ports initialization
    HC595_Init();

    // Initialize timer used to for generate the PWM
    TimerForPWM_Init();
    
    // TODO: initialize UART0

    // Enable interrupts
	sei(); 
	
	SmoothlyShowRGB();

   	for(;;){
   	    // Wait interrupts for PWM generation
   	    
   	    // TODO: Syncronize the UART transfer and PWM
	}

	return 0;
}

