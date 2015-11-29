//
//  fft.hpp
//  module
//
//  Created by Christopher Latina on 11/24/15.
//  Copyright © 2015 Christopher Latina. All rights reserved.
//

#ifndef fft_hpp
#define fft_hpp

#include <stdio.h>
#include <math.h>
#include <iostream>     // std::cout
#include <cmath>        // std::abs
#include "kiss_fft.h"
#include "kiss_fftr.h"
#endif /* fft_hpp */

typedef float SAMPLE;

class FFT {
    public:
    
    int signalSize;
    
    FFT(int numSamples);
    ~FFT();
    float* getSpectrum (const SAMPLE* in);
    
    kiss_fftr_cfg f_fft;
    
    float *tx_in;
    kiss_fft_cpx* cx_in;
    kiss_fft_cpx* cx_out;
    
    float *mag_out;
    float *window;
};