import serial
from time import sleep

import serial.tools.list_ports
print(list(serial.tools.list_ports.comports()))

BAUDRATE = 9600
TIMEOUT = None

port = serial.Serial(port = '/dev/ttyUSB0', baudrate = BAUDRATE,
                     stopbits = serial.STOPBITS_ONE, parity = serial.PARITY_NONE,
                        bytesize = serial.EIGHTBITS, timeout = TIMEOUT)

while True:
    #port.write(b"0x22")
    temp = port.read(1).encode('hex')
    print(temp, int(temp, 16))
    
    sleep(0.1)