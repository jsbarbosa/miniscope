import mauscope as mc

port = "COM3"
serial = mc.Serial(port)

serial.send(mc.Z512)

turns = 0
for i in range(16):
    try:
        serial.send(mc.DOWN)
        turns += 1
        print(turns)
    except:
        break

print(turns)
