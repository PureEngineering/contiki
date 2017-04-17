from database import Database
import sys
import sensor_message_pb
import numpy as np
import matplotlib.pyplot as plt

light = []
accel = []
mag = []
bme = []
hum = []
batmon = []
temp = []
bmeTemp = []

def decode(data):
        message = sensor_message_pb.sensors()
        message.ParseFromString(str(data))
	light.append(message.opt_3001)
	accel.append((message.LIS2DE12_x + message.LIS2DE12_y + message.LIS2DE12_z)/3)
	mag.append((message.lis3mdl_x + message.lis3mdl_y + message.lis3mdl_z) / 3)
	bme.append(message.bme_280_pres)
	hum.append(message.bme_280_hum)
	batmon.append(message.batmon_volt)
	temp.append(message.batmon_temp)
	bmeTemp.append(message.bme_280_temp)
        #print("id = " + str(message.id))
        #print("batmon_temp = " + str(message.batmon_temp))
        #print("batmon_volt = " + str(message.batmon_volt))
        #print("opt_3001 = " + str(message.opt_3001))
        #print("bme_280_pres = " + str(message.bme_280_pres))
        #print("bme_280_temp = " + str(message.bme_280_temp))
        #print("bme_280_hum  = " + str(message.bme_280_hum))
        #print("LIS2DE12_x = " + str(message.LIS2DE12_x))
        #print("LIS2DE12_y = " + str(message.LIS2DE12_y))
        #print("LIS2DE12_z = " + str(message.LIS2DE12_z))
        #print("lis3mdl_x = " + str(message.lis3mdl_x))
        #print("lis3mdl_y = " + str(message.lis3mdl_y))
        #print("lis3mdl_z = " + str(message.lis3mdl_z))
        #print("pir = " + str(message.pir))
        #print("\n")

def main():
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
	plt.figure(figsize = (10, 5))
	plt.title('Sensor Data', fontsize = 12)
	plt.xlabel('Time', fontsize = 10)
	plt.ylabel('Data', fontsize = 10)
	
	plt.plot(timeList, light)
	plt.plot(timeList, accel)
	plt.plot(timeList, temp)
	plt.plot(timeList, batmon)
	plt.plot(timeList, mag)
	plt.plot(timeList, bme)
	plt.plot(timeList, bmeTemp)
	plt.legend(['Light', 'Accelerometer', 'Temperature', 'Battery Monitor', 'Magnetometer', 'Biometric Pressure', "Biometric Temp"], loc = 0)
	plt.show()

if __name__ == "__main__":
        main()
