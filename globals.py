import smbus
import json
import math

def checkLimits():
	if line["target"]["coordinate"]["y"] == -1:
		line["target"]["coordinate"]["y"] = area["range"]["y"]["max"] / 2
	if line["target"]["coordinate"]["x"] == -1:
		line["target"]["coordinate"]["x"] = area["range"]["x"]["max"] / 2
	if line["target"]["coordinate"]["y"] > area["range"]["y"]["max"]:
		line["target"]["coordinate"]["y"] = area["range"]["y"]["max"]
	if line["target"]["coordinate"]["y"] < area["range"]["y"]["min"]:
		line["target"]["coordinate"]["y"] = area["range"]["y"]["min"]

	if line["target"]["coordinate"]["x"] > area["range"]["x"]["max"]:
		line["target"]["coordinate"]["x"] = area["range"]["x"]["max"]
	if line["target"]["coordinate"]["x"] < area["range"]["x"]["min"]:
		line["target"]["coordinate"]["x"] = area["range"]["x"]["min"]
	return;

def mmTOsteps(mm, voh): # umrechnung, voh -> vorne oder hinten
    if voh:
        SPULE = option["spool"]["front"]
    else:
        SPULE = option["spool"]["back"]
    x = mm / (SPULE/1000 * 3.141592 / 200) * int(option["gearRatio"]) * int(option["stepping"])
    if math.isnan(x): # Not a number
        x = 0
    x = int(x + 0.5) # weil bei float in long immer abgerundet wird
    return x;

def stepsTOmm(steps, voh): # umrechnung
    if voh:
        SPULE = option["spool"]["front"]
    else:
        SPULE = option["spool"]["back"]
    x = steps * SPULE/1000 * 3.141592 / 200 / int(option["gearRatio"]) / int(option["stepping"])
    if math.isnan(x): # Not a number
        x = 0
    x = int(x + 0.5) # weil bei float in long immer abgerundet wird
    return x;

def CoordinateTOline():  # Umrechnung von Koordinaten auf Schnurlaenge in mm (Zielwerte)
	#
	#      a       b(tiefe)
	#   +-----+---------------+
	#    \    |            /
	#     \   |h        /
	#    c \  |      / d
	#       \ |   /
	#         +
	#
	#--------------------------+    Kante Buehne = nullpunkt
	#                          |
	#   Wand   --> #
	#   Schnur --> c und d
	#   Decke  --> a und b
	w = area["dimension"]["z"] * 0.5
	h = area["dimension"]["y"] - line["target"]["coordinate"]["y"]  # Abstand vom Boden
	a = area["dimension"]["x"] - line["target"]["coordinate"]["x"]   # Abstand von der Wand
	c = math.sqrt( a*a + h*h + w*w )
	line["target"]["lengh"]["back"] = int(c + 0.5) # weil bei float in long immer abgerundet wird

	b = area["dimension"]["x"] - a
	d = math.sqrt( b*b + h*h + w*w ) # Inhalt der Wurzel
	line["target"]["lengh"]["front"] = int(d + 0.5) # weil bei float in long immer abgerundet wird

	line["target"]["steps"]["front"] = mmTOsteps(line["target"]["lengh"]["front"],1)
	line["target"]["steps"]["back"]  = mmTOsteps(line["target"]["lengh"]["back"],0)

	# 200 Schritte pro Umdrehung
	# gearRatio = 27 Getriebe (untersetzung) auf Motor
	# SPULE Durchmesser
	return;

def emergencyStop():
	bus.write_i2c_block_data(0,0,[])

def checkUnit():
	try:
		recdata = bus.read_i2c_block_data(int(unit1),14)
		check["unit1"]["front"] = recdata[0] | recdata[1] << 8 | recdata[2] << 16 | recdata[3] << 24
		check["unit1"]["back"]  = recdata[4] | recdata[5] << 8 | recdata[6] << 16 | recdata[7] << 24
		recdata = bus.read_i2c_block_data(int(unit1),12)
		option["voltage"]["unit1"] = recdata[0] | recdata[1] << 8 | recdata[2] << 16 | recdata[3] << 24

		recdata = bus.read_i2c_block_data(int(unit2),14)
		check["unit2"]["front"] = recdata[0] | recdata[1] << 8 | recdata[2] << 16 | recdata[3] << 24
		check["unit2"]["back"]  = recdata[4] | recdata[5] << 8 | recdata[6] << 16 | recdata[7] << 24
		recdata = bus.read_i2c_block_data(int(unit2),12)
		option["voltage"]["unit2"] = recdata[0] | recdata[1] << 8 | recdata[2] << 16 | recdata[3] << 24
		return True
	except:
		return False

def get_current():
	recdata = bus.read_i2c_block_data(unit1,14)
	line["current"]["steps"]["front"] = recdata[0] | recdata[1] << 8 | recdata[2] << 16 | recdata[3] << 24
	line["current"]["steps"]["back"]  = recdata[4] | recdata[5] << 8 | recdata[6] << 16 | recdata[7] << 24
	line["current"]["lengh"]["front"] = stepsTOmm(line["current"]["steps"]["front"],1)
	line["current"]["lengh"]["back"]  = stepsTOmm(line["current"]["steps"]["back"],0)
	return

