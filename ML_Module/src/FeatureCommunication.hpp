//
//  FeatureCommunication.hpp
//  module
//
//  Created by Christopher Latina on 4/11/16.
//  Copyright © 2016 Christopher Latina. All rights reserved.
//

#ifndef FeatureCommunication_hpp
#define FeatureCommunication_hpp

#include <thread>
#include <chrono>
#include <stdio.h>
#include <math.h>
#include <stdlib.h> // pulls in declaration of malloc, free, uint16_t

#endif /* FeatureCommunication_hpp */

/* Move these to config file. */
// Inter-Onset Time Interval: ADC 0
// Onset Threshold          : ADC 1
// Spectral lowpass cutoff  : ADC 6
// Spectral hipass cutoff   : ADC 7
// Spectral Scaling - shift : ADC 4
// Spectral scaling - mult  : ADC 5

#define A4_HZ (440.0)
#define OCTAVE_OFFSET (5)

class FeatureCommunication {
public:
    
    /* Public Methods */
    FeatureCommunication ();
    virtual ~FeatureCommunication();
    float   getADCValue(int iADC_channel);
    int     getResolution();
    int     readDigital(int iPinNumber);
    void    writeGPIO(int GPIOChannel, float writeValue, int writeType);
    bool    checkIfValid(int check);
    bool    checkIfValid(float check);
    
    float scaleFrequency(float feature){
        if(feature > A4_HZ * pow(2,OCTAVE_OFFSET)){
            feature = A4_HZ * pow(2,OCTAVE_OFFSET);
        }
        else if(feature < A4_HZ / (float) pow(2,OCTAVE_OFFSET)){
            feature = A4_HZ / (float) pow(2,OCTAVE_OFFSET);
        }
        
        return log2f(feature/A4_HZ)+OCTAVE_OFFSET;
    }
    
    int iFeatureSwitch;
    
protected:
    uint16_t readADC(int _channel);
    
private:

};