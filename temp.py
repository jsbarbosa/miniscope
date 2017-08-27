import serial
from time import sleep
from ast import literal_eval

BAUDRATE = 4800
TIMEOUT = 0.1

READ = 0x32
WRITE = 0x16

TAKEIMAGE = 0x20
STOP = 0x40

MULDELIMITER = ","

port = serial.Serial(port = '/dev/ttyUSB0', baudrate = BAUDRATE,
                     stopbits = serial.STOPBITS_ONE, parity = serial.PARITY_NONE,
                        bytesize = serial.EIGHTBITS, timeout = TIMEOUT)
              
def readline():
	line = b""
	while True:
		byte = port.read(1)
		if byte == b'\n':
			break
		else:
			line += byte
	line = line.decode()
	if len(line) > 0:
		if line[-1] == MULDELIMITER:
			line = line[:-1]
	return line

port.flush()
port.flushInput()
port.flushOutput()

port.write(b"\x16\x00\x40")
sleep(10e-3)

port.write(b"\x16\x01\x0F")
sleep(10e-3)

port.write(b"\x16\x02\x00")
sleep(10e-3)

port.write(b"\x16\x00\x20")

while True:
	val = readline()
	splitted = val.split(",")
	try:
		x, y, v = splitted
		value = "%04x"%int(v)
		valuehex = "".join(["\\x%s"%value[2*i:2*i+2] for i in range(len(value)//2)])
		
		complete = "\\x%02x%s"%(READ, valuehex)
		tosend = literal_eval("b'{}'".format(complete))
		
		port.write(tosend)
		print(val)
		
	except ValueError as E:
		print(E, val, splitted)
		
	
	
	
		
		
	
	
	
