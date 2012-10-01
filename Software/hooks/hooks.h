/* 
 * File:   hooksdll.h
 * Author: KC
 *
 * Created on October 4, 2011, 6:54 PM
 */

#ifndef HOOKSDLL_H
#define	HOOKSDLL_H
#include"windows.h"

#ifdef HOOKSDLL_EXPORTS
#define HOOKSDLL_API extern "C" __declspec(dllexport)
#else
#define HOOKSDLL_API __declspec(dllimport)
#endif

#endif	/* HOOKSDLL_H */

