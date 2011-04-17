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
#include "LightpackUSB.h"

// Flags register
volatile uint8_t g_Flags = 0;

Images_t g_Images = { };

Settings_t g_Settings =
{
        .isSmoothEnabled = true,

        // Number of intermediate colors between old and new, inversely to smooth speed
        .smoothSlowdown = 100,

        .brightness = 50,

        // Maximum number of different colors for each channel (Red, Green and Blue)
        .maxPwmValue = 128,

        // Timer OCR value
        .timerOutputCompareRegValue = 100,
};

volatile uint16_t g_smoothIndex = 0;


static inline void _StartConstantTime(void)
{
    TCNT1 = 0;
}

static inline void _EndConstantTime(const uint8_t time)
{
    while(TCNT1 < time * 256UL) { }
}

void EvalCurrentImage_SmoothlyAlg(void)
{
    uint16_t coefEnd = (g_smoothIndex << 8) / g_Settings.smoothSlowdown;
    uint16_t coefStart = (1UL << 8) - coefEnd;

    for (uint8_t i = 0; i < LEDS_COUNT; i++)
    {
        g_Images.current[i].r = (
                coefStart * g_Images.start[i].r +
                coefEnd   * g_Images.end  [i].r) >> 8;

        g_Images.current[i].g = (
                coefStart * g_Images.start[i].g +
                coefEnd   * g_Images.end  [i].g) >> 8;

        g_Images.current[i].b = (
                coefStart * g_Images.start[i].b +
                coefEnd   * g_Images.end  [i].b) >> 8;
    }

    g_smoothIndex++;

    if (g_smoothIndex > g_Settings.smoothSlowdown)
    {
        g_smoothIndex = g_Settings.smoothSlowdown;
    }
}

void PWM(void)
{
    static uint8_t s_pwmIndex = 0; // index of current PWM level

    if (s_pwmIndex == g_Settings.maxPwmValue)
    {
        s_pwmIndex = 0;

        SET(LEDR);

        _StartConstantTime();

        // Switch OFF LEDs on time sets in g_Settings.brightness
        LedDriver_OffLeds();

        // Also eval current image
        if (g_Settings.isSmoothEnabled)
        {
            EvalCurrentImage_SmoothlyAlg();
        }

        _EndConstantTime(g_Settings.brightness);

        CLR(LEDR);
    }

    SET(LEDW);

    if (g_Settings.isSmoothEnabled)
    {
        LedDriver_UpdatePWM(g_Images.current, s_pwmIndex);
    } else {
        LedDriver_UpdatePWM(g_Images.end, s_pwmIndex);
    }

    CLR(LEDW);

    s_pwmIndex++;
}

void SetAllLedsColors(const uint8_t red, const uint8_t green, const uint8_t blue)
{
    for (uint8_t i = 0; i < LEDS_COUNT; i++)
    {
        g_Images.start[i].r = red;
        g_Images.start[i].g = green;
        g_Images.start[i].b = blue;

        g_Images.current[i].r = red;
        g_Images.current[i].g = green;
        g_Images.current[i].b = blue;

        g_Images.end[i].r = red;
        g_Images.end[i].g = green;
        g_Images.end[i].b = blue;
    }
}

/*
 *  Interrupts of the timer that generates PWM
 */
ISR( TIMER1_COMPA_vect )
{
    // Set next PWM states for all channels
    PWM();

    // Clear timer interrupt flag
    TIFR1 = _BV(OCF1A);

    // Clear timer counter
    TCNT1 = 0x0000;
}

static inline void TimerForPWM_Init(void)
{
    TCCR1A = 0x00;
    TCCR1C = 0x00;

    // Default values of timer prescaller and output compare register
    TCCR1B = _BV(CS10); // div1
    OCR1A = 100;

    TCNT1 = 0x0000;
    TIMSK1 = _BV(OCIE1A);
}

static inline void SetupHardware(void)
{
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Disable clock division */
    clock_prescale_set(clock_div_1);

    /* Hardware Initialization */
    PORTB = 0x00;
    DDRB = 0x00;

    PORTC = 0x00;
    DDRC = 0x00;

    PORTD = 0x00;
    DDRD = 0x00;

    OUTPUT( LEDR );
    OUTPUT( LEDW );
}


static inline void _ProcessFlags(void)
{
    /* if (_FlagProcess(Flag_HaveNewColors)) */

    if (_FlagProcess(Flag_LedsOffAll))
        SetAllLedsColors(0, 0, 0);

    if (_FlagProcess(Flag_TimerOptionsChanged))
    {
        // Pause timer
        TIMSK1 &= (uint8_t)~_BV(OCIE1A);

        // TODO: Use full 16 bit timer option, add support to Software;
        OCR1A = 0xff & g_Settings.timerOutputCompareRegValue;

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

    // Initialize timer for PWM
    TimerForPWM_Init();

    // Initialize USB
    USB_Init();

    sei();


    for (;;)
    {
        ProcessUsbTasks();

        _ProcessFlags();
    }
}

