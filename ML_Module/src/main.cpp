//
//  main.c
//  module
//

//Copyright (c) 2015-2017 Christopher Latina
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, and / or merge, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// This software is protected under Creative Commons Attribution-NonCommercial-ShareAlike. Please respect this commitment
// https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <chrono>

#include "portaudio.h"
#include "FFT.h"
#include "SpectralFeatures.h"
#include "Lfo.h"
#include "FeatureCommunication.hpp"



#ifdef __APPLE__
    #import <CoreAudio/CoreAudio.h>
    #import <AudioToolbox/AudioToolbox.h>
    #import <AudioUnit/AudioUnit.h>
    #import <CoreServices/CoreServices.h>
    #import <Carbon/Carbon.h>
#endif
using namespace std;

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;

Clock::time_point t_commTime;
Clock::time_point timeCompare;
milliseconds ms;

/*
 ** Note that many of the older ISA sound cards on PCs do NOT support
 ** full duplex audio (simultaneous record and playback).
 ** And some only support full duplex at lower sample rates.
 */
#define SAMPLE_RATE         48000
#define PA_SAMPLE_TYPE      paFloat32
#define FRAMES_PER_BUFFER   1024
#define NUM_FEATURES        3

typedef float SAMPLE;
FFT *fft;
SpectralFeatures *features;
float *spectrum;
CLfo *synthesizer;
FeatureCommunication *communicator;

//Feature variables
int onset;
float centroid = 0.0;
float flatness = 0.0;
int minBin;
int maxBin;
float volume = 0.0;
float volume2 = 0.0;
float onsetThreshold;
float interOnsetInterval;
int activeFeature;
bool updateFeature = false;

static int gNumNoInputs = 0;
int numDevices = -1;
/* This routine will be called by the PortAudio engine when audio is needed.
 ** It may be called at interrupt level on some machines so don't do anything
 ** that could mess up the system like calling malloc() or free().
 */

static int audioCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData )
{
    SAMPLE *out = (SAMPLE*)outputBuffer;
    const SAMPLE *in = (const SAMPLE*)inputBuffer;
    unsigned int i;
    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;
    (void) userData;
    
    //Reduce volume
//    SAMPLE* scaledIn = new SAMPLE[framesPerBuffer];
//    for (int s = 0; s < framesPerBuffer; s++){
//        scaledIn[s] = 0.9*in[s];
//    }

    /* Initialize features to zero */
    if( inputBuffer == NULL)
    {
        for( i=0; i<framesPerBuffer; i++ )
        {
            *out++ = 0;  /* left - silent */
            *out++ = 0;  /* right - silent */
        }
        gNumNoInputs += 1;
    }
    else if(in != NULL)
    {
        spectrum = fft->getSpectrum(in);
        if(!isnan(*spectrum) && *spectrum != INFINITY)
        {
            /*** Get Parameters From Hardware ***/
            
            // Case where hardware is not active. IE running on Mac OSX
            if(communicator->checkIfValid(communicator->getADCValue(0))){
                
                // Get volume
                volume  = (communicator->getADCValue(5) - 0.07) / 2.0;
                if(volume < 0)
                    volume = 0;
                volume2 = (communicator->getADCValue(2) - 0.07) / 2.0;
                if(volume2 < 0)
                    volume2 = 0;
                
                //Update minBin and maxBin
                // Manual scaling for voltage offset
                minBin = (int) roundf((features->getBinSize() * (communicator->getADCValue(4)) - 34) * (512 / 462.0));
                maxBin = (int) roundf(( features->getBinSize() * (communicator->getADCValue(1)) - 34) * (512 / 462.0));
                
                // Update inter-onset interval (in ms) 0 - 4.096 s
                interOnsetInterval = communicator->getADCValue(0);
                interOnsetInterval = (float) interOnsetInterval * communicator->getResolution() / 10.0;
                
                //Update threshold
                onsetThreshold = (communicator->getADCValue(3) - 0.05);
            } else { //Set defaults
                volume = 0.6;
                volume2 = 0.6;
                minBin = 0;
                maxBin = features->getBinSize();
                onsetThreshold = 0.25;
                interOnsetInterval = 0.01;
            }
            
            // Set the filter params using minBin and maxBin
            features->setFilterParams(minBin, maxBin);
    
            // Set voltage to low if 10ms has passed
            if(features->getTimePassedSinceLastOnsetInMs() >= 10){
                communicator->writeGPIO(12,0,0);
            }
            
            /*** Extract Spectral Features for the block ***/
            features->extractFeatures(spectrum);
            
            // Get the onset
            onset = features->getOnset(onsetThreshold, interOnsetInterval);
            if(onset){
                communicator->writeGPIO(12,1,0);
            }
            
            /***  Check which feature to output ***/
            if(communicator->readDigital(25) == 0 && !updateFeature){
                //printf("Updating activeFeature! \n");
                activeFeature = (activeFeature+1) % NUM_FEATURES;
                updateFeature = true;
            }
            
            if(communicator->readDigital(25) == 1 && updateFeature){
               updateFeature = false;
            }
            
            if(activeFeature == 0){
                // Map Spectral Centroid and Rolloff to a sine wave
                centroid = features->getSpectralCentroidInFreq();
                //printf("Centroid: %f \n", centroid);
                
                // Map RMS to DC voltage
                //printf("RMS: %f, Crest: %f, \n", features->getRMS(), features->getSpectralCrest());
                
                synthesizer->setLfoType(CLfo::LfoType_t::kSine);
                synthesizer->setParam(CLfo::LfoParam_t::kLfoParamAmplitude, 1.0f);
                
                // Mapped to frequency and 1v / octave
                // Set voltage to low if 10ms has passed
                timeCompare = Clock::now();
                ms = std::chrono::duration_cast<milliseconds>(timeCompare - t_commTime);
                
                //if(ms.count() >= 10){
                    synthesizer->setParam(CLfo::LfoParam_t::kLfoParamFrequency, centroid);
                    t_commTime = Clock::now();
                
                    communicator->writeGPIO(16, (int) roundf(communicator->scaleFrequency(centroid)), 1);
                //}
                
            } else if(activeFeature == 1){
                // Map Spectral Flatness to White noise
                flatness = features->getSpectralFlatness();
                //printf("Spectral Flatness: %f \n", flatness);
                
                if(flatness > 1.0){
                    flatness = 1.0;
                }
                synthesizer->setParam(CLfo::LfoParam_t::kLfoParamFrequency, 880);
                synthesizer->setLfoType(CLfo::LfoType_t::kNoise);
                synthesizer->setParam(CLfo::LfoParam_t::kLfoParamAmplitude, flatness);
                
            } else if(activeFeature == 2){
                // Map Spectral Flatness to DC
                flatness = features->getSpectralFlatness();
                //printf("Spectral Flatness: %f \n", flatness);
                
                if(flatness > 1.0){
                    flatness = 1.0;
                }
                synthesizer->setParam(CLfo::LfoParam_t::kLfoParamFrequency, 18000);
                synthesizer->setLfoType(CLfo::LfoType_t::kDC);
                synthesizer->setParam(CLfo::LfoParam_t::kLfoParamAmplitude, flatness);
            }
            
            
        } else{
            gNumNoInputs += 1;
        }
        
        for( i=0; i<framesPerBuffer; i++ )
        {
            *out++ = volume * *in++;     /* left  - clean */
            *out++ = volume2 * synthesizer->getNext();     /* right - clean */ // add ++ to interleave for stereo
        }
    }
    return paContinue;
}

