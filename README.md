# MachineListening
Eurorack Module for Machine Listening

## Getting Setup
Please refer to my Embedded Audio tutorials to setup your Raspberry Pi for development, including connecting headless and setting up internet sharing.
https://github.com/chrislatina/EmbeddedAudio

These tutorials run on CCRMA's Satellite build. I've setup this environment on a clean Raspian build on the Raspberry Pi Model B+ unit. It is important to update to linux 4.x to allow for i2s mmap configuration for routing audio.

Run ```sudo rpi-update``` after making sure you have enough memory available on your flash drive. If you are worried about using up all of your memory while updating, clean up with locale purge and deb orphan before running the update. More info can be found here: http://www.intraipsum.se/blog/2012/07/14/raspberry-pi-clean-purge/

To check your linux version, run ```uname -a```

## Installation
After cloning this repository, download and install port audio. There is a tutorial for compiling on Linux here: http://portaudio.com/docs/v19-doxydocs/compile_linux.html

Once connected to the internet, the easiest way is to use wget pointing to the latest source of port audio.

```wget http://www.portaudio.com/archives/pa_stable_v19_20140130.tgz```

Unpack the tgz
```tar zxvf fileNameHere.tgz```

When compiling port audio, do so without Jack. The machine listening firmware is intentioanlly compiled without the flag for Jack. This simplifies reading from and writing to the respective audio cards.
```./configure —without-jack```

Download the libsound-dev libraries. You'll need to be connected to the internet (forward by running headless) to use 

```sudo apt-get install libasound-dev```

I suggest using ALSA. The makefile includes all of the following 

```-lrt -lasound -ljack -lpthread```

Use scp to copy a local wav file to your pi. 
```scp file.wav pi@192.168.2.2:~/file.wav```

On the pi, test playback using aplay. Depending upon your audio card setup (explained below) this may play back from the pi's default audio out. 
```aplay Cello.wav```

You may need to replace the libportaudio.a file (there are both versions compiled already for raspi and OSX) with the linux version inside /Module/ML_Module/include/portaudio

```cp /PORTAUDIO/DIR/lib/.libs/libportaudio.a /MachineListening/ML_Module/include/portaudio```

## WiringPi
Next you'll need to download and compile the wiringPi library for for reading and writing to and from GPIO pins. This is very straight forward and simply requires running the build script. The library dependencies in the makefile for compiling this library on Raspberry Pi already reference the wiringPi library.

http://wiringpi.com/download-and-install/

Now you can cd into the /MachineListening/ML_Module directory and run `make`

## Setting up your Audio Cards

First you'll need to install your hifiberry pcm5012a on pi. Below I've posted to web-references if you need more information.

https://www.hifiberry.com/guides/hifiberry-software-configuration/
https://slug.blog.aeminium.org/2015/05/09/raspberry-pi-2-model-b-pcm5102a-i2s/

Firs you'll need to remove specific drivers from your device's blacklist. The filename is not necessarily consistent. On my device I edited the following file:
```sudo nano /etc/modprobe.d/alsa-base-blacklist.conf ```

Comment out all of the following devices

    #blacklist i2c-bcm2708
    #blacklist snd-soc-pcm512a
    #blacklist snd-soc-wm8804
    #blacklist snd-soc-bcm2708
    #blacklist snd-soc-bcm2708-i2s
    #blacklist bcm2708-dmaengine
    #blacklist snd-soc-pcm5102a
    #blacklist snd-soc-rpi-pcm5102a

Edit /etc/modules
```sudo nano /etc/modules```

comment out the device ```#snd-bcm2835```

Add the following devices

    snd_soc_bcm2708
    snd_soc_bcm2708_i2s
    bcm2708_dmaengine
    snd_soc_pcm5102a
    snd_soc_rpi_pcm5102a

Next, you must configure your USB-C audio card for audio capture. Scroll down and set snd-usb-audio to index 1.

```sudo nano /etc/modprobe.d/alsa-base.conf```
    
    options snd-usb-audio index=1

Now update the current audio card. Add the following lines to asound.conf

```sudo nano /etc/asound.conf```

    pcm.!default  {
      type hw card 0
    }
    ctl.!default {
      type hw card 0
    }
    pcm.hifiberry {
    type hw card 0
    }

Edit the config script
```sudo nano /boot/config.txt```

Set the following. Everything else is commented out. Make sure you're pi is updated to linux 4.x so that i2s and mmap works for audio card routing in port audio.

    #uncomment to overclock the arm. 700 MHz is the default.
    arm_freq=900
    core_freq=250
    sdram_freq=450
    over_voltage=2
    # memory split:
    gpu_mem=16
    # enable i2c:
    dtparam=i2c_arm=on
    # enable spi:
    dtparam=spi=on
    # enable i2s:
    dtparam=i2s=on
    # i2s / DAC driver:
    dtoverlay=i2s-mmap
    dtoverlay=hifiberry-dac
    #dtoverlay=rpi-proto

You can reboot your soundcard directly,
```sudo /etc/init.d/alsa-utils restart```

But to properly reconfigure, reboot the entire device
```sudo reboot```

Upon logging back in, check your soundcard configuration,
```cat /proc/asound/cards /proc/asound/modules``` or ```aplay -l```

If correct, the pcm5012a should be assigned to card 0 and the USB-C device assigned to card 1.

    **** List of PLAYBACK Hardware Devices ****
    card 0: sndrpihifiberry [snd_rpi_hifiberry_dac], device 0: HifiBerry DAC HiFi pcm5102a-hifi-0 []
      Subdevices: 0/1
      Subdevice #0: subdevice #0
    card 1: Device [C-Media USB Audio Device], device 0: USB Audio [USB Audio]
      Subdevices: 1/1
      Subdevice #0: subdevice #0
    FIX nmap OVERLAP


dtoverlay=i2s-mmap

## Optimizing Raspbery Pi for realtime streaming Audio

I highy recommend reading this wiki on low latency audio http://wiki.linuxaudio.org/wiki/raspberrypi. Overclocking (http://elinux.org/RPiconfig#Overclocking) is probably not necessary but you can experiment with this if you receive dropouts.

##Booting your program
Edit the boot script to run your program by default. You'll need to make sure to run the startup.py script to assign the GPIO pins.

```sudo nano ~/.bash_profile```

The second call runs the Machine Listening firmware. The commented commands run terminal tedium's test patches using pd.

    sudo python ~/terminal_tedium/software/pullup.py
    sudo ~/MachineListening/ML_Module/mycc
    #sudo ~/pd/bin/pd -nogui -noadc -rt ~/terminal_tedium/software/D_io_test_pcm5102a.$
    #sudo ~/pd/bin/pd -rt -nogui -verbose ~/terminal_tedium/software/adc_test.pd

