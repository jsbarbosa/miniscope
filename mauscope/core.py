import serial
import numpy as np
import time
from time import sleep
from .constants import *
from threading import Thread
import serial.tools.list_ports
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

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

class Table():
    def __init__(self, port, plane = None):
        self.port = Serial(port)
        self.plane = plane

        self.ax = None
        self.fig = None
        self.dot = None
        self.currentx = 0
        self.currenty = 0
        self.currentz = 0

    def setOrigin(self):
        self.port.send(X512)
        self.port.send(Y512)
        self.port.send(Z512)

        print("Setting x...")
        for i in range(XTURNS):
            try:
                self.port.send(LEFT)
            except KeyboardInterrupt:
                break
        print("Setting y...")
        for i in range(YTURNS):
            try:
                self.port.send(BACKWARD)
            except KeyboardInterrupt:
                break
        print("Setting z...")
        for j in range(ZTURNS):
            try:
                self.port.send(DOWN)
            except KeyboardInterrupt:
                break

    def getNSteps(self, res):
        if (not type(res) is int) or (res > 9) or (res < 1):
            raise(Exception("Resolution is not an int number. Min value is 1, max is 9."))

        return int(64 * (9 - res))

    def plot(self, plane, xmax, ymax):
        self.fig = plt.figure()
        x = np.linspace(0, xmax, 10)
        y = np.linspace(0, ymax, 10)
        x, y = np.meshgrid(x, y)
        z = plane.getZ(x, y)

        self.ax = self.fig.gca(projection='3d')
        self.ax.plot_surface(x, y, z)
        self.dot, = self.ax.plot([self.currentx], [self.currenty], [self.currentz], marker="o")

    def scan(self, xlength_cm, ylength_cm, xres = 1, yres = 1, zres = 5, plane = None):
        self.thread = Thread(target = self.scanThread, args=(xlength_cm, ylength_cm, xres, yres, zres, plane))
        # self.thread.daemon = True
        self.thread.start()

    def plotThread(self):
        while self.dot == None:
            sleep(1)
        plt.ion()
        while True:
            plt.pause(0.01)
            self.dot.set_data([self.currentx], [self.currenty])
            self.dot.set_3d_properties([self.currentz])
            plt.draw()

    def updatePlot(self, i):
        self.dot.set_data([self.currentx], [self.currenty])
        self.dot.set_3d_properties([self.currentz])
        return self.dot,

    def scanThread(self, xlength_cm, ylength_cm, xres = 1, yres = 1, zres = 5, plane = None):
        x = xlength_cm * 0.01
        y = ylength_cm * 0.01

        xsteps = self.getNSteps(xres)
        ysteps = self.getNSteps(yres)
        zsteps = self.getNSteps(zres)
        self.port.send(eval("X%d"%xsteps))
        self.port.send(eval("Y%d"%ysteps))
        self.port.send(eval("Z%d"%zsteps))

        nx = int(round((x / XSTEP) / xsteps, 0))
        ny = int(round((y / YSTEP) / ysteps, 0))

        z = 0

        xdir = RIGHT

        if self.plane != None and plane == None:
            plane = self.plane

        self.plane = plane

        if self.plane != None:
            self.plot(self.plane, x, y)

        for i in range(ny):
            if i %2 == 0: xdir = RIGHT
            else: xdir = LEFT

            for j in range(nx):
                if xdir == LEFT:
                    self.currentx = (nx - j)*xsteps*XSTEP
                self.currentx = j*xsteps*XSTEP
                self.currenty = i*ysteps*YSTEP

                if plane != None:
                    self.currentz = plane.getZ(self.currentx, self.currenty)
                    z = self.currentz - z
                    nz = int(round((abs(z) / ZSTEP) / zsteps, 0))
                    for k in range(nz):
                        if z > 0:
                            self.port.send(UP)
                        else:
                            self.port.send(DOWN)
                    z = self.currentz
                self.port.send(xdir)
            self.port.send(FORWARD)
