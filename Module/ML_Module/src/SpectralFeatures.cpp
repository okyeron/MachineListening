//
//  SpectralFeatures.cpp
//  module
//
//  Created by Christopher Latina on 11/24/15.
//  Copyright © 2015 Christopher Latina. All rights reserved.
//

#include "SpectralFeatures.hpp"

SpectralFeatures::SpectralFeatures (int numSamples, int fs) {
    signalSize = numSamples;
    sampleRate = fs;
    prevFlux = 0.0;
    fifo = (float*) malloc(signalSize*sizeof(float));
    fifo = initArray(fifo, signalSize);
}

/*  Method to extract spectral features.
    Input arguments are the magnitude spectrum and block size */
void SpectralFeatures::extractFeatures(float* spectrum)
{
    float power = 0.0;
    float spectrum_sq[signalSize];
    float spectrum_sum = 0.0;
    float spectrum_abs_sum = 0.0;
    
    for (int i=0; i<signalSize; i++) {
        // Calculate the difference between the current block and the previous block's spectrum
        float diff = spectrum[i] - fifo[i];
        
        //Calculate the square
        power += diff * diff;
        
        //Sum of the magnitude spectrum
        spectrum_sum += spectrum[i];
        spectrum_abs_sum += fabsf(spectrum[i]);
        spectrum_sq[i] = spectrum[i] * spectrum[i];
    }
    
    /* Update fifo */
    setFifo(spectrum,signalSize);
    
    /* Calculate Spectral Flux */
    calculateSpectralFlux(power);
    
    // Silent frames
    centroid = 0.0;
    crest = 0.0;
    
    if (spectrum_sum > 0.001){
        //Calculate Spectral Crest
        crest = max_abs_array(spectrum, signalSize) / spectrum_abs_sum;
        
        //Calculate Spectral Centroid
        calculateSpectralCentroid(spectrum, spectrum_sum);
    }
}

void SpectralFeatures::calculateSpectralFlux(float power)
{
    //Calculate Spectral Flux
    flux = sqrt(power) / (signalSize);
    
    // Low pass filter
    float alpha = 0.01;
    flux = (1-alpha)*flux + alpha * prevFlux;
    
    /* Save previous Spectral Flux */
    prevFlux = flux;
}

void SpectralFeatures::calculateSpectralCentroid(float* spectrum, float spectrum_sum)
{
    centroid = 0.0;
    for (int i=0; i<signalSize; i++) {
        centroid += i*spectrum[i];
    }
    centroid = centroid / spectrum_sum;
    
    // Convert centroid from bin index to frequency
    centroid = centroid / signalSize * sampleRate / 2;
    
    // TODO: This needs to be mapped to frequency and 1v / octave
    
}

float SpectralFeatures::max_abs_array(float a[], float num_elements)
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

float SpectralFeatures::getSpectralFlux(){
    float thresh = 0.01;
    int onset = 0;
    /* Print if greater than threshold */
    if(flux > thresh){
        onset = 1;
        printf("Onset: %i, Flux: %f\n", onset, flux);
        printf("Centroid: %f, \n", centroid);
    }
    
    return flux;
}

float SpectralFeatures::getSpectralCrest(){
    return crest;
}

float SpectralFeatures::getSpectralFlatness(){
    return flatness;
}

float SpectralFeatures::getSpectralRolloff(){
    return rolloff;
}

float SpectralFeatures::getSpectralCentroid(){
//    printf("Centroid: %f, \n", centroid);
    return centroid;
    
}

float SpectralFeatures::getRMS(){
    return rms;
}