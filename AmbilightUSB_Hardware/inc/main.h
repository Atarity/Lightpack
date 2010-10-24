/*
 * main.h
 *
 *  Created on: 21.07.2010
 *      Author: brunql
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "RGB.h"

//   Hardware version:
#define VERSION_OF_HARDWARE				(0x0207UL)
#define VERSION_OF_HARDWARE_MAJOR		((VERSION_OF_HARDWARE >> 8) & 0xff)
#define VERSION_OF_HARDWARE_MINOR		(VERSION_OF_HARDWARE & 0x00ff)


// Test pin on PORTA.7 using for debugging and evaluation time intervals
#define TEST_PIN_DDR_INIT()		{ DDRA |= _BV(PA7); }
#define TEST_UP()				{ PORTA |= _BV(PA7); }
#define TEST_DOWN()				{ PORTA &= (uint8_t)~_BV(PA7); }



// If this TRUE we need to update our colors[] array (with smooth if it is used)
extern volatile uint8_t update_colors;

// Save new colors comes from PC
extern volatile uint8_t colors_new[4][3];

// Color max value, i.e. 0 <= color[led][c] < pwm_level_max
extern volatile uint8_t pwm_level_max;

#endif /* MAIN_H_ */
