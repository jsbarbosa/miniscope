import RPi.GPIO as GPIO
from time import sleep

GPIO.setmode(GPIO.BCM)

outs = [17, 27, 22, 23]

current = 0

for item in outs:
    GPIO.setup(item, GPIO.OUT)

def step():
    global current
    
    i = current%4
    if i <= 1:
        GPIO.output(outs[2], GPIO.LOW)
        GPIO.output(outs[3], GPIO.HIGH)
        if i == 0:
            GPIO.output(outs[0], GPIO.LOW)
            GPIO.output(outs[1], GPIO.HIGH)
        else:
            GPIO.output(outs[1], GPIO.LOW)
            GPIO.output(outs[0], GPIO.HIGH)
    else:
        GPIO.output(outs[3], GPIO.LOW)
        GPIO.output(outs[2], GPIO.HIGH)
        if i == 2:
            GPIO.output(outs[1], GPIO.LOW)
            GPIO.output(outs[0], GPIO.HIGH)
        else:
            GPIO.output(outs[0], GPIO.LOW)
            GPIO.output(outs[1], GPIO.HIGH)
            
    current += 1
    
try:
#    GPIO.output(outs[0], GPIO.HIGH)
    while True:
        step()
        sleep(0.5e-2)
#        print(current)
except:
    pass

#GPIO.output(outs[0], 0)
GPIO.cleanup()