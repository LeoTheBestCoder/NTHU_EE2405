import paho.mqtt.client as paho
import matplotlib.pyplot as plt
import numpy as np
import serial
import time
mqttc = paho.Client()

# Settings for connection
host = "localhost"
topic= "Mbed"
port = 1883

x_list = []
y_list = []
z_list = []
tilt_list = []
cnt = 0

Fs = 2.0
Ts = 1.0 / Fs
time_vec = np.arange(0, 20, Ts)
x_vec = np.arange(0, 20, Ts)
y_vec = np.arange(0, 20, Ts)
z_vec = np.arange(0, 20, Ts)
tilt_vec = np.arange(0, 20, Ts)

# Callbacks
def on_connect(self, mosq, obj, rc):
    print("Connected rc: " + str(rc))

def on_message(mosq, obj, msg):
    global cnt
    print('cnt = ', end = '')
    print(cnt)
    print("[Received] Topic: " + msg.topic + ", Message: " + str(msg.payload) + "\n")
    

    # print(f'count = {cnt}')
    print([float(i) for i in msg.payload.split()])
    x_list.append(float(msg.payload.split()[0]))
    y_list.append(float(msg.payload.split()[1]))
    z_list.append(float(msg.payload.split()[2]))
    tilt_list.append(int(msg.payload.split()[3]))
    cnt += 1
    
    if len(x_list) == 40:        
        for i in range(0, 40):
            x_vec[i] = x_list[i]
            y_vec[i] = y_list[i]
            z_vec[i] = z_list[i]
            tilt_vec[i] = tilt_list[i]
        fig, ax = plt.subplots(2, 1)
        ax[0].plot(time_vec, x_vec, 'b', time_vec, y_vec, 'r', time_vec, z_vec, 'g')
        ax[0].set_xlabel('Time')
        ax[0].set_ylabel('ACC Vector')
        ax[1].stem(time_vec, tilt_vec, 'b')
        ax[1].set_xlabel('Time')
        ax[1].set_ylabel('Tilt')
        plt.show()


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




mqttc.loop_start()




