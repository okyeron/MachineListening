//
//  SpectralFeatures.cpp
//  module
//
//  Created by Christopher Latina on 11/24/15.
//  Copyright © 2016 Christopher Latina. All rights reserved.
//

#include "SpectralFeatures.h"
#include "Synthesis.h"

#define A4_HZ (440.0)
#define OCTAVE_OFFSET (5)

SpectralFeatures::SpectralFeatures (int numBins, int fs) {
    binSize = numBins;
    sampleRate = fs;
    prevFlux = 0.0;
    fifo = new float[binSize];
    fifo = initArray(fifo, binSize);
    
    t_threshTime = Clock::now();
    delayTime = 0.01;
    thresh = 0.7;
    
    fc_communicator = new FeatureCommunication();
    
    minThresh = 1e-20;
    lp = 0.0;
    hp = binSize;
    
    power = 0.0;
    spectrum_sq = new float[binSize];
    spectrum_sum = 0.0;
    spectrum_abs_sum = 0.0;
    log_spectrum_sum = 0.0;
    halfwave = 0.0;
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
    
    // Find lowpass and hp values
    lp = binSize * round(fc_communicator->getADCValue(6));
    hp = binSize * round(fc_communicator->getADCValue(7));
    if(lp == -1 || hp == -1){
        lp = 0;
        hp = binSize;
    }
    
    for (int i=lp; i<hp; i++) {
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
    rms = sqrtf((1/(float)(hp -lp)) * power);
    
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
    
    //Write the centroid value to the console
    printf("Centroid: %i, \n", (int) roundf(SpectralFeatures::scaleFrequency(centroid) * 102.4));

    // TODO: This needs to be mapped to frequency and 1v / octave
    if(fc_communicator->readDigital(25))
        fc_communicator->writeGPIO(26, (int) roundf(SpectralFeatures::scaleFrequency(centroid) * 102.4), 1);
}

float SpectralFeatures::scaleFrequency(float feature){
    if(feature > A4_HZ * 32){
        feature = A4_HZ * 32;
    }
    else if(feature < A4_HZ / 32.0){
        feature = A4_HZ / 32.0;
    }
    
    return log2f(feature/A4_HZ)+OCTAVE_OFFSET;
}

void SpectralFeatures::calculateSpectralCrest(float* spectrum, float spectrum_abs_sum){
    crest = max_abs_array(spectrum, binSize) / spectrum_abs_sum;
}

void SpectralFeatures::calculateSpectralFlatness(float log_spectrum_sum, float spectrum_sum) {
    if((spectrum_sum / binSize) > minThresh)
        flatness = exp(log_spectrum_sum / (float) binSize) / (spectrum_sum / (float) binSize);
    else
        flatness = 0.0;
    
    //printf("Flatness: %f, \n", flatness);
}

float SpectralFeatures::getSpectralFlux(){
    //Update threshold
    thresh = 5 * fc_communicator->getADCValue(1);
    if(thresh <= -1){
        thresh = 0.7;
    }
    
    // Reset onset and clock
    onset = 0;
    timeCompare = Clock::now();
    ms = std::chrono::duration_cast<milliseconds>(timeCompare - t_threshTime);
    // printf("TimePassed: .... %lld\n", ms.count());
    
    // Set voltage to low if 10ms has passed
    if(ms.count() >= 10){
        if(!fc_communicator->readDigital(25))
            fc_communicator->writeGPIO(26,0,1);
    }
    
    /* Print and send voltage if spectral flux is greater than threshold */
    if(flux > thresh && ms.count() >= delayTime){
        //printf("DelayTime: %f\n",delayTime);
        onset = 1;
        printf("Onset: %i, Flux: %f, Thresh: %f\n", onset, flux, thresh);
        
        // Update the last read time of threshold
        t_threshTime = Clock::now();
        if(!fc_communicator->readDigital(25))
            fc_communicator->writeGPIO(26,1,0);
       
    }

    // Calculate delayTime (in ms) 0 - 4.096 s
    delayTime = (float) fc_communicator->getADCValue(0) * fc_communicator->getResolution() / 10.0;
    
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
    return centroid;
    
}

float SpectralFeatures::getRMS(){
    return rms;
}
