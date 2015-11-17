//
//  spectral_features.c
//  module
//
//  Created by Christopher Latina on 11/13/15.
//  Copyright © 2015 Christopher Latina. All rights reserved.
//

#include "spectral_features.h"


void spectralFeatures_new(SpectralFeatures *features, const int signalSize)
{
    features->prevFlux = 0.0;
    features->fifo = malloc(signalSize*sizeof(float));
}

void extractSpectralFeatures(SpectralFeatures *features, float* spectrum, const int signalSize)
{
    
    float power = 0.0;
    float *spectrum_sq = malloc(signalSize*sizeof(float));
    float spectrum_sum = 0.0;
    float spectrum_abs_sum = 0.0;
    
    for (int i=0; i<signalSize; i++) {
        power += pow(spectrum[i] - features->fifo[i],2);
        spectrum_sum += spectrum[i];
        spectrum_abs_sum += fabsf(spectrum[i]);
        spectrum_sq[i] = pow(spectrum[i],2);
    }
    
    //Calculate Spectral Flux
    features->flux = sqrt(power) / (signalSize);
    
    // Low pass filter
    float alpha = 0.1;
    features->flux = (1-alpha)*features->flux + alpha * features->prevFlux;
    
    /* Save previous Spectral Flux */
    features->prevFlux = features->flux;
    
    //Calculate Spectral Flux
    // Silent frames
    if (spectrum_sum > 0.001){
        features->centroid = 0.0;
        features->crest = 0.0;
    }
    else
    {
        features->crest = max_abs_array(spectrum, signalSize) / spectrum_abs_sum;

        for (int i=0; i<signalSize; i++) {
            features->centroid += i*spectrum[i];
        }
        // TODO: This needs to be mapped to frequency and 1v / octave
        features->centroid = features->centroid / spectrum_sum;
    }
    
    /* Update fifo */
    memcpy(features->fifo,spectrum,signalSize*sizeof(float));
}

float max_abs_array(float a[], float num_elements)
{
    int i, max=1.175494e-38;
    for (i=0; i<num_elements; i++)
    {
        if (fabsf(a[i])>max)
        {
            max=fabsf(a[i]);
        }
    }
    return(max);
}

float getSpectralFlux(SpectralFeatures *features){
    float thresh = 0.01;
    int onset = 0;
    /* Print if greater than threshold */
    if(features->flux > thresh){
        onset = 1;
        printf("Flux: %i, %f\n", onset, features->flux);
    }
    
    return features->flux;
}

float getSpectralCrest(SpectralFeatures *features){
    return features->crest;
}

float getSpectralFlatness(SpectralFeatures *features){
    return features->flatness;
}

float getSpectralRolloff(SpectralFeatures *features){
    return features->rolloff;
}

float getSpectralCentroid(SpectralFeatures *features){
//    printf("Centroid: %f\n", features->centroid);
    return features->centroid;
    
}

float getRMS(SpectralFeatures *features){
    return features->rms;
}