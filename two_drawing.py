import mauscope as mc

port = "/dev/ttyUSB0"
serial = mc.Serial(port)

serial.send(mc.X512)
serial.send(mc.Y512)
serial.send(mc.Z512)

for i in range(mc.XSTEPS):
    serial.send(mc.RIGHT)
    print("X: %d"%i)

for i in range(mc.YSTEPS//2):
    serial.send(mc.BACKWARD)
    print("Y: %d"%i)

for i in range(mc.ZSTEPS):
    serial.send(mc.UP)
    print("Z: %d"%i)

for i in range(mc.XSTEPS):
    serial.send(mc.LEFT)
    print("X: %d"%i)

for i in range(mc.ZSTEPS):
    serial.send(mc.DOWN)
    print("Z: %d"%i)

for i in range(mc.YSTEPS//2):
    serial.send(mc.BACKWARD)
    print("Y: %d"%i)

for i in range(mc.XSTEPS):
    serial.send(mc.RIGHT)
    print("X: %d"%i)
