import mauscope as mc

mc.printPorts() # print avaible serial ports
port = mc.choosePort() # take user's answer

# defines the stage object
stage = mc.Stage(port)

p1 = (0, 0, 0)
p2 = (0, 1e4, 1e3)
p3 = (1e4, 1e4, 1e3)
plane = mc.findPlane(p1, p2, p3)

# sets its origin
# stage.setOrigin()

stage.scan(1e3, 1e3, plane = plane)
stage.plotThread()
