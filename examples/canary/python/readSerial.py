import serial
import sensor_message_pb
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
	if byte == '':
		print 'No Data'
	else:
		payload = byte[1:-1]
		length =  len(payload)
		lenBytes = ord(byte[0])
		print length
		if length == lenBytes: #46 or length == 47 or length == 48:
			print payload
			message.ParseFromString(payload)
			milli = (int(round(time.time()*1000)))
			db.insert(message.id, milli, "Canary", payload)
