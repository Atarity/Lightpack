import socket, time, imaplib, re, msvcrt, win32api, win32con, win32gui

class lightpack:

	host = '127.0.0.1'    # The remote host
	port = 3636              # The same port as used by the server
	ledMap = [1,2,5,6,7,8,9,3,4,0] 	#mapped LEDs
	
	def __init__(self, _host, _port, _ledMap):
		self.host = _host
		self.port = _port
		self.ledMap = _ledMap
		
	def connect (self):
		self.connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.connection.connect((self.host, self.port))
		self.connection.send('setstatus: on\n')

		
	def setColor(self, n, r, g, b):
		print self.ledMap[n],r,g,b
		cmd = 'setcolor: {0}-{1},{2},{3}\n'.format(self.ledMap[n], r, g, b)
		self.connection.send(cmd)

	def lock(self):
		cmd = 'lock\n'
		self.connection.send(cmd)

	def unlock(self):
		cmd = 'unlock\n'
		self.connection.send(cmd)

	def disconnect(self):
		self.connection.close()
