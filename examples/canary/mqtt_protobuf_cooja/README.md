# Simulating mqtt+protobuf motes in Cooja
#### Download and run the [instant contiki](http://www.contiki-os.org/start.html#install-instant-contiki) VM.
Do the following from within the VM:
#### Install the mqtt broker, bridge-utils, and ncurses
```
sudo apt-get update
sudo apt-get install mosquitto bridge-utils libncurses5-dev
```
#### Clone the [6lbr fork with canary example code](https://github.com/puredaniel/6lbr).
```
git clone --recursive -j8 https://github.com/puredaniel/6lbr.git
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
update-rc.d 6lbr defaults
service 6lbr start
```
#### Configure and run the mqtt broker
```
cd 6lbr/canary/mqtt_protobuf_cooja
sudo cp mosquitto.conf /etc/mosquitto/mosquitto.conf
mosquitto
```
#### Launch Cooja and start simulation
```
cd 6lbr/tools/cooja
ant run
```
  1. Open 6lbr/examples/canary/mqtt_protobuf_cooja/example.csc
  2. Press start or Ctrl+s

#### Add the broker IP to the network bridge
```
sudo ip -6 addr add bbbb::100 dev br0
```
It's possible the bridge has not yet been created by 6lbr, in that case just run the command again

#### Subscribe to the sensor messages
```
sudo apt-get install python-pip
pip install paho-mqtt
cd /6lbr/examples/canary/mqtt_protobuf_cooja/
python sub.py
```

