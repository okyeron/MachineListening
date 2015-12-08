# MachineListening
Eurorack Module for Machine Listening

## Getting Setup
Please refer to my Embedded Audio tutorials to setup your Raspberry Pi for development
https://github.com/chrislatina/EmbeddedAudio

## Installation
After cloning the repository, download and install port audio. There is a tutorial for compiling on Linux here: http://portaudio.com/docs/v19-doxydocs/compile_linux.html

Download the libsound-dev libraries. You'll need to be connected to the internet (forward by running headless) to use 

```sudo apt-get install libasound-dev```

I suggest using ALSA. The makefile includes all of the following 

```-lrt -lasound -ljack -lpthread```

Replace the libportaudio.a file (currently compiled for testing on Mac OSX) with the linux version inside /Module/ML_Module/include/portaudio

```cp /PORTAUDIO/DIR/lib/.libs/libportaudio.a /YOUR/PROJECT/DIR/Module/ML_Module/include/portaudio```

Now you can cd into the Module/ML_Module directory and run `make`

## WiringPi
Next you'll need to download and compile the wiringPi library for for reading and writing to and from GPIO pins. This is very straight forward and simply requires running the build script. The library dependencies in the makefile for compiling this library on Raspberry Pi already reference the wiringPi library.

http://wiringpi.com/download-and-install/

## Optimizing Raspbery Pi for realtime streaming Audio

I highy recommend reading this wiki on low latency audio http://wiki.linuxaudio.org/wiki/raspberrypi. Overclocking (http://elinux.org/RPiconfig#Overclocking) is probably not necessary but you can experiment with this if you receive dropouts.
