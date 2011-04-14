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

#include "../CommonHeaders/RGB.h"

#include "LedDriver.h"
#include "LightpackUSB.h"

// Flags register
volatile uint8_t g_Flags = 0;

// New colors and steps comes from PC
ColorsAndSteps_t g_ImageFrameNew = { };

Settings_t g_Settings =
{
        .isSmoothEnabled = true,

        // Maximum number of different colors for each channel (Red, Green and Blue)
        .maxPwmValue = 128,

        // Timer OCR value
        .timerOutputCompareRegValue = 100,
};



// Colors using in PWM generation
RGB_t m_ImageFrame[LEDS_COUNT] = { };

// Index in smoothing algorithm
volatile uint8_t m_Smooth = 0;

// Index in smoothing algorithm
volatile uint8_t m_IsColorsDiff = true;


static inline uint8_t _GetNextSmoothColor(const uint8_t now, const uint8_t new, const uint8_t step)
{
    uint8_t result = now;

    if (step == 0 || step > 64 /* TODO! */ || m_Smooth % step == 0)
    {
        if (now < new)
            result++;

        if (now > new)
            result--;
    }

    if (now != new)
        m_IsColorsDiff = true;

    return result;
}

static inline void _SmoothlyUpdateColors(void)
{
    m_IsColorsDiff = false;

    for (uint8_t i = 0; i < LEDS_COUNT; i++)
    {
        m_ImageFrame[i].r = _GetNextSmoothColor(
                m_ImageFrame[i].r,
                g_ImageFrameNew.pixels[i].r,
                g_ImageFrameNew.steps[i].sr );

        m_ImageFrame[i].g = _GetNextSmoothColor(
                m_ImageFrame[i].g,
                g_ImageFrameNew.pixels[i].g,
                g_ImageFrameNew.steps[i].sg );

        m_ImageFrame[i].b = _GetNextSmoothColor(
                m_ImageFrame[i].b,
                g_ImageFrameNew.pixels[i].b,
                g_ImageFrameNew.steps[i].sb );
    }

    if (m_IsColorsDiff)
        m_Smooth++;
    else
        m_Smooth = 0x00;
}

void PWM(void)
{
    static uint8_t s_pwmIndex = 0; // index of current PWM level

    if (s_pwmIndex >= g_Settings.maxPwmValue)
    {
        s_pwmIndex = 0x00;

        SET(LEDR);
        if (g_Settings.isSmoothEnabled)
        {
            _SmoothlyUpdateColors();
        }
        CLR(LEDR);
    }

    SET(LEDW);

    if (g_Settings.isSmoothEnabled)
    {
        LedDriver_UpdatePWM(m_ImageFrame, s_pwmIndex);
    } else {
        LedDriver_UpdatePWM(g_ImageFrameNew.pixels, s_pwmIndex);
    }

    CLR(LEDW);

    s_pwmIndex++;
}

void SetAllLedsColors(const uint8_t red, const uint8_t green, const uint8_t blue)
{
    RGB_t * pixel;

    if (g_Settings.isSmoothEnabled)
        pixel = &m_ImageFrame[0];
    else
        pixel = &g_ImageFrameNew.pixels[0];


    for (uint8_t i = 0; i < LEDS_COUNT; i++)
    {
        pixel->r = red;
        pixel->g = green;
        pixel->b = blue;

        pixel++;
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
    if (_FlagProcess(Flag_HaveNewColors))
        m_Smooth = 0x00;

    if (_FlagProcess(Flag_LedsOffAll))
        SetAllLedsColors(0, 0, 0);

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

    // Initialize timer for PWM
    TimerForPWM_Init();

    // Initialize USB
    USB_Init();

    SetAllLedsColors(64, 64, 64);

    for (uint8_t i = 0; i < LEDS_COUNT; i++)
    {
        g_ImageFrameNew.steps[i].sr = 8;
        g_ImageFrameNew.steps[i].sg = 7;
        g_ImageFrameNew.steps[i].sb = 4;
    }

    sei();

    for (;;)
    {
        ProcessUsbTasks();

        _ProcessFlags();
    }
}

