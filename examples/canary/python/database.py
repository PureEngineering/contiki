import mysql.connector as mariadb

class Database:

	def __init__(self, u, p, d, t):
		self.table = t
		self.mariadb_connection = mariadb.connect(user=u, password=p, database=d)
		self.cursor = self.mariadb_connection.cursor()
		create = "CREATE TABLE IF NOT EXISTS " + self.table + " (ID INTEGER PRIMARY KEY AUTO_INCREMENT, uniq_id  INTEGER NOT NULL, time INTEGER NOT NULL, type TEXT NOT NULL, data BLOB NOT NULL)"
		self.cursor.execute(create)
	
	def insert(self, id, time, type, data):
		query = "INSERT INTO " + self.table + " (uniq_id, time, type, data) VALUES (%s, %s, %s, %s)"
		args = (id, time, type, data)
		self.cursor.execute(query, args)
		self.mariadb_connection.commit()
