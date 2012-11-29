import BasePlugin
import PythonQt
from PythonQt.QtGui import *

class ZoneCalculator(BasePlugin.BasePlugin):
    def init(self):
        self.sessionKey = Lightpack.GetSessionKey(self.__class__.__name__)
        print self.sessionKey
        self.resdir = Lightpack.GetPluginsDir()+"/ZoneCalculator/res/"
        return
        
    def name(self):
        """ return the name of the plugin """
        return self.__class__.__name__

    def description(self):
        """ return a short description of the plugin """
        return "Zone Calculator"

    def author(self):
        """ return the author of the plugin """
        return "Eraser <eraser1981@gmail.com>"

    def version(self):
        """ return the version of the plugin """
        return "0.4"
        

    def run(self):
        print self.name()
        return 0;
        

    def settings(self,parent):
        box = QVBoxLayout(parent)
        label = QLabel(parent)
        label.setText('Select preset')
        box.addWidget(label)
        self.comboPreset = QComboBox(parent)
        self.comboPreset.addItem('a')
        self.comboPreset.addItem('b')
        self.comboPreset.addItem('c')
        self.comboPreset.addItem('d')
        self.comboPreset.addItem('e')
        self.comboPreset.addItem('f')
        self.comboPreset.addItem('pi')
        self.comboPreset.addItem('r')
        box.addWidget(self.comboPreset)
        
        self.preview = QLabel(parent)
        self.preview.setPixmap(QPixmap(self.resdir+"a.png"))
        box.addWidget(self.preview)
        
        pushcalc =  QPushButton(parent)
        pushcalc.text = 'Calculate'
        box.addWidget(pushcalc)

        spacer = QWidget(parent)
        spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        box.addWidget(spacer)

        pushcalc.connect('clicked()', self.calculate)
        self.comboPreset.connect('currentIndexChanged(int)', self.combo_activateInput)
        return 0;
        

    def combo_activateInput(self): 
        preset=unicode(self.comboPreset.currentText)
        print preset
        self.preview.setPixmap(QPixmap(self.resdir+preset+".png"))
        
        
    def calculate(self):
        self.sessionKey = Lightpack.GetSessionKey(self.__class__.__name__)
        Lightpack.Lock(self.sessionKey)
        preset=unicode(self.comboPreset.currentText)
        
        self.screen = Lightpack.GetScreenSize()
        self.list = Lightpack.GetLeds()
        self.count = Lightpack.GetCountLeds()
        self.cube = 150
        
        if preset=="a":
            self.presetA()
        if preset=="b":
            self.presetB()
        if preset=="c":
            self.presetC()
        if preset=="d":
            self.presetD()
        if preset=="e":
            self.presetE()
        if preset=="f":
            self.presetF()
        if preset=="pi":
            self.presetP()
        if preset=="r":
            self.presetR()
        
        for rect in self.list:
            rect.setWidth(self.cube)
            rect.setHeight(self.cube)
            
        Lightpack.SetLeds(self.sessionKey,self.list)
        Lightpack.UnLock(self.sessionKey)

    def presetA(self):
        print "PresetA begin"
        wint = (self.screen.width()-self.cube*6)/5
        hint = (self.screen.height()-self.cube*3)/2
        self.list[0].setX(0)        
        self.list[0].setY(self.screen.height()-self.cube)        
        self.list[1].setX(0)        
        self.list[1].setY(self.cube+hint)        
        self.list[2].setX(0)        
        self.list[2].setY(0)        
        self.list[3].setX(self.cube+wint)        
        self.list[3].setY(0)        
        self.list[4].setX(2*(self.cube+wint))        
        self.list[4].setY(0)        
        self.list[5].setX(3*(self.cube+wint))        
        self.list[5].setY(0)        
        self.list[6].setX(4*(self.cube+wint))        
        self.list[6].setY(0)        
        self.list[7].setX(self.screen.width()-self.cube)        
        self.list[7].setY(0)        
        self.list[8].setX(self.screen.width()-self.cube)        
        self.list[8].setY(self.cube+hint)        
        self.list[9].setX(self.screen.width()-self.cube)        
        self.list[9].setY(self.screen.height()-self.cube)        
        print "PresetA end"
        return
    
    def presetB(self):
        wint = (self.screen.width()-self.cube*4)/3
        hint = (self.screen.height()-self.cube*4)/3
        
        self.list[0].setX(0)        
        self.list[0].setY(self.screen.height()-self.cube)
        self.list[1].setX(0)
        self.list[1].setY(2*(self.cube+hint))
        self.list[2].setX(0)
        self.list[2].setY(self.cube+hint)
        self.list[3].setX(0)
        self.list[3].setY(0)        
        self.list[4].setX(self.cube+wint)        
        self.list[4].setY(0)        
        self.list[5].setX(2*(self.cube+wint))        
        self.list[5].setY(0)        
        self.list[6].setX(self.screen.width()-self.cube)        
        self.list[6].setY(0)        
        self.list[7].setX(self.screen.width()-self.cube)        
        self.list[7].setY(self.cube+hint)        
        self.list[8].setX(self.screen.width()-self.cube)        
        self.list[8].setY(2*(self.cube+hint))        
        self.list[9].setX(self.screen.width()-self.cube)        
        self.list[9].setY(self.screen.height()-self.cube)        
        return
    
    def presetC(self):
        wint = (self.screen.width()-self.cube*4)/3
        hint = (self.screen.height()-self.cube*3)/2
        
        self.list[0].setX(self.cube+wint)
        self.list[0].setY(self.screen.height()-self.cube)
        self.list[1].setX(0)
        self.list[1].setY(self.screen.height()-self.cube)
        self.list[2].setX(0)
        self.list[2].setY(self.cube+hint)
        self.list[3].setX(0)
        self.list[3].setY(0)
        self.list[4].setX(self.cube+wint)
        self.list[4].setY(0)
        self.list[5].setX(2*(self.cube+wint))
        self.list[5].setY(0)
        self.list[6].setX(self.screen.width()-self.cube)
        self.list[6].setY(0)
        self.list[7].setX(self.screen.width()-self.cube)
        self.list[7].setY(self.cube+hint)
        self.list[8].setX(self.screen.width()-self.cube)
        self.list[8].setY(self.screen.height()-self.cube)
        self.list[9].setX(2*(self.cube+wint))
        self.list[9].setY(self.screen.height()-self.cube)

    def presetD(self):
        hint = (self.screen.height()-self.cube*5)/4
        
        self.list[0].setX(0)
        self.list[0].setY(self.screen.height()-self.cube)
        self.list[1].setX(0)
        self.list[1].setY(3*(self.cube+hint))
        self.list[2].setX(0)
        self.list[2].setY(2*(self.cube+hint))
        self.list[3].setX(0)
        self.list[3].setY(self.cube+hint)
        self.list[4].setX(0)
        self.list[4].setY(0)
        self.list[5].setX(self.screen.width()-self.cube)
        self.list[5].setY(0)
        self.list[6].setX(self.screen.width()-self.cube)
        self.list[6].setY(self.cube+hint)
        self.list[7].setX(self.screen.width()-self.cube)
        self.list[7].setY(2*(self.cube+hint))
        self.list[8].setX(self.screen.width()-self.cube)
        self.list[8].setY(3*(self.cube+hint))
        self.list[9].setX(self.screen.width()-self.cube)
        self.list[9].setY(self.screen.height()-self.cube)
        return

    def presetE(self):
        wint = (self.screen.width()-self.cube*5)/4
        
        self.list[0].setX(0)
        self.list[0].setY(0)
        self.list[1].setX(self.cube+wint)
        self.list[1].setY(0)
        self.list[2].setX(2*(self.cube+wint))
        self.list[2].setY(0)
        self.list[3].setX(3*(self.cube+wint))
        self.list[3].setY(0)
        self.list[4].setX(self.screen.width()-self.cube)
        self.list[4].setY(0)
        self.list[5].setX(0)
        self.list[5].setY(self.screen.height()-self.cube)
        self.list[6].setX(self.cube+wint)
        self.list[6].setY(self.screen.height()-self.cube)
        self.list[7].setX(2*(self.cube+wint))
        self.list[7].setY(self.screen.height()-self.cube)
        self.list[8].setX(3*(self.cube+wint))
        self.list[8].setY(self.screen.height()-self.cube)
        self.list[9].setX(self.screen.width()-self.cube)
        self.list[9].setY(self.screen.height()-self.cube)
        return

    def presetF(self):
        wint = (self.screen.width()-self.cube*10)/9
        
        self.list[0].setX(0)
        self.list[0].setY(0)
        self.list[1].setX(self.cube+wint)
        self.list[1].setY(0)
        self.list[2].setX(2*(self.cube+wint))
        self.list[2].setY(0)
        self.list[3].setX(3*(self.cube+wint))
        self.list[3].setY(0)
        self.list[4].setX(4*(self.cube+wint))
        self.list[4].setY(0)
        self.list[5].setX(5*(self.cube+wint))
        self.list[5].setY(0)
        self.list[6].setX(6*(self.cube+wint))
        self.list[6].setY(0)
        self.list[7].setX(7*(self.cube+wint))
        self.list[7].setY(0)
        self.list[8].setX(8*(self.cube+wint))
        self.list[8].setY(0)
        self.list[9].setX(self.screen.width()-self.cube)
        self.list[9].setY(0)
        return

    def presetP(self):
        print "PresetP begin"
        sw = self.screen.width()
        sh = self.screen.height()
        c = Lightpack.GetCountLeds()
        x = (2*sh+sw)/(c+2)
        w = sw/x
        h = sh/x
        
        self.cube = x
        
        for i in range(c):
            print i
            sx = 0
            sy = 0
            if i<h:
                sx = 0
                sy = sh-(i+1)*x
            if i>=h and i<h+w-1:
                sx = (i-h+1)*x
                sy = 0
            if i>=h+w-1:
                sx = sw-x
                sy = (i-h-w+2)*x
            self.list[i].setX(sx)        
            self.list[i].setY(sy)        
        
       
        print "PresetP end"
        return
    
    def presetR(self):
        sw = self.screen.width()
        sh = self.screen.height()
        c = Lightpack.GetCountLeds()
        x = (2*sh+2*sw)/(c+4)
        w = sw/x
        h = sh/x
        
        self.cube = x
        
        for i in range(c):
            print i
            sx = 0
            sy = 0
            if i<w/2-1:
                sx = sh/2 - i*x
                sy = sh-x
            if i>=w/2-1 and i<w/2+h-1:
                sx = 0
                sy = sh-(i-w/2+2)*x
            if i>=w/2+h-1 and i<w/2+h+w-2:
                sx = (i-(w/2+h-2))*x
                sy = 0
            if i>=w/2+h+w-2 and i<w/2+h+w+h-3:
                sx = sw-x
                sy = (i-(w/2+h+w-2)+1)*x
            if i>=w/2+h+w+h-3:
                sx = sw-x*(i-(w/2+h+w+h-4))
                sy = sh-x
            self.list[i].setX(sx)        
            self.list[i].setY(sy)        
        
        return
        
    