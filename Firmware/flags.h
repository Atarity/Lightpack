/*
 * flags.h
 *
 *  Created on: 14.04.2011
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

#ifndef FLAGS_H_INCLUDED
#define FLAGS_H_INCLUDED

extern volatile uint8_t g_Flags;

typedef enum FLAGS
{
    Flag_HaveNewColors			= (1 << 0),
    Flag_LedsOffAll 			= (1 << 1),
    Flag_TimerOptionsChanged 	= (1 << 2),

} Flag_t;

static inline void _FlagSet(Flag_t flag)
{
    ATOMIC_BLOCK( ATOMIC_RESTORESTATE ){
        g_Flags |= flag;
    }
}

static inline void _FlagClear(Flag_t flag)
{
    ATOMIC_BLOCK( ATOMIC_RESTORESTATE ){
        g_Flags &= (uint8_t) ~ flag;
    }
}

static inline uint8_t _Flag(Flag_t flag)
{
    uint8_t result;

    ATOMIC_BLOCK( ATOMIC_RESTORESTATE ){
        result = (g_Flags & flag);
    }

    return result;
}

static inline uint8_t _FlagProcess(Flag_t flag)
{
    uint8_t result = _Flag(flag);

    _FlagClear(flag);

    return result;
}

#endif /* FLAGS_H_INCLUDED */
