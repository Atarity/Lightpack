import functools, OSC, time, threading, lightpack
from sets import Set
class LightOSC:

	def __init__(self, lpackHost, lpackPort, ledMap, oscHost, oscPort):
	""" установка начальных значений, создание OSC-сервера, назначение обработчиков, инициализация lightpack
		lpackHost, lpackPort - IP и порт хоста с Лайтпаком
		ledMap               - массив для переопределения порядка светодиодов Лайтпака
		oscHost, oscPort     - IP и порт на котором будет запущен OSC-сервер """ 

		self.r = 127
		self.g = 127
		self.b = 127
		self.smooth = 127
		self.brightness = 0.5
		self.ledMap = [[3,2,1],[4,-1,10],[5,-1,9],[6,7,8]]
		self.enabledLeds = Set([1,2,3,4,5,6,7,8,9,10])
		self.gamma = 2
	
		self.lpack = lightpack.lightpack(lpackHost, lpackPort, ledMap)
		self.lpack.connect()
		self.oscServer = OSC.ThreadingOSCServer((oscHost, oscPort))
		self.oscServer.addDefaultHandlers()
		
		self.oscClient = OSC.OSCClient()

		self.profiles = self.lpack.getProfiles()
		self.currentProfileIndex = 0

		self.mapControls()

		print "Registered Callback-functions are :"
		for addr in self.oscServer.getOSCAddressSpace():
			print addr
	
	def addMsgHandler(self, control, handler):
		self.oscServer.addMsgHandler(control, handler)
	
	def normalize255(self, factor):
		return int(float(factor) * 255);

	def run(self, oscClientHost, oscClientPort):
	""" Запуск сервера
		oscClientHost, oscClientPort - IP и порт OSC-клиента с пользовательским интерфейсом """ 

		print "\nStart OSCServer. Use ctrl-C to quit."
		st = threading.Thread( target = self.oscServer.serve_forever )
		st.start()

		self.lpack.lock()
		self.lpack.setColorToAll(0, 0, 0)

		self.oscClient.connect((oscClientHost, oscClientPort))

		self.updateLpackLeds()

		self.updateClientState()
		try :
			while 1 :		
				time.sleep(5)		

		except KeyboardInterrupt :
			print "\nClosing OSCServer."
			self.oscServer.close()
			self.oscClient.close()
			self.lpack.unlock()
			self.lpack.disconnect()
			print "Waiting for Server-thread to finish"
			st.join() ##!!!
			print "Done"

	def mapControls(self):
		self.addMsgHandler("/1/fader1", self.rFaderHandler)
		self.addMsgHandler("/1/fader2", self.gFaderHandler)
		self.addMsgHandler("/1/fader3", self.bFaderHandler)
		self.addMsgHandler("/1/rotary1", self.brightnessRotaryHandler)
		self.addMsgHandler("/1/rotary2", self.smoothRotaryHandler)
		self.addMsgHandler("/1/rotary3", self.gammaRotaryHandler)
		self.addMsgHandler("/1/multitoggle2/1/1", self.ledToggleHandler)
		self.addMsgHandler("/1/multitoggle2/2/1", self.ledToggleHandler)
		self.addMsgHandler("/1/multitoggle2/3/1", self.ledToggleHandler)
		self.addMsgHandler("/1/multitoggle2/4/1", self.ledToggleHandler)
		self.addMsgHandler("/1/multitoggle2/1/2", self.ledToggleHandler)
		self.addMsgHandler("/1/multitoggle2/4/2", self.ledToggleHandler)
		self.addMsgHandler("/1/multitoggle2/1/3", self.ledToggleHandler)
		self.addMsgHandler("/1/multitoggle2/2/3", self.ledToggleHandler)
		self.addMsgHandler("/1/multitoggle2/3/3", self.ledToggleHandler)
		self.addMsgHandler("/1/multitoggle2/4/3", self.ledToggleHandler)
		self.addMsgHandler("/1/push2", self.prevProfileHandler)
		self.addMsgHandler("/1/push3", self.nextProfileHandler)
