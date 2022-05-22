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
    int initialize();
    ~Satellite();

   private:
    ADS1015 adc1_;
    ADS1015 adc2_;
    // AS7263 payload_;
    BNO055 imu_;
    // GPS
    WholeOrbit wod_;
    std::queue<WholeOrbit::wod_t> wod_data_;
    // PWM

};

#endif