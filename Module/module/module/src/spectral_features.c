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

float extractSpectralFeatures(SpectralFeatures *features, float* spectrum, const int signalSize)
{
    //Calculate Spectral Flux
    float power = 0.0;
    float flux = 0.0;
    
    for (int i=0; i<signalSize; i++) {
        power += pow(spectrum[i] - features->fifo[i],2);
    }
    
    flux = sqrt(power) / (signalSize);
    
    // Low pass filter
    float thresh = 0.01;
    int onset = 0;
    float alpha = 0.1;
    flux = (1-alpha)*flux + alpha * features->prevFlux;
    
    /* Print if greater than threshold */
    if(flux > thresh){
        onset = 1;
        printf("Flux: %i, %f\n", onset, flux);
        
    }
    
    /* Save previous Spectral Flux */
    features->prevFlux = flux;
    
    /* Update fifo */
    memcpy(features->fifo,spectrum,signalSize*sizeof(float));

    return flux;

}