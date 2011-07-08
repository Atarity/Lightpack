import socket, time, imaplib, re, sys

class lightpack:

	host = '127.0.0.1'    # The remote host
	port = 3636              # The same port as used by the server
	ledMap = [1,2,3,4,5,6,7,8,9,10] 	#mapped LEDs
	
	def __init__(self, _host, _port, _ledMap):
		self.host = _host
		self.port = _port
		self.ledMap = _ledMap
		
	def connect (self):
		try: 	#Try to connect to the server API
			self.connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			self.connection.connect((self.host, self.port))
			#self.connection.send('setstatus: on\n')
		except:
			print 'Lightpack API server is missing'
			sys.exit(0)
		
	def setColor(self, n, r, g, b): 	# Set color to the define LED
		#print self.ledMap[n],r,g,b
		cmd = 'setcolor:{0}-{1},{2},{3}\n'.format(self.ledMap[n-1], r, g, b)
		self.connection.send(cmd)
		
	def setColorToAll(self, r, g, b): 	# Set one color to all LEDs
		cmdstr = ''
		for i in self.ledMap:
			cmdstr = str(cmdstr) + str(i) + '-{0},{1},{2};'.format(r,g,b)
		cmd = 'setcolor:' + cmdstr + '\n'
		#print 'auto-> ' + cmd
		self.connection.send(cmd)

	def setGamma(self, g):
		cmd = 'setgamma:{0}\n'.format(g)
		self.connection.send(cmd)		
		
	def setSmooth(self, s):
		cmd = 'setsmooth:{0}\n'.format(s)
		self.connection.send(cmd)
		

	def setProfile(self, p):
		cmd = 'setprofile:{0}\n'.format(p)
		self.connection.send(cmd)
		print (cmd)
		
	def lock(self):
		cmd = 'lock\n'
		self.connection.send(cmd)

	def unlock(self):
		cmd = 'unlock\n'
		self.connection.send(cmd)
	
	def turnOn(self):
		cmd = 'setstatus:on\n'		
		self.connection.send(cmd)
	
	def turnOff(self):
		cmd = 'setstatus:off\n'
		self.connection.send(cmd)

	def disconnect(self):
		self.connection.close()
