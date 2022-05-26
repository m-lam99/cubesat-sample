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
#include "transceiver.h"

#include <queue>
#include <vector>

#define SUN_SENOR_THRESHOLD 0.5

// Each function in Satellite class either returns a 0 or 1
// 0: error
// 1: smooth sailing

class Satellite {
   public:

    Satellite();
    int runSatellite();
    int detumbling();
    int payloadDataCollection();
    int payloadDataTransmission();
    int checkBattery();
    int checkOrbit();
    int orbitCorrection();
    int wodCollection();
    int wodTransmission();
    int deployment();
    std::vector<uint8_t> checkTransceiver();
    int checkDayTime(); 
    int propulsion(std::vector<int> array);
    uint32_t getTime(); 

    ~Satellite();

   private:
    // ADCS sensors
    ADS1015 adc1_;
    ADS1015 adc2_;

    BNO055 imu_;
    GPS gps_;

    // Payload
    Payload payload_;

    // Current sensor for battery
    INA219 current_sensor_batt_;
    
    // WOD
    WholeOrbit wod_;

    // Data collection
    std::queue<WholeOrbit::wod_t> wod_data_;
    std::queue<Payload::payload_data_t> payload_data_;

    // AX25
    ax25::Message message_;
    ax25::ByteArray* encodedMsg_;

    // GPIO
    GPIO prop_GPIO_;
    GPIO burn_GPIO_;

    // 0 if invalid, 1 if valid
    int prop_valid_;
    int burn_valid_;

    // transciever 
    Transceiver transceiver_; 

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

    const float BATsTERY_THRESHOLD  = 2.7;

};

#endif