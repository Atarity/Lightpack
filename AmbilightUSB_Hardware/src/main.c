/*
 * main.c
 *
 *  Created on: 21.07.2010
 *      Author: brunql
 *
 *      Project: AmbilightUSB
 *      	MCU: ATtiny44
 *        Clock: 12MHz
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

// Smoothly changing colors index
volatile uint8_t smooth = 0x00;


static inline void PWM()
{
	if(++pwm_level >= PWM_LEVEL){
		pwm_level = 0x00;
	}

	uint16_t leds = 0x0000;

	if(colors[RIGHT_UP][R] > pwm_level) 	leds |= RIGHT_UP_RED_LS;
	if(colors[RIGHT_UP][G] > pwm_level) 	leds |= RIGHT_UP_GREEN_LS;
	if(colors[RIGHT_UP][B] > pwm_level) 	leds |= RIGHT_UP_BLUE_LS;

	if(colors[RIGHT_DOWN][R] > pwm_level) 	leds |= RIGHT_DOWN_RED_LS;
	if(colors[RIGHT_DOWN][G] > pwm_level) 	leds |= RIGHT_DOWN_GREEN_LS;
	if(colors[RIGHT_DOWN][B] > pwm_level) 	leds |= RIGHT_DOWN_BLUE_LS;

	if(colors[LEFT_DOWN][R] > pwm_level) 	leds |= LEFT_DOWN_RED_LS;
	if(colors[LEFT_DOWN][G] > pwm_level) 	leds |= LEFT_DOWN_GREEN_LS;
	if(colors[LEFT_DOWN][B] > pwm_level) 	leds |= LEFT_DOWN_BLUE_LS;

	if(colors[LEFT_UP][R] > pwm_level) 		leds |= LEFT_UP_RED_LS;
	if(colors[LEFT_UP][G] > pwm_level) 		leds |= LEFT_UP_GREEN_LS;
	if(colors[LEFT_UP][B] > pwm_level) 		leds |= LEFT_UP_BLUE_LS;

	// RGB leds connects with common anode
	HC595_PutUInt16((uint16_t)~leds);
}

static inline void smoothlyChangeColors()
{
	// Check if we have new colors
	if(update_colors){
		if(++smooth >= SMOOTHLY_DELAY){
			update_colors = FALSE;
			for(uint8_t led_index=0; led_index < 4; led_index++){
				for(uint8_t color=0; color < 3; color++){
					if(colors[led_index][color] < colors_new[led_index][color]){
						colors[led_index][color]++;
						update_colors = TRUE;
					}
					if(colors[led_index][color] > colors_new[led_index][color]){
						colors[led_index][color]--;
						update_colors = TRUE;
					}
				}
			}
			smooth = 0x00;
		}
	}

//	// Without smooth
//	for(uint8_t led_index=0; led_index < 4; led_index++){
//		for(uint8_t color=0; color < 3; color++){
//			colors[led_index][color] = colors_new[led_index][color];
//		}
//	}
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

    // Enable interrupts
	sei(); // USB using INT0


   	for(;;){
   		/* Hey, PC! I'm alive! :) */
   		usbPoll();

   		/* Update leds */
   		PWM();

   		/* If have new colors, update it smoothly */
   		smoothlyChangeColors();
	}

	return 0;
}

