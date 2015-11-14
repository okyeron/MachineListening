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
    float prevFlux;
} SpectralFeatures;

void spectralFeatures_new(SpectralFeatures *features, const int signalSize);
float extractSpectralFeatures(SpectralFeatures *features, float* spectrum, const int signalSize);
