#include "Computer.h"
#include <thread>

Computer::Computer()
    : satellite(),
      mode_(START_MODE),
      WOD_state(true)
      {
      }

Computer::~Computer() {}

int Computer::runSatellite(){
    
    while(1){
    // WOD operates independtly of mode unless it has been turned off 
        if(WOD_state){

        }
        
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
}

int Computer::start(){

}

int Computer::ejection(){}

int Computer::orbitalInsertion(){}

int Computer::deployment(){}

int Computer::idle(){}

int Computer::normal(){}

int Computer::stationKeeping(){}

int Computer::transmit(){}

int Computer::safe(){}

int Computer::endOfLife(){}