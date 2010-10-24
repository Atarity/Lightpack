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

ISR( TIM1_COMPA_vect )
{
	// Enable interrupts for usbPoll();
	sei();

	// Generate one PWM on all channels
	PWM();

	// Clear timer counter
	TCNT1 = 0x0000;
}


static inline void ShowRGB()
{
	HC595_PutUInt16((uint16_t)~
			(RIGHT_UP_RED_LS | LEFT_UP_RED_LS |
					RIGHT_DOWN_RED_LS | LEFT_DOWN_RED_LS));
	_delay_ms(400);

	HC595_PutUInt16((uint16_t)~
			(RIGHT_UP_GREEN_LS | LEFT_UP_GREEN_LS |
					RIGHT_DOWN_GREEN_LS | LEFT_DOWN_GREEN_LS));
	_delay_ms(400);

	HC595_PutUInt16((uint16_t)~
			(RIGHT_UP_BLUE_LS | LEFT_UP_BLUE_LS |
					RIGHT_DOWN_BLUE_LS | LEFT_DOWN_BLUE_LS));
	_delay_ms(400);
}

static inline void TimerForPWM_Init()
{
	TCCR1A = 0x00;
	TCCR1C = 0x00;

	// Default values of timer prescaller and output compare register
	TCCR1B = _BV(CS11) | _BV(CS10); // 64
	OCR1A = 7;

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

    // Swith ON all leds red, green and blue LEDs with delays
    ShowRGB();


    // Initialize timer used to for generate the PWM
    TimerForPWM_Init();


    // Enable interrupts
	sei(); // USB using INT0


   	for(;;){
   		/* Hey, PC! I'm alive! :) */
   		usbPoll();
	}

	return 0;
}

