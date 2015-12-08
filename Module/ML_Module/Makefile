#
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#

# define the C compiler to use
CC = g++ 

# define any compile-time flags
CFLAGS =  -g -Wall

# define any directories containing header files other than /usr/include
#
INCLUDES = -Iinclude/portaudio -Iinclude/kissfft

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS = -Linclude/portaudio

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
ifeq ($(UNAME_S),Linux)
    LIBS = -lportaudio -lwiringPi -lrt -lasound -ljack -lpthread

endif
ifeq ($(UNAME_S),Darwin)
    LIBS = -lportaudio -lrt -lasound -ljack -lpthread

endif

# define Apple frameworks if compiling for OSX
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	LIBS = -lportaudio
	FRAMEWORKS = -framework CoreServices -framework CoreFoundation -framework AudioUnit -framework AudioToolbox -framework CoreAudio
endif


# define the C source files
SRCS = src/main.cpp src/FFT.cpp src/SpectralFeatures.cpp include/kissfft/kiss_fft.c include/kissfft/kiss_fftr.c

# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
OBJS = $(SRCS:.c=.o)

# define the executable file 
MAIN = mycc

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean

all:    $(MAIN)
	@echo  Simple compiler named mycc has been compiled

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(FRAMEWORKS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

src/main.o: /usr/include/stdlib.h /usr/include/Availability.h
src/main.o: /usr/include/AvailabilityInternal.h /usr/include/_types.h
src/main.o: /usr/include/sys/_types.h /usr/include/sys/cdefs.h
src/main.o: /usr/include/sys/_symbol_aliasing.h
src/main.o: /usr/include/sys/_posix_availability.h
src/main.o: /usr/include/machine/_types.h /usr/include/i386/_types.h
src/main.o: /usr/include/sys/_pthread/_pthread_types.h
src/main.o: /usr/include/sys/wait.h /usr/include/sys/_types/_pid_t.h
src/main.o: /usr/include/sys/_types/_id_t.h /usr/include/sys/signal.h
src/main.o: /usr/include/sys/appleapiopts.h /usr/include/machine/signal.h
src/main.o: /usr/include/i386/signal.h /usr/include/machine/_mcontext.h
src/main.o: /usr/include/i386/_mcontext.h /usr/include/mach/i386/_structs.h
src/main.o: /usr/include/sys/_pthread/_pthread_attr_t.h
src/main.o: /usr/include/sys/_types/_sigaltstack.h
src/main.o: /usr/include/sys/_types/_ucontext.h
src/main.o: /usr/include/sys/_types/_sigset_t.h
src/main.o: /usr/include/sys/_types/_size_t.h
src/main.o: /usr/include/sys/_types/_uid_t.h /usr/include/sys/resource.h
src/main.o: /usr/include/stdint.h /usr/include/sys/_types/_int8_t.h
src/main.o: /usr/include/sys/_types/_int16_t.h
src/main.o: /usr/include/sys/_types/_int32_t.h
src/main.o: /usr/include/sys/_types/_int64_t.h /usr/include/_types/_uint8_t.h
src/main.o: /usr/include/_types/_uint16_t.h /usr/include/_types/_uint32_t.h
src/main.o: /usr/include/_types/_uint64_t.h
src/main.o: /usr/include/sys/_types/_intptr_t.h
src/main.o: /usr/include/sys/_types/_uintptr_t.h
src/main.o: /usr/include/_types/_intmax_t.h /usr/include/_types/_uintmax_t.h
src/main.o: /usr/include/sys/_types/_timeval.h /usr/include/machine/endian.h
src/main.o: /usr/include/i386/endian.h /usr/include/sys/_endian.h
src/main.o: /usr/include/libkern/_OSByteOrder.h
src/main.o: /usr/include/libkern/i386/_OSByteOrder.h /usr/include/alloca.h
src/main.o: /usr/include/sys/_types/_ct_rune_t.h
src/main.o: /usr/include/sys/_types/_rune_t.h
src/main.o: /usr/include/sys/_types/_wchar_t.h
src/main.o: /usr/include/sys/_types/_null.h /usr/include/machine/types.h
src/main.o: /usr/include/i386/types.h /usr/include/sys/_types/_dev_t.h
src/main.o: /usr/include/sys/_types/_mode_t.h /usr/include/stdio.h
src/main.o: /usr/include/sys/_types/_va_list.h /usr/include/sys/stdio.h
src/main.o: /usr/include/sys/_types/_off_t.h
src/main.o: /usr/include/sys/_types/_ssize_t.h /usr/include/secure/_stdio.h
src/main.o: /usr/include/secure/_common.h /usr/include/string.h
src/main.o: /usr/include/sys/_types/_rsize_t.h
src/main.o: /usr/include/sys/_types/_errno_t.h /usr/include/strings.h
src/main.o: /usr/include/secure/_string.h include/portaudio/portaudio.h
src/main.o: src/fft.h /usr/include/math.h include/kissfft/kiss_fft.h
src/main.o: include/kissfft/kiss_fftr.h src/SpectralFeatures.h
src/FFT.o: src/FFT.h /usr/include/stdio.h /usr/include/sys/cdefs.h
src/FFT.o: /usr/include/sys/_symbol_aliasing.h
src/FFT.o: /usr/include/sys/_posix_availability.h /usr/include/Availability.h
src/FFT.o: /usr/include/AvailabilityInternal.h /usr/include/_types.h
src/FFT.o: /usr/include/sys/_types.h /usr/include/machine/_types.h
src/FFT.o: /usr/include/i386/_types.h
src/FFT.o: /usr/include/sys/_pthread/_pthread_types.h
src/FFT.o: /usr/include/sys/_types/_va_list.h
src/FFT.o: /usr/include/sys/_types/_size_t.h /usr/include/sys/_types/_null.h
src/FFT.o: /usr/include/sys/stdio.h /usr/include/sys/_types/_off_t.h
src/FFT.o: /usr/include/sys/_types/_ssize_t.h /usr/include/secure/_stdio.h
src/FFT.o: /usr/include/secure/_common.h /usr/include/math.h
src/FFT.o: include/kissfft/kiss_fft.h /usr/include/stdlib.h
src/FFT.o: /usr/include/sys/wait.h /usr/include/sys/_types/_pid_t.h
src/FFT.o: /usr/include/sys/_types/_id_t.h /usr/include/sys/signal.h
src/FFT.o: /usr/include/sys/appleapiopts.h /usr/include/machine/signal.h
src/FFT.o: /usr/include/i386/signal.h /usr/include/machine/_mcontext.h
src/FFT.o: /usr/include/i386/_mcontext.h /usr/include/mach/i386/_structs.h
src/FFT.o: /usr/include/sys/_pthread/_pthread_attr_t.h
src/FFT.o: /usr/include/sys/_types/_sigaltstack.h
src/FFT.o: /usr/include/sys/_types/_ucontext.h
src/FFT.o: /usr/include/sys/_types/_sigset_t.h
src/FFT.o: /usr/include/sys/_types/_uid_t.h /usr/include/sys/resource.h
src/FFT.o: /usr/include/stdint.h /usr/include/sys/_types/_int8_t.h
src/FFT.o: /usr/include/sys/_types/_int16_t.h
src/FFT.o: /usr/include/sys/_types/_int32_t.h
src/FFT.o: /usr/include/sys/_types/_int64_t.h /usr/include/_types/_uint8_t.h
src/FFT.o: /usr/include/_types/_uint16_t.h /usr/include/_types/_uint32_t.h
src/FFT.o: /usr/include/_types/_uint64_t.h
src/FFT.o: /usr/include/sys/_types/_intptr_t.h
src/FFT.o: /usr/include/sys/_types/_uintptr_t.h
src/FFT.o: /usr/include/_types/_intmax_t.h /usr/include/_types/_uintmax_t.h
src/FFT.o: /usr/include/sys/_types/_timeval.h /usr/include/machine/endian.h
src/FFT.o: /usr/include/i386/endian.h /usr/include/sys/_endian.h
src/FFT.o: /usr/include/libkern/_OSByteOrder.h
src/FFT.o: /usr/include/libkern/i386/_OSByteOrder.h /usr/include/alloca.h
src/FFT.o: /usr/include/sys/_types/_ct_rune_t.h
src/FFT.o: /usr/include/sys/_types/_rune_t.h
src/FFT.o: /usr/include/sys/_types/_wchar_t.h /usr/include/machine/types.h
src/FFT.o: /usr/include/i386/types.h /usr/include/sys/_types/_dev_t.h
src/FFT.o: /usr/include/sys/_types/_mode_t.h /usr/include/string.h
src/FFT.o: /usr/include/sys/_types/_rsize_t.h
src/FFT.o: /usr/include/sys/_types/_errno_t.h /usr/include/strings.h
src/FFT.o: /usr/include/secure/_string.h include/kissfft/kiss_fftr.h
src/SpectralFeatures.o: src/SpectralFeatures.h /usr/include/stdio.h
src/SpectralFeatures.o: /usr/include/sys/cdefs.h
src/SpectralFeatures.o: /usr/include/sys/_symbol_aliasing.h
src/SpectralFeatures.o: /usr/include/sys/_posix_availability.h
src/SpectralFeatures.o: /usr/include/Availability.h
src/SpectralFeatures.o: /usr/include/AvailabilityInternal.h
src/SpectralFeatures.o: /usr/include/_types.h /usr/include/sys/_types.h
src/SpectralFeatures.o: /usr/include/machine/_types.h
src/SpectralFeatures.o: /usr/include/i386/_types.h
src/SpectralFeatures.o: /usr/include/sys/_pthread/_pthread_types.h
src/SpectralFeatures.o: /usr/include/sys/_types/_va_list.h
src/SpectralFeatures.o: /usr/include/sys/_types/_size_t.h
src/SpectralFeatures.o: /usr/include/sys/_types/_null.h
src/SpectralFeatures.o: /usr/include/sys/stdio.h
src/SpectralFeatures.o: /usr/include/sys/_types/_off_t.h
src/SpectralFeatures.o: /usr/include/sys/_types/_ssize_t.h
src/SpectralFeatures.o: /usr/include/secure/_stdio.h
src/SpectralFeatures.o: /usr/include/secure/_common.h /usr/include/stdlib.h
src/SpectralFeatures.o: /usr/include/sys/wait.h
src/SpectralFeatures.o: /usr/include/sys/_types/_pid_t.h
src/SpectralFeatures.o: /usr/include/sys/_types/_id_t.h
src/SpectralFeatures.o: /usr/include/sys/signal.h
src/SpectralFeatures.o: /usr/include/sys/appleapiopts.h
src/SpectralFeatures.o: /usr/include/machine/signal.h
src/SpectralFeatures.o: /usr/include/i386/signal.h
src/SpectralFeatures.o: /usr/include/machine/_mcontext.h
src/SpectralFeatures.o: /usr/include/i386/_mcontext.h
src/SpectralFeatures.o: /usr/include/mach/i386/_structs.h
src/SpectralFeatures.o: /usr/include/sys/_pthread/_pthread_attr_t.h
src/SpectralFeatures.o: /usr/include/sys/_types/_sigaltstack.h
src/SpectralFeatures.o: /usr/include/sys/_types/_ucontext.h
src/SpectralFeatures.o: /usr/include/sys/_types/_sigset_t.h
src/SpectralFeatures.o: /usr/include/sys/_types/_uid_t.h
src/SpectralFeatures.o: /usr/include/sys/resource.h /usr/include/stdint.h
src/SpectralFeatures.o: /usr/include/sys/_types/_int8_t.h
src/SpectralFeatures.o: /usr/include/sys/_types/_int16_t.h
src/SpectralFeatures.o: /usr/include/sys/_types/_int32_t.h
src/SpectralFeatures.o: /usr/include/sys/_types/_int64_t.h
src/SpectralFeatures.o: /usr/include/_types/_uint8_t.h
src/SpectralFeatures.o: /usr/include/_types/_uint16_t.h
src/SpectralFeatures.o: /usr/include/_types/_uint32_t.h
src/SpectralFeatures.o: /usr/include/_types/_uint64_t.h
src/SpectralFeatures.o: /usr/include/sys/_types/_intptr_t.h
src/SpectralFeatures.o: /usr/include/sys/_types/_uintptr_t.h
src/SpectralFeatures.o: /usr/include/_types/_intmax_t.h
src/SpectralFeatures.o: /usr/include/_types/_uintmax_t.h
src/SpectralFeatures.o: /usr/include/sys/_types/_timeval.h
src/SpectralFeatures.o: /usr/include/machine/endian.h
src/SpectralFeatures.o: /usr/include/i386/endian.h /usr/include/sys/_endian.h
src/SpectralFeatures.o: /usr/include/libkern/_OSByteOrder.h
src/SpectralFeatures.o: /usr/include/libkern/i386/_OSByteOrder.h
src/SpectralFeatures.o: /usr/include/alloca.h
src/SpectralFeatures.o: /usr/include/sys/_types/_ct_rune_t.h
src/SpectralFeatures.o: /usr/include/sys/_types/_rune_t.h
src/SpectralFeatures.o: /usr/include/sys/_types/_wchar_t.h
src/SpectralFeatures.o: /usr/include/machine/types.h
src/SpectralFeatures.o: /usr/include/i386/types.h
src/SpectralFeatures.o: /usr/include/sys/_types/_dev_t.h
src/SpectralFeatures.o: /usr/include/sys/_types/_mode_t.h /usr/include/math.h
