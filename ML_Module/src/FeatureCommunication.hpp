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
// Spectral lowpass cutoff  : ADC 2
// Spectral hipass cutoff   : ADC 3
// Spectral Scaling - shift : ADC 4
// Spectral scaling - mult  : ADC 5


class FeatureCommunication {
public:
    
    /* Public Methods */
    FeatureCommunication ();
    float   getADCValue(int iADC_channel);
    int     getResolution();
    int     readDigital(int iPinNumber);
    void    writeGPIO(int GPIOChannel, int writeValue, int writeType);
    
protected:
    uint16_t readADC(int _channel);
    
private:

};