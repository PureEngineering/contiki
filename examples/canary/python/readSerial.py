import serial
import sensor_message_pb
import base64

message = sensor_message_pb.sensors()

arr = ""

baudRate = 115200
serialPort = '/dev/ttyUSB0'

ser = serial.Serial(port = serialPort, baudrate = baudRate, timeout = 1)
while ser.is_open:
	byte = ser.readline()
	if byte == '':
		print 'No Data'
	#elif byte == '\n':
		#print arr
		#arr = ""
	else:
		#arr += byte
		print byte
