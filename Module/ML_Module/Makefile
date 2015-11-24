#
# This is an example Makefile for a mlm program.
# Typing 'make' or 'make mlm' will create the executable file.
#

# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#
# for C++ define  CC = g++
CC = g++
CFLAGS  = -g -Wall

# typing 'make' will invoke the first target entry in the file 
# (in this case the default target entry)
# you can name this target entry anything, but "default" or "all"
# are the most commonly used names by convention
#
default: mlm

# To create the executable file mlm we need the object files
# mlm.o
#
mlm:  main.o
	$(CC) $(CFLAGS) -o mlm main.o 

# To create the object file mlm.o, we need the sources
LIBS = -lpthread -ldl
LDFLAGS= -Linclude/portaudio -llibportaudio

main.o:  src/main.cpp src/FFT.cpp src/FFT.hpp src/SpectralFeatures.cpp src/SpectralFeatures.hpp include/portaudiosrc/portaudio.h include/kissfft/kiss_fft.h include/kissfft/kiss_fft.c include/kissfft/kiss_fftr.h include/kissfft/kiss_fftr.c include/kissfft/kissfft.hh
	$(CC) $(CFLAGS) -c src/main.cpp LIBS LDFLAGS


# To start over from scratch, type 'make clean'.  This
# removes the executable file, as well as old .o object
# files and *~ backup files:
#
clean: 
	$(RM) mlm *.o *~