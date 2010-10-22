Ambilight USB
=============

This very simple project makes ambilight for you PC. Watching films now can be more fun.   
Hardware: `ATtiny44`, `74HC595` and 4 RGB leds   
Firmware: written in `C`, using `V-USB` (HID Class device)   
Software: `C++`, `Qt`, `X11` (GUI tested in Ubuntu 10.04)   
[Description of the device in pictures](http://brunql.github.com/ambilight/)   

Software versions:
==================
Version 2.7.3
-------------
Sets configurations of:   
*   delay between updates   
*   count of pixels (by setting size and steps)  
*   show/hide transparent/black background of catching pixels  
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
Version 2.6
-----------
Firmware using V-USB as HID.    
Get data request sends info to PC with version of hardware.   
Set data request sends info about the brightness of each LED to device.  

Version 1.*
-----------
Prototyping on ATmega32 with DM134B   

For more information see sources and [downloads](http://github.com/brunql/AmbilightUSB/downloads)

Have fun!
=========
