import BasePlugin
import PythonQt
import math
import random
from PythonQt.QtGui import *

class Animate(BasePlugin.BasePlugin):
    def init(self):
        self.on = Lightpack.GetStatus()
        self.sessionKey = Lightpack.GetSessionKey(self.__class__.__name__)
        self.timeranim = PythonQt.QtCore.QTimer(None)
        random.seed()
        self.log( "init")

    def dispose(self):
        self.timeranim.stop()
        del self.timeranim
        Lightpack.SetStatus(self.sessionKey,self.on)
        Lightpack.UnLock(self.sessionKey)
        self.log( "dispose")

    def name(self):
        """ return the name of the plugin """
        return self.__class__.__name__

    def description(self):
        """ return a short description of the plugin """
        return "Simple animation"

    def author(self):
        """ return the author of the plugin """
        return "Timur Sattarov <tim.helloworld@gmail.com>"

    def version(self):
        """ return the version of the plugin """
        return "0.1"

    def run(self):
        self.on = Lightpack.GetStatus()
        if Lightpack.Lock(self.sessionKey) :
            leds = Lightpack.GetCountLeds()
            #self.ledMap = [6,7,8,9,10,4,5,1,2,3]
            self.ledMap = [ x for x in range(1, leds+1)]
            Lightpack.SetStatus(self.sessionKey,1)
            self.onAnimationChange()
        self.log("run")
        
    def stop(self):
        self.timeranim.stop()
        Lightpack.UnLock(self.sessionKey)
        self.log("stop")

    def settings(self, parent):
        layout = QVBoxLayout(parent)

        self.rbAnimation1 = QRadioButton(parent)
        self.rbAnimation1.setText('Animation 1')
        self.rbAnimation1.connect('clicked()', self.onAnimationChange)
        layout.addWidget(self.rbAnimation1)

        self.rbAnimation2 = QRadioButton(parent)
        self.rbAnimation2.setText('Animation 2')
        self.rbAnimation2.connect('clicked()', self.onAnimationChange)
        layout.addWidget(self.rbAnimation2)

        self.rbSnakeAnimation = QRadioButton(parent)
        self.rbSnakeAnimation.setText('Snake')
        self.rbSnakeAnimation.connect('clicked()', self.onAnimationChange)
        layout.addWidget(self.rbSnakeAnimation)

        self.rbCylonAnimation = QRadioButton(parent)
        self.rbCylonAnimation.setText('Cylon')
        self.rbCylonAnimation.connect('clicked()', self.onAnimationChange)
        layout.addWidget(self.rbCylonAnimation)

        labemap = QLabel(parent)
        labemap.setText("Leds map (separator - ','):")
        layout.addWidget(labemap)

        editmap = QLineEdit(parent)
        map = Lightpack.GetSettingMain('Animate/LedsMap')
        if (map == None):
            map = self.mapToStr(self.defaultMap())
            self.changeMap(map)
        editmap.setText(map)
        editmap.connect('textChanged(QString)', self.changeMap)
        layout.addWidget(editmap)

        spacer = QWidget(parent)
        spacer.setSizePolicy(QSizePolicy.Fixed, QSizePolicy.Expanding)
        layout.addWidget(spacer)
        self.rbAnimation1.setChecked(True)

    def defaultMap(self):
        try:
            leds = Lightpack.GetCountLeds()
            map = [n for n in range (1, leds+1)]
        except Exception, e:
            self.log(str(e))
            map = [1,2,3,4,5,6,7,8,9,10]
        return map

    def changeMap(self,value):
        Lightpack.SetSettingMain('Animate/LedsMap',value)
        self.setMap()

    def mapToStr(self,map):
        return ','.join(str(x) for x in map) 

    def setMap(self):
        try:
            map = Lightpack.GetSettingMain('Animate/LedsMap')
            self.ledMap = [int(n) for n in map.split(',')]
            self.log(str(self.ledMap))
        except Exception, e:
            self.log(str(e))
            self.ledMap = self.defaultMap()     

    def Animation1(self):
        try:
            self.i = self.i+1
            leds = Lightpack.GetCountLeds()
            for k  in range (0, leds):
                t = float(self.i/4 - k*2)/10
                r = int((math.sin(t)+1)*127)
                g = int((math.cos(t*0.7)+1)*127)
                b = int((math.cos(1.3 + t*0.9)+1)*127)
                self.lastFrame[self.ledMap[k]-1]=QColor(r,g,b)
            Lightpack.SetFrame(self.sessionKey, self.lastFrame)
            self.i += 1
        except Exception, e:
            self.log(str(e))

    def Animation2(self):
        try:
            self.i = self.i+1
            self.lastFrame
            newFrame = self.lastFrame
            leds = self.ledsCount
            att = 0.95
            for k  in range (0, leds):
                if random.randrange(100) < 10 :
                    r = random.randrange(255)
                    g = random.randrange(255)
                    b = random.randrange(255)
                else :
                    r = int(newFrame[k].red() * att)
                    g = int(newFrame[k].green() * att)
                    b = int(newFrame[k].blue() * att)

                newFrame[k] = QColor(r,g,b)
            Lightpack.SetFrame(self.sessionKey, newFrame)
            self.lastFrame = newFrame
            self.i += 1
        except Exception, e:
            self.log(str(e))

    def SnakeAnimation(self):
        for k in range (0, self.ledsCount) :    
            idx = (self.i+k) % self.ledsCount
            if k < 3 :
                self.lastFrame[self.ledMap[idx]-1]=QColor(255,0,0)
            else :
                self.lastFrame[self.ledMap[idx]-1]=QColor(0,0,125)
        Lightpack.SetFrame(self.sessionKey, self.lastFrame)
        self.i += 1

    def CylonAnimation(self):
        for k in self.cylonMap: 
            idx = self.cylonMap.index(k)
            cyclePhase = self.i % self.cylonCycleLength 
            if (cyclePhase > self.cylonWidth-1 and idx == 2*(self.cylonWidth-1) - cyclePhase) or (idx == cyclePhase):
                for i in k:
                    self.lastFrame[i-1]=QColor(255,0,0)
            else:
                for i in k:
                    self.lastFrame[i-1]=QColor(0,0,0)
        Lightpack.SetFrame(self.sessionKey, self.lastFrame)
        self.i += 1

    def onAnimationChange(self):
        self.timeranim.timeout.disconnect()
        self.ledsCount = Lightpack.GetCountLeds()
        self.lastFrame=[ QColor(0,0,0) for k in range(0, self.ledsCount)]
        if self.rbAnimation1.isChecked():
            self.i=self.ledsCount*2 + random.randrange(20000)
            self.timeranim.setInterval(70)
            self.timeranim.timeout.connect(self.Animation1)
            self.timeranim.start()

        elif self.rbAnimation2.isChecked():
            self.timeranim.setInterval(200)
            self.timeranim.timeout.connect(self.Animation2)
            self.timeranim.start()

        elif self.rbSnakeAnimation.isChecked():
            self.timeranim.setInterval(200)
            self.timeranim.timeout.connect(self.SnakeAnimation)
            self.timeranim.start()

        elif self.rbCylonAnimation.isChecked():
            self.timeranim.setInterval(200)
            self.cylonMap = [ [1,2,3], [4,5], [6,7], [8,9,10] ]
            self.cylonWidth = len(self.cylonMap)
            self.cylonCycleLength = self.cylonWidth*2 - 2
            self.timeranim.timeout.connect(self.CylonAnimation)
            self.timeranim.start()
        """ default function """
