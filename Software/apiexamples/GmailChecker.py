# Gmail checker script. It check you mail thru IMAP and run clockwise "snake" effect if you have unread messages in the box

import lightpack, time, imaplib, re, getpass

def gmail_checker(username,password):
        import imaplib,re
        i=imaplib.IMAP4_SSL('imap.gmail.com')
        try:
                i.login(username,password)
                x,y=i.status('INBOX','(MESSAGES UNSEEN)')
                messages=int(re.search('MESSAGES\s+(\d+)',y[0]).group(1))
                unseen=int(re.search('UNSEEN\s+(\d+)',y[0]).group(1))
                return (messages,unseen)
        except:
                return False,0

lpack = lightpack.lightpack('127.0.0.1', 3636, [2,3,6,7,8,9,10,4,5,1] )
lpack.connect()
mail = raw_input('Gmail: ')
passwd = getpass.getpass('password: ')
while True :
	messages,unseen = gmail_checker(mail,passwd)
	print "%i messages, %i unseen" % (messages,unseen)	+ ' @ ' + time.ctime()
	if unseen > 0 :		
		lpack.lock()
		lpack.setColorToAll(0,0,0)
		time.sleep(2)
		lpack.setSmooth(20)		
		i = 1;
		while i < 80 :
			i = i+1
			for k  in range (0, 10) :	
				idx = (i+k) % 10
				if k < 4 :
					lpack.setColor(idx,255,0,0)					
				else :
					lpack.setColor(idx,0,0,125)												
			time.sleep(0.15)						
		lpack.unlock()				
	time.sleep(30);