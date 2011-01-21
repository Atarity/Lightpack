Lightpack software
==================

Version sw4.0.0
---------------
brunql, January 13, 2011

* See hardware changes for details (hw4.0)

Version sw3.0.3
---------------
brunql, December 11, 2010

* Fully support 8 LEDs by software
* Added logs tab to settings window
* Added indexes to GrabDesktopWindowLeds labels
* Bugfix with first tab in settings window
* Bugfix with finding center of the screen in GrabDesktopWindowLeds (primary screen height and desktop height are differs)
* Updated "About" dialog: hardware ATmega8 + 8 LEDs
* Updated "About" dialog: added license GPLv3 with link on www.gnu.org/licenses/gpl-3.0.html 

Version sw3.0.2
---------------
brunql, December 11, 2010

* Read and write USB data buffers expanded to 0x20 bytes
* Rewrite function updateColors() for 8 LEDs
* Added CMD_NOP to over functions which sends info to device 
* Intermediate version before fully support 8 LEDs

Version sw3.0.1
---------------
brunql, December 8, 2010

* Bugfix with multiply monitors (wrong determine width of screen, using full desktop->width() but need only available witdh of the primary screen) 
* Right now AmbiligthUSB can be using only with primary screen

Version sw3.0.0
---------------
brunql, December 7, 2010

* Split firmware and hardware versions
* Added option to "Hardware options" for switch on/off "USB send data only if colors changes"
* In "Hardware options" added option to set smootly delay
* 

Version sw2.7.7 (release)
---------------
brunql, November 12, 2010

* License GPLv3
* Bugfix: timer options and colors depth didn't sync with device; 
* Bugfix: if unplug and plug device, it didn't reopen; now, then screen picture changed, device will be reopened;
* Move changelogs form README.md to different files
* Using markdown for CHANGELOGs and INSTALL files
* Rename: AmbilightUSB_Hardware to Firmware; AmbilightUSB_PC to Software; 

Version sw2.7.6
---------------
brunql, November 03, 2010

* Windows support
* Added radio buttons switch colored or white fill of grabbing rects
* Added new hardware option to set color depth
* Split code of getting and sending colors in 2 classes
* Make one *.pro file for windows and linux

Version sw2.7.5
---------------
brunql, October 25, 2010

Update GUI:

* All settings now in 2 tabs (software and hardware settings)
* Added new settings 'white balance' (red, green and blue coefs)
* Added new settings 'timer prescaler' (1, 8, 64, 256, 1024) and 'timer OCR' (Output Compare Register) settings
* Eval PWM generation frequency and show it in hardware settings

Timer settings for controlling the PWM frequency

Version sw2.7.4
---------------
brunql, October 24, 2010

Rewrote the code to grab screen image and find the average color value of exciting fields
Added display of exciting fields as filled rectangles in different colors
Added scroll bars to change the width and height of the exciting fields

Version sw2.7.3
---------------
brunql, September 18, 2010

Sets configurations of:

* delay between updates
* count of pixels (by setting size and steps)
* show/hide transparent/black background of catching pixels

Shows FPS   
Added logging to `${HOME}/.ambilight.log`   
Added saving configurations in `${HOME}/.config/brunql.dev/AmbilightUSB.ini`   
Start to writing this version log  


Version sw2.*
-------------
brunql, Summer, 2010

Start to writing Qt based GUI version of PC software.  
After starting the app is minimized to tray.  


Version sw1.*
-------------
brunql, Spring, 2010

Console version of PC software  
Using X11 library to catch pixels colors  
