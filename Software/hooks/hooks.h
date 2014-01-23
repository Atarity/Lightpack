/* 
 * File:   hooksdll.h
 * Author: KC
 *
 * Created on October 4, 2011, 6:54 PM
 */

#ifndef HOOKSDLL_H
#define	HOOKSDLL_H

#if !defined NOMINMAX
#define NOMINMAX
#endif

#if !defined WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include"windows.h"

#ifdef HOOKSDLL_EXPORTS
#define HOOKSDLL_API extern "C" __declspec(dllexport)
#else
#define HOOKSDLL_API __declspec(dllimport)
#endif

#endif	/* HOOKSDLL_H */

