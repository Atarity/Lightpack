/*
 * main.c
 *
 *   Created on: 21.07.2010
 *       Author: Mike Shatohin (brunql)
 *
 *      Project: AmbilightUSB
 *          MCU: ATmega8
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
#include "commands.h"

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



void UartSendByte(uint8_t byte)
{
    while((UCSRA & _BV(UDRE)) == 0x00);

    UDR = byte;
}



static inline void UartInit()
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

    // TX, RX Enabled, RX interrupts
    UCSRB = _BV(TXEN) | _BV(RXEN);

    // Even Parity, 2-stop bits, 8 bit
    UCSRC = _BV(URSEL) | _BV(UPM1) | _BV(USBS) | _BV(UCSZ1) | _BV(UCSZ0);
}

//
//    Initializing I/O, USB and starts PWM generation
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


    UartInit();

    // Low speed USB device initialization
    usbInit_FakeUsbDisconnect();

    // Enable interrupts
    sei(); // USB using INT0

    uint8_t transmit_led_index = 0;
    uint8_t transmit_color_index = 0;

    enum FSM{
        SEND_SoF,
        RECEIVE_RESULT,
        SEND_NEXT_BYTE,
        REPEAT_SEND_BYTE,
        STOP,
    } fsm = STOP;

    uint8_t last_sent_byte = 0x00;

    for(;;){
        /* Hey, PC! I'm alive! :) */
        usbPoll();

        if(update_colors){
            fsm = SEND_SoF;
            update_colors = FALSE;
        }

        switch(fsm){
        case SEND_SoF:
            transmit_led_index = 0x00;
            transmit_color_index = 0x00;

            last_sent_byte = START_OF_FRAME_COLORS;
            UartSendByte(last_sent_byte);

            fsm = RECEIVE_RESULT;
            break;
        case RECEIVE_RESULT:
            if(UCSRA & _BV(RXC)){
                uint8_t result = UDR;
                if(result == RECEIVE_OK){
                    if(transmit_led_index < LEDS_COUNT){
                        fsm = SEND_NEXT_BYTE;
                    }else{
                        fsm = STOP;
                    }
                }else if(result == RECEIVE_FAIL){
                    fsm = REPEAT_SEND_BYTE;
                }else{
                    // All bad, start from first byte
                    fsm = SEND_SoF;
                }
            }
            break;

        case SEND_NEXT_BYTE:
            last_sent_byte = colors_new[transmit_led_index][transmit_color_index];
            UartSendByte(last_sent_byte);

            if(++transmit_color_index >= 3){
                transmit_color_index = 0;
                transmit_led_index++;
            }
            fsm = RECEIVE_RESULT;
            break;
        case REPEAT_SEND_BYTE:
            UartSendByte(last_sent_byte);
            fsm = RECEIVE_RESULT;
            break;
        case STOP:
            break;
        default:
            fsm = STOP;
            break;
        };
    }

    return 0;
}
