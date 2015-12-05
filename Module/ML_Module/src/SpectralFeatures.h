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
    int binSize;
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
    void calculateSpectralCrest(float* spectrum, float spectrum_abs_sum);
    void calculateSpectralFlatness(float* spectrum);

    float* initArray(float* array, int signalSize){
        for (int i=0; i<signalSize; i++)
        {
            array[i] = 0.0;
        }
        
        return array;
    }
    
    float max_abs_array(float a[], float num_elements)
    {
        int i;
        float max=1.175494e-38;
        for (i=0; i<num_elements; i++)
        {
            if (fabsf(a[i])>max)
            {
                max=fabsf(a[i]);
            }
        }
        return(max);
    }
    
    float* log_array(float a[], float num_elements)
    {
        int i;
        for (i=0; i<num_elements; i++)
        {
            a[i]=log(a[i]);
        }
        return a;
    }
    
    float* add_array(float a[], float num_elements, float add)
    {
        int i;
        for (i=0; i<num_elements; i++)
        {
            a[i]=a[i] + add;
        }
        return a;
    }
    
    void setFifo(float* array, int signalSize){
        for (int i=0; i<signalSize; i++)
        {
            fifo[i] = array[i];
        }
    }
};
