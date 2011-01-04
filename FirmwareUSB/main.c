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
 *  This firmware for the microcontroller, which transceiver USB in UART
 *
 *  Command to fill flash and set fuses:
 *  avrdude  -pm8 -cusbasp -u -Uflash:w:FirmwareUSB.hex:a -Ulfuse:w:0x9f:m -Uhfuse:w:0xc9:m
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

#include "main.h"
#include <util/delay.h>

#include "RGB.h"
#include "vusb.h"       /* Using V-USB library from OBDEV team */


//
// Global variables
//

// If TRUE new colors comes from PC
volatile uint8_t update_colors = FALSE;

// Contains new colors (comes from PC)
volatile uint8_t colors_new[LEDS_COUNT][3] = {
		{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
		{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}
};

static inline void UartInitTX()
{
#define BAUD 115200
#include <util/setbaud.h>
	UBRRL = UBRRL_VALUE;
	UBRRH = UBRRH_VALUE;
#if USE_2X
	UCSRA |= (1 << U2X);
#else
	UCSRA &= ~(1 << U2X);
#endif

	UCSRB = _BV(TXEN);
	UCSRC = _BV(URSEL) | _BV(USBS) | _BV(UCSZ1) | _BV(UCSZ0);
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


    UartInitTX();

    // Low speed USB device initialization
    usbInit_FakeUsbDisconnect();

    // Enable interrupts
	sei(); // USB using INT0

	uint8_t sendme_led = 0;
	uint8_t sendme_color = 0;


   	for(;;){
   		/* Hey, PC! I'm alive! :) */
   	    usbPoll();
   	    
   	    if(update_colors){
   	    	while((UCSRA&(1<<UDRE)) == 0);

   	    	if(sendme_led == LEDS_COUNT && sendme_color == 0){
   	    		UDR = 0xff;
   	    		sendme_led = 0x00;
   	    	}else{
   	    		UDR = colors_new[sendme_led][sendme_color];

   	    		if(++sendme_color == 3){
   	   	    		sendme_color = 0;
   	   	    		if(++sendme_led == LEDS_COUNT){
   	   	    			sendme_led = LEDS_COUNT;
   	   	    			update_colors = FALSE;
   	   	    		}
   	   	    	}
   	    	}
   	    }
	}

	return 0;
}

