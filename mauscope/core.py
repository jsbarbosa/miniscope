import serial
from time import sleep
from .constants import *
import serial.tools.list_ports

class Serial(serial.Serial):
    def __init__(self, port):
        serial.Serial.__init__(self, port = port, baudrate = BAUDRATE,
                             stopbits = serial.STOPBITS_ONE, parity = serial.PARITY_NONE,
                                bytesize = serial.EIGHTBITS, timeout = TIMEOUT)

        self.flush()

    def send(self, hex_int):
        self.write([hex_int])
        ans = int.from_bytes(self.read(1), byteorder='big')
        if ans != SYSTEM_RETURN:
            raise Exception()

def printPorts():
    ports = list(serial.tools.list_ports.comports())
    ports = "\n".join([str(port) for port in ports])

    print("Currently avaiable ports are:\n%s"%ports)

def choosePort():
    while True:
        try:
            port = input("Please choose port: ")
            serial = Serial(port)
            return serial
        except Exception as e:
            print(e)

def commandLoop(serial):
    while True:
        try:
            command = input("Command: ")
            if command == "exit":
                break
            exec("serial.send(%s)"%command)
        except Exception as e:
            pass
            #print(e)
