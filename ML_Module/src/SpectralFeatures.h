//
//  SpectralFeatures.h
//  module
//
//Copyright (c) 2015-2017 Christopher Latina
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, and / or merge, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// This software is protected under Creative Commons Attribution-NonCommercial-ShareAlike. Please respect this commitment
// https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


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
    float getSpectralFlux();
    float getSpectralRolloff();
    float getSpectralRolloffInFreq();
    float getSpectralCentroid();
    float getSpectralCentroidInFreq();
    float getRMS();
    float getRMSInDb();
    int getBinSize();
    void setFilterParams(int minBin, int maxBin);
    
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
    
protected:
    void reset();
    void calculateRMS(float power);
    void calculateSpectralFlux(float diff_sum);
    void calculateSpectralCentroid(float* spectrum, float power, int minBin, int maxBin);
    void calculateSpectralCrest(float* spectrum, float spectrum_abs_sum);
    void calculateSpectralFlatness(float log_spectrum_sum, float spectrum_sum);
    void calculateSpectralRolloff(float* spectrum, float spectrum_sum, float roloff_percentage);
    bool checkSilence(float power);
    
    /* Feature variables */
    int binSize;
    int sampleRate;
    float *fifo;
    float *diff;
    float flux;
    float prevFlux;
    float crest;
    float flatness;
    float prevFlatness;
    float rolloff;
    float centroid;
    float prevCentroid;
    float rms;
    bool  fifoFilled;
    
    Clock::time_point t_threshTime;
    
    int onset;
    Clock::time_point timeCompare;
    milliseconds ms;
    
    float power;
    float *spectrum_sq;
    float spectrum_sum;
    float log_spectrum_sum;
    float spectrum_abs_sum;
    float halfwave;
    
private:
    float minThresh;
    
    int minBin;
    int maxBin;
    
    int getFilteredBinSize();
    
    void initArray(float* array, int signalSize){
        for (int i=0; i<signalSize; i++)
        {
            array[i] = 0.0;
        }
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
        fifoFilled = true;
        for (int i=0; i<signalSize; i++)
        {
            fifo[i] = array[i];
        }
    }
    
};

