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
#include "fft.h"
#include "spectral_features.h"



#import <CoreAudio/CoreAudio.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AudioUnit/AudioUnit.h>
#import <CoreServices/CoreServices.h>
#import <Carbon/Carbon.h>

/*
 ** Note that many of the older ISA sound cards on PCs do NOT support
 ** full duplex audio (simultaneous record and playback).
 ** And some only support full duplex at lower sample rates.
 */
#define SAMPLE_RATE         (44100)
#define PA_SAMPLE_TYPE      paFloat32
#define FRAMES_PER_BUFFER   (512)


typedef float SAMPLE;

static int onsetCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData );

FFT fft;
float *spectrum;
float fifo [FRAMES_PER_BUFFER] = { 0 };
float prevFlux = 0.0;

static int gNumNoInputs = 0;
/* This routine will be called by the PortAudio engine when audio is needed.
 ** It may be called at interrupt level on some machines so don't do anything
 ** that could mess up the system like calling malloc() or free().
 */


static int onsetCallback( const void *inputBuffer, void *outputBuffer,
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
    const int signalSize = FRAMES_PER_BUFFER;
    float buf[signalSize];
    
    if( inputBuffer == NULL )
    {
        for( i=0; i<framesPerBuffer; i++ )
        {
            *out++ = 0;  /* left - silent */
            *out++ = 0;  /* right - silent */
        }
        gNumNoInputs += 1;
    }
    else
    {

        spectrum = getSpectrum(&fft, in, signalSize);

        //Calculate Spectral Flux
        float power = 0.0;
        float flux = 0.0;

        for (int i=0; i<signalSize; i++) {
            power += pow(spectrum[i] - fifo[i],2);
        }
        
        flux = sqrt(power) / (signalSize);
        
        // TODO: Low pass filter
        float thresh = 0.01;
        int onset = 0;
        float alpha = 0.1;
        
        flux = (1-alpha)*flux + alpha * prevFlux;
        if(flux > thresh){
            onset = 1;
            printf("Flux: %i, %f\n", onset, flux);
            
        }
        prevFlux = flux;
        
        //Update fifo
        memcpy(fifo,spectrum,signalSize*sizeof(float));
        
//        printf("Output:");
//        for(i=0;i<signalSize;i++)
//        {
//            buf[i] = (spectrum[i])/ (signalSize/2);
//            printf("%f",buf[i]);
//        }

        
        for( i=0; i<framesPerBuffer; i++ )
        {
            *out++ = 0; //*in++;     /* left  - clean */
            *out++ = 0; //*in++;     /* right - clean */
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
    
    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    if (inputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default input device.\n");
        goto error;
    }
    inputParameters.channelCount = 2;       /* stereo input */
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    
    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default output device.\n");
        goto error;
    }
    outputParameters.channelCount = 2;       /* stereo output */
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    
    /* Allocated memory for FFT */

    fft_new(&fft, FRAMES_PER_BUFFER);
    spectrum = malloc(FRAMES_PER_BUFFER * sizeof(double));
    
    
    err = Pa_OpenStream(
                        &stream,
                        &inputParameters,
                        &outputParameters,
                        SAMPLE_RATE,
                        FRAMES_PER_BUFFER,
                        0, /* paClipOff, */  /* we won't output out of range samples so don't bother clipping them */
                        onsetCallback,
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

