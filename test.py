from time import sleep
from serial import Serial
import serial.tools.list_ports as find_ports

ports = find_ports.comports()
for port in ports:
    print(port)

class MauscopeSerial(Serial):
    def __init__(self, port, baudrate = 9600, wait = 0.05):
        super(MauscopeSerial, self).__init__(port, baudrate = baudrate)
        self.wait = wait

    def make8Byte(self, steps, motor, direction):
        steps = steps << 3
        motor = motor << 1
        return steps | motor | direction

    def send(self, steps, motor, direction):
        self.write([self.make8Byte(steps, motor, direction)])
        sleep(self.wait)

#port = input("Please enter the port: ")
port = "/dev/ttyUSB0"
serial = MauscopeSerial(port = port, baudrate = 9600)

# while True:
#     try:
#         steps = int(input("Steps: "))
#         motor = int(input("Motor: "))
#         direction = int(input("Direction: "))
#         number = make8Byte(steps, motor, direction)
#         serial.write([number])
#         sleep(1)
#     except KeyboardInterrupt:
#         break
#     except ValueError:
#         pass

while True:
    try:
        serial.send(31, 1, 1)
    except KeyboardInterrupt:
        break
serial.close()
