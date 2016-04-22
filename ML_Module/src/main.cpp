//
//  main.c
//  module
//
//  Created by Christopher Latina on 11/5/15.
//  Copyright © 2015 Christopher Latina. All rights reserved.
//


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "portaudio.h"
#include "FFT.h"
#include "SpectralFeatures.h"
#include "Lfo.h"
#include "FeatureCommunication.hpp"

#ifdef __arm__
    #include <wiringPi.h>
#endif

#ifdef __APPLE__
    #import <CoreAudio/CoreAudio.h>
    #import <AudioToolbox/AudioToolbox.h>
    #import <AudioUnit/AudioUnit.h>
    #import <CoreServices/CoreServices.h>
    #import <Carbon/Carbon.h>
#endif
using namespace std;


/*
 ** Note that many of the older ISA sound cards on PCs do NOT support
 ** full duplex audio (simultaneous record and playback).
 ** And some only support full duplex at lower sample rates.
 */
#define SAMPLE_RATE         44100
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
float onsetThreshold;
float interOnsetInterval;
int activeFeature = 0;

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
            
            // Get minBin and maxBin values
            printf("PINS: %f, %f, %f, %f, %f, %f, %f, %f \n\n", communicator->getADCValue(0), communicator->getADCValue(1),
                   communicator->getADCValue(2), communicator->getADCValue(2), communicator->getADCValue(3),
                   communicator->getADCValue(5),communicator->getADCValue(6), communicator->getADCValue(7));
            
            if(communicator->getADCValue(6) != 55555){
                // Manual scaling for voltage offset
                minBin = (int) roundf((features->getBinSize() * (communicator->getADCValue(6)) - 33) * (512 / 462.0));
            } else {
               minBin = 0;
            }
            
            if(communicator->getADCValue(7) != 55555){
                // Manual scaling for voltage offset
                maxBin = (int) roundf(( features->getBinSize() * (communicator->getADCValue(7)) - 33) * (512 / 462.0));
            } else {
                maxBin = features->getBinSize();
            }
    
            //Update threshold
            onsetThreshold = communicator->getADCValue(1);
            if(onsetThreshold == 55555){
                onsetThreshold = 0.7;
            } else {
                onsetThreshold = 5 * onsetThreshold;
            }
            
            printf("Onset Thresh: %f \n", onsetThreshold);
            
            // Update inter-onset interval (in ms) 0 - 4.096 s
            interOnsetInterval = communicator->getADCValue(0);
            if(interOnsetInterval == 55555){
                interOnsetInterval = 0.01;
            } else {
                interOnsetInterval = (float) interOnsetInterval * communicator->getResolution() / 10.0;
            }
            printf("Interonset: %f \n\n", interOnsetInterval);
            
            // Set voltage to low if 10ms has passed
            if(features->getTimePassedSinceLastOnsetInMs() >= 10){
                communicator->writeGPIO(26,0,0);
            }
            
            // Set the filter params
            features->setFilterParams(0, 512);
            
            // Check which feature to output
            if(communicator->readDigital(23) == 1){
                printf("Updating activeFeature! \n");
                activeFeature = (activeFeature+1) % NUM_FEATURES;
            }
            
            /*** Extract Features ***/
            
            // Extract Spectral features for the block
            features->extractFeatures(spectrum);
            
            //printf("Thresh: %f \n", onsetThreshold);
            // Get the onset
            onset = features->getOnset(onsetThreshold, interOnsetInterval);
            if(onset){
                communicator->writeGPIO(26,1,0);
            }
            
            if(activeFeature == 0){
                // Map Spectral Centroid and Rolloff to a sine wave
                centroid = features->getSpectralCentroid();
                
                printf("Centroid: %f \n", centroid);
                synthesizer->setLfoType(CLfo::LfoType_t::kSine);
                synthesizer->setParam(CLfo::LfoParam_t::kLfoParamAmplitude, 1.0f);
                synthesizer->setParam(CLfo::LfoParam_t::kLfoParamFrequency, centroid);
                
                // Mapped to frequency and 1v / octave
                communicator->writeGPIO(16, (int) roundf(communicator->scaleFrequency(centroid) * 102.4), 1);
            } else if(activeFeature == 1){
                // Map Spectral Flatness to White noise
                flatness = features->getSpectralFlatness();
                
                printf("Spectral Flatness: %f \n", flatness);
                
                // ***TODO***: Make sure flatness is between 0 and 1.0
                if(flatness > 1.0){
                    flatness = 1.0;
                }
                synthesizer->setParam(CLfo::LfoParam_t::kLfoParamFrequency, 440);
                synthesizer->setLfoType(CLfo::LfoType_t::kNoise);
                synthesizer->setParam(CLfo::LfoParam_t::kLfoParamAmplitude, flatness);
            }
            
            // Map RMS to DC voltage
            
             printf("------ \n\n");
            
        } else{
            gNumNoInputs += 1;
        }
        
        for( i=0; i<framesPerBuffer; i++ )
        {
            *out++ = 0.6 * *in++;     /* left  - clean */
            *out++ = 0.6 * synthesizer->getNext();     /* right - clean */ // add ++ to interleave for stereo
        }
    }
    return paContinue;
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

    const   PaDeviceInfo *deviceInfo;
    numDevices  = Pa_GetDeviceCount();
    for(int i=0; i<numDevices; i++ )
    {
    	deviceInfo = Pa_GetDeviceInfo( i );
	printf("Device Info: %s, Host API: %d, SampleRate: %f\n",deviceInfo->name, deviceInfo->hostApi,
 deviceInfo->defaultSampleRate);
    }

    if(numDevices > 1){
        // Set input to USB -- device 1 -- for testing on OSX, switch to 0
        inputParameters.device = 1;
    } else {
        inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    }
    if (inputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default input device.\n");
        goto error;
    }
    
    //This may need to be stereo if testing on OSX
    inputParameters.channelCount = 1;       /* mono input */
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
