import serial
import numpy as np
from copy import copy
from time import sleep
from threading import Thread
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

HALFPOINTS = 127

READ = 0x32
WRITE = 0x16

TAKEIMAGE = 0x20
STOP = 0x40

STARTBYTE = 0x11

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
			value = "%06x"%int(v)
			valuehex = "".join(["\\x%s"%value[2*i:2*i+2] for i in range(len(value)//2)])

			complete = "\\x%02x\\x%02x%s"%(STARTBYTE, READ, valuehex)
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

	def writeBytes(self, numbers, stdout = True):
		valuehex = "\\x%02x"%STARTBYTE
		internal = [0]*4
		internal[0] = numbers[0]

		if len(numbers) == 2:
			internal[1] = numbers[1]
		elif len(numbers) == 3:
			internal[1] = numbers[1]
			internal[2:] = self.from16to8(numbers[2])
		else:
			internal[1:] = numbers[1:]

		for number in internal:
			valuehex += "\\x%02x"%number
		tosend = literal_eval("b'{}'".format(valuehex))
		
		if stdout: print(tosend)
		
		self.write(tosend)

		sleep(10e-3)


port = Serial(port = '/dev/ttyUSB0', baudrate = BAUDRATE,
                     stopbits = serial.STOPBITS_ONE, parity = serial.PARITY_NONE,
                        bytesize = serial.EIGHTBITS, timeout = TIMEOUT)

port.totalFlush()

port.writeBytes([WRITE, STOP])
port.writeBytes([WRITE, HALFPOINTSADDR, 0, HALFPOINTS])
port.writeBytes([WRITE, DELAYXADDR, 0, 0])
port.writeBytes([WRITE, DELAYYADDR, 0, 0])
port.writeBytes([WRITE, NMESUREMENTSADDR, 0, 200])
port.writeBytes([WRITE, TAKEIMAGE])

MATRIX = np.zeros((2*HALFPOINTS + 1, 2*HALFPOINTS + 1))
CURRENT_MAT = np.zeros_like(MATRIX)

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
			CURRENT_MAT[:] = 0
			CURRENT_MAT[y, x] = 1
			
			print(x, y, v)
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

palette = copy(plt.cm.Greys)
palette.set_bad('r', 1.0)

image = ax.imshow(MATRIX, vmin = 0, vmax = 100, cmap=palette, interpolation = 'none')
cbar = fig.colorbar(image)

ax.set_xlabel("Measurements")
ax.set_ylabel("Voltage (V)")
def animate(i):
	image.set_array(np.ma.masked_where(CURRENT_MAT == 1, MATRIX))

	return image,

ani = FuncAnimation(fig, animate, interval=50, blit=True)
plt.show()

np.savetxt("Data.txt", MATRIX, fmt = "%d")
port.writeBytes([WRITE, STOP])
