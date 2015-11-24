//
//  fft.cpp
//  module
//
//  Created by Christopher Latina on 11/24/15.
//  Copyright © 2015 Christopher Latina. All rights reserved.
//

#include "FFT.hpp"

FFT::FFT(int numSamples)
{
    signalSize = numSamples;
    
    //Allocate kiss_fft params
    f_fft = kiss_fftr_alloc(2*signalSize,0,0,0);
    
    tx_in = (float*) malloc(signalSize*sizeof(float));
    cx_in = (kiss_fft_cpx*) malloc(signalSize*sizeof(kiss_fft_cpx));
    cx_out= (kiss_fft_cpx*) malloc(signalSize*sizeof(kiss_fft_cpx));
    
    mag_out = (float*) malloc(signalSize*sizeof(float));
    window = (float*) malloc(signalSize*sizeof(float));
    
    // Create the Hann Window
    for (int i = 0; i < signalSize; i++) {
        window[i] = 0.5 * (1 - cos(2*M_PI*i/(signalSize)));
    }
}

// Returns the real part of the fft_out result
float* FFT::getSpectrum (const SAMPLE* in)
{
    
    // Window in
    for (int i = 0; i < signalSize; i++) {
        tx_in[i] = window[i] * in[i];
    }
    
    memcpy(cx_in,(kiss_fft_cpx*) tx_in, signalSize*sizeof(SAMPLE));
    
    // Do the FFT
    kiss_fftr(f_fft,(kiss_fft_scalar*) cx_in, cx_out);
    
    // Only return the real part
    for (int i = 0; i < signalSize; i++){
        // Output the magnitude spectrum
        mag_out[i] = std::abs(sqrt(cx_out[i].r*cx_out[i].r + cx_out[i].i*cx_out[i].i));
    }
    
    /* Free memory */
    // kiss_fft_cleanup();
    
    return mag_out;
}

