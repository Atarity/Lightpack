/*
 * LedDriver.c
 *
 *  Created on: 13.04.2011
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

#include <avr/io.h>
#include "iodefs.h"
#include "Lightpack.h"
#include "LedDriver.h"

#include "version.h"

#if (LIGHTPACK_HW == 5)

/*
 *  Hardware 5.x
 */


#define LATCH_PIN	(B, 0)
#define SCK_PIN		(B, 1)
#define MOSI_PIN	(B, 2)


static const uint8_t LedsNumberForOneDriver = 5;

static inline void _SPI_Write8(const uint8_t byte)
{
    SPDR = byte;
    while ((SPSR & (1 << SPIF)) == false) { }
}

static inline void _SPI_Write16(const uint16_t word)
{
    _SPI_Write8((word >> 8) & 0xff);
    _SPI_Write8(word & 0xff);
}

static inline void _LedDriver_LatchPulse(void)
{
    SET(LATCH_PIN);
    CLR(LATCH_PIN);
}

static inline void _LedDriver_UpdateLeds(
        const uint8_t startIndex, const uint8_t endIndex,
        const RGB_t imageFrame[LEDS_COUNT],
        const uint8_t pwmIndex )
{
    uint16_t sendme = 0x0000;
    uint16_t bit  = 1;

    for (uint8_t i = startIndex; i < endIndex; i++)
    {
        if (imageFrame[i].r > pwmIndex)
            sendme |= bit;
        bit <<= 1;

        if (imageFrame[i].g > pwmIndex)
            sendme |= bit;
        bit <<= 1;

        if (imageFrame[i].b > pwmIndex)
            sendme |= bit;
        bit <<= 1;
    }

    _SPI_Write16(sendme);
}

void LedDriver_Init(void)
{
    OUTPUT(SCK_PIN);
    OUTPUT(MOSI_PIN);
    OUTPUT(LATCH_PIN);

    CLR(LATCH_PIN);

    // Setup SPI Master with max SPI clock speed (F_CPU / 2)
    SPSR = (1 << SPI2X);
    SPCR = (1 << SPE) | (1 << MSTR);
}

void LedDriver_UpdatePWM(const RGB_t imageFrame[LEDS_COUNT], const uint8_t pwmIndex)
{
    // ...
    // LedDriver2
    //      10     9     8     7     6
    // 0 B G R B G R B G R B G R B G R
    // LedDriver1
    //       5     4     3     2     1
    // 0 B G R B G R B G R B G R B G R

//    for (int8_t i = LEDS_COUNT - LedsNumberForOneDriver; i >= 0; i -= LedsNumberForOneDriver)
//    {
//        _LedDriver_UpdateLeds(i, i + LedsNumberForOneDriver,
//                LevelsForPWM, pwmIndex);
//    }

    _LedDriver_UpdateLeds(LedsNumberForOneDriver, LEDS_COUNT, imageFrame, pwmIndex);
    _LedDriver_UpdateLeds(0, LedsNumberForOneDriver, imageFrame, pwmIndex);

    _LedDriver_LatchPulse();
}

#elif (LIGHTPACK_HW == 4)

/*
 *  Hardware 4.x
 */

#define LATCH_1	    (B, 4)
#define CLK_1	    (B, 5)
#define DATA_1 	    (B, 6)

#define LATCH_2	    (B, 0)
#define CLK_2	    (B, 1)
#define DATA_2 	    (B, 2)

enum LEDS { LED1, LED2, LED3, LED4, LED5, LED6, LED7, LED8 };

static inline void _LedDrivers_LatchPulse(void)
{
    SET(LATCH_1);
    SET(LATCH_2);

    CLR(LATCH_1);
    CLR(LATCH_2);
}

static inline void _LedDrivers_ClkUp(void)
{
    SET(CLK_1);
    SET(CLK_2);
}

static inline void _LedDrivers_ClkDown(void)
{
    CLR(CLK_1);
    CLR(CLK_2);
}

// Parallel serial output of the one led for each driver
static inline void _LedDrivers_PSO(
        const RGB_t imageFrame[LEDS_COUNT],
        const uint8_t pwmIndex,
        const uint8_t ledIndex1,
        const uint8_t ledIndex2 )
{
    // LedDriver connection: NC B G R
    _LedDrivers_ClkDown();
    CLR(DATA_1);
    CLR(DATA_2);
    _LedDrivers_ClkUp();

    _LedDrivers_ClkDown();
    if (imageFrame[ledIndex1].b > pwmIndex) SET(DATA_1) else CLR(DATA_1)
    if (imageFrame[ledIndex2].b > pwmIndex) SET(DATA_2) else CLR(DATA_2)
    _LedDrivers_ClkUp();

    _LedDrivers_ClkDown();
    if (imageFrame[ledIndex1].g > pwmIndex) SET(DATA_1) else CLR(DATA_1)
    if (imageFrame[ledIndex2].g > pwmIndex) SET(DATA_2) else CLR(DATA_2)
    _LedDrivers_ClkUp();

    _LedDrivers_ClkDown();
    if (imageFrame[ledIndex1].r > pwmIndex) SET(DATA_1) else CLR(DATA_1)
    if (imageFrame[ledIndex2].r > pwmIndex) SET(DATA_2) else CLR(DATA_2)
    _LedDrivers_ClkUp();

}


void LedDriver_Init(void)
{
    CLR(CLK_1);
    CLR(CLK_2);

    CLR(LATCH_1);
    CLR(LATCH_2);

    CLR(DATA_1);
    CLR(DATA_2);

    OUTPUT(CLK_1);
    OUTPUT(CLK_2);

    OUTPUT(LATCH_1);
    OUTPUT(LATCH_2);

    OUTPUT(DATA_1);
    OUTPUT(DATA_2);
}

void LedDriver_UpdatePWM(const RGB_t imageFrame[LEDS_COUNT], const uint8_t pwmIndex)
{
    _LedDrivers_PSO(imageFrame, pwmIndex, LED4, LED8);
    _LedDrivers_PSO(imageFrame, pwmIndex, LED3, LED7);
    _LedDrivers_PSO(imageFrame, pwmIndex, LED2, LED6);
    _LedDrivers_PSO(imageFrame, pwmIndex, LED1, LED5);

    _LedDrivers_LatchPulse();
}

#else
#	error "LIGHTPACK_HW must be defined in '../CommonHeaders/LIGHTPACK_HW.h' to major number of the hardware revision"
#endif /* (LIGHTPACK_HW switch) */

