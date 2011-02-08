/*
 * Lightpack.c
 *
 *  Created on: 11.01.2011
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Lightpack? This is content-appropriate ambient lighting system for your computer!
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
 *  GenericHID demo created by:
 *  Copyright (c) 2010  Dean Camera (dean [at] fourwalledcubicle [dot] com)
 *
 */


#include "Lightpack.h"

#include "LedDriver.h"
#include "../CommonHeaders/RGB.h"
#include "../CommonHeaders/commands.h"

#include "Version.h"

volatile uint8_t UpdateColors = FALSE;

volatile uint8_t ColorsLevelsForPWM[LEDS_COUNT][3]; // colors using in PWM generation
volatile uint8_t ColorsLevelsForPWM_New[LEDS_COUNT][3]; // last colors comes from USB


volatile uint8_t PwmIndexMaxValue = 64;

volatile uint8_t Smooth = 0; // Index in smoothing algorithm
volatile uint8_t SmoothDelay = 0; // 0 = smooth off, 1 = smooth on
volatile uint8_t SmoothStep[LEDS_COUNT][3]; // Save steps for smoothing each led

volatile uint8_t MaxDiff = 0; // Maximum difference between old and new color


/** Buffer to hold the previously generated HID report, for comparison purposes inside the HID class driver. */
uint8_t PrevHIDReportBuffer[GENERIC_REPORT_SIZE];


/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t Generic_HID_Interface =
	{
		.Config =
			{
				.InterfaceNumber              = 0,

				.ReportINEndpointNumber       = GENERIC_IN_EPNUM,
				.ReportINEndpointSize         = GENERIC_EPSIZE,
				.ReportINEndpointDoubleBank   = false,

				.PrevReportINBuffer           = PrevHIDReportBuffer,
				.PrevReportINBufferSize       = sizeof(PrevHIDReportBuffer),
			},
	};



void SmoothlyUpdateColors(void)
{
	UpdateColors = FALSE;

    for(uint8_t color=0; color < 3; color++){
        for(uint8_t ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){

        	uint8_t now = ColorsLevelsForPWM[ledIndex][color];
        	uint8_t new = ColorsLevelsForPWM_New[ledIndex][color];

            if(Smooth % SmoothStep[ledIndex][color] == 0){
                if(now < new){
                    now++;
                }
                if(now > new){
                    now--;
                }
                ColorsLevelsForPWM[ledIndex][color] = now;
            }

            if(now != new){
            	UpdateColors = TRUE;
            }
        }
    }

    if(UpdateColors){
    	Smooth++;
    }else{
    	Smooth = 0x00;
    }
}





void PWM(void)
{
    static uint8_t PwmIndex = 0; // index of current PWM level

    if(++PwmIndex >= PwmIndexMaxValue){
        PwmIndex = 0x00;


        if(SmoothDelay != 0){
        	if(UpdateColors){
        		SmoothlyUpdateColors(); // == 483us
        	}else{
        		_delay_us(483); // go away annoying flickering!
        	}
        }
    }


    // LedDriver connection: NC B G R
    LedDrivers_ClkDown();
    LedDriver1_OutOff();
    LedDriver2_OutOff();
    LedDrivers_ClkUp();

    for(uint8_t color=0; color<3; color++){
    	LedDrivers_ClkDown();
    	if(ColorsLevelsForPWM[LED4][color] > PwmIndex) LedDriver1_OutOn(); else LedDriver1_OutOff();
    	if(ColorsLevelsForPWM[LED8][color] > PwmIndex) LedDriver2_OutOn(); else LedDriver2_OutOff();
    	LedDrivers_ClkUp();
    }

    LedDrivers_ClkDown();
    LedDriver1_OutOff();
    LedDriver2_OutOff();
    LedDrivers_ClkUp();

    for(uint8_t color=0; color<3; color++){
    	LedDrivers_ClkDown();
    	if(ColorsLevelsForPWM[LED3][color] > PwmIndex) LedDriver1_OutOn(); else LedDriver1_OutOff();
    	if(ColorsLevelsForPWM[LED7][color] > PwmIndex) LedDriver2_OutOn(); else LedDriver2_OutOff();
    	LedDrivers_ClkUp();
    }

    LedDrivers_ClkDown();
    LedDriver1_OutOff();
    LedDriver2_OutOff();
    LedDrivers_ClkUp();

    for(uint8_t color=0; color<3; color++){
    	LedDrivers_ClkDown();
    	if(ColorsLevelsForPWM[LED2][color] > PwmIndex) LedDriver1_OutOn(); else LedDriver1_OutOff();
    	if(ColorsLevelsForPWM[LED6][color] > PwmIndex) LedDriver2_OutOn(); else LedDriver2_OutOff();
    	LedDrivers_ClkUp();
    }

    LedDrivers_ClkDown();
    LedDriver1_OutOff();
    LedDriver2_OutOff();
    LedDrivers_ClkUp();

    for(uint8_t color=0; color<3; color++){
    	LedDrivers_ClkDown();
    	if(ColorsLevelsForPWM[LED1][color] > PwmIndex) LedDriver1_OutOn(); else LedDriver1_OutOff();
    	if(ColorsLevelsForPWM[LED5][color] > PwmIndex) LedDriver2_OutOn(); else LedDriver2_OutOff();
    	LedDrivers_ClkUp();
    }

    LedDrivers_LatchPulse();
}



