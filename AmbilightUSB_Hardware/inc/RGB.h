/*
 * RGB.h
 *
 *  Created on: 23.07.2010
 *      Author: brunql
 */

#ifndef RGB_H_
#define RGB_H_


// On this "screen", digit is number of led (leds behind of screen)
//   ╔═════════╗
//  3╠  Linux  ╣0
//  2╠  brunql ╣1
//   ╚═════════╝
#define RIGHT_UP	0
#define RIGHT_DOWN	1
#define LEFT_DOWN	2
#define LEFT_UP		3

// Define indexes for colors and colors_new arrays
#define R	0
#define G	1
#define B	2


// Hardware RGB bits definitions:

// Right side
#define RED_RIGHT 		0b0100ul
#define GREEN_RIGHT 	0b0010ul
#define BLUE_RIGHT 		0b0001ul

#define RIGHT_UP_RED_LS		(RED_RIGHT 		<< 12)
#define RIGHT_UP_GREEN_LS	(GREEN_RIGHT 	<< 12)
#define RIGHT_UP_BLUE_LS	(BLUE_RIGHT 	<< 12)

#define RIGHT_DOWN_RED_LS	(RED_RIGHT 		<< 8)
#define RIGHT_DOWN_GREEN_LS	(GREEN_RIGHT 	<< 8)
#define RIGHT_DOWN_BLUE_LS	(BLUE_RIGHT 	<< 8)

// Left side
#define RED_LEFT 	0b1000ul
#define GREEN_LEFT 	0b0100ul
#define BLUE_LEFT 	0b0010ul

#define LEFT_DOWN_RED_LS	(RED_LEFT 	<< 4)
#define LEFT_DOWN_GREEN_LS	(GREEN_LEFT << 4)
#define LEFT_DOWN_BLUE_LS	(BLUE_LEFT 	<< 4)

#define LEFT_UP_RED_LS		(RED_LEFT 	<< 0)
#define LEFT_UP_GREEN_LS	(GREEN_LEFT << 0)
#define LEFT_UP_BLUE_LS		(BLUE_LEFT 	<< 0)

// Commands to device, sends it in first byte of data[]
#define CMD_RIGHT_SIDE 0x01 /* all data for right side of screen */
#define CMD_LEFT_SIDE  0x02 /* all data for left side of screen */
#define CMD_OFF_ALL    0x03 /* switch off all leds */

// Be sure that this is defined
#ifndef TRUE
#	define TRUE 	0xff
#	define FALSE	0x00
#endif


#endif /* RGB_H_ */
