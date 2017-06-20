#!/usr/bin/env python

# pull up GPIO23-25 (tact switches)

import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)

GPIO.setup(23, GPIO.IN, pull_up_down = GPIO.PUD_UP)
GPIO.setup(24, GPIO.IN, pull_up_down = GPIO.PUD_UP)
GPIO.setup(25, GPIO.IN, pull_up_down = GPIO.PUD_UP)

# and pull up GPIO 4, 14, 17, and 27 (gate inputs)

GPIO.setup( 4, GPIO.IN, pull_up_down = GPIO.PUD_UP)
GPIO.setup(14, GPIO.IN, pull_up_down = GPIO.PUD_UP)
GPIO.setup(17, GPIO.IN, pull_up_down = GPIO.PUD_UP)
GPIO.setup(27, GPIO.IN, pull_up_down = GPIO.PUD_UP)