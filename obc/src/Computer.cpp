#include "Computer.h"
#include <thread>
#include <unistd.h>

Computer::Computer()
    : satellite(),
      mode_(START_MODE),
      WOD_transmit(true),
      stop_transmit(true),
      collect_data(false) 
      {
      }

Computer::~Computer() {
    stop_continuousWOD = true
}

int Computer::runSatellite(){
    // Start WOD transmission 
    thread tWODtransmit(&Computer::continuousWOD, this);
    stop_continuousWOD = false; // start wod

    while(1){

        switch (mode_)
            {
            case START_MODE:
                start();
                break;
            case ORBIT_INSERTION_MODE:
                orbitalInsertion();
                break;
            case EJECTION_MODE:
                ejection;
                break;
            case DEPLOYMENT_MODE:
                deployment();
                break;
            case IDLE_MODE:
                idle();
                break;
            case NORMAL_MODE:
                normal();
                break;
            case STATION_KEEPING_MODE:
                stationKeeping();
                break;
            case TRANSMIT_MODE:
                transmit();
                break;
            case SAFE_MODE:
                safe();
                break;
            case END_OF_LIFE:
                endOfLife();
                break;
            default:
                break;
            }
    }
    stop_continuousWOD = true; 
    tWODtransmit.join();  

}

void Computer::start(){

}

void Computer::ejection(){}

void Computer::orbitalInsertion(){}

void Computer::deployment(){}

void Computer::idle(){

    if(satellite.checkOrbit() == 0){        //in a bad orbit 
        mode_ = STATION_KEEPING_MODE;
    }
    else if(collect_data){
        mode_ = NORMAL_MODE; 
    }
    else if(transmit_data){
        mode_ = TRANSMIT_MODE; 
    }
}

void Computer::normal(){}

void Computer::stationKeeping(){}

void Computer::transmit(){}

void Computer::safe(){}

void Computer::endOfLife(){}

int Computer::continuousWOD(){
    // exits function when stop_transmission 
    
    while(!stop_continuousWOD){
        if(WOD_transmit){
            satellite.wodTransmission();
        }
        usleep(30 * 1000000); 
        // transmit every 30 seconds 
    }

    return 0; 
}