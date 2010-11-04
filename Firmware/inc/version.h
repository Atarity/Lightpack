/*
 * version.h
 *
 *  Created on: 03.11.2010
 *      Author: brunql
 */

#ifndef VERSION_H_
#define VERSION_H_

//   Hardware version:
#define VERSION_OF_HARDWARE				(0x0208UL)
#define VERSION_OF_HARDWARE_MAJOR		((VERSION_OF_HARDWARE >> 8) & 0xff)
#define VERSION_OF_HARDWARE_MINOR		(VERSION_OF_HARDWARE & 0x00ff)


#endif /* VERSION_H_ */
