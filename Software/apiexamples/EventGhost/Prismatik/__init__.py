import eg
import lightpack

eg.RegisterPlugin(
    name = "Prismatik for EventGhost",
    author = "Lucleonhart, 0xdefec, SaladFork",
    version = "1.0.1",
    kind = "external",
    description = "Control the Lightpack's Prismatik software. Enable API Server in Prismatik's experimental settings."
)

class Lightpack(eg.PluginBase):

    def __init__(self):
        self.AddAction(TurnOn)
        self.AddAction(TurnOff)
        self.AddAction(LowBrightness)
        self.AddAction(MaxBrightness)
        self.AddAction(NextProfile)

    def __start__(self, host, port, apikey):
        self.lpack = lightpack.lightpack(host, int(port), apikey, range(1, 20))
        self.lpack.connect()

    def __stop__(self):
        self.lpack.disconnect()

    def __close__(self):
        pass

    def Configure(self, host="127.0.0.1", port="3636", apikey=""):
        panel = eg.ConfigPanel()
        sizer = panel.sizer

        txtHost = wx.TextCtrl(panel, -1, host, size=(250, -1))
        txtPort = wx.TextCtrl(panel, -1, port, size=(250, -1))
        txtApikey = wx.TextCtrl(panel, -1, apikey, size=(250, -1))

        sizer.AddMany([
            (panel.StaticText("Host: ")),
            (txtHost),
            (panel.StaticText("Port: ")),
            (txtPort),
            (panel.StaticText("API Key: ")),
            (txtApikey)
        ])

        while panel.Affirmed():
            panel.SetResult(txtHost.GetValue(), txtPort.GetValue(), txtApikey.GetValue())


class TurnOn(eg.ActionBase):

    name = "Turn on"
    description = "Turns on the lights"

    def __call__(self):
        self.plugin.lpack.lock()
        self.plugin.lpack.turnOn()
        self.plugin.lpack.unlock()

class TurnOff(eg.ActionBase):

    name = "Turn off"
    description = "Turns off the Lights"

    def __call__(self):
        self.plugin.lpack.lock()
        self.plugin.lpack.turnOff()
        self.plugin.lpack.unlock()

class LowBrightness(eg.ActionBase):

    name = "Low brightness"
    description = "Sets the brightness to 20%"

    def __call__(self):
        self.plugin.lpack.lock()
        self.plugin.lpack.setBrightness(20)
        self.plugin.lpack.unlock()

class MaxBrightness(eg.ActionBase):

    name = "Maximum brightness"
    description = "Sets the brightness to 100%"

    def __call__(self):
        self.plugin.lpack.lock()
        self.plugin.lpack.setBrightness(100)
        self.plugin.lpack.unlock()

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
