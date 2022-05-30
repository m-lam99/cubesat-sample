// Satellite class to handle different modes of operation

#ifndef COMPUTER_H
#define COMPUTER_H

#include "Satellite.h"
#include "controller.h"

#include <queue>
#include <vector>

<<<<<<< HEAD
//#define MAX_LIFETIME 2630000 // in seconds 
<<<<<<< HEAD
#define MAX_LIFETIME 30 // in second
=======
#define MAX_LIFETIME 30 // in seconds 
>>>>>>> eaa96852dc99a30b53b85e421f0b5dca4c305c14
=======
#define MAX_LIFETIME 2630000 // in seconds 
//#define MAX_LIFETIME 30 // in seconds 
>>>>>>> 01f56d4c099649a8a6a1baedbc8383114f2a592c

#define START_MODE 0x36
#define DETUMBLING_MODE 0x37
#define DEPLOYMENT_MODE 0x38
#define IDLE_MODE 0x31
#define NORMAL_MODE 0x32
#define TRANSMIT_MODE 0x34
#define STATION_KEEPING_MODE 0x33
#define SAFE_MODE 0x30
#define END_OF_LIFE 0x35

#define CMD_SEND_WOD 0x40
#define CMD_WOD_ON 0x41
#define CMD_WOD_OFF 0x42 
#define CMD_SEND_MODE 0x43
#define SOS 0x44 

class Computer {
   public:

    Computer();
    int runSatellite();
    ~Computer();

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

    int command; 

    // Create a satellite
    Satellite satellite;   
    bool stop_continuousWOD; // overall control of the continuousWO
    bool stop_payloadTransmit;
    bool stop_transmit; 
    bool stop_receive; 

    bool is_deployed; 

    uint32_t start_time; 
    bool orbit_insertion_complete; 

    // command mode 
    bool WOD_transmit; // set by command
    bool collect_data; 
    bool transmit_data;     // to ground station
    
    // 
    bool payload_collection; 
    bool still_transmitting; 

    // Receive data 
    bool can_receive_WOD;
    bool can_receive_payload;
    bool new_command; 
};

#endif