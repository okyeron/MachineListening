//
//  SpectralFeatures.cpp
//  module
//
//  Created by Christopher Latina on 11/24/15.
//  Copyright © 2015 Christopher Latina. All rights reserved.
//

#include "SpectralFeatures.h"
#ifdef __arm__
    #include <stdio.h>
    #include <stdint.h>
    #include <stdlib.h>
    #include <string.h>
    #include <time.h>

    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/tcp.h>
    #include <unistd.h>
    #include <sys/time.h>

    #include <math.h>
    #include <wiringPi.h>
    #include <softPwm.h>
    #include <wiringPiSPI.h>
#endif

#define ADC_SPI_CHANNEL 1
#define ADC_SPI_SPEED 1000000
#define ADC_NUM_CHANNELS 8
#define RESOLUTION 4095 // 1023 if using MCP3008; 4095 if using MCP3208
#define DEADBAND 2

SpectralFeatures::SpectralFeatures (int numBins, int fs) {
    binSize = numBins;
    sampleRate = fs;
    prevFlux = 0.0;
    fifo = new float[binSize];
    fifo = initArray(fifo, binSize);
    
    //Move this to a GPIO class
     #ifdef __arm__
        wiringPiSetupGpio();
        wiringPiSPISetup(ADC_SPI_CHANNEL, ADC_SPI_SPEED);
    
        // GPIO Digital Output
        pinMode(16, OUTPUT); //Spectral Feature output
        softPwmCreate(16,0,1000);
    
        pinMode(26, OUTPUT); //Onset Trigger output
    
        // Switches
        pinMode(23, INPUT); //Switch 1
        pinMode(24, INPUT); //Switch 2
        pinMode(25, INPUT); //Switch 3
    
        // Onset time threshold
    
        // Spectral feature, hipass thresh
    
        // Spectral feature, lowpass thresh
    
    #endif
}

uint16_t adc[8] = {0, 0, 0, 0, 0, 0, 0, 0}; //  store prev.
uint8_t  map_adc[8] = {5, 2, 7, 6, 3, 0, 1, 4}; // map to panel [1 - 2 - 3; 4 - 5 - 6; 7, 8]

uint8_t SENDMSG;

uint16_t readADC(int _channel){ // 12 bit
#ifdef __arm__
    uint8_t spi_data[3];
    uint8_t input_mode = 1; // single ended = 1, differential = 0
    uint16_t result, tmp;
    
    spi_data[0] = 0x04; // start flag
    spi_data[0] |= (input_mode<<1); // shift input_mode
    spi_data[0] |= (_channel>>2) & 0x01; // add msb of channel in our first command byte
    
    spi_data[1] = _channel<<6;
    spi_data[2] = 0x00;
    
    wiringPiSPIDataRW(ADC_SPI_CHANNEL, spi_data, 3);
    result = (spi_data[1] & 0x0f)<<8 | spi_data[2];
    tmp = adc[_channel]; // prev.
    if ( (result - tmp) > DEADBAND || (tmp - result) > DEADBAND ) { tmp = result ; SENDMSG = 1; }
    adc[_channel] = tmp;
    return tmp;
#endif
    return 0;
}

/*  Method to extract spectral features.
    Input arguments are the magnitude spectrum and block size */
void SpectralFeatures::extractFeatures(float* spectrum)
{
    float power = 0.0;
    float spectrum_sq[binSize];
    float spectrum_sum = 0.0;
    float spectrum_abs_sum = 0.0;
    float halfwave = 0.0;
    
    for (int i=0; i<binSize; i++) {
        // Calculate the difference between the current block and the previous block's spectrum
        float diff = spectrum[i] - fifo[i];
        
        // Half wave recitification.
        halfwave += (diff + fabsf(diff))/2.0;
        
        //Calculate the square
        power += diff * diff;
        
        //Sum of the magnitude spectrum
        spectrum_sum += spectrum[i];
        spectrum_abs_sum += fabsf(spectrum[i]);
        
        
        spectrum_sq[i] = spectrum[i] * spectrum[i];
    }
    
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
    
    //printf("Centroid: %f, \n", centroid);
    
    #ifdef __arm__n
        // TODO: This needs to be mapped to frequency and 1v / octave
        softPwmWrite (16,centroid/2);
    #endif
}

void SpectralFeatures::calculateSpectralCrest(float* spectrum, float spectrum_abs_sum){
    crest = max_abs_array(spectrum, binSize) / spectrum_abs_sum;
}

void SpectralFeatures::calculateSpectralFlatness(float* spectrum) {
//    float min_thresh =1e-20;
//    
//    float *xLog = log_array(spectrum, binSize);
//    xLog = add_array(spectrum,binsize);
//    
//    vtf     = exp(mean(XLog,1)) ./ (mean(X,1));
//    
//    // avoid NaN for silence frames
//    vtf (sum(X,1) == 0) = 0;
}

float SpectralFeatures::getSpectralFlux(){
    float thresh = (float) 5 * (RESOLUTION - readADC(1)) / (float) RESOLUTION ;
    int onset = 0;
    /* Print if greater than threshold */
    if(flux > thresh){
        onset = 1;
        printf("Onset: %i, Flux: %f\n", onset, flux);
        printf("ADC: %d, %d, %d, %i, %d, %d, %d, %d\n", readADC(0), readADC(1), readADC(2), readADC(3), readADC(4), readADC(5), readADC(6), readADC(7));
        #ifdef __arm__
            digitalWrite(26, HIGH);
            int delayTime = (int)(RESOLUTION - readADC(0)) / 10.0;
            delay(delayTime);
            digitalWrite(26, LOW);
        #endif
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