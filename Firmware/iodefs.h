/*
 * iodefs.h
 *
 *  Created on: 13.04.2011
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Lightpack is a content-appropriate ambient lighting system for any computer
 *
 *  Copyright (c) 2011 Mike Shatohin, mikeshatohin [at] gmail.com
 *  Copyright (c) 2006-2010 Dick Streefland
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

#ifndef IODEFS_H_INCLUDED
#define IODEFS_H_INCLUDED

// Bit manipulation macros
#define	BIT_CLR(reg,bit)	{ (reg) &= ~_BV(bit); }
#define	BIT_SET(reg,bit)	{ (reg) |=  _BV(bit); }
#define	BIT_TST(reg,bit)	(((reg) & _BV(bit)) != 0)

// I/O port manipulation macros
#define	DDR_CLR(p,b)		BIT_CLR(DDR  ## p, b)
#define	DDR_SET(p,b)		BIT_SET(DDR  ## p, b)
#define	PORT_CLR(p,b)		BIT_CLR(PORT ## p, b)
#define	PORT_SET(p,b)		BIT_SET(PORT ## p, b)
#define	PORT_TST(p,b)		BIT_TST(PORT ## p, b)
#define	PIN_TST(p,b)		BIT_TST(PIN  ## p, b)
#define	PIN_SET(p,b)		BIT_SET(PIN  ## p, b)

// Macros that can be used with an argument of the form (port,bit)
#define	INPUT(bit)			DDR_CLR bit
#define	OUTPUT(bit)			DDR_SET bit
#define	CLR(bit)			PORT_CLR bit
#define	SET(bit)			PORT_SET bit
#define	ISSET(bit)			PORT_TST bit
#define	TST(bit)			PIN_TST bit
#define	TOGGLE(bit)			PIN_SET bit

#endif /* IODEFS_H_INCLUDED */
