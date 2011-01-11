Lightpack hardware
==================

Version hw4.1
-------------
brunql, January 11, 2011

* Prototyping on AT90USB162 maket board

Version hw4.0
-------------
brunql, January 4, 2011

* Prototyping on 2 MCUs
* First (ATmega8A) work as USB to UART transceiver
* Second (ATmega8A) - control LEDs

Version hw3.7
-------------
brunql, December 13, 2010

* Fix bug with USB D+ D- connection
* Fix bug in schema and board with LATCH of 74HC595
* In PCB 6 (six!) wired jumpers, fixme

Version hw3.0
-------------
brunql, December, 2010

* ATmega8A
* 4 x 74HC595
* 8 x RGB LEDs
* USB Type B
* One PCB with wires to LEDs

Version hw2.1.2
---------------
brunql, December 9, 2010

Edited only schematic of AmbilightUSB:

* Bugfix quartz 16Mhz, but must 12Mhz
* Bugfix 74HC595 connection (was LATCH - SCL, VCC - RCK, must LATCH - RCK, VCC - SCL)
* Added value of the electrolytic capacitor C1: 1-10uF

Version hw2.1
-------------
brunql, July 26, 2010

* Two printed circuit boards are connected using 5 wires
* ATtiny44-20SSU
* 2 x 74HC595
* 4 x RGB LEDs (SMD)
* many resistors (3 for each RGB LED)
* miniUSB Type B 

Version hw1.*
-------------
brunql, Spring, 2010

* using hardware from ColorsCMP project
* ATmega32
* DM134B
* first 2 (and then 4) RGB LEDs
* miniUSB Type B
