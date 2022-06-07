// Satellite class to handle different modes of operation

#ifndef COMPUTER_H
#define COMPUTER_H

#include "Satellite.h"
#include "controller.h"
#include "Test.h"

#include <queue>
#include <vector>

#define MAX_LIFETIME 2630000 // in seconds 
//#define MAX_LIFETIME 30 // in seconds 

#define START_MODE 0x36
#define DETUMBLING_MODE 0x37
#define DEPLOYMENT_MODE 0x38
#define IDLE_MODE 0x31
#define NORMAL_MODE 0x32
#define TRANSMIT_MODE 0x34
#define STATION_KEEPING_MODE 0x33
#define SAFE_MODE 0x39
#define END_OF_LIFE 0x35

#define CMD_SEND_WOD 0x40
#define CMD_WOD_ON 0x41
#define CMD_WOD_OFF 0x42 
#define CMD_SEND_MODE 0x43
#define SOS 0x44 
#define CMD_TEST 0x45

#define TEST_MODE 0x50

class Computer {
   public:

    Computer();
    int runSatellite();
    ~Computer();
    void littleRun(); 

   private:

    uint8_t mode_;

    void commandHandling();

    void start();
    void detumbling();
    void deployment();
    void idle();
    void normal();
    void stationKeeping();
    void receive();
    void transmit();
    void safe();
    void endOfLife();

    int continuousWOD(); 
    int payloadTransmit(); 
    void commandReceive(); 

    void runTest(); 
    int command; 
    
    // Create Test Object 
    Test test; 
    int test_mode; 

    // Create a satellite
    Satellite satellite;   
    bool stop_continuousWOD; // overall control of the continuousWO

    bool is_deployed; 

    uint32_t start_time; 
    bool orbit_insertion_complete; 

    // command mode 
    bool transmit_data;     // to ground station
    

    // Receive data 
    bool new_command; 
};

#endif