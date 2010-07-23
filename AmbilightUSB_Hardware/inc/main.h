/*
 * main.h
 *
 *  Created on: 21.07.2010
 *      Author: brunql
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "RGB.h"


// If this TRUE we need to update our colors[] array (with smooth if it is used)
extern volatile uint8_t update_colors;

// Save new colors comes from PC
extern volatile uint8_t colors_new[4][3];

#endif /* MAIN_H_ */
