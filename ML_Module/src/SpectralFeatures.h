//
//  SpectralFeatures.hpp
//  module
//
//  Created by Christopher Latina on 11/24/15.
//  Copyright © 2015 Christopher Latina. All rights reserved.
//

#ifndef SpectralFeatures_hpp
#define SpectralFeatures_hpp

#include <thread>
#include <chrono>
#include <stdio.h>
#include <stdlib.h> // pulls in declaration of malloc, free
#include <math.h>

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;


#endif /* SpectralFeatures_hpp */


class SpectralFeatures {
public:
    /* Feature variables */
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
    float delayTime; //Delay in MS
    
    Clock::time_point t_threshTime;
    
    
    float thresh;
    int onset;
    Clock::time_point timeCompare;
    milliseconds ms;
    
    float power;
    float *spectrum_sq;
    float spectrum_sum;
    float log_spectrum_sum;
    float spectrum_abs_sum;
    float halfwave;
    
    /* Public Methods */
    SpectralFeatures ();
    virtual ~SpectralFeatures();
    void init (int numBins, int fs);
    void extractFeatures(float* spectrum);
    
    /* Public methods to get features */
    float getTimePassedSinceLastOnsetInMs();
    float getOnset(float threshold, float interOnsetinterval);
    float getSpectralCrest();
    float getSpectralFlatness();
    float getSpectralRolloff();
    float getSpectralCentroid();
    float getRMS();
    int getBinSize();
    float scaleFrequency(float feature);
    void setFilterParams(int minBin, int maxBin);
    
protected:
    void reset();
    void calculateSpectralFlux(float halfwave);
    void calculateSpectralCentroid(float* spectrum, float spectrum_sum);
    void calculateSpectralCrest(float* spectrum, float spectrum_abs_sum);
    void calculateSpectralFlatness(float log_spectrum_sum, float spectrum_sum);

private:
    float minThresh;
    
    int minBin;
    int maxBin;
    
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

