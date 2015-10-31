#!/usr/bin/env python
 
import alsaaudio as aa
import audioop
from time import sleep

import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BOARD)
GPIO.setwarnings(False)

#Define physical header pin numbers for 6 LEDs
RPiPins=[11,12,13,15,16,18]
#Set all pins as output
for pin in RPiPins:
      GPIO.setup(pin, GPIO.OUT)

# Set up audio
data_in = aa.PCM(aa.PCM_CAPTURE, aa.PCM_NONBLOCK)
data_in.setchannels(2)
data_in.setrate(44100)
data_in.setformat(aa.PCM_FORMAT_S16_LE)

data_in.setperiodsize(256)

while True:
   # Read data from device
   l,data = data_in.read()
   if l:
      # catch frame error
      try:
         max_vol=audioop.max(data,2)
         scaled_vol = max_vol//4680      
         if scaled_vol==0:
            for pin in range(0,6):
               GPIO.output(RPiPins[pin], False)
         else:
            for pin in range(0,scaled_vol):
               GPIO.output(RPiPins[pin], True)
            for pin in range(scaled_vol,6):
               GPIO.output(RPiPins[pin], False)
            
      except audioop.error, e:
         if e.message !="not a whole number of frames":
            raise e