// Satellite class to handle different modes of operation

#ifndef COMPUTER_H
#define COMPUTER_H

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
#include "Satellite.h"

#include <queue>
#include <vector>

#define START_MODE 0
#define EJECTION_MODE 1
#define ORBIT_INSERTION_MODE 2
#define DEPLOYMENT_MODE 3
#define IDLE_MODE 4
#define NORMAL_MODE 5
#define TRANSMIT_MODE 6
#define STATION_KEEPING_MODE 7
#define SAFE_MODE 8
#define END_OF_LIFE 9 

class Computer {
   public:

    Computer();
    int runSatellite();
    ~Computer();

   private:

    int start();
    int ejection(); 
    int orbitalInsertion();
    int deployment();
    int idle();
    int normal();
    int stationKeeping();
    int transmit();
    int safe();
    int endOfLife();


    // Create a satellite
    Satellite satellite;   
    uint8_t mode_;

};

#endif