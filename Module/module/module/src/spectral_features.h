//
//  spectral_features.h
//  module
//
//  Created by Christopher Latina on 11/13/15.
//  Copyright © 2015 Christopher Latina. All rights reserved.
//

#ifndef spectral_features_h
#define spectral_features_h

#include <stdio.h>
#include <math.h>


#endif /* spectral_features_h */

typedef struct {
    const int sampleSize;
    float *fifo;
    float flux;
    float prevFlux;
    float crest;
    float flatness;
    float rolloff;
    float centroid;
    float rms;

    
} SpectralFeatures;

void spectralFeatures_new(SpectralFeatures *features, const int signalSize);
void extractSpectralFeatures(SpectralFeatures *features, float* spectrum, const int signalSize);

float max_abs_array(float a[], float num_elements);

float getSpectralFlux(SpectralFeatures *features);
float getSpectralCrest(SpectralFeatures *features);
float getSpectralFlatness(SpectralFeatures *features);
float getSpectralRolloff(SpectralFeatures *features);
float getSpectralCentroid(SpectralFeatures *features);
float getRMS(SpectralFeatures *features);
