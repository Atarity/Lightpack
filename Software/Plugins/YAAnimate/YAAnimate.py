import BasePlugin
import PythonQt
import math, random
from PythonQt.QtGui import *


class YAAnimate(BasePlugin.BasePlugin):
    def init(self):
        self.on = Lightpack.GetStatus()
        self.sessionKey = Lightpack.GetSessionKey(self.__class__.__name__)
        self.timeranim = PythonQt.QtCore.QTimer(None)
        self.timeranim.timeout.connect(self.Timeout)
        random.seed()
        leds = Lightpack.GetCountLeds()
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
        return "Yet another simple animation"

    def author(self):
        """ return the author of the plugin """
        return "Timur Sattarov <tim.helloworld@gmail.com>"

    def version(self):
        """ return the version of the plugin """
        return "0.1"

    def Timeout(self):
        try:
            self.i = self.i+1
            self.lastFrame
            newFrame = self.lastFrame
            leds = Lightpack.GetCountLeds()
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
            print e
    
    def runAnimation(self):
        self.lastFrame = []
        try:
            for k  in range (0, Lightpack.GetCountLeds()):
                self.lastFrame.append( QColor(0,0,0))
            self.i=1
            self.timeranim.setInterval(200)
            self.timeranim.start()
            
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


