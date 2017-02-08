from database import Database
import paho.mqtt.client as mqtt
import sensor_message_pb
import time
import sys

message = sensor_message_pb.sensors()
# Initialize a Database object
db = Database("newuser", "secretpassword", str(sys.argv[1]))

# This callback is triggered when successfully connected
def on_connect(client, userdata, flags, rc):
	print("Connected")
	client.publish("mqttclientpython","Python Script Test Message.")

# This callback is triggered when a new message is received
def on_message(client, userdata, msg):
	print("Encoded Message: " + str(msg.payload) + "\n")
        message.ParseFromString(msg.payload)
	db.insert(message.id, time.time(), "Canary", msg.payload)

# This callback is triggered when the client has subscribed successfully 
def on_subscribe(client, userdata, mid, granted_qos):
	print("Subscribed")

def main():
	mClient = mqtt.Client()
	# Set mqtt callback functions
	mClient.on_connect = on_connect
	mClient.on_message = on_message
	mClient.on_subscribe = on_subscribe
	# Connect to 6lbr 
	mClient.connect("bbbb::101")
	mClient.subscribe("c", 0)
	mClient.loop_forever()

if __name__ == "__main__":
	main() 
