import serial
import matplotlib.pyplot as plt

ser = serial.Serial(port = '/dev/ttyUSB0', baudrate = 115200, timeout = 1)

totalCount = 0
count = 0
mag_x = []
mag_y = []
mag_z = []

while ser.is_open:
	byte = ser.readline()
	if byte == '':
		print 'No Data'
	else:	
		print count % 3
		print byte[:-1]
		if count % 3 == 0:
			mag_x.append(byte[:-1])
		elif count % 3 == 1:
			mag_y.append(byte[:-1])
		elif count % 3 == 2:
			mag_z.append(byte[:-1])
			#count = 0
		count += 1
		if count > 902:
			break

print len(mag_x)
print len(mag_y)
print len(mag_z)
print mag_x[1:]
print mag_y[1:]
print mag_z[1:]

myCount = []

for i in range(300):
	myCount.append(i)

plt.subplot(111)
plt.plot(myCount, mag_x[1:])
plt.show()
