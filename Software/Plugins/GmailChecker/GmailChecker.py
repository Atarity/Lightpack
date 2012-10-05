import imaplib

import BasePlugin
import PythonQt
from PythonQt.QtGui import *


class GmailChecker(BasePlugin.BasePlugin):
    def init(self):
        self.unseen = 0
        self.on = Lightpack.GetStatus()
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
        
        self.timeranim = PythonQt.QtCore.QTimer(None)
        self.timeranim.connect('timeout()', self.stopAnimation)
        
        
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
        return "0.6"

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
        self.on = Lightpack.GetStatus()
        Lightpack.Lock(self.sessionKey)
        self.i=1
        self.timer.start()
        time = int(Lightpack.GetSettingMain('GmailChecker/TimeAnim'))
        if (time == None):
            time = 0
        if time>0:
            self.timeranim.setInterval(time * 60000)
            self.timeranim.start()
        
        

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
        self.timeranim.stop()
        if (self.on == 1):
            Lightpack.SetStatus(self.sessionKey,1)
        Lightpack.UnLock(self.sessionKey)
        self.tick = 0
        
    def changeAcc(self,account):
        Lightpack.SetSettingMain('GmailChecker/Username',account)

    def changePass(self,password):
        Lightpack.SetSettingMain('GmailChecker/Password',password)
    
    def changeTimeCheck(self,value):
        if (value != ""):
            Lightpack.SetSettingMain('GmailChecker/TimeCheck',value)
            time = value
        else:
            Lightpack.SetSettingMain('GmailChecker/TimeCheck',1)
            time = 1
        self.timerCheck.setInterval(time * 60000)
        

    def changeTimeAnim(self,value):
        if (value != ""):
            Lightpack.SetSettingMain('GmailChecker/TimeAnim',value)
            time = int(value)
        else:
            Lightpack.SetSettingMain('GmailChecker/TimeAnim',0)
            time = 0
        if time>0:
            self.timeranim.setInterval(time * 60000)
            self.timeranim.start()
        else:
            self.timeranim.stop()
    
    def changeTimeBegin(self,value):
        if (value!=""):
            Lightpack.SetSettingMain('GmailChecker/TimeBegin',value)
        else:
            Lightpack.SetSettingMain('GmailChecker/TimeBegin',0)
    
    def changeTimeEnd(self,value):
        if (value!=""):
            Lightpack.SetSettingMain('GmailChecker/TimeEnd',value)
        else:
            Lightpack.SetSettingMain('GmailChecker/TimeEnd',24)
        
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
        edittime.setValidator(QIntValidator(0, 65536, edittime))
        time = Lightpack.GetSettingMain('GmailChecker/TimeCheck')
        if (time == None):
            time = 1
        edittime.setText(time)
        box.addWidget(edittime)
        
        labelanim = QLabel(SettingsBox)
        labelanim.setText("Animation period (min; 0 - no off animation)")
        box.addWidget(labelanim)
        editanim = QLineEdit(SettingsBox)
        editanim.setValidator(QIntValidator(0, 65536, editanim))
        time = Lightpack.GetSettingMain('GmailChecker/TimeAnim')
        if (time == None):
            time = 0
        editanim.setText(time)
        box.addWidget(editanim)
        
        labelbegin = QLabel(SettingsBox)
        labelbegin.setText("Begin check (hour)")
        box.addWidget(labelbegin)
        editbegin = QLineEdit(SettingsBox)
        editbegin.setValidator(QIntValidator(0, 65536, editbegin))
        time = Lightpack.GetSettingMain('GmailChecker/TimeBegin')
        if (time == None):
            time = 0
        editbegin.setText(time)
        box.addWidget(editbegin)
        
        labelend = QLabel(SettingsBox)
        labelend.setText("End check (hour)")
        box.addWidget(labelend)
        editend = QLineEdit(SettingsBox)
        editend.setValidator(QIntValidator(0, 65536, editend))
        time = Lightpack.GetSettingMain('GmailChecker/TimeEnd')
        if (time == None):
            time = 0
        editend.setText(time)
        box.addWidget(editend)
        
        self.labelunseen = QLabel(SettingsBox)
        self.labelunseen.setText("Unread messages : "+str(self.unseen))
        box.addWidget(self.labelunseen)
        pushcheck =  QPushButton(SettingsBox)
        box.addWidget(pushcheck)
        pushcheck.text = 'Check now'

        #box.addSpacing(150)

        editAcc.connect('textChanged(QString)', self.changeAcc)
        editPass.connect('textChanged(QString)', self.changePass)
        edittime.connect('textChanged(QString)', self.changeTimeCheck)
        editanim.connect('textChanged(QString)', self.changeTimeAnim)
        editbegin.connect('textChanged(QString)', self.changeTimeBegin)
        editend.connect('textChanged(QString)', self.changeTimeEnd)
        pushcheck.connect('clicked()', self.gmail_checker)
        return 0; 

    def gmail_checker(self):
            print "check"
            timebegin = int(Lightpack.GetSettingMain('GmailChecker/TimeBegin'))
            if (timebegin == None):
                timebegin = 0
            timeend = int(Lightpack.GetSettingMain('GmailChecker/TimeEnd'))
            if (timeend == None):
                timeend = 0
            import datetime
            now = datetime.datetime.now()
            hour = now.hour
            if (hour < timebegin):
                self.stopAnimation();
                return
            if (hour >= timeend):
                self.stopAnimation();
                return
            import imaplib,re
            username = Lightpack.GetSettingMain('GmailChecker/Username')
            password = Lightpack.GetSettingMain('GmailChecker/Password')
            i=imaplib.IMAP4_SSL('imap.gmail.com')
            try:
                i.login(username,password)
                x,y=i.status('INBOX','(MESSAGES UNSEEN)')
                messages=int(re.search('MESSAGES\s+(\d+)',y[0]).group(1))
                cur_unseen=int(re.search('UNSEEN\s+(\d+)',y[0]).group(1))
                print cur_unseen
                if self.unseen != cur_unseen:
                    self.unseen = cur_unseen
                    if self.unseen > 0 :	
                        print "unseen"
                        self.runAnimation()
                    else:
                        print "no mail"
                        self.stopAnimation()
                    if (self.labelunseen):   
                        self.labelunseen.setText("Unread messages : "+str(self.unseen))
            except Exception, e:
                print e
                if (self.labelunseen):    
                    self.labelunseen.setText("Unread messages : error")
                return
            
            


