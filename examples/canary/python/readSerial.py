import serial
import sensor_message_pb
from database import Database
import sys
import time
#import thread

def readSerial(db, message, ser, one):
	while ser.is_open:
		byte = ser.readline()
		if byte == '':
			print 'No Data'
		else:
			payload = byte[1:-1]
			length =  len(payload)
			lenBytes = ord(byte[0])
			print length
			if length > 0 and length == lenBytes:
				print payload
				message.ParseFromString(payload)
				milli = (int(round(time.time()*1000)))
				db.insert(message.id, milli, "Canary", payload)
				if (one):
					break

def main():
	if(len(sys.argv) == 1):
		print "Please Enter Database Name"
		return
	db = Database("newuser", "secretpassword", str(sys.argv[1]))
	message = sensor_message_pb.sensors()
	try:
		baudRate = 115200
		serialPort = '/dev/ttyUSB0'
		ser = serial.Serial(port = serialPort, baudrate = baudRate, timeout = 1)
	except:
		print "Error: could not open serial port"
		return

	readSerial(db, message, ser, 0)
	#readSerial()
	#try:
		#thread.start_new_thread(readSerial, (ser,))
	#except:
		#print "Error: unable to start thread"
	
	#while 1:
		#pass

if __name__ == "__main__":
        main()
