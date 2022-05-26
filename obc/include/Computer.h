// Satellite class to handle different modes of operation

#ifndef COMPUTER_H
#define COMPUTER_H

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

    void start();
    void ejection(); 
    void orbitalInsertion();
    void deployment();
    void idle();
    void normal();
    void stationKeeping();
    void transmit();
    void safe();
    void endOfLife();

    int continuousWOD(); 

    // Create a satellite
    Satellite satellite;   
    uint8_t mode_;
    bool stop_continuousWOD; // overall control of the continuousWO
    
    bool WOD_transmit; // set by command
    bool collect_data; 
    bool transmit_data;     // to ground station
};

#endif