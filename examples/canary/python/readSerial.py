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
		payload = byte[2:-1]
		length =  len(payload)
		print length
		if str(length) == str(byte[0] + byte[1]):
			print payload
			message.ParseFromString(payload)
			milli = (int(round(time.time()*1000)))
			db.insert(message.id, milli, "Canary", payload)
