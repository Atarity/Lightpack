import eg
import lightpack

eg.RegisterPlugin(
	name = "Prismatik for Eventghost",
	author = "Lucleonhart, 0xdefec, SaladFork",
	version = "1.0",
	kind = "external",
	description = "Control the Lightpack's Prismatik software. Enable API Server in Prismatik's experimental settings (leave the key empty and port at 3636)."
)

class Lightpack(eg.PluginBase):

	def __init__(self):
		self.AddAction(TurnOn)
		self.AddAction(TurnOff)
		self.AddAction(Toggle)
		self.AddAction(LowBrightness)
		self.AddAction(MaxBrightness)
		self.AddAction(NextProfile)

		self.lpack = lightpack.lightpack("127.0.0.1", 3636, "", [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20] )
		self.lpack.connect()


class TurnOn(eg.ActionBase):

	name = "Turn on"
	description = "Turn on the Lights."

	def __call__(self):
		self.plugin.lpack.lock()
		self.plugin.lpack.turnOn()
		self.plugin.lpack.unlock()
		print "Lightpack lights are on now"

class TurnOff(eg.ActionBase):

	name = "Turn off"
	description = "Turn off the Lights."

	def __call__(self):
		self.plugin.lpack.lock()
		self.plugin.lpack.turnOff()
		self.plugin.lpack.unlock()
		print "Lightpack lights are off now"

class Toggle(eg.ActionBase):

	name = "Toggle lights on/off"
	description = "Toggles the lights on or off"

	def __call__(self):
		self.plugin.lpack.lock()
		status = self.plugin.lpack.getStatus()
		if status.strip() == 'on':
			self.plugin.lpack.turnOff()
			print "turning off"
		else:
			self.plugin.lpack.turnOn()
			print "turning on"
		self.plugin.lpack.unlock()
		print "Lightpack lights are "+self.plugin.lpack.getStatus().strip()+" now"

class LowBrightness(eg.ActionBase):

	name = "Low Brightness"
	description = "Sets the brightness to 20%"

	def __call__(self):
		self.plugin.lpack.lock()
		self.plugin.lpack.setBrightness(20)
		self.plugin.lpack.unlock()
		print "Brightness now at 20%"

class MaxBrightness(eg.ActionBase):

	name = "Maximum Brightness"
	description = "Sets the brightness to 100%"

	def __call__(self):
		self.plugin.lpack.lock()
		self.plugin.lpack.setBrightness(100)
		self.plugin.lpack.unlock()
		print "Brightness now at 100%"

class NextProfile(eg.ActionBase):

	name = "Next profile"
	description = "Switches to the next prismatik profile"

	def __call__(self):
		self.plugin.lpack.lock()
		profiles = self.plugin.lpack.getProfiles()
		currentProfile = self.plugin.lpack.getProfile().strip()
		profilePos = profiles.index(currentProfile)
		if profilePos == len(profiles)-1:
			next = 0
		else:
			next = profilePos+1
		self.plugin.lpack.setProfile(profiles[next])
		self.plugin.lpack.unlock()
		print "Switched profile to "+profiles[next]
