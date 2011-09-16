#Skype contact waiting script. It flashes Lightpack when target buddy in contact list are changed his online status.
#You need Skype4Py module and lightpack.py class for using it.

import Skype4Py, lightpack, time, sys

skype = Skype4Py.Skype() # Create an instance of the Skype class
lpack = lightpack.lightpack('127.0.0.1', 3636, [2,3,6,7,8,9,10,4,5,1] )
skype.Attach() # Connect the Skype object to the Skype client
targetstat = ['ONLINE', 'DND', 'SKYPEME'] # Array of target user statuses
lpack.connect()	

contact = raw_input('Input Skype username for waiting: ') # Skype username which we waiting for

while True:
	if skype.User(contact).OnlineStatus in targetstat : # If current buddy online status in targetstat array turn animation on
		print skype.User(contact).FullName + ' is ' + skype.User(contact).OnlineStatus + ' @ ' + time.ctime()
		lpack.lock()
		lpack.setSmooth(100)
		lpack.setColorToAll(0,0,0)
		time.sleep(0.3)
		for k in range(0,5):
			lpack.setColorToAll(0,255,0)
			time.sleep(1);
			lpack.setColorToAll(0,0,0)
			time.sleep(1);
		lpack.unlock()		
	else:
		print skype.User(contact).FullName + ' is ' + skype.User(contact).OnlineStatus + ' @ ' + time.ctime()
	time.sleep(30)
