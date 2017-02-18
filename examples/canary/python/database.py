import mysql.connector as mariadb
from mysql.connector import errorcode

class Database:

	def __init__(self, u, p, d):
		self.user = u
		self.password = p
		self.database = d
		self.table = "sensorData"
		self.createTable()

	def openConnection(self):
		self.connection = mariadb.connect(user=self.user, password=self.password)
		self.cursor = self.connection.cursor()
		self.setDatabase()

	def closeConnection(self):
		self.cursor.close()
		self.connection.close()

	def setDatabase(self):
		try:
		    self.connection.database = self.database
		except mariadb.Error as err:
		    if err.errno == errorcode.ER_BAD_DB_ERROR:
			self.createDatabase()
			self.connection.database = self.database
		    else:
        	    	print(err)
                    	exit(1)
		
	def createDatabase(self):
		try:
		    self.cursor.execute("CREATE DATABASE {} DEFAULT CHARACTER SET 'utf8'".format(self.database))
		except mariadb.Error as err:
        	    print("Failed creating database: {}".format(err))
                    exit(1)

	def createTable(self):
		self.openConnection()
		query = """ CREATE TABLE IF NOT EXISTS """ + self.table + """ 
			     (ID INTEGER PRIMARY KEY AUTO_INCREMENT, 
			     uniq_id  INTEGER NOT NULL, time INTEGER NOT NULL, 
			     type TEXT NOT NULL, data BLOB NOT NULL) """
		self.cursor.execute(query)
		self.closeConnection()

	def insert(self, id, time, type, data):
		self.openConnection()
		query = """ INSERT INTO """ + self.table + """
			    (uniq_id, time, type, data) 
			    VALUES (%s, %s, %s, %s) """
		args = (id, time, type, data)
		self.cursor.execute(query, args)
		self.connection.commit()
		self.closeConnection()

	def read(self):
		self.openConnection()
		query = "SELECT uniq_id, time, type, data FROM " + self.table
		self.cursor.execute(query)
		for(uniq_id, time, type, data) in self.cursor:
			print("{},{},{},{}").format(uniq_id, time, type, data)
		self.closeConnection() 
