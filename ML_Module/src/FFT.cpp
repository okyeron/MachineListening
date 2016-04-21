//
//  fft.cpp
//  module
//
//  Created by Christopher Latina on 11/24/15.
//  Copyright © 2015 Christopher Latina. All rights reserved.
//

#include "FFT.h"
using namespace std;


FFT::FFT(){
    //Empty use init
}

void FFT::init(int numSamples){
    signalSize = numSamples;
    
    //Allocate kiss_fft params
    f_fft = kiss_fftr_alloc(signalSize,0,0,0);
    
    tx_in = new float[signalSize];
    cx_in = new kiss_fft_cpx[signalSize];
    cx_out= new kiss_fft_cpx[signalSize];
    
    mag_out = new float[signalSize/2];
    window = new float[signalSize];
    
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
    
    memcpy(cx_in,(kiss_fft_cpx*) tx_in, (signalSize)*sizeof(SAMPLE));
    
    // Do the FFTr
    kiss_fftr(f_fft,(kiss_fft_scalar*) cx_in, cx_out);
    
    // Only return the real part
    for (int i = 0; i < signalSize/2; i++){
        // Output the magnitude spectrum
        mag_out[i] = sqrtf(cx_out[i].r*cx_out[i].r + cx_out[i].i*cx_out[i].i);
    }
    
    return mag_out;
}

//// Define the destructor.
FFT::~FFT() {
    // Deallocate the memory
    kiss_fftr_free(f_fft);
    delete [] window;
    delete [] mag_out;
    delete [] tx_in;
    delete [] cx_out;
    delete [] cx_in;
    kiss_fft_cleanup();
}

