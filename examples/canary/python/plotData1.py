from database import Database
import sys
import sensor_message_pb
import numpy as np
import matplotlib.pyplot as plt
import math
import matplotlib.animation as animation

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

db = Database("newuser", "secretpassword", str(sys.argv[1]))

fig = plt.figure()
ax1 = fig.add_subplot(1,1,1)
li, = ax1.plot(myCount, mag, 'k')
#ax2 = fig.add_subplot(2,2,2)
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

countCool = 0
c = 0
def updateXY():
	global myCount
	global mag
	if(countCool > 100):
		myCount = myCount[len(myCount)-100:len(myCount)]
		mag = mag[len(mag)-100:len(mag)]

def update():
	#plt.clear()
	global countCool
	sensorData = db.read()
	for i in range(countCool, len(sensorData)):
		decode(sensorData[i][3])
		myCount.append(countCool)
		countCool += 1
	updateXY()

def animate(i):
	update()
	li.set_xdata(myCount)
	li.set_ydata(mag)
	ax1.relim()
	ax1.autoscale_view(True,True,True)
	fig.canvas.draw()
	#ax1.clear()
        #x1.plot(myCount, mag, 'k')

def main():
	fig.canvas.draw()
	ani = animation.FuncAnimation(fig, animate, interval = 1, blit=False)
	plt.show(block=False)

if __name__ == "__main__":
	main()
