/*
 * Lightpack.c
 *
 *  Created on: 11.01.2011
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

#include "Lightpack.h"
#include "LedDriver.h"
#include "LedManager.h"
#include "LightpackUSB.h"

volatile uint8_t g_Flags = 0;
volatile uint8_t g_isUsbLedOn = 1;

const uint8_t kT0CounterBase = 6;
const uint8_t kUsbLedHighLevel = 2;
const uint8_t kUsbLedLowLevel = 1;
const uint16_t kPostPrescaler = 5000;

uint8_t t0_counter = 0;
uint16_t t0_extra_counter = 0;

Images_t g_Images = { };

Settings_t g_Settings =
{
        .isSmoothEnabled = true,

        // Number of intermediate colors between old and new
        .smoothSlowdown = 100,

        .brightness = 50,

        // Maximum number of different colors for each channel (Red, Green and Blue)
        .maxPwmValue = 128,

        // Timer OCR value
        .timerOutputCompareRegValue = 100,
};

/*
 *  Interrupts of the timer that generates PWM
 */
ISR( TIMER1_COMPA_vect )
{
    LedManager_UpdateColors();

    // Clear timer interrupt flag
    TIFR1 = _BV(OCF1A);
}

ISR ( TIMER0_OVF_vect )
{
    t0_counter ++;
    t0_extra_counter ++;

    if ( t0_counter == kT0CounterBase ) {
        t0_counter = 0;
        SET(USBLED);
    }
    if ( t0_extra_counter == kPostPrescaler )
    {
        g_isUsbLedOn = 1;
        t0_extra_counter = 0;
    }
    
    if( g_isUsbLedOn != 0 )
    {
        if ( t0_counter == kUsbLedHighLevel )
        {
            CLR(USBLED);
        }
    } else
    {
        if ( t0_counter == kUsbLedLowLevel )
        {
            CLR(USBLED);
        }
    }
}

// Watchdog interrupt
ISR ( WDT_vect )
{
    _BlinkUsbLed(3, 40);
    for(;;);
}

static inline void Timer_Init(void)
{
    TCCR1A = 0x00;
    TCCR1C = 0x00;
    TCCR1B = 0x00;
    TCCR0A = 0x00;
    TCCR0B = 0x00;

    // Setup default value
    OCR1A = g_Settings.timerOutputCompareRegValue;

    TIMSK1 = _BV(OCIE1A); // Main timer
    TIMSK0 = _BV(TOIE0); // Usb led timer

    // Start timer
    TCCR1B = _BV(CS10); // div1
    TCCR0B = _BV(CS01); // div by 8

    TCNT1 = 0x0000;
    TCNT0 = 0x0000;
}

static inline void SetupHardware(void)
{
    // Watchdog configuration: interrupt after 250ms and reset after 500ms;
    wdt_enable(WDTO_250MS);
    // Enable watchdog interrupt
    WDTCSR |= _BV(WDIE);

    /* Disable clock division */
    clock_prescale_set(clock_div_1);

    /* Hardware Initialization */
    PORTB = 0x00;
    DDRB = 0x00;

    PORTC = 0x00;
    DDRC = 0x00;

    PORTD = 0x00;
    DDRD = 0x00;

    OUTPUT(USBLED);
    SET(USBLED);
}

static inline void _ProcessFlags(void)
{
    /* if (_FlagProcess(Flag_HaveNewColors)) */

    if (_FlagProcess(Flag_LedsOffAll))
        LedManager_FillImages(0, 0, 0);

    if (_FlagProcess(Flag_TimerOptionsChanged))
    {
        // Pause timer
        TIMSK1 &= (uint8_t)~_BV(OCIE1A);

        OCR1A = g_Settings.timerOutputCompareRegValue;

        // Restart timer
        TCNT1 = 0x0000;
        TIMSK1 = _BV(OCIE1A);
    }
}

/*
 *  Main program entry point
 */
int main(void)
{
    SetupHardware();

    // Led driver ports initialization
    LedDriver_Init();

    _SmoothSwitchOnUsbLed(0x40);

    // Initialize timer for update LedDriver-s
    Timer_Init();

    // Initialize USB
    USB_Init();

    sei();

    for (;;)
    {
        wdt_reset();
        ProcessUsbTasks();
        _ProcessFlags();
    }

    // Avoid annoying warning
    return 0;
}

