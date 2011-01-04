Firmware USB changes
====================

This firmware for the microcontroller, which transceiver USB in UART

Version fw_usb4.0
-----------------
brunql, January 4, 2011

* Nothing works =)
* See hardware changes verison 4.0 for details 


One MCU versions
================

USB and PWM in one MCU

Version fw3.3
-----------
brunql, December 7, 2010

* Fully support 8 LEDs by firmware
* Rewrite interface (RGB.h, commands.h) for using indexes LED1, LED2, etc.; CMD_LED_1_2, CMD_LED_3_4, etc.
* Updated PWM generation for 8 LEDs
* USB HID device descriptor updated (8 LEDs): REPORT_COUNT = 0x20 bytes

Version fw3.2
-----------
brunql, December 7, 2010

* Rewrite PWM generation for hw3.6 (separate connection DATA0 and DATA1 pins of two 74HC595, work only with first 4 LEDs)
* Rewrite smooth change colors, find max_diff and divide it on each diff for find step
* Bugfix windows CRC error with USB HID device descriptor: now REPORT_SIZE = 0x08 bytes

Version fw3.0
-----------
brunql, December 3, 2010

* Change MCU to ATmega8
* Split firmware and hardware versions
* Simple algorithm to smoothly change colors (incrementing in cycle while values of colors and colors_new are not equals)
* USB HID device descriptor updated (4 LEDs): REPORT_COUNT = REPORT_SIZE = 0x10 bytes


Version fw2.8 (release)
-----------
brunql, November 03, 2010

* Added new command to set color depth
* Change report descriptor size (size of sending, receiving buffers)
* In usb config set using FAST_CRC algorithm

Version fw2.7
-----------
brunql, October 25, 2010

New command to set prescaller and output compare register of timer that used to PWM generation.   
New command to set color max value (pwm_level_max)   
When you connect the device to the USB, LEDs are smoothly grow and fade the red, green and blue colors.   

Version fw2.6
-----------
brunql, July 27, 2010

Firmware using V-USB as HID.    
Get data request sends info to PC with version of hardware.   
Set data request sends info about the brightness of each LED to device.  

Version fw1.*
-----------
brunql, Spring, 2010

Prototyping on ATmega32 with DM134B   