void printAllPins(){
    printf("PINS: %f, %f, %f, %f, %f, %f, %f, %f \n\n", communicator->getADCValue(0), communicator->getADCValue(1), communicator->getADCValue(2), communicator->getADCValue(2), communicator->getADCValue(3), communicator->getADCValue(5),communicator->getADCValue(6), communicator->getADCValue(7));
}

/*******************************************************************/
int main(void);
int main(void)
{
    PaStreamParameters inputParameters, outputParameters;
    PaStream *stream;
    PaError err;

    err = Pa_Initialize();
    if( err != paNoError ) goto error;

//	printAllPins();   

    const   PaDeviceInfo *deviceInfo;
    numDevices  = Pa_GetDeviceCount();
    for(int i=0; i<numDevices; i++ )
    {
    	deviceInfo = Pa_GetDeviceInfo( i );
	printf("Device Info: %s, Host API: %d, SampleRate: %f\n",deviceInfo->name, deviceInfo->hostApi,
 deviceInfo->defaultSampleRate);
	printf("Audio I/O: %d/%d\n",deviceInfo->maxInputChannels, deviceInfo->maxOutputChannels);
    }

    if(numDevices > 1){
        // Set input to USB -- device 1 -- for testing on OSX, switch to 0
        // Terminal Tedium with wm8731 use  device 0
        inputParameters.device = 0;
    } else {
        inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    }
    if (inputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default input device.\n");
        goto error;
    }
    
    //This may need to be stereo if testing on OSX
    //inputParameters.channelCount = 1;       /* mono input */
    //Terminal Tedium with wm8731 setting to Stereo
    inputParameters.channelCount = 2;       /* Stereo input */
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    
    //outputParameters.device = 1;
    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default output device.\n");
        goto error;
    }
    outputParameters.channelCount = 2;       /* stereo output */
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    
    /* Instantiate FFT */
    fft = new FFT();
    fft->init(FRAMES_PER_BUFFER);
    spectrum = new float[FRAMES_PER_BUFFER/2];
    
    /* Instantiate Spectral Features */
    features = new SpectralFeatures();
    features->init(FRAMES_PER_BUFFER/2, SAMPLE_RATE);
    synthesizer = new CLfo(SAMPLE_RATE);
    
    communicator = new FeatureCommunication();
    
    activeFeature = 0;
    
    err = Pa_OpenStream(
                        &stream,
                        &inputParameters,
                        &outputParameters,
                        SAMPLE_RATE,
                        FRAMES_PER_BUFFER,
                        0, /* paClipOff, */  /* we won't output out of range samples so don't bother clipping them */
                        audioCallback,
                        NULL );
    if( err != paNoError ) goto error;
    
    err = Pa_StartStream( stream );
    if( err != paNoError ) goto error;
    
    printf("Hit ENTER to stop program.\n");
    getchar();
    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;
    
    printf("Finished. gNumNoInputs = %d\n", gNumNoInputs );
    Pa_Terminate();
    return 0;
    
error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return -1;
}
