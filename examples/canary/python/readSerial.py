import serial
import sensor_message_pb
from database import Database
import sys
import time
#import thread
import numpy as np
import matplotlib.pyplot as plt
import math
import matplotlib.animation as animation

myCount = []
mag = []
fig = plt.figure()
ax = fig.add_subplot(1,1,1)
#ln, = plt.plot([], [], 'ro', animated=True)
#ax.set_xlim(0, 1000)
#ax.set_ylim(0, 20000)

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
					mag.append(math.sqrt(math.pow(message.lis3mdl_x, 2) + math.pow(message.lis3mdl_y, 2) + math.pow(message.lis3mdl_z, 2)))
					myCount.append(len(myCount)+1)
					break

def animate(i, db, message, ser):
	readSerial(db, message, ser, 1)
	ax.clear()
	ax.plot(myCount, mag)
	#ln.set_data(myCount, mag)
	#return ln,

def main():
	live = 0
	if(len(sys.argv) == 1):
		print "Please Enter Database Name"
		return
	elif(len(sys.argv) == 3):
		live = 1
	
	db = Database("newuser", "secretpassword", str(sys.argv[1]))
	message = sensor_message_pb.sensors()
	try:
		baudRate = 115200
		serialPort = '/dev/ttyUSB0'
		ser = serial.Serial(port = serialPort, baudrate = baudRate, timeout = 1)
	except:
		print "Error: could not open serial port"
		return

	if(live == 0):
		readSerial(db, message, ser, live)
	else:
		ani =  animation.FuncAnimation(fig, animate, fargs = (db, message, ser), interval = 1)
		plt.show()
	#readSerial()
	#try:
		#thread.start_new_thread(readSerial, (ser,))
	#except:
		#print "Error: unable to start thread"
	
	#while 1:
		#pass

if __name__ == "__main__":
        main()
