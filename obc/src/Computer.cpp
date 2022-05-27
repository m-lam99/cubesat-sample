#include "Computer.h"
#include <thread>
#include <unistd.h>
#include <chrono>
#include <future>

using namespace std; 

Computer::Computer()
    : satellite(),
      mode_(START_MODE),
      WOD_transmit(true),
      stop_transmit(true),
      stop_payloadTransmit(true), 
      collect_data(false), 
      payload_collection(false),
      orbit_insertion_complete(false) 
      {
          start_time = satellite.getTime(); 

      }

Computer::~Computer() {
    stop_continuousWOD = true; 
}

int Computer::payloadTransmit() {
     // exits function when stop_transmission 

    while(!stop_payloadTransmit){
        if(payload_collection){
           //still_transmitting = satellite.payloadDataTransmission();
           still_transmitting = true; 
           std::cout << "TRANSMITTING" << std::endl; 
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // transmit every seconds 
    }

    return 0; 
}

int Computer::runSatellite(){
    // Start WOD transmission 
    thread tWODtransmit(&Computer::continuousWOD, this);
    thread tPayloadTransmit(&Computer::payloadTransmit, this);
    stop_payloadTransmit = false; 
    stop_continuousWOD = false; // start wod

    while(1){

        // // Enters safe mode from any 
        // if(!satellite.checkBattery()){
        //     mode_ = SAFE_MODE; 
        // }

        switch (TRANSMIT_MODE)
            {
            case START_MODE:
                start();
                break;
            case ORBIT_INSERTION_MODE:
                orbitalInsertion();
                break;
            case EJECTION_MODE:
                ejection();
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
    stop_payloadTransmit = true;
    tWODtransmit.join();  
    tPayloadTransmit.join(); 

}

void Computer::start(){

}

void Computer::ejection(){}

void Computer::orbitalInsertion(){

    // check if in orbit 
    orbit_insertion_complete = true; 

    // then we deploy
    mode_ = DEPLOYMENT_MODE;     

}

void Computer::deployment(){
    // deploy things 
    satellite.deployment(); 
}

void Computer::idle(){

    // do nothing 

    uint32_t operational_time = satellite.getTime() - start_time; 

    if(operational_time > MAX_LIFETIME){
        mode_ = END_OF_LIFE; 
    }
    else if(satellite.checkOrbit() == 0){        //in a bad orbit 
        mode_ = STATION_KEEPING_MODE;
    }
    else if(collect_data && satellite.checkDayTime()){
        mode_ = NORMAL_MODE; 
    }
    else if(transmit_data){
        mode_ = TRANSMIT_MODE; 
    }

}

void Computer::normal(){
    
    // collect data
    satellite.payloadDataCollection(); 

    // Return to idle and see if more collection is needed
    mode_ = IDLE_MODE; 

    // 
}

void Computer::stationKeeping(){
    
}

void Computer::transmit(){
    // transmits payload 
    payload_collection = true; 
    
    usleep(1500000); // wait for the payload data to be transmitted 

    if(!still_transmitting){
        mode_ = IDLE_MODE;
        payload_collection = false; 
    }

}

void Computer::safe(){

    // Changes back to idle 
    if(satellite.checkBattery()){
        if(!orbit_insertion_complete){
            mode_ = ORBIT_INSERTION_MODE;
        }
        else{
            mode_ = IDLE_MODE; 
        }
    }   
    
}

void Computer::endOfLife(){
    // do nothing  
}

int Computer::continuousWOD(){
    // exits function when stop_transmission 
    
    while(!stop_continuousWOD){
        if(WOD_transmit){
           // satellite.wodTransmission();
           std::cout << "WOD Transmission" << std::endl; 
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(30000));
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));

        // transmit every 30 seconds 
    }

    return 0; 
}