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

```cp lib/.libs/libportaudio.a /YOUR/PROJECT/DIR/Module/ML_Module/include/portaudio```

Now you can cd into the Module/ML_Module directory and run `make`
