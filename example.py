from time import sleep
from serial import Serial
import serial.tools.list_ports as find_ports

X_MOTOR = 0
Y_MOTOR = 1
Z_MOTOR = 2
POSITIVE = 1
NEGATIVE = 0

ports = find_ports.comports()
for port in ports:
    print(port)

def getUserInput(label):
    while True:
        try:
            return int(input(label + ": "))
        except ValueError:
            pass

class MauscopeSerial(Serial):
    def __init__(self, port, wait = 0.01):
        super(MauscopeSerial, self).__init__(port, baudrate = 9600)
        self.wait = wait

    def make8Byte(self, steps, motor, direction):
        if steps > 32 or steps < 1:
            raise(ValueError("Wrong number of steps: %d. Range is 1 to 32."%steps))
        steps -= 1
        steps = (steps << 3)

        if motor > 2 or motor < 0:
            raise(ValueError("Wrong motor. Range is 0 to 2."))
        motor = motor << 1

        if direction > 1 or motor < 0:
            raise(ValueError("Wrong direction. Either is 0 or 1."))
        return steps | motor | direction

    def send(self, steps, motor, direction):
        number = self.make8Byte(steps, motor, direction)
        self.write([number])
        sleep(self.wait)

    def printB(self, number):
        print("{0:b}".format(number))

port = input("Please enter the port: ")
serial = MauscopeSerial(port = port)

while True:
    try:
        steps = getUserInput("Steps")
        motor = getUserInput("Motor")
        direction = getUserInput("Direction")
        serial.send(steps, motor, direction)
    except KeyboardInterrupt:
        break
    except ValueError as e:
        print(e)
print("")
serial.close()
