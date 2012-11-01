import BasePlugin
import PythonQt
from PythonQt.QtGui import *

class ZoneCalculator(BasePlugin.BasePlugin):
    def init(self):
        self.sessionKey = Lightpack.GetSessionKey(self.__class__.__name__)
        return
        
    def name(self):
        """ return the name of the plugin """
        return self.__class__.__name__

    def description(self):
        """ return a short description of the plugin """
        return "Zone Calculator"

    def author(self):
        """ return the author of the plugin """
        return "Eraser"

    def version(self):
        """ return the version of the plugin """
        return "0.1"
        

    def run(self):
        print self.name()
        return 0;
        

    def settings(self):
        box = QVBoxLayout(SettingsBox)
        self.label = QLabel(SettingsBox)
        self.label.setText('Select preset')
        box.addWidget(self.label)
        
        pushcheck =  QPushButton(SettingsBox)
        pushcheck.text = 'Calculate'
        box.addWidget(pushcheck)

        spacer = QWidget(SettingsBox)
        spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        box.addWidget(spacer)

        pushcheck.connect('clicked()', self.calculate)
        
        
        return 0;
        
    def calculate(self):
        print "check"
        
        Lightpack.Lock(self.sessionKey)
        screen = Lightpack.GetScreenSize()
        if (self.label):    
            self.label.setText(str(screen.width())+"x"+str(screen.height()))
        count = Lightpack.GetCountLeds()
        cube = 150
        
        wint = (screen.width()-cube*4)/3
        hint = (screen.height()-cube*3)/2
        
        
        list = Lightpack.GetLeds()
       
        
        list[0].setX(cube+wint)        
        list[0].setY(screen.height()-cube)        
        list[1].setX(0)        
        list[1].setY(screen.height()-cube)        
        list[2].setX(0)        
        list[2].setY(cube+hint)        
        list[3].setX(0)        
        list[3].setY(0)        
        list[4].setX(cube+wint)        
        list[4].setY(0)        
        list[5].setX(2*(cube+wint))        
        list[5].setY(0)        
        list[6].setX(screen.width()-cube)        
        list[6].setY(0)        
        list[7].setX(screen.width()-cube)        
        list[7].setY(cube+hint)        
        list[8].setX(screen.width()-cube)        
        list[8].setY(screen.height()-cube)        
        list[9].setX(2*(cube+wint))        
        list[9].setY(screen.height()-cube)        
        
        for rect in list:
            rect.setWidth(cube)
            rect.setHeight(cube)
            
        Lightpack.SetLeds(self.sessionKey,list)
        
        Lightpack.UnLock(self.sessionKey)


