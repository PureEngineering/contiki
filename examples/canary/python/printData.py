from sensorData import SensorData
import sys
import sensor_message_pb

def printData(d):
	sd = SensorData(d)
	l = sd.getLight()
	a = sd.getAccel()
	m = sd.getMag()
	bme = sd.getBme()
	batmon = sd.getBatmon()
	f = sd.getFields()
	for i in range(sd.getLength()):
		print("Fields: {}, {}, {}, {}").format(f[i][0], f[i][1], f[i][2], f[i][3])
		print("Light: {}").format(l[i])
		print("Accelerometer: x:{} y:{} z:{}").format(a[i][0], a[i][1], a[i][2])
		print("Magnetometer: x:{} y:{} z:{}").format(m[i][0], m[i][1], m[i][2])
		print("Biometric: pressure:{} humdity{} temperature:{}").format(bme[i][0], bme[i][1], bme[i][2])
		print("Batmon: volt:{} temp:{}").format(batmon[i][0], batmon[i][1])
		print ""			

def main():
	printData(str(sys.argv[1]))

if __name__ == "__main__":
        main()
