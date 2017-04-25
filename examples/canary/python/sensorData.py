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

	def getFields(self):
		return self.fields
