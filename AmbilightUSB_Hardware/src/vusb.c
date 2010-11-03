/*
 * vusb.c
 *
 *  Created on: 14.05.2010
 *      Author: brunql
 */

#include <avr/io.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */
#include <avr/pgmspace.h>   /* required by usbdrv.h */

#include "main.h"
#include "vusb.h"
#include "74HC595.h"
#include "commands.h"

// USB HID Report descriptor
PROGMEM char usbHidReportDescriptor[22] = {    /* USB report descriptor */
		0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
		0x09, 0x01,                    // USAGE (Vendor Usage 1)
		0xa1, 0x01,                    // COLLECTION (Application)
		0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
		0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
		0x75, 0x08,                    //   REPORT_SIZE (8)
		0x95, 0x08,                    //   REPORT_COUNT (8 bytes)
		0x09, 0x00,                    //   USAGE (Undefined)
		0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)
		0xc0                           // END_COLLECTION
};

// Read info from device
uint8_t   usbFunctionRead(uint8_t *data, uint8_t len)
{
	// TODO: return info from temperature sensor ATtiny44 (ADC8)
	if(len >= 2){
		data[0] = VERSION_OF_HARDWARE_MAJOR;
		data[1] = VERSION_OF_HARDWARE_MINOR;
	}
	return len;
}

// Write info to device
uint8_t   usbFunctionWrite(uint8_t *data, uint8_t len)
{
	// TODO: data[CMD_INDEX]
	if(data[0] == CMD_RIGHT_SIDE){
		colors_new[RIGHT_UP][R] = data[1];
		colors_new[RIGHT_UP][G] = data[2];
		colors_new[RIGHT_UP][B] = data[3];

		colors_new[RIGHT_DOWN][R] = data[4];
		colors_new[RIGHT_DOWN][G] = data[5];
		colors_new[RIGHT_DOWN][B] = data[6];

		update_colors = TRUE;

	}else if(data[0] == CMD_LEFT_SIDE){
		colors_new[LEFT_UP][R] = data[1];
		colors_new[LEFT_UP][G] = data[2];
		colors_new[LEFT_UP][B] = data[3];

		colors_new[LEFT_DOWN][R] = data[4];
		colors_new[LEFT_DOWN][G] = data[5];
		colors_new[LEFT_DOWN][B] = data[6];

		update_colors = TRUE;

	}else if(data[0] == CMD_OFF_ALL){
		HC595_PutUInt16(0x0000);
		for(uint8_t i=0; i<3; i++){
			colors_new[0][i] = 0x00;
			colors_new[1][i] = 0x00;
			colors_new[2][i] = 0x00;
			colors_new[3][i] = 0x00;
		}
		update_colors = TRUE;
	}else if(data[0] == CMD_SET_TIMER_OPTIONS){
		TIMSK1 &= (uint8_t)~_BV(OCIE1A);

		// TODO: data[CMD_SET_PRESCALLER_INDEX]
		switch(data[1]){
			case CMD_SET_PRESCALLER_1: 		TCCR1B = _BV(CS10); break;
			case CMD_SET_PRESCALLER_8:		TCCR1B = _BV(CS11); break;
			case CMD_SET_PRESCALLER_64: 	TCCR1B = _BV(CS11) | _BV(CS10); break;
			case CMD_SET_PRESCALLER_256:	TCCR1B = _BV(CS12); break;
			case CMD_SET_PRESCALLER_1024:	TCCR1B = _BV(CS12) | _BV(CS11); break;
		}

		// TODO: data[CMD_SET_OCR_INDEX]
		OCR1A = data[2];

		TCNT1 = 0x0000;
		TIMSK1 = _BV(OCIE1A);
	}else if(data[0] == CMD_SET_PWM_LEVEL_MAX_VALUE){
		pwm_level_max = data[1];
	}

	return 1;
}


usbMsgLen_t usbFunctionSetup(uint8_t data[8])
{
	usbRequest_t    *rq = (void *)data;

	if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* HID class request */
		if(rq->bRequest == USBRQ_HID_GET_REPORT){  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
			return USB_NO_MSG;  /* use usbFunctionRead() to obtain data */
		}else if(rq->bRequest == USBRQ_HID_SET_REPORT){
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
