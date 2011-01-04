/*
 * vusb.c
 *
 *  Created on: 14.05.2010
 *      Author: Mike Shatohin (brunql)
 *     Project: AmbilightUSB
 *
 *  AmbilightUSB is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2010, 2011 Mike Shatohin, mikeshatohin [at] gmail.com
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
 

#include <avr/io.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */
#include <avr/pgmspace.h>   /* required by usbdrv.h */

#include "main.h"
#include "vusb.h"
#include "RGB.h"
#include "commands.h"




// USB HID Report descriptor
PROGMEM char usbHidReportDescriptor[22] = {    /* USB report descriptor */
		0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
		0x09, 0x01,                    // USAGE (Vendor Usage 1)
		0xa1, 0x01,                    // COLLECTION (Application)
		0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
		0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
		0x75, 0x08,                    //   REPORT_SIZE
		0x95, 0x08,                    //   REPORT_COUNT
		0x09, 0x00,                    //   USAGE (Undefined)
		0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)
		0xc0                           // END_COLLECTION
};

// Read info from device
uint8_t   usbFunctionRead(uint8_t *data, uint8_t len)
{
	// TODO: return info from temperature sensor ATtiny44 (ADC8)
	if(len >= 2){
		// Hardware version
		data[INDEX_HW_VER_MAJOR] = VERSION_OF_HARDWARE_MAJOR;
		data[INDEX_HW_VER_MINOR] = VERSION_OF_HARDWARE_MINOR;

		// Firmware version
		data[INDEX_FW_VER_MAJOR] = VERSION_OF_FIRMWARE_MAJOR;
		data[INDEX_FW_VER_MINOR] = VERSION_OF_FIRMWARE_MINOR;
	}
	return len;
}

// Write info to device
uint8_t   usbFunctionWrite(uint8_t *data, uint8_t len)
{
	// TODO: data[CMD_INDEX]
	if(data[0] == CMD_LEDS_1_2){
		colors_new[LED1][R] = data[1];
		colors_new[LED1][G] = data[2];
		colors_new[LED1][B] = data[3];

		colors_new[LED2][R] = data[4];
		colors_new[LED2][G] = data[5];
		colors_new[LED2][B] = data[6];

	}else if(data[0] == CMD_LEDS_3_4){
		colors_new[LED3][R] = data[1];
		colors_new[LED3][G] = data[2];
		colors_new[LED3][B] = data[3];

		colors_new[LED4][R] = data[4];
		colors_new[LED4][G] = data[5];
		colors_new[LED4][B] = data[6];

	}else if(data[0] == CMD_LEDS_5_6){
		colors_new[LED5][R] = data[1];
		colors_new[LED5][G] = data[2];
		colors_new[LED5][B] = data[3];

		colors_new[LED6][R] = data[4];
		colors_new[LED6][G] = data[5];
		colors_new[LED6][B] = data[6];

	}else if(data[0] == CMD_LEDS_7_8){
		colors_new[LED7][R] = data[1];
		colors_new[LED7][G] = data[2];
		colors_new[LED7][B] = data[3];

		colors_new[LED8][R] = data[4];
		colors_new[LED8][G] = data[5];
		colors_new[LED8][B] = data[6];

		update_colors = TRUE;

	}else if(data[0] == CMD_OFF_ALL){
		for(uint8_t i=0; i<LEDS_COUNT; i++){
			colors_new[i][R] = 0x00;
			colors_new[i][G] = 0x00;
			colors_new[i][B] = 0x00;
		}

		update_colors = TRUE;

	}
//
//	TODO: Add this to FirmwarePWM
//
//	else if(data[0] == CMD_SET_TIMER_OPTIONS){
//		TIMSK &= (uint8_t)~_BV(OCIE1A);
//
//		// TODO: data[DATA_INDEX_CMD_SET_PRESCALLER]
//		switch(data[1]){
//			case CMD_SET_PRESCALLER_1: 		TCCR1B = _BV(CS10); break;
//			case CMD_SET_PRESCALLER_8:		TCCR1B = _BV(CS11); break;
//			case CMD_SET_PRESCALLER_64: 	TCCR1B = _BV(CS11) | _BV(CS10); break;
//			case CMD_SET_PRESCALLER_256:	TCCR1B = _BV(CS12); break;
//			case CMD_SET_PRESCALLER_1024:	TCCR1B = _BV(CS12) | _BV(CS11); break;
//		}
//
//		// TODO: data[DATA_INDEX_CMD_SET_OCR]
//		OCR1A = data[2];
//
//		TCNT1 = 0x0000;
//		TIMSK = _BV(OCIE1A);
//	}else if(data[0] == CMD_SET_PWM_LEVEL_MAX_VALUE){
//		pwm_level_max = data[1];
//	}else if(data[0] == CMD_SMOOTH_CHANGE_COLORS){
//		smooth_delay = data[1];
//	}

	return 1;
}


usbMsgLen_t usbFunctionSetup(uint8_t data[8])
{
	usbRequest_t    *rq = (void *)data;

	if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* HID class request */
		if(rq->bRequest == USBRQ_HID_GET_REPORT){  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
			//return 0;
			return USB_NO_MSG;  /* use usbFunctionRead() to obtain data */
		}else if(rq->bRequest == USBRQ_HID_SET_REPORT){
			//return 0;
			return USB_NO_MSG;  /* use usbFunctionWrite() to receive data from host */
		}
	}else{
		/* ignore vendor type requests, we don't use any */
	}
	return 0;
}

// Re-enumeration device on USB
void usbInit_FakeUsbDisconnect(void)
{
	usbInit();
	usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
	uint8_t i = 0xff;
	while(i --> 0){         /* fake USB disconnect for > 250 ms */
		_delay_ms(1);
	}
	usbDeviceConnect();
	usbPoll();
}
