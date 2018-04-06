#!/usr/bin/python
import RPi.GPIO as GPIO
from time import sleep
import os

url = "http://127.0.0.1:82/load"
previous = True # purposely won't do anything at startup when console is off or no wire connected

GPIO.setmode(GPIO.BCM)     # set up BCM GPIO numbering
GPIO.setup(4, GPIO.IN, pull_up_down=GPIO.PUD_UP)

try:
    while True:
		if GPIO.input(4) == 1 and previous == False:
			print "Console turned off"
			previous = True
			os.system('curl -d \'{"slot":100, "name":"Parkposition"}\' -X POST '+url)
		elif GPIO.input(4) == 0 and previous == True:
			print "Console turned on"
			previous = False
			os.system('curl -d \'{"slot":1, "name":"Standart"}\' -X POST '+url)
		sleep(0.1)
		
finally:
	GPIO.cleanup()
