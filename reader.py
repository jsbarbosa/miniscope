import serial
from time import sleep

BAUDRATE = 4800
TIMEOUT = 1

SLEEP_TIME = 0

port = serial.Serial(port = '/dev/ttyUSB0', baudrate = BAUDRATE,
                     stopbits = serial.STOPBITS_ONE, parity = serial.PARITY_NONE,
                        bytesize = serial.EIGHTBITS, timeout = TIMEOUT)

port.flush()

while True:
	port.write([0xB1])
	ans = int.from_bytes(port.read(), byteorder='big')
	if ans == 0xff:
		sleep(SLEEP_TIME)
	port.write([0xA1])
	ans = int.from_bytes(port.read(), byteorder='big')
	if ans == 0xff:
		sleep(SLEEP_TIME)
