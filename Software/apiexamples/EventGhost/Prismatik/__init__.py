import eg
import lightpack

eg.RegisterPlugin(
    name = "Prismatik",
    author = "Lucleonhart, 0xdefec, SaladFork",
    version = "1.0.1",
    kind = "external",
    description = "Control the Lightpack's Prismatik software. Enable API Server in Prismatik's experimental settings."
)

class Lightpack(eg.PluginBase):

    def __init__(self):
        self.AddAction(TurnOn)
        self.AddAction(TurnOff)
        self.AddAction(SetProfile)
        self.AddAction(NextProfile)
        self.AddAction(SetBrightness)
        self.AddAction(SetColor)
        self.AddAction(SetColorToAll)

        self.numLeds = 20

    def __start__(self, host, port, apikey):
        self.ledMap = range(1, self.numLeds + 1)
        self.lpack = lightpack.lightpack(host, int(port), self.ledMap, apikey or None)
        self.lpack.connect()

    def __stop__(self):
        self.lpack.disconnect()

    def __close__(self):
        pass

    def Configure(self, host="127.0.0.1", port="3636", apikey=""):
        panel = eg.ConfigPanel()
        sizer = panel.sizer

        txtHost = panel.TextCtrl(host, size=(250, -1))
        txtPort = panel.TextCtrl(port, size=(250, -1))
        txtApikey = panel.TextCtrl(apikey, size=(250, -1))

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

class SetBrightness(eg.ActionBase):

    name = "Set brightness"
    description = "Sets the LED brightness to a specific value"

    def __call__(self, brightness):
        self.plugin.lpack.lock()
        self.plugin.lpack.setBrightness(brightness)
        self.plugin.lpack.unlock()

    def Configure(self, brightness="100"):
        panel = eg.ConfigPanel()
        sizer = panel.sizer

        txtBrightness = panel.TextCtrl(brightness, size=(250, -1))

        sizer.Add(panel.StaticText("Brightness (%): "))
        sizer.Add(txtBrightness)

        while panel.Affirmed():
            panel.SetResult(txtBrightness.GetValue())

class NextProfile(eg.ActionBase):

    name = "Next profile"
    description = "Switches to the next Prismatik profile"

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

class SetProfile(eg.ActionBase):

    name = "Set profile"
    description = "Switches to a specific Prismatik profile"

    def __call__(self, profile):
        self.plugin.lpack.lock()
        self.plugin.lpack.setProfile(profile)
        self.plugin.lpack.unlock()

        setProfile = self.plugin.lpack.getProfile().strip()

        if setProfile != profile:
            self.PrintError("Could not switch to profile '%s'!" % profile)

    def Configure(self, profile="Lightpack"):
        profiles = self.plugin.lpack.getProfiles()

        panel = eg.ConfigPanel()
        sizer = panel.sizer

        cmbProfiles = panel.ComboBox(profile, choices=profiles, size=(250, -1))

        sizer.Add(panel.StaticText("Profile: "))
        sizer.Add(cmbProfiles)

        while panel.Affirmed():
            panel.SetResult(cmbProfiles.GetValue())

class SetColor(eg.ActionBase):

    name = "Set LED light color"
    description = "Sets the color of a specific LED light"

    def __call__(self, n, r, g, b):
        self.plugin.lpack.lock()
        self.plugin.lpack.setColor(n, r, g, b)
        self.plugin.lpack.unlock()

    def Configure(self, n=1, r=0, g=0, b=0):
        panel = eg.ConfigPanel()
        sizer = panel.sizer

        leds = [str(led) for led in range(1, self.plugin.numLeds + 1)]

        cmbLeds = panel.ComboBox(str(n), choices=leds, size=(75, -1))
        colorCtrl = wx.ColourPickerCtrl(panel, col=wx.Colour(r, g, b), size=wx.Size(75, -1))

        sizer.AddMany([
            panel.StaticText("LED: "),
            cmbLeds,
            panel.StaticText("Color: "),
            colorCtrl
        ])

        while panel.Affirmed():
            color = colorCtrl.GetColour()
            panel.SetResult(cmbLeds.GetValue(), color[0], color[1], color[2])

class SetColorToAll(eg.ActionBase):

    name = "Set all LED lights color"
    description = "Sets the color of all LED lights"

    def __call__(self, r, g, b):
        self.plugin.lpack.lock()
        self.plugin.lpack.setColorToAll(r, g, b)
        self.plugin.lpack.unlock()

    def Configure(self, r=0, g=0, b=0):
        panel = eg.ConfigPanel()
        sizer = panel.sizer

        colorCtrl = wx.ColourPickerCtrl(panel, col=wx.Colour(r, g, b), size=wx.Size(75, -1))

        sizer.AddMany([
            panel.StaticText("Color: "),
            colorCtrl
        ])

        while panel.Affirmed():
            color = colorCtrl.GetColour()
            panel.SetResult(color[0], color[1], color[2])
