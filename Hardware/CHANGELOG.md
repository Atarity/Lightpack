Hardware changes
================

Version 2.1.2
-------------
Edited only schematic of AmbilightUSB
* Bugfix quartz 16Mhz, but must 12Mhz
* Bugfix 74HC595 connection (was LATCH - SCL, VCC - RCK, must LATCH - RCK, VCC - SCL)
* Added value of the electrolytic capacitor C1: 1-10uF

Version 2.1
-----------
brunql, July 26, 2010

* Two printed circuit boards are connected using 5 wires
* ATtiny44-20SSU
* 2 x 74HC595
* 4 x RGB LEDs (SMD)
* many resistors (3 for each RGB LED)
* miniUSB Type B 

Version 1.*
-----------
brunql, Spring, 2010

* using hardware from ColorsCMP project
* ATmega32
* DM134B
* first 2 (and then 4) RGB LEDs
* miniUSB Type B
