#!/usr/bin/python
from BaseHTTPServer import BaseHTTPRequestHandler,HTTPServer
from SocketServer import ThreadingMixIn
import threading
from time import sleep,time
import json
import globals
import os
import subprocess

PORT_NUMBER = 82
globals.init()
lastCall  = time()
lastCheck = time()
checked   = False

# This class allows to handle requests in separated threads. No further content needed.
class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
	pass


#This class will handles any incoming request from the browser
class myHandler(BaseHTTPRequestHandler):

	#Handler for the POST requests
	def do_POST(self):
		content_length = int(self.headers['Content-Length'])
		post_data = self.rfile.read(content_length)

		if self.path ==  "/target" or self.path ==  "/targetAdmin":
			try:
				del globals.option["loaded"]
			except:
				pass
			globals.line["target"]["coordinate"] = json.loads(post_data)
			if self.path !=  "/targetAdmin":
				globals.checkLimits()
			globals.CoordinateTOline()
			globals.set_target()
			self.send_response(200)
			self.end_headers()

		elif self.path ==  "/load":
			globals.option["loaded"] = json.loads(post_data)
			tempSlot = str(globals.option["loaded"]["slot"])
			globals.line["target"]["coordinate"]["x"] = globals.memory[tempSlot]["x"]
			globals.line["target"]["coordinate"]["y"] = globals.memory[tempSlot]["y"]
			globals.checkLimits()
			globals.CoordinateTOline()
			globals.set_target()
			self.send_response(200)
			self.end_headers()

		elif self.path ==  "/calibrate":
			print "calibrate"
			globals.line["target"]["coordinate"] = json.loads(post_data)
			globals.checkLimits()
			globals.CoordinateTOline()
			globals.set_position()
			self.send_response(200)
			self.end_headers()

		elif self.path ==  "/calibrateSingle":
			print "calibrate single"
			globals.line["target"]["steps"] = globals.line["current"]["steps"]
			calibrateSingleData = json.loads(post_data)
			globals.calibrateSingle(calibrateSingleData["motor"], calibrateSingleData["amount"])
			globals.set_target()
			self.send_response(200)
			self.end_headers()

		elif self.path ==  "/emergencyStop":
			globals.emergencyStop()
			self.send_response(200)
			self.end_headers()

		elif self.path ==  "/option":
			if globals.line["target"]["lengh"]["front"] != globals.line["current"]["lengh"]["front"] or globals.line["target"]["lengh"]["back"] != globals.line["current"]["lengh"]["back"] or globals.line["target"]["coordinate"]["x"] == 0 or globals.line["target"]["coordinate"]["y"] == 0:
				self.send_response(423)
				self.end_headers()
				self.wfile.write("send a target before calibrating. wait until motor stops before submitting")
			else:
				tempSpoolFront = globals.option["spool"]["front"]
				tempSpoolBack  = globals.option["spool"]["back"]
				globals.option = json.loads(post_data)
				globals.set_option()
				globals.CoordinateTOline()
				globals.set_position()
				optionF = open("option.json","w")
				optionF.write(json.dumps(globals.option,indent=4, sort_keys=True))
				optionF.close()
				self.send_response(200)
				self.end_headers()

		elif self.path ==  "/area":
			globals.area = json.loads(post_data)
			areaF = open("area.json","w")
			areaF.write(json.dumps(globals.area,indent=4, sort_keys=True))
			areaF.close()
			self.send_response(200)
			self.end_headers()

		elif self.path ==  "/writeMemory":
			globals.memory.update(json.loads(post_data))
			memoryF = open("memory.json","w")
			memoryF.write(json.dumps(globals.memory,indent=4, sort_keys=True))
			memoryF.close()
			self.send_response(200)
			self.end_headers()

		elif self.path ==  "/deleteSlot":
			del globals.memory[str(json.loads(post_data)["slot"])]
			memoryF = open("memory.json","w")
			memoryF.write(json.dumps(globals.memory,indent=4, sort_keys=True))
			memoryF.close()
			self.send_response(200)
			self.end_headers()

		else:
			self.send_response(404)
			self.end_headers()
			self.wfile.write("not a function")
			return

	#Handler for the GET requests
	def do_GET(self):
		global lastCall, lastCheck, checked
		if checked == None:
			checked = False

		if checked == False:
			checked = globals.checkUnit()
			lastCheck = time()

		if time() - lastCheck > 3: #prevent too much i2c
			checked = globals.checkUnit()
			lastCheck = time()

		if checked == False:
			self.send_response(200)
			self.send_header('Content-type','application/json') # intentionally producing an error in jquery
			self.end_headers()
			self.wfile.write("i2c error")
			print "i2c error"

		if globals.option["voltage"]["unit1"] < 8000000:
			checked = False
			globals.emergencyStop()
			self.send_response(200)
			self.send_header('Content-type','application/json') # intentionally producing an error in jquery
			self.end_headers()
			self.wfile.write("power loss unit 1")
			print "power loss unit 1"

		if globals.option["voltage"]["unit2"] < 8000000:
			checked = False
			globals.emergencyStop()
			self.send_response(200)
			self.send_header('Content-type','application/json') # intentionally producing an error in jquery
			self.end_headers()
			self.wfile.write("power loss unit 2")
			print "power loss unit 2"

		if checked == True:
			self.send_response(200)
			self.send_header('Content-type','application/json')
			self.end_headers()
			# Send the html message
			if self.path ==  "/line":
				if time() - lastCall > 0.8: #prevent too much i2c
					globals.get_current()
					lastCall = time()
				self.wfile.write(json.dumps(globals.line))

			elif self.path ==  "/area":
				self.wfile.write(json.dumps(globals.area))

			elif self.path ==  "/option":
				self.wfile.write(json.dumps(globals.option))

			elif self.path ==  "/i2cScan":
				op = os.popen("/usr/sbin/i2cdetect -y 1").read()
				self.wfile.write(op)

			elif self.path ==  "/kill":
				self.wfile.write("kill proces...")
				server.socket.close()
				os.system("/usr/bin/killall autoswitch.py")
				os.system("/usr/bin/killall webserver.py")
				os.system("/usr/bin/killall cmmm.py")

			elif self.path ==  "/reboot":
				self.wfile.write("rebooting...")
				server.socket.close()
				os.system("/sbin/shutdown -r now")

			elif self.path ==  "/shutdown":
				self.wfile.write("shutting down...")
				server.socket.close()
				os.system("/sbin/shutdown -h now")

			elif self.path ==  "/invert1":
				globals.invertMotor(1)
				self.wfile.write("{}")
			elif self.path ==  "/invert2":
				globals.invertMotor(2)
				self.wfile.write("{}")
			elif self.path ==  "/invert3":
				globals.invertMotor(3)
				self.wfile.write("{}")
			elif self.path ==  "/invert4":
				globals.invertMotor(4)
				self.wfile.write("{}")

			elif self.path ==  "/readMemory":
				self.wfile.write(json.dumps(globals.memory))

			elif self.path ==  "/all":
				if time() - lastCall > 0.8: #prevent too much i2c
					globals.get_current()
					lastCall = time()
				tempAll = {"line":{}, "option":{}, "area":{}}
				tempAll["line"]   = globals.line
				tempAll["option"] = globals.option
				tempAll["area"]   = globals.area
				self.wfile.write(json.dumps(tempAll))
			else:
				self.wfile.write("not a function")
				return

try:
	server = ThreadedHTTPServer(('', PORT_NUMBER), myHandler)
	print 'Started cmmm service on port ' , PORT_NUMBER
	server.serve_forever()

except KeyboardInterrupt:
	print '^C received, shutting down the web server'
	server.socket.close()

finally:
	server.socket.close()
