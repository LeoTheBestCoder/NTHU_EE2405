import serial
import time
import paho.mqtt.client as paho
import matplotlib.pyplot as plt
import numpy as np
mqttc = paho.Client()

# Settings for connection
host = "localhost"
topic= "Mbed"
port = 1883

# XBee setting
serdev = '/dev/ttyUSB0'
s = serial.Serial(serdev, baudrate=9600)

s.write("+++".encode())
char = s.read(2)
print("Enter AT mode.")
print(char.decode())

s.write("ATMY 0x236\r\n".encode())
char = s.read(3)
print("Set MY 0x236.")
print(char.decode())

s.write("ATDL0x136\r\n".encode())
char = s.read(3)
print("Set DL0x136.")
print(char.decode())

s.write("ATID 0x1\r\n".encode())
char = s.read(3)
print("Set PAN ID 0x1.")
print(char.decode())

s.write("ATWR\r\n".encode())
char = s.read(3)
print("Write config.")
print(char.decode())

s.write("ATMY\r\n".encode())
char = s.read(4)
print("MY :")
print(char.decode())

s.write("ATDL\r\n".encode())
char = s.read(4)
print("DL : ")
print(char.decode())

s.write("ATCN\r\n".encode())
char = s.read(3)
print("Exit AT mode.")
print(char.decode())

print("start sending RPC")
all_string = []
# x_vec = []
# y_vec = []
# z_vec = []
# tilt_vec = []
cnt_vec = [2] * 40

for i in range(60):
    # send RPC to remote
    s.write("/GetData/run\r".encode())
    # string = s.readline()
    # print(string)
    time.sleep(1)

    if i >= 2:
        string_in = s.readline()
        print(string_in.split())
        all_string.append(string_in)
        # x_vec.append(float(string_in.split()[0])
        # y_vec.append(float(string_in.split()[1])
        # z_vec.append(float(string_in.split()[2])

print(all_string)
for stat in all_string:
    if all_string.index(stat) == len(all_string) - 1:
        cnt_vec.append(int(stat.split()[4]))
    else:
        idx = all_string.index(stat)
        if all_string[idx + 1] < all_string[idx]:
            cnt_vec.append(int(stat.split()[4]))


print(cnt_vec)
        


Fs = 1.0
Ts = 1.0 / Fs
time_vec = np.arange(0, 20, Ts)    # time vector; create Fs samples between 0 and 10.0 sec.
cnt_plot = np.arange(0, 20, Ts)

for i in range(20):
    cnt_plot[i] = cnt_vec[i]

print(f'cnt_plot = {cnt_plot}')

fig, ax = plt.subplots(1, 1)
ax.plot(time_vec, cnt_plot, 'r')
ax.set_xlabel('Time')
ax.set_ylabel('cnt')
plt.show()


# Callbacks
def on_connect(self, mosq, obj, rc):
    print("Connected rc: " + str(rc))

def on_message(mosq, obj, msg):
    print("[Received] Topic: " + msg.topic + ", Message: " + str(msg.payload) + "\n");

def on_subscribe(mosq, obj, mid, granted_qos):
    print("Subscribed OK")

def on_unsubscribe(mosq, obj, mid, granted_qos):
    print("Unsubscribed OK")

# Set callbacks
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_subscribe = on_subscribe
mqttc.on_unsubscribe = on_unsubscribe

# Connect and subscribe
print("Connecting to " + host + "/" + topic)
mqttc.connect(host, port=1883, keepalive=60)
mqttc.subscribe(topic, 0)

for stat in all_string:
    mesg = stat
    mqttc.publish(topic, mesg)
    print(mesg)
    time.sleep(0.1)

s.close()
