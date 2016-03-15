//
//  fft.c
//  module
//
//  Created by Christopher Latina on 11/12/15.
//  Copyright © 2015 Christopher Latina. All rights reserved.
//

#include "fft.h"

void fft_new(FFT *fft, const int signalSize)
{    
    //Allocate kiss_fft params
    fft->f_fft = kiss_fftr_alloc(2*signalSize,0,0,0);
    
    fft->tx_in = malloc(signalSize*sizeof(SAMPLE));
    fft->cx_in = malloc(signalSize*sizeof(kiss_fft_cpx));
    fft->cx_out= malloc(signalSize*sizeof(kiss_fft_cpx));

    fft->real_out = malloc(signalSize*sizeof(float));
    fft->window = malloc(signalSize*sizeof(float));
    
    /* Create the Hann Window */
    for (int i = 0; i < signalSize; i++) {
        fft->window[i] = 0.5 * (1 - cos(2*M_PI*i/(signalSize)));
    }
}

/* Returns the real part of the fft_out result */
float* getSpectrum (FFT *fft, const SAMPLE* in, const int signalSize)
{

    /* Window in */
    for (int i = 0; i < signalSize; i++) {
        fft->tx_in[i] = fft->window[i] * in[i];
    }
    
    memcpy(fft->cx_in,(kiss_fft_cpx*) fft->tx_in, signalSize*sizeof(SAMPLE));
    
    /* Do the FFT */
    kiss_fftr(fft->f_fft,(kiss_fft_scalar*) fft->cx_in, fft->cx_out);

    /* Only return the real part */
    for (int i = 0; i < signalSize; i++){
        fft->real_out[i] = fft->cx_out[i].r;
    }
    
    /* Free memory */
    // kiss_fft_cleanup();
    
    return fft->real_out;
}

