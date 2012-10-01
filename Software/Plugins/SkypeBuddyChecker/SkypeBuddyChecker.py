#Skype contact waiting script. It flashes Lightpack when target buddy in contact list are changed his online status.
#You need Skype4Py module for using it.

import Skype4Py, time, sys
import BasePlugin
import PythonQt
from PythonQt.QtGui import *

class SkypeBuddyChecker(BasePlugin.BasePlugin):
	def init(self):
		self.skype = Skype4Py.Skype() # Create an instance of the Skype class
		self.lpack = Lightpack.lightpack('127.0.0.1', 3636, [2,3,6,7,8,9,10,4,5,1] )
		self.skype.Attach() # Connect the Skype object to the Skype client
		self.targetstat = ['ONLINE', 'DND', 'SKYPEME'] # Array of target user statuses
		self.lpack.connect()	

		self.sessionKey = Lightpack.GetSessionKey(self.__class__.__name__)
		self.timer = PythonQt.QtCore.QTimer(None)
		self.timer.setInterval(100)
		self.timer.timeout.connect(self.Timeout)
		self.timerCheck = PythonQt.QtCore.QTimer(None)
		self.timerCheck.setInterval(60000)
		self.timerCheck.connect('timeout()', self.checkSkype)

		self.ledMap = [6,1,2,7,3,4,8,9,10,5]
		print "init"


	def name(self):
		""" return the name of the plugin """
		return self.__class__.__name__

	def description(self):
		""" return a short description of the plugin """
		return "Check Google mail account"

	def author(self):
		""" return the author of the plugin """
		return "Tim"

	def version(self):
		""" return the version of the plugin """
		return "0.2"

	def settings(self):
		""" default function """
		box = QVBoxLayout(SettingsBox)
		label = QLabel(SettingsBox)
		label.setText("Account")
		box.addWidget(label)
		editAcc = QLineEdit(SettingsBox)
		editAcc.setText(Lightpack.GetSettingMain('GmailChecker/Username'))
		box.addWidget(editAcc)

		label2 = QLabel(SettingsBox)
		label2.setText("Password")
		box.addWidget(label2)
		editPass = QLineEdit(SettingsBox)
		editPass.setEchoMode(PythonQt.QtGui.QLineEdit.Password)
		#        editPass.setText()Lightpack.GetObfuscatedSettingMain('GmailChecker/Password'))
		box.addWidget(editPass)
		box.addSpacing(150)

		editAcc.connect('textChanged(QString)', self.changeAcc)
		editPass.connect('textChanged(QString)', self.changePass)
		return 0

	def run(self):
		self.timerCheck.start()
		print "run"

	def stop(self):
		self.timerCheck.stop()
		self.timer.stop()
		self.lpack.UnLock(self.sessionKey)
		print "stop"

	def runAnimation(self):
		self.lpack.Lock(self.sessionKey)
		self.i=1
		self.timer.start()

	def stopAnimation(self):
		self.timer.stop()
		self.lpack.UnLock(self.sessionKey)
		self.tick = 0


	def checkSkype(self):
		if self.skype.User("1").OnlineStatus in targetstat : # If current buddy online status in targetstat array turn animation on
			print skype.User("1").FullName + ' is ' + skype.User("1").OnlineStatus + ' @ ' + time.ctime()
			self.lpack.lock()
			self.lpack.setSmooth(100)
			self.lpack.setColorToAll(0,0,0)
			#time.sleep(0.3)
			for k in range(0,5):
				self.lpack.setColorToAll(0,255,0)
			#	time.sleep(1);
				self.lpack.setColorToAll(0,0,0)
			#	time.sleep(1);
			self.lpack.unlock()		
		else:
			print self.skype.User(self.contact).FullName + ' is ' + skype.User(self.contact).OnlineStatus + ' @ ' + time.ctime()

	def dispose(self):
		self.timer.stop()
		self.timerCheck.setInterval(0)
		self.timerCheck.stop()
		self.timerCheck.disconnect('timeout()', self.checkSkype)
		del self.timer
		del self.timerCheck
		print "dispose"


