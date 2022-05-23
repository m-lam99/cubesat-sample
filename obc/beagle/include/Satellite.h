// Satellite class to handle different modes of operation

#ifndef SATELLITE_h
#define SATELLITE_H

#include "ADS1015.h"
#include "AS7263.h"
#include "BNO055.h"
#include "GPS.h"
#include "INA219.h"
#include "PWM.h"
#include "Temperature.h"
#include "WholeOrbit.h"
#include "ax25.h"

#include <queue>

class Satellite {
   public:
    struct payload_data{
        uint16_t R;
        uint16_t S;
        uint16_t T;
        uint16_t U;
        uint16_t V;
        uint16_t W;
    };

    typedef struct payload_data payload_data_t;

    Satellite();
    int detumbling();
    int payloadDataCollection();
    int payloadDataTransmission();
    int checkBattery();
    int checkOrbit();
    int orbitCorrection();
    int wodCollection();
    int wodTransmission();
    int deployment();
    int checkTransceiver();
    int initialize();
    ~Satellite();

   private:
    // ADCS sensors
    ADS1015 adc1_;
    ADS1015 adc2_;
    BNO055 imu_;

    // Payload
    // AS7263 payload_;
    
    // GPS
    WholeOrbit wod_;
    std::queue<WholeOrbit::wod_t> wod_data_;
    ax25::Message message_;

    unsigned char srcaddr[6] = {
        'N',
        'I',
        'C',
        'E',
        ' ',
        ' '
    };
    unsigned char destaddr[6] = {
        'U',
        'S',
        'Y',
        'D',
        'G',
        'S'
    };
    // PWM

};

#endif