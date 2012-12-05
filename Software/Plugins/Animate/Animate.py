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
        self.timeranim.timeout.connect(self.Timeout)
        self.ledMap = [1,2,3,6,7,8,9,10,4,5]
        leds = Lightpack.GetCountLeds()
        random.seed()
        print "init"

    def dispose(self):
        self.timeranim.stop()
        del self.timeranim
        Lightpack.SetStatus(self.sessionKey,self.on)
        Lightpack.UnLock(self.sessionKey)
        print "dispose"

    def name(self):
        """ return the name of the plugin """
        return self.__class__.__name__
 
    def description(self):
        """ return a short description of the plugin """
        return "Simple animation for Lightpack devices only"

    def author(self):
        """ return the author of the plugin """
        return "Timur Sattarov <tim.helloworld@gmail.com>"

    def version(self):
        """ return the version of the plugin """
        return "0.1"

    def Timeout(self):
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
            print e
    
    def runAnimation(self):
        try:
            self.i=Lightpack.GetCountLeds()*2 + random.randrange(20000)
            self.timeranim.setInterval(70)
            self.timeranim.start()
            self.lastFrame=[]
            for k in range(0, Lightpack.GetCountLeds()):
                self.lastFrame.append(QColor(0,0,0))
            
        except Exception, e:
            print e
        

    def run(self):
        self.on = Lightpack.GetStatus()
        if Lightpack.Lock(self.sessionKey) :
            Lightpack.SetStatus(self.sessionKey,1)
            self.runAnimation()
        print "run"
        
    def stop(self):
        self.timeranim.stop()
        Lightpack.UnLock(self.sessionKey)
        print "stop"

    def settings(self):
        """ default function """


