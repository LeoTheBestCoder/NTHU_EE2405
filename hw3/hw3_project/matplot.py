import matplotlib.pyplot as plt
import numpy as np
import serial
import time

Fs = 10.0
Ts = 1.0 / Fs
time_vec = np.arange(0, 10, Ts)    # time vector; create Fs samples between 0 and 10.0 sec.
x_vec = np.arange(0, 10, Ts)
y_vec = np.arange(0, 10, Ts)
z_vec = np.arange(0, 10, Ts)
tilt_vec = np.arange(0, 10, Ts)

serdev = '/dev/ttyACM0'
s = serial.Serial(serdev, baudrate = 115200)

for i in range(0, 100):
    line = s.readline()
    line_sep = line.split()
    x_vec[i] = float(line_sep[0])
    y_vec[i] = float(line_sep[1])
    z_vec[i] = float(line_sep[2])
    tilt_vec[i] = int(line_sep[3])

fig, ax = plt.subplots(2, 1)
ax[0].plot(time_vec, x_vec, 'b', time_vec, y_vec, 'r', time_vec, z_vec, 'g')
ax[0].set_xlabel('Time')
ax[0].set_ylabel('ACC Vector')
ax[1].stem(time_vec, tilt_vec, 'b')
ax[1].set_xlabel('Time')
ax[1].set_ylabel('Tilt')
plt.show()
s.close()