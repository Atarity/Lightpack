/*
 * commands.h
 *
 *  Created on: 04.08.2010
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Lightpack is a content-appropriate ambient lighting system for any computer
 *
 *  Copyright (c) 2010, 2011 Mike Shatohin, mikeshatohin [at] gmail.com
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
 

#ifndef COMMANDS_H_INCLUDED
#define COMMANDS_H_INCLUDED

// Commands to device, sends it in first byte of data[]
enum COMMANDS{
    CMD_UPDATE_LEDS = 1,
    CMD_OFF_ALL,                     /* switch off all leds */
    CMD_SET_TIMER_OPTIONS,           /* timer settings */
    CMD_SET_PWM_LEVEL_MAX_VALUE,     /* each color max value */
    CMD_SET_SMOOTH_SLOWDOWN,
    CMD_SET_BRIGHTNESS,

    CMD_NOP = 0x0F
};

enum PRESCALLERS{
    CMD_SET_PRESCALLER_1,
    CMD_SET_PRESCALLER_8,
    CMD_SET_PRESCALLER_64,
    CMD_SET_PRESCALLER_256,
    CMD_SET_PRESCALLER_1024,
};


enum DATA_VERSION_INDEXES{
    INDEX_FW_VER_MAJOR = 1,
    INDEX_FW_VER_MINOR,
};

#endif /* COMMANDS_H_INCLUDED */
