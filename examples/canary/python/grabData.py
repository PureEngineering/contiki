from database import Database
import sys

def main():
	db = Database("newuser", "secretpassword", str(sys.argv[1]))
	db.read()

if __name__ == "__main__":
	main()
