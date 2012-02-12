#!/usr/bin/python

#
# Lightpack hardware main tests
#

import lightpack, time, re, random, sys

print ('')
print ('Lightpack hardware main tests')
print ('')
print ('WARN: Disable authorization for proper execution of the script.')
print ('      Open software, check "Profiles->Expert mode" and off auth on "Dev tab".')
print ('')

delay = 0.5
brightness = 1.0

if len(sys.argv) > 1 :
	delay = float(sys.argv[1])
	print ("Set delay: {0} (input:{1})".format(delay, sys.argv[1]))
	print ('')

if len(sys.argv) > 2 :
	brightness = float(sys.argv[2])
	print ("Set brightness: {0} (input:{1})".format(brightness, sys.argv[2]))
	print ('')

print ('Delay {0} sec.'.format(delay))
print ('Brightness {0}%.'.format(brightness*100))

color = int(255 * brightness)

lpack = lightpack.lightpack('127.0.0.1', 3636, [5,4,3,1,2,6,7,8,9,10])
lpack.connect()
lpack.lock()
lpack.turnOn()
lpack.setSmooth(0)

# simple testing procedure
while True :
	print ('1. set all LEDs BLACK.')	
	lpack.setColorToAll(0, 0, 0)
	time.sleep(delay)
	print ('2. set all LEDs WHITE.')	
	lpack.setColorToAll(255, 255, 255)
	time.sleep(delay)
	print ('3. set all LEDs RED.')	
	lpack.setColorToAll(255,0,0)
	time.sleep(delay)
	print ('4. set all LEDs GREEN.')	
	lpack.setColorToAll(0,255,0)
	time.sleep(delay)
	print ('5. set all LEDs BLUE.')	
	lpack.setColorToAll(0,0,255)
	time.sleep(delay)
# end of simple testing procedure

while True :
			
	print ('1. Set color on each LED.')	
	
	for rgb in range (0, 3) : 
		for led in range (1, 11) : 
			if rgb == 0 :
				print ('  - RED   = {0}, led = {1}'.format(color, led))
				lpack.setColor(led, color, 0, 0)
			elif rgb == 1 :
				print ('  - GREEN = {0}, led = {1}'.format(color, led))
				lpack.setColor(led, 0, color, 0)
			elif rgb == 2 :
				print ('  - BLUE  = {0}, led = {1}'.format(color, led))
				lpack.setColor(led, 0, 0, color)
	
			time.sleep(delay)
			lpack.setColorToAll(0, 0, 0)
	
	print ('2. White color on each LED.')
	
	for led in range (1, 11) : 
		print ('  - R={0}, G={0}, B={0}, led = {1}'.format(color, led))
		lpack.setColor(led, color, color, color)
		time.sleep(delay)
		lpack.setColorToAll(0, 0, 0)

	print ('3. Set one color on all LEDs.')
	
	for times in range (0, 3) :
		print ('  - R={0}, G={1}, B={2}, all LEDs'.format(color, 0, 0))
		lpack.setColorToAll(color, 0, 0)
		time.sleep(delay)
		lpack.setColorToAll(0, 0, 0)
		time.sleep(delay)

	for times in range (0, 3) :
		print ('  - R=0, G={0}, B=0, all LEDs'.format(color))
		lpack.setColorToAll(0, color, 0)
		time.sleep(delay)
		lpack.setColorToAll(0, 0, 0)
		time.sleep(delay)

	for times in range (0, 3) :	
		print ('  - R=0, G=0, B={0}, all LEDs'.format(color))
		lpack.setColorToAll(0, 0, color)
		time.sleep(delay)
		lpack.setColorToAll(0, 0, 0)
		time.sleep(delay)
	

	print ('4. White color on all LEDs.')
	
	for times in range (0, 3) : 
		print ('  - R={0}, G={0}, B={0}, all LEDs'.format(color))
		lpack.setColorToAll(color, color, color)
		time.sleep(delay)
		lpack.setColorToAll(0, 0, 0)
		time.sleep(delay)

