import serial
import sensor_message_pb
import base64
from database import Database
import sys
import time

db = Database("newuser", "secretpassword", str(sys.argv[1]))

message = sensor_message_pb.sensors()

arr = ""

baudRate = 115200
serialPort = '/dev/ttyUSB0'

ser = serial.Serial(port = serialPort, baudrate = baudRate, timeout = 1)
while ser.is_open:
	byte = ser.readline()
	#byte = ser.read()
	if byte == '':
		print 'No Data'
	#elif byte == '\n':
		#arr = base64.b64encode(arr)
		#message.ParseFromString(arr)
		#print arr
		#arr = ""
	else:
		#arr += byte
		length =  len(byte[2:-1])
		#print int(byte[:-2])
		#print ("len %d", length)
		#print byte[0] + byte[1]
		print length
		if str(length) == str(byte[0] + byte[1]):
			#print "HI"
			#print length
			print byte[2:-1]
			message.ParseFromString(byte[2:-1])
			milli = (int(round(time.time()*1000)))
			db.insert(message.id, milli, "Canary", byte[2:-1])
