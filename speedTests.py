import mauscope as mc
from time import time

port = mc.Serial("/dev/ttyUSB0")

stage = mc.Stage(port)

stage.setOrigin()
file = open("ytime.dat", "w")
DELIM = ","
COLUMNS = ["Resolution", "Trace (s)", "Retrace (s)", "Vtrace (um/s)", "Vretrace (um/s)"]
file.write(DELIM.join(COLUMNS) + "\n")

res = [i for i in range(1, 10)]

print(mc.XSTEP, mc.YSTEP, mc.ZSTEP)

for i in res:
    call_steps = stage.getNSteps(i)
    number_calls = (mc.YTURNS * mc.TURN) // call_steps
    stage.setYStepsPerCall(call_steps)

    init = time()
    stage.move(number_calls, mc.FORWARD)
    final0 = time()
    stage.move(number_calls, mc.BACKWARD)
    final1 = time()

    t0 = final0 - init
    t1 = final1 - final0


    results = ["%d"%i, "%.3f"%t0, "%.3f"%t1, "%.3f"%(mc.YLENGTH/t0), "%.3f"%(mc.YLENGTH/t1)]
    results = DELIM.join(results)
    file.write(results + "\n")

file.close()
# stage.move(mc.XTURNS)
# stage.scan(mc.XLENGTH, 1, xres = 8)
