/*
 * USB_ID.h
 *
 *  Created on: 12.01.2011
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Copyright (c) 2011 Mike Shatohin, mikeshatohin [at] gmail.com, Timur Sattarov (tim.helloworld at gmail.com)
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
 

#ifndef USB_ID_H_
#define USB_ID_H_

/* earlier version of Lightpack used to use Atmel's VID:PID pair */
/* find it in GenericHID demo from LUFA project */

#define USB_OLD_VENDOR_ID   0x03EB
#define USB_OLD_PRODUCT_ID  0x204F

/* Openmoko's free VID:PID http://wiki.openmoko.org/wiki/USB_Product_IDs */

#define USB_VENDOR_ID   0x1D50
#define USB_PRODUCT_ID  0x6022

#define USB_MANUFACTURER_STRING     "lightpack.googlecode.com"
#define USB_PRODUCT_STRING          "Lightpack"

#endif /* USB_ID_H_ */
