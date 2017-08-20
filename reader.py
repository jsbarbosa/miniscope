import serial
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
SLEEPTIME = 1e-0

ports = serial.tools.list_ports.comports()
ports = [str(port) for port in ports]
print(ports)

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
		if line[-1] == MULDELIMITER:
			line = line[:-1]
		return line
	
	def internalLoop(self):
		self.flush()
		while True:
			self.data = self.readLine()
	
	def getData(self):
		return self.data

port = Serial(port = '/dev/ttyUSB0', baudrate = BAUDRATE,
                     stopbits = serial.STOPBITS_ONE, parity = serial.PARITY_NONE,
                        bytesize = serial.EIGHTBITS, timeout = TIMEOUT)
                        
port.addInternal()

while True:
	print(port.getData())
	sleep(SLEEPTIME)
#~ fig, ax = plt.subplots()

#~ i = 0
#~ EACH = 10
#~ BUFFER = 50
#~ 
#~ ts = [i for i in range(BUFFER)]
#~ values = [0]*BUFFER
#~ 
#~ plot = ax.plot([], [])[0]
#~ dot = ax.plot([], [], "o")[0]
#~ ax.set_ylim(0, VREF)
#~ ax.set_xlim(0, BUFFER-1)
#~ 
#~ ax.set_xlabel("Measurements")
#~ ax.set_ylabel("Voltage (V)")
#~ 
#~ text = ax.text(0.8*BUFFER, 0.9*VREF, "")
#~ 
#~ def animate(i):
	#~ value = port.getData()
	#~ voltage = VREF * value / MAXADC
	#~ values[i%BUFFER] = voltage
	#~ 
	#~ text.set_text("$V = %.4f$"%voltage)	
	#~ plot.set_data(ts, values)
	#~ dot.set_data([i%BUFFER], [voltage])
		#~ 
	#~ return plot, dot, text
#~ 
#~ ani = FuncAnimation(fig, animate, interval=50)
#~ plt.show()
