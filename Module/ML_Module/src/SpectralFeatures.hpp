//
//  SpectralFeatures.hpp
//  module
//
//  Created by Christopher Latina on 11/24/15.
//  Copyright © 2015 Christopher Latina. All rights reserved.
//

#ifndef SpectralFeatures_hpp
#define SpectralFeatures_hpp

#include <stdio.h>
#include <stdlib.h> // pulls in declaration of malloc, free
#include <math.h>


#endif /* SpectralFeatures_hpp */


class SpectralFeatures {
public:
    int signalSize;
    int sampleRate;
    float *fifo;
    float flux;
    float prevFlux;
    float crest;
    float flatness;
    float rolloff;
    float centroid;
    float rms;
    
    /* Public methods */
    SpectralFeatures (int numSamples, int fs);
    void extractFeatures(float* spectrum);
    float max_abs_array(float a[], float num_elements);
    
    /* Public methods to get features */
    float getSpectralFlux();
    float getSpectralCrest();
    float getSpectralFlatness();
    float getSpectralRolloff();
    float getSpectralCentroid();
    float getRMS();
    
protected:
    void calculateSpectralFlux(float power);
    void calculateSpectralCentroid(float* spectrum, float spectrum_sum);

    float* initArray(float* array, int signalSize){
        for (int i=0; i<signalSize; i++)
        {
            array[i] = 0.0;
        }
        
        return array;
    }
    
    void setFifo(float* array, int signalSize){
        for (int i=0; i<signalSize; i++)
        {
            fifo[i] = array[i];
        }
    }
};

