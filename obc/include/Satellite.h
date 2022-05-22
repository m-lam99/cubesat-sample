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

class Satellite {
   public:
    struct wod{
        uint32_t time;
        uint8_t mode;
        uint8_t voltage_batt;
        uint8_t current_batt;
        uint8_t current_3v3;
        uint8_t current_5v;
        uint8_t temp_comms;
        uint8_t temp_eps;
        uint8_t temp_batt;
    }

    typedef wod_t;

    Satellite(/* args */);
    int detumbling();
    int payloadDataCollection();
    int payloadDataTransmission();
    int checkBattery();
    int checkOrbit();
    int orbitCorrection();
    int wodTransmission();
    int deployment();
    int checkTransceiver();
    int initialize();
    ~Satellite();

   private:
    ADS1015 adc1_;
    ADS1015 adc2_;
    AS7263 payload_;
    BNO055 imu_;
    // GPS
    WholeOrbit wod_;
    // PWM

};

#endif