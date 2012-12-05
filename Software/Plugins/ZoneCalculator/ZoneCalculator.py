import BasePlugin
import PythonQt
import math
from PythonQt.QtGui import *

class ZoneCalculator(BasePlugin.BasePlugin):
    def init(self):
        self.sessionKey = Lightpack.GetSessionKey(self.__class__.__name__)
        self.log(self.sessionKey)
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
        self.log('run')
        return 0

    def settings(self,parent):
        box = QGridLayout(parent)
        label = QLabel(parent)
        label.setText('Select preset')
        box.addWidget(label, 0, 1)
        self.comboPreset = QComboBox(parent)
        self.comboPreset.addItem('a')
        self.comboPreset.addItem('b')
        self.comboPreset.addItem('c')
        self.comboPreset.addItem('d')
        self.comboPreset.addItem('e')
        self.comboPreset.addItem('f')
        self.comboPreset.addItem('pi')
        self.comboPreset.addItem('r')
        box.addWidget(self.comboPreset,0,2)

        self.preview = QLabel(parent)
        self.preview.setPixmap(QPixmap(self.resdir+"a.png"))
        box.addWidget(self.preview,1,1,1,2)

        groupBox = QGroupBox(parent)
        groupBox.setTitle('Count direction')
        groupVBox =  QVBoxLayout()
        groupBox.setLayout(groupVBox)
        groupBox.setSizePolicy(QSizePolicy.Fixed, QSizePolicy.Expanding)
        box.addWidget(groupBox, 2, 1, 1, 2)

        self.rbDirCw = QRadioButton(parent)
        self.rbDirCw.setText('Clock-wise')
        self.rbDirCw.setChecked(True)
        groupVBox.addWidget(self.rbDirCw)

        self.rbDirCcw = QRadioButton(parent)
        self.rbDirCcw.setText('Counter clock-wise')
        groupVBox.addWidget(self.rbDirCcw)

        label = QLabel(parent)
        label.setText('Start count from:')
        box.addWidget(label, 3,1) 

        self.sbCountFrom = QSpinBox(parent)
        self.sbCountFrom.setMinimum(1)
        self.sbCountFrom.setMaximum(255)
        box.addWidget(self.sbCountFrom, 3, 2)

        pushcalc =  QPushButton(parent)
        pushcalc.text = 'Calculate'
        box.addWidget(pushcalc,4,1,1,2,4)

        spacer = QWidget(parent)
        spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        spacer.setFixedWidth(1)
        box.addWidget(spacer,5,1)

        pushcalc.connect('clicked()', self.calculate)
        self.comboPreset.connect('currentIndexChanged(int)', self.combo_activateInput)
        box.setColumnStretch(0,2)
        box.setColumnStretch(1,1)
        box.setColumnStretch(2,1)
        box.setColumnStretch(3,2)

        return 0

    def combo_activateInput(self):
        preset=unicode(self.comboPreset.currentText)
        self.log(preset)
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
            self.presetRM()

        for rect in self.list:
            rect.setWidth(self.cube)
            rect.setHeight(self.cube)

        Lightpack.SetLeds(self.sessionKey,self.list)
        Lightpack.UnLock(self.sessionKey)

    def presetA(self):
        self.log("PresetA begin")
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
        self.log("PresetA end")
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

    def calcListIndex(self,i):
        return (i + self.sbCountFrom.value - 1) % self.count

    def presetP(self):
        self.log("PresetP begin")
        sw = self.screen.width()
        sh = self.screen.height()
        c = Lightpack.GetCountLeds()
        x = (2*sh+sw)/(c+2)
        w = sw/x
        h = sh/x
        startFrom = self.sbCountFrom.value

        self.cube = x
        if self.rbDirCw.isChecked():
            for i in range(c):
                self.log(str(i))
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
                idx = self.calcListIndex(i)
                self.list[idx].setX(sx)
                self.list[idx].setY(sy)

        else:
            for i in range(c):
                self.log(str(i))
                sx = 0
                sy = 0
                if i<h:
                    sx = sw-x
                    sy = sh-(i+1)*x
                if i>=h and i<h+w-1:
                    sx = sw - (i-h+1)*x
                    sy = 0
                if i>=h+w-1:
                    sx = 0
                    sy = (i-h-w+2)*x
                idx = self.calcListIndex(i)
                self.list[idx].setX(sx)
                self.list[idx].setY(sy)

        self.log("PresetP end")
        return

    def presetR(self):
        sw = self.screen.width()
        sh = self.screen.height()
        c = Lightpack.GetCountLeds()
        x = (2*sh+2*sw)/(c+4)
        w = sw/x
        h = sh/x

        self.cube = x

        if self.rbDirCw.isChecked():
            for i in range(c):
                self.log(str(i))
                sx = 0
                sy = 0
                if i<w/2 :
                    sx = sw/2 - (i+1)*x
                    sy = sh-x
                if i>=w/2 and i<w/2+h-1:
                    sx = 0
                    sy = sh-(i-w/2+2)*x
                if i>=w/2+h-1 and i<w/2+h+w-1:
                    sx = (i-(w/2+h-2))*x
                    sy = 0
                if i>=w/2+h+w-1 and i<w/2+h+h+w-2:
                    sx = sw-x
                    sy = (i-(w/2+h+w-1)+1)*x
                if i>=w/2+h+h+w-2:
                    sx = sw-x*(i-(w/2+h+w+h-4))
                    sy = sh-x
                idx = self.calcListIndex(i)
                self.list[idx].setX(sx)
                self.list[idx].setY(sy)

        else:
            for i in range(c):
                self.log(str(i))
                sx = 0
                sy = 0
                if i<w/2:
                    sx = sw/2 + i*x
                    sy = sh-x
                if i>=w/2 and i<w/2+h-1:
                    sx = sw-x
                    sy = sh-(i-w/2+2)*x
                if i>=w/2+h-1 and i<w/2+h+w-1:
                    sx = sw - (i-(w/2+h-2))*x
                    sy = 0
                if i>=w/2+h+w-1 and i<w/2+h+w+h-2:
                    sx = 0
                    sy = (i-(w/2+h+w-2))*x
                if i>=w/2+h+w+h-2:
                    sx = x*(i-(w/2+h+w+h-4))
                    sy = sh-x
                idx = self.calcListIndex(i)
                self.list[idx].setX(sx)
                self.list[idx].setY(sy)

        return

    def rotate(self, x, y, dirCw):
        dirMul = -1 if dirCw else 1
        nx = int(round(x * math.cos(math.pi/2) + dirMul * y * math.sin(math.pi/2)))
        ny = int(round(dirMul * (-x) * math.sin(math.pi/2) + y * math.cos(math.pi/2)))
        return nx,ny

    def presetRM(self):
        sw = self.screen.width()
        sh = self.screen.height()
        c = Lightpack.GetCountLeds()
        x = int(round((2*sh+2*sw)/float(c+6)))
        w = sw/x
        h = sh/x
        if h < 3:
            h = 3
            w = c/2 - h + 2
            x = sh/h
        dirCw = self.rbDirCw.isChecked()

        self.cube = x
        oh = (sh - h*x)
        ow = (sw - w*x)
       
        if dirCw:
            ox = -1
            oy = 0
        else:
            ox = 0
            oy = 1

        dx = int(round(ox * ( x + ow/float(w-1))))
        dy = int(round(oy * ( x + oh/float(h-1))))

        if dirCw:
            sx = -dx
            sy = sh-x
        else:
            sx = 0
            sy = sh-x-dy 

        eps = x/10

        for i in range(c):
            idx = self.calcListIndex(i)
            self.log('i={}, idx={}'.format(i, idx))
            if (dx > 0 and sx+dx+x >= sw+eps) or (dx < 0 and sx+dx < -eps) or (dy > 0 and sy+dy+x >= sh+eps) or (dy < 0 and sy+dy < -eps):
                self.log('rotating ox={}, oy={}, sx={}, sy={}'.format(ox,oy,sx,sy))
                (ox,oy) = self.rotate(ox, oy, dirCw)
                self.log('rotated ox={}, oy={}, sx={}, sy={}'.format(ox,oy,sx,sy))

            dx = int(round(ox * ( x + ow/float(w-1))))
            dy = int(round(oy * ( x + oh/float(h-1))))
            self.log('dx={}, dy={}, sx={}, sy={}'.format(dx,dy,sx,sy))
            sx+=dx
            sy+=dy

            self.list[idx].setX(sx)
            self.list[idx].setY(sy)
        return
