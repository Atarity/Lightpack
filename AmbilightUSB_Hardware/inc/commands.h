/*
 * commands.h
 *
 *  Created on: 04.08.2010
 *      Author: brunql
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

// Commands to device, sends it in first byte of data[]
#define CMD_RIGHT_SIDE 0x01 /* all data for right side of screen */
#define CMD_LEFT_SIDE  0x02 /* all data for left side of screen */
#define CMD_OFF_ALL    0x03 /* switch off all leds */

#endif /* COMMANDS_H_ */
