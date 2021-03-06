//
//  SpectralFeatures.cpp
//  module
//
//Copyright (c) 2015-2017 Christopher Latina
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, and / or merge, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// This software is protected under Creative Commons Attribution-NonCommercial-ShareAlike. Please respect this commitment
// https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


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
    prevCentroid = 0.0;
    prevFlatness = 0.0;
    fifo = new float[binSize];
    initArray(fifo, binSize);
    fifoFilled = false;
    
    
    t_threshTime = Clock::now();
    
    // Initialize Features to zero
    flux = 0;
    prevFlux = 0;
    crest = 0;
    flatness = 0;
    rolloff = 0;
    centroid = 0;
    prevCentroid = 0;
    rms = 0;
    
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
    
    //Make sure filter stays within bounds
    if(minBin < 0){
        minBin = 0;
    }
    if(minBin >= binSize ){
        minBin = binSize-1;
    }
    if(maxBin > binSize ){
        maxBin = binSize;
    }
    if(maxBin < 0){
        maxBin = 1;
    }
    if(minBin >= maxBin){
        minBin = maxBin -1;
        if(minBin == -1){
            minBin = 0;
            maxBin = 1;
        }
    }
    
    this->minBin = minBin;
    this->maxBin = maxBin;
    
    //printf("minBin: %i, maxBin: %i\n", minBin, maxBin);
}

int SpectralFeatures::getBinSize(){
    return binSize;
}

int SpectralFeatures::getFilteredBinSize(){
    return maxBin - minBin;
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
    float diff_sum = 0.0;
    
    for (int i=minBin; i<maxBin; i++) {
        // Get power difference between consecutive spectra
        //diff_sum += (spectrum[i] - fifo[i])*(spectrum[i] - fifo[i]);
        diff_sum  += ((spectrum[i] - fifo[i] + fabsf(spectrum[i] - fifo[i]))/2.0)*((spectrum[i] - fifo[i] + fabsf(spectrum[i] - fifo[i]))/2.0);
        
//        // Half wave recitify the diff.
        halfwave += ((spectrum[i] - fifo[i] + fabsf(spectrum[i] - fifo[i]))/2.0);
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
    
    /* Update fifo */
    setFifo(spectrum,binSize);
    
    if (!checkSilence(power)){
        // Calculate Spectral Flux
        calculateSpectralFlux(halfwave);
        
        // Calculate Spectral Roloff
        calculateSpectralRolloff(spectrum, spectrum_sum, 0.85);
        
        //Calculate RMS
        calculateRMS(power);
        
        //Calculate Spectral Crest
        calculateSpectralCrest(spectrum, spectrum_abs_sum);
        
        //Calculate Spectral Centroid
        calculateSpectralCentroid(spectrum, power, minBin, maxBin);
        
        //Calculate Spectral Flatness
        calculateSpectralFlatness(log_spectrum_sum, spectrum_sum);
        
    }
}

bool SpectralFeatures::checkSilence(float power) {
    if (power < 0.000001) {
        return true; //return false if Silence is detected
    }
    return false;
}


void SpectralFeatures::calculateRMS(float power){
    try {
        rms = sqrtf(power / (float)(binSize*binSize));
    } catch (std::logic_error e) {
        rms = 0.0;
    }
}

void SpectralFeatures::calculateSpectralFlux(float diff_sum){
    //Calculate Spectral Flux
    flux = diff_sum / (float)(binSize);
    
    // Low pass filter (optional)
    float alpha = 0.0;
    flux = (1-alpha)*flux + alpha * prevFlux;
    
    /* Save previous Spectral Flux */
    prevFlux = flux;
}

void SpectralFeatures::calculateSpectralCentroid(float* spectrum, float power, int minBin, int maxBin){
    centroid = 0.0;
    for (int i=minBin; i<maxBin; i++) {
        centroid += i*spectrum[i]*spectrum[i];
    }
    
    try {
        centroid = centroid / (power);
    } catch (std::logic_error e) {
        centroid = 0.0;
    }
    
    centroid = (centroid / (float) binSize);
    centroid = centroid;
    
    // Low pass filter
    float alpha = 0.5;
    centroid = (1-alpha)*centroid + alpha * prevCentroid;
    
    prevCentroid = centroid;
}

void SpectralFeatures::calculateSpectralCrest(float* spectrum, float spectrum_abs_sum){
    crest = 0.0;
    try {
        crest = max_abs_array(spectrum, (float)  binSize) / spectrum_abs_sum;
    } catch (std::logic_error e) {
        crest = 0.0;
        return;
    }
}

void SpectralFeatures::calculateSpectralFlatness(float log_spectrum_sum, float spectrum_sum) {
    if((spectrum_sum / (float) binSize) > minThresh){
        try {
            flatness = exp(log_spectrum_sum / (float) binSize) / (spectrum_sum / (float) binSize);
        } catch (std::logic_error e) {
            flatness = 0.0;
            return;
        }
    }
    else{
        flatness = 0.0;
    }
    
    // Low pass filter
    float alpha = 0.5;
    flatness = (1-alpha)*flatness + alpha * flatness;
    prevFlatness = flatness;
}

void SpectralFeatures::calculateSpectralRolloff(float* spectrum, float spectrum_sum, float roloff_percentage){
    
    float threshold = roloff_percentage*spectrum_sum;
    int i; float cumSum = 0;
    for (i=0; i<binSize; i++) {
        cumSum +=spectrum[i];
        if (cumSum > threshold) {
            break;
        }
    }
    
    //Normalize
    rolloff = (float) i/binSize;
}

float SpectralFeatures::getTimePassedSinceLastOnsetInMs(){
    return ms.count();
}

float SpectralFeatures::getOnset(float thresh, float interOnsetinterval){
    
    // Reset onset and clock
    onset = 0;
    timeCompare = Clock::now();
    ms = std::chrono::duration_cast<milliseconds>(timeCompare - t_threshTime);
    // printf("TimePassed: .... %lld\n", ms.count());
    
    /* Print and send voltage if spectral flux is greater than threshold */
    if(flux > thresh && ms.count() >= interOnsetinterval){
        onset = 1;
        //printf("Onset: %i, Flux: %f, Thresh: %f\n", onset, flux, thresh);
        
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

float SpectralFeatures::getSpectralRolloffInFreq(){
    return rolloff * sampleRate / 2;
}

float SpectralFeatures::getSpectralCentroid(){
    return centroid;
}

float SpectralFeatures::getSpectralCentroidInFreq(){
    return centroid * sampleRate / 2;
}

float SpectralFeatures::getSpectralFlux(){
    return flux;
}

float SpectralFeatures::getRMS(){
    return rms;
}

float SpectralFeatures::getRMSInDb(){
    float rmsdB = rms;
    if(rmsdB < 1e-5)
        rmsdB = 1e-5; // -100dB
    return 20*log10(rmsdB);
}
