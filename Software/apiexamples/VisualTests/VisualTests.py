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

# Color RGB indexes
R = 0
G = 1
B = 2
ALL = 3

def getColors(_rgbIndex, _color) :
	_colors = [0, 0, 0]
	if _rgbIndex == R or _rgbIndex == G or _rgbIndex == B :
		_colors[_rgbIndex] = _color
	elif _rgbIndex == ALL :
		_colors[0] = _color
		_colors[1] = _color
		_colors[2] = _color
	else :
		print ('Fail in blinkAllLeds() on index: {0}'.format(_rgbIndex))
		sys.exit(1);
	return _colors


def setLedColor(_led, _rgbIndex, _color, _delay) :
	_colors = getColors(_rgbIndex, _color)
	print ('  - R={0}, G={1}, B={2}, LED={3}'.format(_colors[0], _colors[1], _colors[2], _led))
	lpack.setColor(_led, _colors[0], _colors[1], _colors[2])
	time.sleep(_delay)
	lpack.setColorToAll(0, 0, 0)

def blinkAllLeds(_rgbIndex, _color, _delay, _times, _smooth = 0) :
	lpack.setSmooth(_smooth)
	_colors = getColors(_rgbIndex, _color)
	print ('  - R={0}, G={1}, B={2}, all LEDs'.format(_colors[0], _colors[1], _colors[2]))
	for i in range(0, _times) :
		lpack.setColorToAll(_colors[0], _colors[1], _colors[2])
		time.sleep(_delay)
		lpack.setColorToAll(0, 0, 0)
		time.sleep(_delay)
	lpack.setSmooth(0)


delay = 0.3
brightness = 1.0
smooth = 100
smoothDelay = 1 # seconds

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
lpack.setColorToAll(0, 0, 0)

while True :
			
	print ('1. Set color on each LED.')	

	for rgb in range (0, 3) : 
		for led in range (1, 11) : 
			setLedColor(led, rgb, color, delay)
		
	print ('2. White color on each LED.')
	
	for led in range (1, 11) : 
		setLedColor(led, ALL, color, delay)

	print ('3. Set one color on all LEDs.')

	blinkAllLeds(R, color, delay, 3)
	blinkAllLeds(R, color, smoothDelay, 1, smooth)

	blinkAllLeds(G, color, delay, 3)
	blinkAllLeds(G, color, smoothDelay, 1, smooth)

	blinkAllLeds(B, color, delay, 3)
	blinkAllLeds(B, color, smoothDelay, 1, smooth)	

	print ('4. White color on all LEDs.')

	blinkAllLeds(ALL, color, delay, 3)
	blinkAllLeds(ALL, color, smoothDelay, 1, smooth)

