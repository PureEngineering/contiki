import paho.mqtt.client as mqtt
import sensor_message_pb2

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("c")

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