#
# Обработчики событий
#
	def rFaderHandler(self, addr, tags, stuff, source):
		self.r = self.normalize255(stuff[0])
		print str(addr) + " --> " + str(stuff[0])
		self.updateLpackLeds()
		self.updateClientState()

	def gFaderHandler(self, addr, tags, stuff, source):
		self.g = self.normalize255(stuff[0])
		print str(addr) + " --> " + str(stuff[0])
		self.updateLpackLeds()
		self.updateClientState()

	def bFaderHandler(self, addr, tags, stuff, source):
		self.b = self.normalize255(stuff[0])
		print str(addr) + " --> " + str(stuff[0])
		self.updateLpackLeds()
		self.updateClientState()

	def brightnessRotaryHandler(self, addr, tags, stuff, source):
		self.brightness = stuff[0]
		print str(addr) + " --> " + str(stuff[0])
		self.updateLpackLeds()
		self.updateClientState()

	def prevProfileHandler(self, addr, tags, stuff, source):
		if stuff[0] < 0.5:
			if self.currentProfileIndex > 0:
				self.currentProfileIndex -= 1
			else :
				self.currentProfileIndex = len(self.profiles) - 1
			
			self.updateLpackProfile()
			
			print str(addr) + " --> " + str(stuff[0])
			self.updateClientState()

	def nextProfileHandler(self, addr, tags, stuff, source):
		if stuff[0] < 0.5:
			if self.currentProfileIndex < len(self.profiles) - 1 :
				self.currentProfileIndex += 1
			else :
				self.currentProfileIndex = 0
			
			self.updateLpackProfile()
			
			print str(addr) + " --> " + str(stuff[0])
			self.updateClientState()

	def smoothRotaryHandler(self, addr, tags, stuff, source):
		print str(addr) + " --> " + str(stuff[0])
		self.smooth = self.normalize255(stuff[0]);
		self.updateLpackSmooth()

	def gammaRotaryHandler(self, addr, tags, stuff, source):
		print str(addr) + " --> " + str(stuff[0])
		self.gamma = stuff[0] * 9.99 + 0.01
		self.updateLpackGamma()
		self.updateLpackLeds()
		
	def ledToggleHandler(self, addr, tags, stuff, source):
		path = addr.split('/')
		print "{},{},{},{}".format(addr, tags, stuff, source)
		x = int(path[3])-1
		y = int(path[4])-1
		ledNum = int(self.ledMap[x][y])
		
		#disable led
		if stuff[0] < 0.5 and (ledNum in self.enabledLeds) :
			print str(ledNum) + " is disabled"
			self.enabledLeds.remove(ledNum)
			self.lpack.setColor(ledNum, 0, 0, 0)
			
		#enable led
		if stuff[0] > 0.5 and (ledNum not in self.enabledLeds) :
			print str(ledNum) + " is enabled"
			self.enabledLeds.add(ledNum)
			self.lpack.setColor(ledNum, self.getR(), self.getG(), self.getB())
#
# Вспомогательные функции для работы с Лайтпаком
#
	def updateLpackGamma(self):
		self.lpack.setGamma(self.gamma)

	def updateLpackSmooth(self):
		self.lpack.setSmooth(self.smooth)

	def updateLpackProfile(self):
		newProfile = self.profiles[self.currentProfileIndex]
		self.lpack.setProfile(newProfile)
		
	def updateLpackLeds(self):
		for ledNum in self.enabledLeds:
			self.lpack.setColor(ledNum, self.getR(), self.getG(), self.getB())
	
	def getR(self):
		return self.withBrightnessApplied(self.r)
		
	def getG(self):
		return self.withBrightnessApplied(self.g)
		
	def getB(self):
		return self.withBrightnessApplied(self.b)
	
	def withBrightnessApplied(self, color):
		return int(color * self.brightness)

	def updateClientState(self):
		msg = OSC.OSCMessage()
		msg.setAddress("/1/rotary1")
		msg.append(self.brightness)
		self.oscClient.send(msg)

		msg = OSC.OSCMessage()
		msg.setAddress("/1/rotary2")
		msg.append(float(self.smooth) / 255.0)
		self.oscClient.send(msg)

		msg = OSC.OSCMessage()
		msg.setAddress("/1/rotary3")
		msg.append((self.gamma - 0.01) / 9.99)
		self.oscClient.send(msg)
		
		msg = OSC.OSCMessage()
		msg.setAddress("/1/fader1")
		msg.append(float(self.r) / 255.0)
		self.oscClient.send(msg)

		msg = OSC.OSCMessage()
		msg.setAddress("/1/fader2")
		msg.append(float(self.g) / 255.0)
		self.oscClient.send(msg)

		msg = OSC.OSCMessage()
		msg.setAddress("/1/fader3")
		msg.append(float(self.b) / 255.0)
		self.oscClient.send(msg)

		msg = OSC.OSCMessage()
		msg.setAddress("/1/label4")
		msg.append(self.profiles[self.currentProfileIndex])
		self.oscClient.send(msg)
		
		for i in range(1, 5):
			for k in range(1, 4):
				msg = OSC.OSCMessage()
				address = "/1/multitoggle2/{}/{}".format(i,k)
				msg.setAddress(address)
				msg.append(1.0)
				self.oscClient.send(msg)
				

lightOSC = LightOSC('127.0.0.1', 3636, [4,3,10,9,8,7,6,2,1,5], '192.168.1.112', 8000)
lightOSC.run('192.168.1.102',9000)
