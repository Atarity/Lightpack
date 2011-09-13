#inaccurate pyLightpack class animation examples for 10 LED Lightpack configuration (3+2+3+2 clockwise from the left edge).
import lightpack, time, re, random

lpack = lightpack.lightpack('127.0.0.1', 3636, [2,3,6,7,8,9,10,4,5,1] )
lpack.connect()
lpack.lock()

first = True
while True :
	for k  in range (0, 10) :	
		if first :
			lpack.setColorToAll(80,0,0)
		else :
			lpack.setColorToAll(0,0,80)
	first = not first

exit

