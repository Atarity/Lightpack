import socket, time, imaplib, re, msvcrt, win32api, win32con, win32gui

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

HOST = '127.0.0.1'    # The remote host
PORT = 3636              # The same port as used by the server
leds = [1,2,5,6,7,8,9,3,4,0] 	#mapped LEDs

while True:
	messages,unseen = gmail_checker('username@gmail.com','password')
	print "%i messages, %i unseen" % (messages,unseen)	
	if unseen>0 :		
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		s.connect((HOST, PORT))
		#time.sleep(3)
		s.send('getstatusapi'+'\n')
		s.send('setstatus: on'+'\n')
		s.send('lock'+'\n')
		#turn off all LEDs
		for i  in range (0, 10) :	
			s.send('setcolor:' + str(leds[i]) + '-0,0,0'+'\n')
		i = 0;
		while 1 :
			i = i+1
			for k  in range (0, 10) :	
				idx = (i+k) % 10
				if k < 4 :
					cma = 'setcolor:' + str(leds[idx]) + '-255,0,0'+'\n'
				else :
					cma = 'setcolor:' + str(leds[idx]) + '-0,0,125'+'\n'
		#		print repr(cma)
				s.send(cma)	
				
			time.sleep(0.15)
				
		s.send('unlock'+'\n')
		data = s.recv(4096)
		s.close()
		print 'Received:', repr(data)
	time.sleep(30);