void SetAllLedsColors(uint8_t red, uint8_t green, uint8_t blue)
{
    for(uint8_t i=0; i<LEDS_COUNT; i++){
        ColorsLevelsForPWM[i][R] = red;
        ColorsLevelsForPWM[i][G] = green;
        ColorsLevelsForPWM[i][B] = blue;
    }
}

//
// Interrupts of the timer that generates PWM
//
ISR( TIMER1_COMPA_vect )
{
    // Set next PWM states for all channels
    PWM();

    // Clear timer interrupt flag
    TIFR1 = _BV(OCF1A);

    // Clear timer counter
    TCNT1 = 0x0000;
}

static inline void TimerForPWM_Init(void)
{
    TCCR1A = 0x00;
    TCCR1C = 0x00;

    // Default values of timer prescaller and output compare register
    TCCR1B = _BV(CS10); // div1
    OCR1A = 100;

    TCNT1 = 0x0000;
    TIMSK1 = _BV(OCIE1A);
}


/*
 *  Main program entry point
 */
int main(void)
{
    SetupHardware();

    // Led driver ports initialization
    LedDriver_Init();

    // Initialize timer for PWM
    TimerForPWM_Init();

    // Initialize USB
    USB_Init();

	sei();

	SmoothDelay = TRUE;

	SetAllLedsColors(20,20,20);

	for (;;)
	{
		HID_Device_USBTask(&Generic_HID_Interface);
		USB_USBTask();
	}
}

void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* Hardware Initialization */
    PORTB = 0x00;
    DDRB = 0x00;

    PORTC = 0x00;
    DDRC = 0x00;

    PORTD = 0x00;
    DDRD = 0x00;

	OUTPUT( LED );
	CLR( LED );
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	SET( LED );
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
    CLR( LED );
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Generic_HID_Interface);

	USB_Device_EnableSOFEvents();
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	HID_Device_ProcessControlRequest(&Generic_HID_Interface);
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
	HID_Device_MillisecondElapsed(&Generic_HID_Interface);
}

/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in]     HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID    Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in]     ReportType  Type of the report to create, either HID_REPORT_ITEM_In or HID_REPORT_ITEM_Feature
 *  \param[out]    ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out]    ReportSize  Number of bytes written in the report (or zero if no report is to be sent
 *
 *  \return Boolean true to force the sending of the report, false to let the library determine if it needs to be sent
 */
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                         uint8_t* const ReportID,
                                         const uint8_t ReportType,
                                         void* ReportData,
                                         uint16_t* const ReportSize)
{
	uint8_t *ReportData_u8 = (uint8_t *)ReportData;

    // Hardware version
	ReportData_u8[INDEX_HW_VER_MAJOR] = VERSION_OF_HARDWARE_MAJOR;
	ReportData_u8[INDEX_HW_VER_MINOR] = VERSION_OF_HARDWARE_MINOR;

    // Firmware version
	ReportData_u8[INDEX_FW_VER_MAJOR] = VERSION_OF_FIRMWARE_MAJOR;
	ReportData_u8[INDEX_FW_VER_MINOR] = VERSION_OF_FIRMWARE_MINOR;

	*ReportSize = GENERIC_REPORT_SIZE;
	return true;
}

