/*
 * commands.h
 *
 *  Created on: 04.08.2010
 *      Author: brunql
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

// Commands to device, sends it in first byte of data[]
enum COMMANDS{
	CMD_RIGHT_SIDE, 	/* all data for right side of screen */
	CMD_LEFT_SIDE, 		/* all data for left side of screen */
	CMD_OFF_ALL, 		/* switch off all leds */
	CMD_SET_TIMER_OPTIONS, 			/* timer settings */
	CMD_SET_PWM_LEVEL_MAX_VALUE, 	/* each color max value */
};

enum PRESCALLERS{
	CMD_SET_PRESCALLER_1,
	CMD_SET_PRESCALLER_8,
	CMD_SET_PRESCALLER_64,
	CMD_SET_PRESCALLER_256,
	CMD_SET_PRESCALLER_1024,
};

#endif /* COMMANDS_H_ */
