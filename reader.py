import serial
from time import sleep

BAUDRATE = 4800
TIMEOUT = 1

SLEEP_TIME = 1e-4

XLEFT = 0xA0
XRIGHT = 0xA1

YLEFT = 0xB0
YRIGHT = 0xB1

ZLEFT = 0xC0
ZRIGHT = 0xC1

TURNSTEPS = 0

port = serial.Serial(port = '/dev/ttyUSB0', baudrate = BAUDRATE,
                     stopbits = serial.STOPBITS_ONE, parity = serial.PARITY_NONE,
                        bytesize = serial.EIGHTBITS, timeout = TIMEOUT)

port.flush()

sequence = [ZLEFT]

port.write([0xCA])
ans = int.from_bytes(port.read(), byteorder='big')

#for i in range(TURNSTEPS):
while True:
    try:
        for step in sequence:
            port.write([step])
            ans = int.from_bytes(port.read(), byteorder='big')
            while ans != 0xff:
                ans = int.from_bytes(port.read(), byteorder='big')
                sleep(SLEEP_TIME)
        TURNSTEPS += 1
        print(TURNSTEPS)
    except KeyboardInterrupt:
        break

print(TURNSTEPS)