/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the created report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
	uint8_t *ReportData_u8 = (uint8_t *)ReportData;

	uint8_t cmd = ReportData_u8[0]; // command from enum COMMANDS{ ... };

	uint8_t i = 1; // new data for colors levels starts form ReportData_u8[1]

	switch(cmd){
	case CMD_UPDATE_LEDS:
	    if(SmoothDelay == 0){
	        // Just put new colors directly to ColorsLevelsForPWM[]
	        for(uint8_t ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
	            ColorsLevelsForPWM[ledIndex][R] = ReportData_u8[i++];
	            ColorsLevelsForPWM[ledIndex][G] = ReportData_u8[i++];
	            ColorsLevelsForPWM[ledIndex][B] = ReportData_u8[i++];
	            SmoothStep[ledIndex][R] = ReportData_u8[i++];
	            SmoothStep[ledIndex][G] = ReportData_u8[i++];
	            SmoothStep[ledIndex][B] = ReportData_u8[i++];
	        }
	    }else{
	        // SmoothDelay not zero and put new colors to ColorsLevelsForPWM_New[]
	        for(uint8_t ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
	            ColorsLevelsForPWM_New[ledIndex][R] = ReportData_u8[i++];
	            ColorsLevelsForPWM_New[ledIndex][G] = ReportData_u8[i++];
	            ColorsLevelsForPWM_New[ledIndex][B] = ReportData_u8[i++];
	            SmoothStep[ledIndex][R] = ReportData_u8[i++];
	            SmoothStep[ledIndex][G] = ReportData_u8[i++];
	            SmoothStep[ledIndex][B] = ReportData_u8[i++];
	        }
	        UpdateColors = TRUE;
	        Smooth = 0x00;
	    }
	    break;
	case CMD_OFF_ALL:
	    if(SmoothDelay == 0){
	        // Just put new colors directly to ColorsLevelsForPWM[]
	        SetAllLedsColors(0, 0, 0);
	    }else{
	        // SmoothDelay not zero and put new colors to ColorsLevelsForPWM_New[]
	        for(uint8_t ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
	            ColorsLevelsForPWM_New[ledIndex][R] = 0;
	            ColorsLevelsForPWM_New[ledIndex][G] = 0;
	            ColorsLevelsForPWM_New[ledIndex][B] = 0;
	        }
	        UpdateColors = TRUE;
	        Smooth = 0x00;
	    }

	    break;
	case CMD_SET_TIMER_OPTIONS:
	    TIMSK1 &= (uint8_t)~_BV(OCIE1A);

	    // TODO: ReportData_u8[DATA_INDEX_CMD_SET_PRESCALLER]
	    switch(ReportData_u8[1]){
	    case CMD_SET_PRESCALLER_1:      TCCR1B = _BV(CS10); break;
	    case CMD_SET_PRESCALLER_8:      TCCR1B = _BV(CS11); break;
	    case CMD_SET_PRESCALLER_64:     TCCR1B = _BV(CS11) | _BV(CS10); break;
	    case CMD_SET_PRESCALLER_256:    TCCR1B = _BV(CS12); break;
	    case CMD_SET_PRESCALLER_1024:   TCCR1B = _BV(CS12) | _BV(CS11); break;
	    }

	    // TODO: ReportData_u8[DATA_INDEX_CMD_SET_OCR]
	    OCR1A = ReportData_u8[2];

	    TCNT1 = 0x0000;
	    TIMSK1 = _BV(OCIE1A);
        break;
	case CMD_SET_PWM_LEVEL_MAX_VALUE:
	    PwmIndexMaxValue = ReportData_u8[1];
	    break;
	case CMD_SMOOTH_CHANGE_COLORS:
	    SmoothDelay = ReportData_u8[1];
        break;
	case CMD_NOP:
        break;
	}
}

