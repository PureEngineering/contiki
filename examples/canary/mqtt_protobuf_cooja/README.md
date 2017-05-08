# Simulating mqtt+protobuf motes in Cooja
#### Download and run the [instant contiki](http://www.contiki-os.org/start.html#install-instant-contiki) VM.
Password for user "user" is "root."
Do the following from within the VM:
#### Install the mqtt broker, bridge-utils, and ncurses
```
sudo apt-get update
sudo apt-get install mosquitto bridge-utils libncurses5-dev
```
#### Clone the [6lbr](https://github.com/cetic/6lbr) and [contiki](https://github.com/PureEngineering/contiki) repos.
```
git clone --recursive -j8 https://github.com/cetic/6lbr.git
git clone --recursive -j8 https://github.com/PureEngineering/contiki.git
```
#### [Build, install, and run 6lbr](https://github.com/cetic/6lbr/wiki/Other-Linux-Software-Configuration)
```
cd 6lbr/examples/6lbr
make all
make plugins
make tools
```
As root:
```
make install
make plugins-install
```
Start 6lbr as a service:
cp contiki/examples/canary/mqtt_protobuf_cooja/6lbr.conf /etc/6lbr/6lbr.conf
update-rc.d 6lbr defaults
service 6lbr start
```
or

Start an instance of 6lbr:
```
6lbr ./contiki/examples/canary/mqtt_protobuf_cooja/6lbr.conf
```
#### Configure and run the mqtt broker
```
sudo cp contiki/examples/canary/mqtt_protobuf_cooja/mosquitto.conf /etc/mosquitto/mosquitto.conf
mosquitto
```
#### Launch Cooja and start simulation
```
cd 6lbr/tools/cooja
ant run
```
  1. Open contiki/examples/canary/mqtt_protobuf_cooja/example.csc
  2. Press start or Ctrl+s

#### Add the broker IP to the network bridge
```
sudo ip -6 addr add bbbb::101 dev br0
```
It's possible the bridge has not yet been created by 6lbr, in that case just run the command again

#### Subscribe to the sensor messages
```
sudo apt-get install python-pip
pip install paho-mqtt
python contiki/examples/canary/mqtt_protobuf_cooja/sub.py
```

