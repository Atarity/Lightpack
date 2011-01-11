// ======================================================================
// Copyright 2006-2010 Dick Streefland
//
// This is free software, licensed under the terms of the GNU General
// Public License as published by the Free Software Foundation.
// ======================================================================

#include <avr/io.h>

// Preprocessor magic
#define	CAT2(a,b)		CAT2EXP(a, b)
#define	CAT2EXP(a,b)		a ## b
#define	CAT3(a,b,c)		CAT3EXP(a, b, c)
#define	CAT3EXP(a,b,c)		a ## b ## c

// Bit manipulation macros
#define	BIT_CLR(reg,bit)	{ (reg) &= ~ _BV(bit); }
#define	BIT_SET(reg,bit)	{ (reg) |=   _BV(bit); }
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
#define	INPUT(bit)		DDR_CLR bit
#define	OUTPUT(bit)		DDR_SET bit
#define	CLR(bit)		PORT_CLR bit
#define	SET(bit)		PORT_SET bit
#define	ISSET(bit)		PORT_TST bit
#define	TST(bit)		PIN_TST bit
#define	TOGGLE(bit)		PIN_SET bit
