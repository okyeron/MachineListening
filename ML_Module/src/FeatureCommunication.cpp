//
//  FeatureCommunication.cpp
//  module
//
//  Created by Christopher Latina on 4/11/16.
//  Copyright © 2016 Christopher Latina. All rights reserved.
//

#include "FeatureCommunication.hpp"

#ifdef __arm__
    #include <stdio.h>
    #include <stdint.h>
    #include <stdlib.h>
    #include <string.h>
    #include <time.h>

    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/tcp.h>
    #include <unistd.h>
    #include <sys/time.h>

    #include <math.h>
    #include <wiringPi.h>
    #include <softPwm.h>
    #include <wiringPiSPI.h>
#endif

#define ADC_SPI_CHANNEL 1
#define ADC_SPI_SPEED 1000000
#define ADC_NUM_CHANNELS 8
#define RESOLUTION 4095 // 1023 if using MCP3008; 4095 if using MCP3208
#define DEADBAND 2
#define ERROR_INT 55555

FeatureCommunication::FeatureCommunication(){
    iFeatureSwitch = 0;
    // ARM specific
    #ifdef __arm__
        wiringPiSetupGpio();
        wiringPiSPISetup(ADC_SPI_CHANNEL, ADC_SPI_SPEED);
        
        // GPIO Digital Output
        pinMode(16, OUTPUT); //Spectral Feature output
        softPwmCreate(16,0,1024);
        
        pinMode(26, OUTPUT); //Onset Trigger output
        //softPwmCreate(26,0,1024);
    
        // Switches
        pinMode(23, INPUT); //Switch 1
        pinMode(24, INPUT); //Switch 2
        pinMode(25, INPUT); //Switch 3
    #endif
}

FeatureCommunication::~FeatureCommunication(){
    
}

/* From Terminal Tedium */
uint16_t adc[8] = {0, 0, 0, 0, 0, 0, 0, 0}; //  store prev.
uint8_t  map_adc[8] = {5, 2, 7, 6, 3, 0, 1, 4}; // map to panel [1 - 2 - 3; 4 - 5 - 6; 7, 8]
uint8_t SENDMSG;

uint16_t FeatureCommunication::readADC(int _channel){ // 12 bit
    #ifdef __arm__
        uint8_t spi_data[3];
        uint8_t input_mode = 1; // single ended = 1, differential = 0
        uint16_t result, tmp;
        
        spi_data[0] = 0x04; // start flag
        spi_data[0] |= (input_mode<<1); // shift input_mode
        spi_data[0] |= (_channel>>2) & 0x01; // add msb of channel in our first command byte
        
        spi_data[1] = _channel<<6;
        spi_data[2] = 0x00;
        
        wiringPiSPIDataRW(ADC_SPI_CHANNEL, spi_data, 3);
        result = (spi_data[1] & 0x0f)<<8 | spi_data[2];
        tmp = adc[_channel]; // prev.
        if ( (result - tmp) > DEADBAND || (tmp - result) > DEADBAND ) { tmp = result ; SENDMSG = 1; }
        adc[_channel] = tmp;
        return tmp;
    #endif
    return 0;
}

int FeatureCommunication::getResolution(){
    return RESOLUTION;
}

float FeatureCommunication::getADCValue(int iADC_channel){
    #ifdef __arm__
        return (float) (RESOLUTION - readADC(iADC_channel)) / (float) RESOLUTION;
    #endif
    return ERROR_INT;
}

int FeatureCommunication::readDigital(int iPinNumber){
    #ifdef __arm__
        return digitalRead(iPinNumber);
    #endif
    return ERROR_INT;
}

void FeatureCommunication::writeGPIO(int GPIOChannel, float writeValue, int writeType){
    #ifdef __arm__
        if(writeType == 1){
            softPwmWrite (GPIOChannel, writeValue);
        } else {
            if(writeValue == 0){
                digitalWrite(GPIOChannel, LOW);
            } else {
                digitalWrite(GPIOChannel, HIGH);
            }
        }
    #endif
}
