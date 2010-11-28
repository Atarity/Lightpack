/*
 * commands.h
 *
 *  Created on: 04.08.2010
 *      Author: Mike Shatohin (brunql)
 *     Project: AmbilightUSB
 *
 *  AmbilightUSB is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2010 Mike Shatohin, mikeshatohin [at] gmail.com
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


enum DATA_VERSION_INDEXES{
	INDEX_HW_VER_MAJOR,
	INDEX_HW_VER_MINOR,
	INDEX_FW_VER_MAJOR,
	INDEX_FW_VER_MINOR,
};


#endif /* COMMANDS_H_ */
