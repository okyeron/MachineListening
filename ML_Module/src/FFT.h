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
#include "_kiss_fft_guts.h"

#endif /* fft_hpp */

typedef float SAMPLE;

class FFT {
public:
    float* getSpectrum (const SAMPLE* in);
    
    int signalSize;
    kiss_fftr_cfg f_fft;
    
    float *tx_in;
    kiss_fft_cpx* cx_in;
    kiss_fft_cpx* cx_out;
    
    float *mag_out;
    float *window;
    
    /* Public methods */
    FFT();
    virtual ~FFT();
    void init(int numSamples);
    
protected:
    
private:
    
};