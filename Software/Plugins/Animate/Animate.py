import imaplib

import BasePlugin
import PythonQt
import math
from PythonQt.QtGui import *


class Animate(BasePlugin.BasePlugin):
    def init(self):
        self.on = Lightpack.GetStatus()
        self.sessionKey = Lightpack.GetSessionKey(self.__class__.__name__)
        self.timeranim = PythonQt.QtCore.QTimer(None)
        self.timeranim.timeout.connect(self.Timeout)
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
        return "Simple animation"

    def author(self):
        """ return the author of the plugin """
        return "Timur Sattarov <tim.helloworld@gmail.com>"

    def version(self):
        """ return the version of the plugin """
        return "0.1"

    def Timeout(self):
        try:
            self.i = self.i+1
            frame=[]
            leds = Lightpack.GetCountLeds()
            for k  in range (0, leds):
                t = self.i/3 + int(k / 8)*8 + (k % 8 if (k % 16 < 8) else (7- k%8 ))
                r = int((math.sin(t/10)+1)*127)
                g = int((math.cos(t/10)+1)*127)
                b = int((math.cos(1.3 + t/10)+1)*127)
                frame.append(QColor(r,g,b))
            Lightpack.SetFrame(self.sessionKey, frame)
            self.i += 1
        except Exception, e:
            print e
    
    def runAnimation(self):
        try:
            self.i=1
            self.timeranim.setInterval(70)
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


