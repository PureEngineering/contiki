import paho.mqtt.client as mqtt
import sensor_message_pb2
import MySQLdb

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("c")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    message.ParseFromString(str(msg.payload))
    print(message)
    print(msg.topic+" "+str(msg.payload))

message = sensor_message_pb2.sensors()
client = mqtt.Client(protocol=mqtt.MQTTv31)
client.on_connect = on_connect
client.on_message = on_message

client.connect("bbbb::101", 1883, 60)
client.loop_forever()
