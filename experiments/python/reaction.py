import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

led = 4

GPIO.setup(led, GPIO.OUT)

for x in range(0, 25):
	GPIO.output(led,1)
	time.sleep(0.05)
	GPIO.output(led,0)
	time.sleep(0.05)

GPIO.cleanup()
