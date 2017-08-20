import serial
from time import sleep

import serial.tools.list_ports
print(list(serial.tools.list_ports.comports()))

BAUDRATE = 4800
TIMEOUT = 0.1

port = serial.Serial(port = '/dev/ttyUSB0', baudrate = BAUDRATE,
                     stopbits = serial.STOPBITS_ONE, parity = serial.PARITY_NONE,
                        bytesize = serial.EIGHTBITS, timeout = TIMEOUT)

i = 0
each = 50
while True:
    line = port.readline()
    try:
        value = int(line)
        if i%each == 0:
            print(value)
            i = 0
    except:
        pass
    i += 1