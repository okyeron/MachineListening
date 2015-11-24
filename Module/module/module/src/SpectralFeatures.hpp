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
#include <math.h>


#endif /* SpectralFeatures_hpp */


class SpectralFeatures {
public:

    int signalSize;
    float *fifo;
    float flux;
    float prevFlux = 0.0;
    float crest;
    float flatness;
    float rolloff;
    float centroid;
    float rms;
    
    /* Public methods */
    SpectralFeatures (int signalSize);
    void extractFeatures(float* spectrum, const int signalSize);
    float max_abs_array(float a[], float num_elements);
    
    /* Public methods to get features */
    float getSpectralFlux();
    float getSpectralCrest();
    float getSpectralFlatness();
    float getSpectralRolloff();
    float getSpectralCentroid();
    float getRMS();
    
protected:
    void setFifo(float* spectrum){
        this->fifo = spectrum;
    }
    
    float* getFifo(){
        return this->fifo;
    }
    
};

