from database import Database
import sys
import sensor_message_pb
import numpy as np
import matplotlib.pyplot as plt
import math

light = []
accel = []
mag = []
bme = []
hum = []
batmon = []
temp = []
bmeTemp = []
magX = []
magY = []
magZ = []

count = 0
myCount = []
def decode(data):
        message = sensor_message_pb.sensors()
        message.ParseFromString(str(data))
	light.append(message.opt_3001)
	accel.append((message.LIS2DE12_x + message.LIS2DE12_y + message.LIS2DE12_z)/3)
	mag.append(math.sqrt(math.pow(message.lis3mdl_x, 2) + math.pow(message.lis3mdl_y, 2) + math.pow(message.lis3mdl_z, 2)))
	magX.append(message.lis3mdl_x)
	magY.append(message.lis3mdl_y)
	magZ.append(message.lis3mdl_z)	
	bme.append(message.bme_280_pres)
	hum.append(message.bme_280_hum)
	batmon.append(message.batmon_volt)
	temp.append(message.batmon_temp)
	bmeTemp.append(message.bme_280_temp)

def subtractAvg(magField):
	avg = 0
	result = []
	for m in magField:
		avg += m
	avg /= len(magField)
	for m in magField:
		result.append(m-avg)
	return result

def main():
	countCool = 0
        db = Database("newuser", "secretpassword", str(sys.argv[1]))
        sensorData = db.read()
	timeList = []
        for d in sensorData:
		id = d[0]
		time = d[1]
		type = d[2]
		data = d[3]
		decode(data)
		timeList.append(time)
		countCool += 1
		myCount.append(countCool)
	#plt.figure(figsize = (10, 5))
	plt.title('Sensor Data', fontsize = 12)
	plt.xlabel('Time', fontsize = 10)
	plt.ylabel('Data', fontsize = 10)
	
	#plt.plot(timeList, light)
	#plt.plot(timeList, accel)
	#plt.plot(timeList, temp)
	#plt.plot(timeList, batmon)
	magX1 = subtractAvg(magX)
	magY1 = subtractAvg(magY)
	magZ1 = subtractAvg(magZ)
	mag2 = []
	for i in range(countCool):
		mag2.append(math.sqrt(math.pow(magX1[i], 2) + math.pow(magY1[i], 2) + math.pow(magZ1[i], 2)))
		#mag2.append((magX1[i] + magY1[i] + magZ1[i]) / 3)
	
	plot1 = plt.subplot(2, 2, 1)
	plot1.set_title('Magnetometer')
	plt.plot(myCount, mag2, 'k')
	plot2 = plt.subplot(2, 2, 2)
	plot2.set_title('Magnetometer X')
	plt.plot(myCount, magX1, 'b')
	plot3 = plt.subplot(2, 2, 3)
	plot3.set_title('Magnetometer Y')
	plt.plot(myCount, magY1, 'g')
	plot4 = plt.subplot(2, 2, 4)
	plot4 = plot4.set_title('Magnetometer Z')
	plt.plot(myCount, magZ1, 'r')
	#plt.plot(timeList, bme)
	#plt.plot(timeList, bmeTemp)
	#plt.legend(['Magnetometer', 'Magnetometer X', 'Magnetomer Y', 'Magnetometer Z'], loc = 0);
	#plt.legend(['Light', 'Accelerometer', 'Temperature', 'Battery Monitor', 'Magnetometer', 'Biometric Pressure', "Biometric Temp"], loc = 0)
	plt.show()

if __name__ == "__main__":
        main()
