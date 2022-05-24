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
#include "Payload.h"
#include "GPIO.h"

#include <queue>

class Satellite {
   public:

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
    int propulsion(int* array);
    ~Satellite();

   private:
    // ADCS sensors
    ADS1015 adc1_;
    ADS1015 adc2_;
    BNO055 imu_;
    GPS gps_;

    // Payload
    Payload payload_;
    
    // GPS
    WholeOrbit wod_;
    std::queue<WholeOrbit::wod_t> wod_data_;
    std::queue<Payload::payload_data_t> payload_data_;
    ax25::Message* message_;

    GPIO outGPIO;
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

    uint8_t mode_;

};

#endif