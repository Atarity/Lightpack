AmbilightUSB
============

AmbilightUSB is a very simple implementation of the backlight for a laptop (as in Philips TV). A couple of simple printed circuit boards, controller, shift registers, a few LEDs and AmbilightUSB is ready!   
Configuration program lives in the tray, without interfering with habitual work and watching movies.   

**Hardware**: `ATtiny44`, `2 x 74HC595` and `4 x RGB LEDs`   
**Firmware**: written in `C`, using `V-USB` (HID Class device)   
**Software**: `C++`, `Qt` (all tested in Ubuntu 10.04)   

[Description](http://brunql.github.com/ambilight/) of the device in pictures   

Software versions:
==================
Version 2.7.5
-------------
Update GUI:

* All settings now in 2 tabs (software and hardware settings)
* Added new settings 'white balance' (red, green and blue coefs)
* Added new settings 'timer prescaler' (1, 8, 64, 256, 1024) and 'timer OCR' (Output Compare Register) settings
* Eval PWM generation frequency and show it in hardware settings

Timer settings for controlling the PWM frequency

Version 2.7.4
-------------
Rewrote the code to grab screen image and find the average color value of exciting fields
Added display of exciting fields as filled rectangles in different colors
Added scroll bars to change the width and height of the exciting fields

Version 2.7.3
-------------
Sets configurations of:

* delay between updates
* count of pixels (by setting size and steps)
* show/hide transparent/black background of catching pixels

Shows FPS   
Added logging to `${HOME}/.ambilight.log`   
Added saving configurations in `${HOME}/.config/brunql.dev/AmbilightUSB.ini`   
Start to writing this version log  


Version 2.*
-----------
Start to writing Qt based GUI version of PC software.  
After starting the app is minimized to tray.  


Version 1.*
-----------
Console version of PC software  
Using X11 library to catch pixels colors  


Firmware versions:
==================
Version 2.7
-----------
New command to set prescaller and output compare register of timer that used to PWM generation.   
New command to set color max value (pwm_level_max)   
When you connect the device to the USB, LEDs are smoothly grow and fade the red, green and blue colors.   

Version 2.6
-----------
Firmware using V-USB as HID.    
Get data request sends info to PC with version of hardware.   
Set data request sends info about the brightness of each LED to device.  

Version 1.*
-----------
Prototyping on ATmega32 with DM134B   

Have fun!
=========

For more information see sources and [downloads](http://github.com/brunql/AmbilightUSB/downloads)

Sorry for bad English (=
