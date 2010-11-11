Version 2.7.6
-------------

* Windows support
* Added radio buttons switch colored or white fill of grabbing rects
* Added new hardware option to set color depth
* Split code of getting and sending colors in 2 classes
* Make one *.pro file for windows and linux

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
