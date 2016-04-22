//
//  SpectralFeatures.cpp
//  module
//
//  Created by Christopher Latina on 11/24/15.
//  Copyright © 2016 Christopher Latina. All rights reserved.
//

#include "SpectralFeatures.h"

/* Constructor */
SpectralFeatures::SpectralFeatures () {
    // Empty, use init
}

/* Destructor */
SpectralFeatures::~SpectralFeatures(){
    reset();
}

/* Iniit function */
void SpectralFeatures::init (int numBins, int fs) {
    binSize = numBins;
    sampleRate = fs;
    prevFlux = 0.0;
    fifo = new float[binSize];
    fifo = initArray(fifo, binSize);
    
    t_threshTime = Clock::now();
    delayTime = 0.01;
    thresh = 0.7;
    
    minThresh = 1e-20;
    minBin = 0;
    maxBin = binSize;
    
    power = 0.0;
    spectrum_sq = new float[binSize];
    spectrum_sum = 0.0;
    spectrum_abs_sum = 0.0;
    log_spectrum_sum = 0.0;
    halfwave = 0.0;
}

/* Reset function */
void SpectralFeatures::reset(){
    delete [] fifo;
    delete [] spectrum_sq;
    
}

void SpectralFeatures::setFilterParams(int minBin, int maxBin){
    this->minBin = minBin;
    this->maxBin = maxBin;
    
    if(minBin < 0){
        minBin = 0;
    }
    if(maxBin > binSize ){
        maxBin = binSize;
    }
    if(maxBin < 0){
        maxBin = 0;
    }
    if(minBin >= maxBin){
        minBin = maxBin -1;
        if(minBin == -1){
            minBin = 0;
            maxBin = 1;
        }
    }
    
    //printf("minBin: %i, maxBin: %i\n", minBin, maxBin);
}

int SpectralFeatures::getBinSize(){
    return binSize;
}

/*  Method to extract spectral features.
    Input arguments are the magnitude spectrum and block size */
void SpectralFeatures::extractFeatures(float* spectrum)
{
    power = 0.0;
    spectrum_sum = 0.0;
    spectrum_abs_sum = 0.0;
    halfwave = 0.0;
    log_spectrum_sum = 0.0;
    
    for (int i=minBin; i<maxBin; i++) {
        // Calculate the difference between the current block and the previous block's spectrum
        float diff = spectrum[i] - fifo[i];
        
        // Half wave recitify the diff.
        halfwave += (diff + fabsf(diff))/2.0;
        
        //Calculate the square
        power += spectrum[i] * spectrum[i];
        
        //Sum of the magnitude spectrum
        spectrum_sum += spectrum[i];
        
        //Sum of the absolute value of the magnitude spectrum
        spectrum_abs_sum += fabsf(spectrum[i]);
        
        //Logarithmic sum of the magnitude spectrum
        log_spectrum_sum += log(spectrum[i]);
        
        //Square of the magnitude spectrum
        spectrum_sq[i] = spectrum[i] * spectrum[i];
    }
    
    //Calculate RMS
    rms = sqrtf((1/(float)(maxBin-minBin)) * power);
    
    /* Update fifo */
    setFifo(spectrum,binSize);
    
    /* Calculate Spectral Flux */
    calculateSpectralFlux(halfwave);
    
    // Silent frames
    centroid = 0.0;
    crest = 0.0;
    
    if (spectrum_sum > 0.001){
        //Calculate Spectral Crest
        calculateSpectralCrest(spectrum, spectrum_abs_sum);
        
        //Calculate Spectral Centroid
        calculateSpectralCentroid(spectrum, spectrum_sum);
        
        //Calculate Spectral Flatness
        calculateSpectralFlatness(log_spectrum_sum, spectrum_sum);
    }
}

void SpectralFeatures::calculateSpectralFlux(float halfwave)
{
    //Calculate Spectral Flux
    flux = halfwave / (binSize);
    
    // Low pass filter
    float alpha = 0.1;
    flux = (1-alpha)*flux + alpha * prevFlux;
    
    /* Save previous Spectral Flux */
    prevFlux = flux;
}

void SpectralFeatures::calculateSpectralCentroid(float* spectrum, float spectrum_sum)
{
    centroid = 0.0;
    for (int i=0; i<binSize; i++) {
        centroid += i*spectrum[i];
    }
    centroid = centroid / spectrum_sum;
    
    // Convert centroid from bin index to frequency
    centroid = (centroid / (float) binSize) * (sampleRate / 2);
}

void SpectralFeatures::calculateSpectralCrest(float* spectrum, float spectrum_abs_sum){
    crest = max_abs_array(spectrum, binSize) / spectrum_abs_sum;
}

void SpectralFeatures::calculateSpectralFlatness(float log_spectrum_sum, float spectrum_sum) {
    if((spectrum_sum / binSize) > minThresh)
        flatness = exp(log_spectrum_sum / (float) binSize) / (spectrum_sum / (float) binSize);
    else
        flatness = 0.0;
}

float SpectralFeatures::getTimePassedSinceLastOnsetInMs(){
    return ms.count();
}

float SpectralFeatures::getOnset(float threshold, float interOnsetinterval){

    // Reset onset and clock
    onset = 0;
    timeCompare = Clock::now();
    ms = std::chrono::duration_cast<milliseconds>(timeCompare - t_threshTime);
    // printf("TimePassed: .... %lld\n", ms.count());
    
    /* Print and send voltage if spectral flux is greater than threshold */
    if(flux > thresh && ms.count() >= interOnsetinterval){
        onset = 1;
        printf("Onset: %i, Flux: %f, Thresh: %f\n", onset, flux, thresh);
        
        // Update the last read time of threshold
        t_threshTime = Clock::now();
    }
    
    return onset;
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
    return centroid;
}

float SpectralFeatures::getRMS(){
    return rms;
}
