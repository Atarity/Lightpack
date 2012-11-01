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
        return "0.2"
        

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
        box.addWidget(self.comboPreset)
        
        self.preview = QLabel(parent)
        self.preview.setPixmap(QPixmap("Plugins/ZoneCalculator/res/a.png"))
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
        self.preview.setPixmap(QPixmap("Plugins/ZoneCalculator/res/"+preset+".png"))
        
        
    def calculate(self):
        Lightpack.Lock(self.sessionKey)
        preset=unicode(self.comboPreset.currentText)
        print preset
        
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
        
        for rect in self.list:
            rect.setWidth(self.cube)
            rect.setHeight(self.cube)
            
        Lightpack.SetLeds(self.sessionKey,self.list)
        Lightpack.UnLock(self.sessionKey)

    def presetA(self):
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
