from database import Database
import sys

def main():
	db = Database("newuser", "secretpassword", str(sys.argv[1]))
	sensorData = db.read()
	for k in sensorData:
		print("{},{},{},{}").format(sensorData[k][0], sensorData[k][1], sensorData[k][2], sensorData[k][3])

if __name__ == "__main__":
	main()
