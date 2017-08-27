import serial
import numpy as np
from threading import Thread
from time import sleep
import serial.tools.list_ports
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

BAUDRATE = 4800
TIMEOUT = 0.1
VREF = 5.0
MAXADC = 2**10 - 1
MULDELIMITER = ","
SLEEPTIME = 0

HALFPOINTS = 20

#~ ports = serial.tools.list_ports.comports()
#~ ports = [str(port) for port in ports]
#~ print(ports)

class Serial(serial.Serial):
	def addInternal(self):
		self.data = 0
		self.thread = Thread(target = self.internalLoop)
		self.thread.setDaemon(True)
		self.thread.start()
		
	def readLine(self):
		line = b""
		while True:
			byte = self.read(1)
			if byte == b'\n':
				break
			else:
				line += byte
		line = line.decode("utf-8")
		if len(line) > 0:
			if line[-1] == MULDELIMITER:
				line = line[:-1]
		return line
	
	def internalLoop(self):
		self.flush()
		while True:
			try:
				self.data = self.readLine()
			except KeyboardInterrupt:
				break
	
	def getData(self):
		try:
			data = port.readLine().split(MULDELIMITER)
			self.write("0".encode())
			return [int(item) for item in data]
		except:
			return [0, 0, 0]

port = Serial(port = '/dev/ttyUSB0', baudrate = BAUDRATE,
                     stopbits = serial.STOPBITS_ONE, parity = serial.PARITY_NONE,
                        bytesize = serial.EIGHTBITS, timeout = TIMEOUT)
                        
#port.addInternal()

MATRIX = np.zeros((2*HALFPOINTS + 1, 2*HALFPOINTS + 1))

def updateMatrix():
	while True:
		try:
			x, y, v = port.getData()
			MATRIX[y + HALFPOINTS, x + HALFPOINTS] = v
			sleep(SLEEPTIME)
		except:
			pass
	
matrixThread = Thread(target = updateMatrix)
matrixThread.setDaemon(True)
matrixThread.start()

fig, ax = plt.subplots()

image = ax.imshow(MATRIX, vmin = 0, vmax = 1023, interpolation = 'none', cmap='Greys')
cbar = fig.colorbar(image)

ax.set_xlabel("Measurements")
ax.set_ylabel("Voltage (V)")

def animate(i):
	image.set_array(MATRIX)
		
	return image,

ani = FuncAnimation(fig, animate, interval=50, blit=True)
plt.show()
