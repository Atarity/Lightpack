import imaplib

import BasePlugin
import PythonQt
from PythonQt.QtGui import *


class GmailChecker(BasePlugin.BasePlugin):
    def init(self):
        self.sessionKey = Lightpack.GetSessionKey(self.__class__.__name__)
        self.timer = PythonQt.QtCore.QTimer(None)
        self.timer.setInterval(100)
        self.timer.timeout.connect(self.Timeout)
        self.timerCheck = PythonQt.QtCore.QTimer(None)
        time = int(Lightpack.GetSettingMain('GmailChecker/TimeCheck'))
        if (time == None):
            time = 1
        self.timerCheck.setInterval(time * 60000)
        self.timerCheck.connect('timeout()', self.gmail_checker)
        
        self.ledMap = [6,1,2,7,3,4,8,9,10,5]
        print "init"
    
    def dispose(self):
        self.timerCheck.setInterval(0)
        self.timerCheck.stop()
        self.timerCheck.disconnect('timeout()', self.gmail_checker)
        del self.timer
        del self.timerCheck
        print "dispose"

    def name(self):
        """ return the name of the plugin """
        return self.__class__.__name__
 
    def description(self):
        """ return a short description of the plugin """
        return "Check Google mail account"

    def author(self):
        """ return the author of the plugin """
        return "Eraser <eraser1981@gmail.com>"

    def version(self):
        """ return the version of the plugin """
        return "0.3"

    def Timeout(self):
        self.i = self.i+1
        leds = Lightpack.GetCountLeds()
        for k  in range (0, leds):
            idx = (self.i+k) % leds
            if k < 3 :
                Lightpack.SetColor(self.sessionKey,self.ledMap[idx]-1,255,0,0)
            else :
                if k<6:
                     Lightpack.SetColor(self.sessionKey,self.ledMap[idx]-1,0,0,128)
                else:
                     Lightpack.SetColor(self.sessionKey,self.ledMap[idx]-1,0,0,0)        
    
    def runAnimation(self):
        Lightpack.Lock(self.sessionKey)
        self.i=1
        self.timer.start()

    def run(self):
        self.timerCheck.start()
        print "run"
        
    def stop(self):
        self.timerCheck.stop()
        self.timer.stop()
        Lightpack.UnLock(self.sessionKey)
        print "stop"

    def stopAnimation(self):
        self.timer.stop()
        Lightpack.UnLock(self.sessionKey)
        self.tick = 0
        
    def changeAcc(self,account):
        Lightpack.SetSettingMain('GmailChecker/Username',account)

    def changePass(self,password):
        Lightpack.SetSettingMain('GmailChecker/Password',password)
    
    def changeTimeCheck(self,time):
        Lightpack.SetSettingMain('GmailChecker/TimeCheck',time)
        
    def settings(self):
        """ default function """
        box = QVBoxLayout(SettingsBox)
        label = QLabel(SettingsBox)
        label.setText("Account")
        box.addWidget(label)
        editAcc = QLineEdit(SettingsBox)
        editAcc.setText(Lightpack.GetSettingMain('GmailChecker/Username'))
        box.addWidget(editAcc)
        
        label2 = QLabel(SettingsBox)
        label2.setText("Password")
        box.addWidget(label2)
        editPass = QLineEdit(SettingsBox)
        editPass.setEchoMode(PythonQt.QtGui.QLineEdit.Password)
        editPass.setText(Lightpack.GetSettingMain('GmailChecker/Password'))
        box.addWidget(editPass)
        
        labeltime = QLabel(SettingsBox)
        labeltime.setText("Check interval (min)")
        box.addWidget(labeltime)
        edittime = QLineEdit(SettingsBox)
        time = Lightpack.GetSettingMain('GmailChecker/TimeCheck')
        if (time == None):
            time = 1
        edittime.setText(time)
        box.addWidget(edittime)
        
        
        box.addSpacing(150)

        editAcc.connect('textChanged(QString)', self.changeAcc)
        editPass.connect('textChanged(QString)', self.changePass)
        edittime.connect('textChanged(QString)', self.changeTimeCheck)
        return 0; 

    def gmail_checker(self):
            print "check"
            import imaplib,re
            username = Lightpack.GetSettingMain('GmailChecker/Username')
            password = Lightpack.GetSettingMain('GmailChecker/Password')
            i=imaplib.IMAP4_SSL('imap.gmail.com')
            try:
                    i.login(username,password)
                    x,y=i.status('INBOX','(MESSAGES UNSEEN)')
                    messages=int(re.search('MESSAGES\s+(\d+)',y[0]).group(1))
                    unseen=int(re.search('UNSEEN\s+(\d+)',y[0]).group(1))
                    if unseen > 0 :	
                        print "unseen"
                        self.runAnimation();
                    else:
                        print "no mail"
                        self.stopAnimation();
            except:
                print "error"
                return
            

