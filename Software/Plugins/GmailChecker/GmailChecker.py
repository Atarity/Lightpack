import imaplib

import BasePlugin
import PythonQt
from PythonQt.QtGui import *


class GmailChecker(BasePlugin.BasePlugin):
    def init(self):
        self.unseen = 0
        self.reminder = False
        self.ledMap = self.defaultMap()
        self.setMap()
        self.on = Lightpack.GetStatus()
        self.sessionKey = Lightpack.GetSessionKey(self.__class__.__name__)
        self.timer = PythonQt.QtCore.QTimer(None)
        self.timer.setInterval(100)
        self.timer.timeout.connect(self.Timeout)
        self.timerCheck = PythonQt.QtCore.QTimer(None)
        time = Lightpack.GetSettingMain('GmailChecker/TimeCheck')
        if (time == None):
            time = 1
        else:
            time = int(time)
        self.timerCheck.setInterval(time * 60000)
        self.timerCheck.connect('timeout()', self.gmail_checker)
        self.timeranim = PythonQt.QtCore.QTimer(None)
        self.timeranim.connect('timeout()', self.stopAnimation)
        self.timerrem = PythonQt.QtCore.QTimer(None)
        self.timerrem.connect('timeout()', self.reminderProc)
        self.log("init")
    
    def dispose(self):
        self.timerCheck.setInterval(0)
        self.timerCheck.stop()
        self.timerrem.stop()
        self.timeranim.stop()
        self.timerCheck.disconnect('timeout()', self.gmail_checker)
        del self.timer
        del self.timeranim
        del self.timerCheck
        del self.timerrem
        self.log("dispose")

    def name(self):
        """ return the name of the plugin """
        return self.__class__.__name__
 
    def description(self):
        """ return a short description of the plugin """
        return "This plugin will check your Google account for new mail. If there are unseen messages it run Lightpack animation to notice you.<br /><br /> IT'S STRICKTLY RECOMMENDED TO USE YOUR <b>APPLICATION PASSWORD</b> INSTEAD OF YOUR MAIN GMAIL PASSWORD FOR AUTHORIZATION!<br /><br /> You can generate app password in your Google account settings (account &rarr; security &rarr; authorizing applications and sites). This password will be stored at <b>main.conf</b> file in Lightpack profile folder <b>as plain text</b>.<br /><br /> You can use LEDs map property to adjust your LEDs sequense for properly animation."

    def author(self):
        """ return the author of the plugin """
        return "Eraser <eraser1981@gmail.com>"

    def version(self):
        """ return the version of the plugin """
        return "0.9"

    def reminderProc(self):
        self.log("reminder")
        self.reminder = True
        
        
    def Timeout(self):
        try:
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
        except Exception, e:
            self.log(str(e))
    
    def runAnimation(self):
        try:
            if self.timer.isActive():
                return
            self.on = Lightpack.GetStatus()
            Lightpack.Lock(self.sessionKey)
            Lightpack.SetStatus(self.sessionKey,1)
            self.i=1
            self.timer.start()
            self.timerrem.stop()
            time = Lightpack.GetSettingMain('GmailChecker/TimeAnim')
            if (time == None):
                time = 0
            else:
                time = int(time)
            if time>0:
                self.timeranim.setInterval(time * 60000)
                self.timeranim.start()
            
        except Exception, e:
            self.log(str(e))
        

    def run(self):
        self.timerCheck.start()
        self.log("run")
        
    def stop(self):
        self.timerCheck.stop()
        self.timer.stop()
        self.timeranim.stop()
        self.timerrem.stop()
        Lightpack.UnLock(self.sessionKey)
        self.log("stop")

    def stopAnimation(self):
        if self.timer.isActive() == False:
            return
        self.timer.stop()
        self.timeranim.stop()
        time = Lightpack.GetSettingMain('GmailChecker/TimeReminder')
        if (time == None):
            time = 0
        else:
            time = int(time)
        if time>0:
            self.timerrem.setInterval(time * 60000)
            self.timerrem.start()
        Lightpack.SetStatus(self.sessionKey,self.on)
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
    
    def changeTimeRem(self,value):
        if (value != ""):
            Lightpack.SetSettingMain('GmailChecker/TimeReminder',value)
        else:
            Lightpack.SetSettingMain('GmailChecker/TimeReminder',0)
    
    
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

    def changeMap(self,value):
        Lightpack.SetSettingMain('GmailChecker/LedsMap',value)
        self.setMap()

    def defaultMap(self):
        try:
            leds = Lightpack.GetCountLeds()
            map = [int(n+1) for n in range (0, leds)]
        except Exception, e:
            self.log(str(e))
            map = [1,2,3,4,5,6,7,8,9,10]
        return map
    
    def mapToStr(self,map):
        return ','.join(str(x) for x in map) 
     
    def setMap(self):
        try:
            map = Lightpack.GetSettingMain('GmailChecker/LedsMap')
            self.ledMap = [int(n) for n in map.split(',')]
            self.log(str(self.ledMap))
        except Exception, e:
            self.log(str(e))
            self.ledMap = self.defaultMap()     
            
    def settings(self,parent):
        """ default function """
        box = QGridLayout(parent)
        #box.setSpacing(5)        
        box.setColumnStretch(0, 2)        
        box.setColumnStretch(1, 1)
        box.setColumnStretch(2, 2)
        box.setColumnStretch(3, 1) # empty column
        label = QLabel(parent)
        label.setText("Account")
        box.addWidget(label,1,0)
        editAcc = QLineEdit(parent)
        editAcc.setText(Lightpack.GetSettingMain('GmailChecker/Username'))
        editAcc.setPlaceholderText('evil.genius')
        box.addWidget(editAcc,1,1,1,2)
        
        label2 = QLabel(parent)
        label2.setText("Password")
        box.addWidget(label2,2,0)
        editPass = QLineEdit(parent)
        editPass.setEchoMode(PythonQt.QtGui.QLineEdit.Password)
        editPass.setText(Lightpack.GetSettingMain('GmailChecker/Password'))
        editPass.setPlaceholderText('application password only')        
        #editPass.setFixedWidth(200) #we can use this for layout if needed
        box.addWidget(editPass,2,1,1,2)
        
        labeltime = QLabel(parent)
        labeltime.setText("Check every")
        box.addWidget(labeltime,3,0)
        labeltime2 = QLabel(parent)
        labeltime2.setText("min")
        box.addWidget(labeltime2,3,2)
        edittime = QLineEdit(parent)
        edittime.setValidator(QIntValidator(0, 65536, edittime))
        time = Lightpack.GetSettingMain('GmailChecker/TimeCheck')
        if (time == None):
            time = 1
            self.changeTimeCheck(1)
        edittime.setText(time)
        box.addWidget(edittime,3,1)
        
        labelanim = QLabel(parent)
        labelanim.setText("Run animation for")
        box.addWidget(labelanim,4,0)
        labelanim2 = QLabel(parent)
        labelanim2.setText("min (0 — no animation)")
        box.addWidget(labelanim2,4,2)
        editanim = QLineEdit(parent)
        editanim.setValidator(QIntValidator(0, 65536, editanim))
        time = Lightpack.GetSettingMain('GmailChecker/TimeAnim')
        if (time == None):
            time = 0
            self.changeTimeAnim(0)
        editanim.setText(time)
        box.addWidget(editanim,4,1)
        
        labelrem = QLabel(parent)
        labelrem.setText("Remind me every")
        box.addWidget(labelrem,5,0)
        labelrem2 = QLabel(parent)
        labelrem2.setText("min (0 — do not remind me)")
        box.addWidget(labelrem2,5,2)
        editrem = QLineEdit(parent)
        editrem.setValidator(QIntValidator(0, 65536, editrem))
        time = Lightpack.GetSettingMain('GmailChecker/TimeReminder')
        if (time == None):
            time = 0
            self.changeTimeRem(0)
        editrem.setText(time)
        box.addWidget(editrem,5,1)
        
        labelbegin = QLabel(parent)
        labelbegin.setText("Start checking at")
        box.addWidget(labelbegin,6,0)
        labelbegin2 = QLabel(parent)
        labelbegin2.setText("hour")
        box.addWidget(labelbegin2,6,2)
        editbegin = QLineEdit(parent)
        editbegin.setValidator(QIntValidator(0, 24, editbegin))
        editbegin.setPlaceholderText('0')
        time = Lightpack.GetSettingMain('GmailChecker/TimeBegin')
        if (time == None):
            time = 0
            self.changeTimeBegin(0)
        editbegin.setText(time)
        box.addWidget(editbegin,6,1)
        
        labelend = QLabel(parent)
        labelend.setText("End checking at")
        box.addWidget(labelend,7,0)
        labelend2 = QLabel(parent)
        labelend2.setText("hour")
        box.addWidget(labelend2,7,2)
        editend = QLineEdit(parent)
        editend.setValidator(QIntValidator(0, 24, editend))
        editend.setPlaceholderText('24')
        time = Lightpack.GetSettingMain('GmailChecker/TimeEnd')
        if (time == None):
            time = 0
            self.changeTimeEnd(24)
        editend.setText(time)
        box.addWidget(editend,7,1)
        
        labemap = QLabel(parent)
        labemap.setText("LEDs sequence map")
        box.addWidget(labemap,8,0)
        editmap = QLineEdit(parent)
        map = Lightpack.GetSettingMain('GmailChecker/LedsMap')
        if (map == None):
            map = self.mapToStr(self.defaultMap())
            self.changeMap(map)
        editmap.setText(map)
        editmap.setPlaceholderText('1,2,3,4,5,6,7,8,9,10')
        box.addWidget(editmap,8,1,1,2)
        
        spacerA = QWidget(parent)
        spacerA.setFixedSize(20,20)
        #spacerA.setSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        box.addWidget(spacerA,9,0)
        
        self.labelunseen = QLabel(parent)
        self.labelunseen.setText("Unread messages: "+str(self.unseen))
        box.addWidget(self.labelunseen,10,0,1,2)
        pushcheck =  QPushButton(parent)
        pushcheck.text = 'Check now'
        box.addWidget(pushcheck,10,2)
        
        spacerB = QWidget(parent)
        spacerB.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        box.addWidget(spacerB,11,0)
        
        
        editAcc.connect('textChanged(QString)', self.changeAcc)
        editPass.connect('textChanged(QString)', self.changePass)
        edittime.connect('textChanged(QString)', self.changeTimeCheck)
        editanim.connect('textChanged(QString)', self.changeTimeAnim)
        editrem.connect('textChanged(QString)', self.changeTimeRem)
        editbegin.connect('textChanged(QString)', self.changeTimeBegin)
        editend.connect('textChanged(QString)', self.changeTimeEnd)
        editmap.connect('textChanged(QString)', self.changeMap)
        pushcheck.connect('clicked()', self.gmail_checker)
        return 0; 

    def gmail_checker(self):
        self.log("check")
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
            self.log(str(cur_unseen))
            if ((self.unseen != cur_unseen) or (self.reminder == True)):
                self.unseen = cur_unseen
                self.reminder = False
                if self.unseen > 0 :
                    self.log("unseen")
                    self.runAnimation()
                else:
                    self.log("no mail")
                    self.stopAnimation()
                if (self.labelunseen):   
                    self.labelunseen.setText("Unread messages : "+str(self.unseen))
        except Exception, e:
            self.log(str(e))
            if (self.labelunseen):    
                self.labelunseen.setText("Unread messages : error")
            return
            
            


