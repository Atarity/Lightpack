import time, lightpack

lpack = lightpack.lightpack('127.0.0.1', 3636, [1,2,5,6,7,8,9,3,4,0] )
lpack.connect()
lpack.lock()
for i in range(0,10):
	lpack.setColor(i, 0,0,0)
time.sleep(1)
lpack.setColor(i, 0,255,0)
time.sleep(60)
lpack.unlock()
lpack.disconnect()
