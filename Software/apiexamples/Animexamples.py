#inaccurate pyLightpack class animation examples for 10 LED Lightpack configuration (3+2+3+2 clockwise from the left edge).
import lightpack, time, re, random

lpack = lightpack.lightpack('127.0.0.1', 3636, [2,3,6,7,8,9,10,4,5,1] )
lpack.connect()
lpack.lock()

print ('***pyLightpack animation examples (read script header first)***')
print ('	1 -- Basic flash 4 times')
print ('	2 -- "Snake"')
print ('	3 -- Cylon effect')
print ('	4 -- Basic random color fluid')
choice = raw_input('My choice is: ')

while True :	
	if int(choice) == 1 :	
		lpack.setSmooth(100)	# 4 basic flashes
		lpack.setColorToAll(0,0,0)
		time.sleep(0.3)
		for k in range(0,5):
			lpack.setColorToAll(0,255,0)
			time.sleep(1);
			lpack.setColorToAll(0,0,0)
			time.sleep(1);
		lpack.setColorToAll(0,0,0)	
		time.sleep(4)
	
	elif int(choice) == 2 :
		lpack.setSmooth(10)		# Snake
		i = 1
		while i < 80 :
			i = i+1
			for k  in range (0, 10) :	
				idx = (i+k) % 10
				if k < 3 :
					lpack.setColor(idx,255,0,0)					
				else :
					lpack.setColor(idx,0,0,125)												
			time.sleep(0.05)
	
	elif int(choice) == 3 :
		lpack.setSmooth(10)	# Tiny Cylon
		lpack.setColorToAll(0,0,0)	
		time.sleep(1)	
		top = [3, 4, 5, 6]
		bottom = [1, 10, 9, 8]
		left = [1, 2, 3]
		right = [6, 7, 8]
		on = [255, 0, 0]
		off = [0, 0, 0]
		lpack.setSmooth(45)	
		
		lpack.setColor(top[0], on[0], on[1], on[2])
		lpack.setColor(bottom[0], on[0], on[1], on[2])
		for k in range (0, 3) :
			lpack.setColor(left[k], on[0], on[1], on[2])
		l = 0
		while l < 15 :	
			for i in range (1, 4) :		
				lpack.setColor(top[i], on[0], on[1], on[2])
				lpack.setColor(bottom[i], on[0], on[1], on[2])						
				if i == 3 :
					for n in range (0, 3) :
						lpack.setColor(right[n], on[0], on[1], on[2])
				if i >= 1 :			
					lpack.setColor(top[i-1], off[0], off[1], off[2])
					lpack.setColor(bottom[i-1], off[0], off[1], off[2])
					lpack.setColor(left[1], off[0], off[1], off[2])				
				time.sleep(0.2)
			for m in [2,1,0] :
				lpack.setColor(top[m], on[0], on[1], on[2])
				lpack.setColor(bottom[m], on[0], on[1], on[2])
				if m == 2 :
					lpack.setColor(right[1], off[0], off[1], off[2])
				elif m == 0 :
					lpack.setColor(left[1], on[0], on[1], on[2])
				lpack.setColor(top[m+1], off[0], off[1], off[2])
				lpack.setColor(bottom[m+1], off[0], off[1], off[2])
				time.sleep(0.2)
			l += 1
		lpack.setColorToAll(0,0,0)
		time.sleep(5)
	
	elif int(choice) == 4 :
		lpack.setSmooth(250)	# Random color fluid
		lpack.setGamma(2.00)
		lpack.setColorToAll(255, 255, 255)
		ark= []
		for i in range (0, 101) :
			n = i % 2
			if n == 0 :
				ark.append(0)
			else:
				ark.append(random.randint(200, 255))
		random.shuffle(ark)
		#print ark
		while True :			
			num = random.randint(1, 10)
			r = random.choice(ark)
			g = random.choice(ark)
			b = random.choice(ark)
			if num != 1 and num != 10 :
				lpack.setColor(num, r, g, b)
				time.sleep(random.uniform(0, 1))
				lpack.setColor(num-1, r, g, b)			
				lpack.setColor(num+1, r, g, b)
			elif num == 1 :
				lpack.setColor(num+1, r, g, b)
				time.sleep(random.uniform(0, 1))
				lpack.setColor(num, r, g, b)			
				lpack.setColor(num+2, r, g, b)
			elif num == 10 :
				lpack.setColor(num-1, r, g, b)
				time.sleep(random.uniform(0, 1))
				lpack.setColor(num, r, g, b)			
				lpack.setColor(num-2, r, g, b)
			time.sleep(random.uniform(1, 3))
		