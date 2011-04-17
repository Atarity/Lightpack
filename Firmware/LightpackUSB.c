/*
 * LightpackUSB.c
 *
 *  Created on: 13.04.2011
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Lightpack is a content-appropriate ambient lighting system for any computer
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
 */

#include "Lightpack.h"
#include "LightpackUSB.h"
#include "version.h"

#include "../CommonHeaders/COMMANDS.h"

// Buffer to hold the previously generated HID report, for comparison purposes inside the HID class driver.
uint8_t PrevHIDReportBuffer[GENERIC_REPORT_SIZE];

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


void ProcessUsbTasks(void)
{
    HID_Device_USBTask(&Generic_HID_Interface);
    USB_USBTask();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
    SET(LEDR);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
    CLR(LEDR);
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

    switch (cmd)
    {

    case CMD_UPDATE_LEDS:

        for (uint8_t ledIndex = 0; ledIndex < LEDS_COUNT; ledIndex++)
        {

            g_Images.start[ledIndex].r = g_Images.current[ledIndex].r;
            g_Images.start[ledIndex].g = g_Images.current[ledIndex].g;
            g_Images.start[ledIndex].b = g_Images.current[ledIndex].b;

            g_Images.end[ledIndex].r = ReportData_u8[i++];
            g_Images.end[ledIndex].g = ReportData_u8[i++];
            g_Images.end[ledIndex].b = ReportData_u8[i++];


            // TODO: Remove smooth steps from software!
            i++;
            i++;
            i++;
        }

        g_smoothIndex = 0;

        _FlagSet(Flag_HaveNewColors);

        break;

    case CMD_OFF_ALL:

        // TODO: Test me!
        _FlagSet(Flag_LedsOffAll);

        break;

    case CMD_SET_TIMER_OPTIONS:

        g_Settings.timerOutputCompareRegValue =
                ((uint16_t)ReportData_u8[3] << 8) | ReportData_u8[2];

        _FlagSet(Flag_TimerOptionsChanged);

        break;

    case CMD_SET_PWM_LEVEL_MAX_VALUE:

        g_Settings.maxPwmValue = ReportData_u8[1];

        break;

    case CMD_SET_SMOOTH_SLOWDOWN:

        g_Settings.isSmoothEnabled = ReportData_u8[1];
        g_Settings.smoothSlowdown  = ReportData_u8[1]; /* not a bug */

        break;

    case CMD_SET_BRIGHTNESS:

        g_Settings.brightness = ReportData_u8[1];

        break;

    case CMD_NOP:
        break;
    }
}
