//
//  fft.c
//  module
//
//  Created by Christopher Latina on 11/12/15.
//  Copyright © 2015 Christopher Latina. All rights reserved.
//

#include "fft.h"

typedef float SAMPLE;

typedef struct {
    kiss_fftr_cfg f_fft;
    
    kiss_fft_cpx* cx_in;
    kiss_fft_cpx* cx_out;
} FFT;

float* getSpectrum (FFT *fft, int signalSize, SAMPLE* in)
{
    
    float* real_out = malloc(signalSize*sizeof(float));
    
    /* Window the input */
    for (int i = 0; i < signalSize; i++) {
        double mul = 0.5 * (1 - cos(2*M_PI*i/(signalSize)));
        in[i] = mul * in[i];
    }
    
    fft->cx_in = (kiss_fft_cpx*) in;
    
    //Allocate kiss_fft params
    fft->f_fft = kiss_fftr_alloc(signalSize,0,0,0);
    
    /* Do the FFT */
    kiss_fftr(fft->f_fft,(kiss_fft_scalar*) fft->cx_in, fft->cx_out);

    for (int i = 0; i < signalSize; i++){
        real_out[i] = fft->cx_out[i].r;
    }
    
    /* Free memory */
    kiss_fft_cleanup();
    free(fft->f_fft);
    free(fft->cx_out);
    free(fft->cx_in);
    free(fft);
    
    return real_out;
}

