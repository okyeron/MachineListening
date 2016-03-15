//
//  fft.h
//  module
//
//  Created by Christopher Latina on 11/12/15.
//  Copyright © 2015 Christopher Latina. All rights reserved.
//

#ifndef fft_h
#define fft_h

#include <stdio.h>
#include <math.h>
#include "kiss_fft.h"
#include "kiss_fftr.h"

#endif /* fft_h */

typedef float SAMPLE;

typedef struct {
    kiss_fftr_cfg f_fft;
    
    float *tx_in;
    kiss_fft_cpx* cx_in;
    kiss_fft_cpx* cx_out;

    float *real_out;
    float *window;
} FFT;

void fft_new(FFT *fft, const int signalSize);
float* getSpectrum (FFT *fft, const SAMPLE* in, const int signalSize);