def set_target():
	sendval = []
	sendval.append(int(line["target"]["steps"]["front"])       & 0xFF)
	sendval.append(int(line["target"]["steps"]["front"]) >>  8 & 0xFF)
	sendval.append(int(line["target"]["steps"]["front"]) >> 16 & 0xFF)
	sendval.append(int(line["target"]["steps"]["front"]) >> 24 & 0xFF)
	sendval.append(int(line["target"]["steps"]["back"])       & 0xFF)
	sendval.append(int(line["target"]["steps"]["back"]) >>  8 & 0xFF)
	sendval.append(int(line["target"]["steps"]["back"]) >> 16 & 0xFF)
	sendval.append(int(line["target"]["steps"]["back"]) >> 24 & 0xFF)
	bus.write_i2c_block_data(0,3,sendval)
	return

def set_position():
	sendval = []
	sendval.append(int(line["target"]["steps"]["front"])       & 0xFF)
	sendval.append(int(line["target"]["steps"]["front"]) >>  8 & 0xFF)
	sendval.append(int(line["target"]["steps"]["front"]) >> 16 & 0xFF)
	sendval.append(int(line["target"]["steps"]["front"]) >> 24 & 0xFF)
	sendval.append(int(line["target"]["steps"]["back"])       & 0xFF)
	sendval.append(int(line["target"]["steps"]["back"]) >>  8 & 0xFF)
	sendval.append(int(line["target"]["steps"]["back"]) >> 16 & 0xFF)
	sendval.append(int(line["target"]["steps"]["back"]) >> 24 & 0xFF)
	bus.write_i2c_block_data(0,13,sendval)
	return

def calibrateSingle(motor, amount):
	if motor == 1:
		address = unit1 # left
		motorNR = 1  	# front
	if motor == 2:
		address = unit1 # left
		motorNR = 2  	# back
	if motor == 3:
		address = unit2 # right
		motorNR = 1  	# front
	if motor == 4:
		address = unit2 # right
		motorNR = 2  	# back
	if motorNR == 1:
		voh = 1
	else:
		voh = 0
	print address
	print motorNR
	amount = mmTOsteps(amount, voh)
	print amount
	sendval = []
	sendval.append(motorNR)
	sendval.append(int(amount)       & 0xFF)
	sendval.append(int(amount) >>  8 & 0xFF)
	sendval.append(int(amount) >> 16 & 0xFF)
	sendval.append(int(amount) >> 24 & 0xFF)
	bus.write_i2c_block_data(address,7,sendval)
	return


def set_option():
	sendval = []
	sendval.append(int(option["stepping"]))
	bus.write_i2c_block_data(0, 8,sendval)

	sendval = []
	sendval.append(int(option["gearRatio"]))
	bus.write_i2c_block_data(0,11,sendval)

	sendval = []
	sendval.append(0)
	sendval.append(int(option["motorSpeedLimit"])       & 0xFF)
	sendval.append(int(option["motorSpeedLimit"]) >>  8 & 0xFF)
	sendval.append(int(option["motorSpeedLimit"]) >> 16 & 0xFF)
	sendval.append(int(option["motorSpeedLimit"]) >> 24 & 0xFF)
	bus.write_i2c_block_data(0,9,sendval)
	return

def invertMotor(motor):
	if motor == 1:
		address = 11 # left
		motorNR = 1  # front
	if motor == 2:
		address = 11 # left
		motorNR = 2  # back
	if motor == 3:
		address = 12 # right
		motorNR = 1  # front
	if motor == 4:
		address = 12 # right
		motorNR = 2  # back
	sendval = []
	sendval.append(motorNR)
	bus.write_i2c_block_data(address,5,sendval)
	return

def init():
	#I2C
	global unit1, unit2, bus
	unit1 = 0x0b
	unit2 = 0x0c
	bus = smbus.SMBus(1)

	global line, option, area, memory

	global check
	check = {"unit1":{}, "unit2":{}}

	try:
		optionF = open("option.json","r")
		option = json.loads(optionF.read())
		optionF.close()
	except:
		option = {"spool": {"front": 30000, "back": 150000}, "motorSpeedLimit": 800, "gearRatio": 27, "stepping": 1, "voltage":{}}

	try:
		areaF = open("area.json","r")
		area  = json.loads(areaF.read())
		areaF.close()
	except:
		area = {"range": {"y": {"max": 4000, "min": 1500}, "x": {"max": 6000, "min": 1500}}, "dimension": {"y": 5800, "x": 7300, "z": 4600}}

	try:
		memoryF = open("memory.json","r")
		memory  = json.loads(memoryF.read())
		memoryF.close()
	except:
		memory = {"1": {"y": 2201, "x": 5500}, "3": {"y": 2200, "x": 5500}, "2": {"y": 2200, "x": 5500}, "5": {"y": 2200, "x": 5500}, "4": {"y": 2200, "x": 5500}, "7": {"y": 2200, "x": 5500}, "6": {"y": 2200, "x": 5500}, "9": {"y": 2200, "x": 5500}, "8": {"y": 2200, "x": 5500}, "100": {"y": 3200, "x": 5500}}

	line = {"current":{"lengh":{"front":0,"back":0},"steps":{"front":0, "back":0}},"target":{"lengh":{"front":0,"back":0},"steps":{"front":0, "back":0},"coordinate":{"x":0,"y":0}},"speed":{"lengh":{"front":0,"back":0}}}
