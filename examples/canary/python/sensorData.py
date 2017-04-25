from database import Database
import sensor_message_pb

class SensorData:
	
	def __init__(self, d):
		self.light = []
		self.accel = []
		self.mag = []
		self.bme = []
		self.batmon = []
		db = Database("newuser", "secretpassword", d)
		self.fields = db.read()
		for f in self.fields:
			data = f[3]
			self.decode(data)
	
	def decode(self, data):
		message = sensor_message_pb.sensors()
		message.ParseFromString(str(data))
		self.light.append(message.opt_3001)
		self.accel.append([message.LIS2DE12_x, message.LIS2DE12_y, message.LIS2DE12_z])
		self.mag.append([message.lis3mdl_x, message.lis3mdl_y, message.lis3mdl_z])
		self.bme.append([message.bme_280_pres, message.bme_280_hum, message.bme_280_temp])
		self.batmon.append([message.batmon_volt, message.batmon_temp])

	def getLight(self):
		return self.light
	
	def getAccel(self):
		return self.accel

	def getMag(self):
		return self.mag

	def getBme(self):
		return self.bme

	def getBatmon(self):
		return self.batmon

	def getFields(self):
		return self.fields
	
	def getLength(self):
		return len(self.fields)
	
