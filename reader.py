import serial
import numpy as np
from threading import Thread
from time import sleep
import serial.tools.list_ports
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

from ast import literal_eval

BAUDRATE = 4800
TIMEOUT = 0.1
VREF = 5.0
MAXADC = 2**10 - 1
MULDELIMITER = ","
SLEEPTIME = 0

HALFPOINTSADDR = 0x01
DELAYXADDR = 0x02
DELAYYADDR = 0x03
NMESUREMENTSADDR = 0x04

HALFPOINTS = 20

READ = 0x32
WRITE = 0x16

TAKEIMAGE = 0x20
STOP = 0x40

#~ ports = serial.tools.list_ports.comports()
#~ ports = [str(port) for port in ports]
#~ print(ports)

class Serial(serial.Serial):
	def readLine(self):
		line = b""
		while True:
			byte = self.read(1)
			if byte == b'\n':
				break
			else:
				line += byte
		line = line.decode()
		if len(line) > 0:
			if line[-1] == MULDELIMITER:
				line = line[:-1]
		return line

	def getData(self):
		data = port.readLine().split(MULDELIMITER)
		try:
			x, y, v = data
			value = "%04x"%int(v)
			valuehex = "".join(["\\x%s"%value[2*i:2*i+2] for i in range(len(value)//2)])

			complete = "\\x%02x%s"%(READ, valuehex)
			tosend = literal_eval("b'{}'".format(complete))

			self.write(tosend)
			return [int(item) for item in data]

		except ValueError as E:
			print(E, val, splitted)
			return [0, 0, 0]

	def totalFlush(self):
		self.flush()
		self.flushInput()
		self.flushOutput()

	def from16to8(self, value):
		valuehex = "%04x"%int(str(value), 16)
		return [int(valuehex[i:i+2]) for i in range(0, 4, 2)]

	def writeBytes(self, numbers):
		valuehex = ""
		internal = [0]*4
		internal[0] = numbers[0]

		if len(numbers) == 2:
			internal[2:] = self.from16to8(numbers[1])
		if len(numbers) == 3:
			internal[2:] = self.from16to8(numbers[2])
		else:
			internal[1:] = numbers[1:]

		for number in internal:
			valuehex += "\\x%02x"%number
		tosend = literal_eval("b'{}'".format(valuehex))

		self.write(tosend)
		print(valuehex)

		sleep(10e-3)


port = Serial(port = '/dev/ttyUSB0', baudrate = BAUDRATE,
                     stopbits = serial.STOPBITS_ONE, parity = serial.PARITY_NONE,
                        bytesize = serial.EIGHTBITS, timeout = TIMEOUT)

port.totalFlush()

port.writeBytes([WRITE, STOP])
port.writeBytes([WRITE, HALFPOINTSADDR, HALFPOINTS])
port.writeBytes([WRITE, DELAYXADDR, 0])
port.writeBytes([WRITE, DELAYYADDR, 0])
port.writeBytes([WRITE, NMESUREMENTSADDR, 2])
port.writeBytes([WRITE, TAKEIMAGE])

MATRIX = np.zeros((2*HALFPOINTS + 1, 2*HALFPOINTS + 1))

last_x = 0
last_y = 0
def updateMatrix():
	global last_x, last_y
	i = 0
	while True:
		try:
			x, y, v = port.getData()
			x += HALFPOINTS
			y += HALFPOINTS
			MATRIX[y, x] = v
			sleep(SLEEPTIME)

			if y == 2*HALFPOINTS:
				if (abs(x-last_x) == 1) & ((x == 0) | (x == 2*HALFPOINTS)):
					port.writeBytes([WRITE, STOP])
					port.writeBytes([WRITE, TAKEIMAGE])
			last_x = x
			last_y = y

		except:
			pass

matrixThread = Thread(target = updateMatrix)
matrixThread.setDaemon(True)
matrixThread.start()

fig, ax = plt.subplots()

image = ax.imshow(MATRIX, vmin = 0, vmax = 1023, cmap='Greys', interpolation = 'none')
last_plot = ax.plot([0], [0], "s", color = "red", alpha = 0.5)[0]
cbar = fig.colorbar(image)

ax.set_xlabel("Measurements")
ax.set_ylabel("Voltage (V)")

def animate(i):
	image.set_array(MATRIX)
	last_plot.set_data([last_x], [last_y])

	return image, last_plot

ani = FuncAnimation(fig, animate, interval=50, blit=True)
plt.show()

port.writeBytes([WRITE, STOP])
