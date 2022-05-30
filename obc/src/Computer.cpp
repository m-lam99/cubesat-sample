#include "Computer.h"

#include <unistd.h>

#include <chrono>
#include <future>
#include <thread>

using namespace std;

Computer::Computer()
    : satellite(),
      mode_(IDLE_MODE),
      WOD_transmit(true),
      stop_transmit(true),
      stop_payloadTransmit(true),
      stop_receive(true),
      collect_data(false),
      payload_collection(false),
      orbit_insertion_complete(false),
      can_receive_WOD(true),
      is_deployed(false), 
      can_receive_payload(true) {
    cout << "constructing" << endl;
    start_time = satellite.getTime();
    new_command = false;
    cout << "Computer initialised" << endl;
}

Computer::~Computer() { stop_continuousWOD = true; }

int Computer::payloadTransmit() {
    // exits function when stop_transmission

    while (!stop_payloadTransmit) {
        if (payload_collection) {
            can_receive_payload = false;
            still_transmitting = satellite.payloadDataTransmission();
            can_receive_payload = true;
            // still_transmitting = true;
            std::cout << "TRANSMITTING" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // transmit every seconds
    }

    return 0;
}

int Computer::runSatellite() {
    // Start WOD transmission
    thread tWODtransmit(&Computer::continuousWOD, this);
    thread tPayloadTransmit(&Computer::payloadTransmit, this);
    thread tCommandReceive(&Computer::commandReceive, this);

    stop_payloadTransmit = false;
    stop_continuousWOD = false;  // start wod
    stop_receive = false;

    std::cout << "RUNNING SATELLITE" << std::endl;
    while (1) {
        // Enters safe mode from any
        if (!satellite.checkBattery()) {
            mode_ = SAFE_MODE;
        } else if (new_command) {
            // change mode
           cout << "New command received" << endl;
            commandHandling();
            new_command = false;
        }

        switch (mode_) {
            case START_MODE:
                start();
                break;
            case DETUMBLING_MODE:
                detumbling();
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
    stop_receive = true;

    tCommandReceive.join();
    tWODtransmit.join();
    tPayloadTransmit.join();
}

void Computer::commandHandling(){
    if(command <= 0x38 && command >= 0x30){
        mode_ = command; 
        cout << "Command " << command << "received" << endl; 
    }
    else if (command == CMD_SEND_WOD){
        can_receive_payload = false; 
        std::vector<uint8_t> message ={WOD_transmit};
        satellite.transmitMessage(message);
        can_receive_payload = true;
        cout << "Send WOD command received" << endl; 

    }
    else if (command == CMD_WOD_OFF){
        WOD_transmit = false;
        cout << "WOD off command received" << endl; 
    }
    else if (command == CMD_WOD_ON){
        WOD_transmit = true;
        cout << "WOD on command received" << endl; 
    }
    else if (command == CMD_SEND_MODE){
        can_receive_payload = false; 
        std::vector<uint8_t> message ={mode_};
        satellite.transmitMessage(message);
        can_receive_payload = true;
        cout << "SEND mode command received" << endl; 

    }
    else if (command == SOS){
        mode_ = SAFE_MODE;
        cout << "SOS received" << endl;
    }
    return; 
}
void Computer::start() {
    cout << "START mode" << endl; 
    mode_ = DETUMBLING_MODE;
}

void Computer::detumbling() {

    cout << "Detumbling mode" << endl; 

    // put in bdot 
    cout << "PLEASE ADD B DOT" << endl; 
    
    //testing will be done by timing how long sat takes to
    // stop spinning on bearing table w + wo detumbling

    // check if in orbit
    orbit_insertion_complete = true;

    // then we deploy
    if(!is_deployed){
        mode_ = DEPLOYMENT_MODE;
    }
    else{
        mode_ = IDLE_MODE; 
    }
}

void Computer::deployment() {

    cout << "Deployment mode" << endl; 
    is_deployed = true; 
    // deploy things
    satellite.deployment();
}

void Computer::idle() {
    // do nothing
    cout << "Idle mode" << endl; 
    uint32_t operational_time = satellite.getTime() - start_time;

    if (operational_time > MAX_LIFETIME) {
        mode_ = END_OF_LIFE;
    } else if (satellite.checkOrbit() == 0) {  // in a bad orbit
        mode_ = STATION_KEEPING_MODE;
    } else if (collect_data && satellite.checkDayTime()) {
        mode_ = NORMAL_MODE;
    }
}

void Computer::normal() {

    cout << "Normal Mode" << endl; 
    // collect data
    satellite.payloadDataCollection();

    // Return to idle and see if more collection is needed
    mode_ = IDLE_MODE;

    //
}

void Computer::stationKeeping() {
    // Direction we wish to point in
    double phi = 0.0;
    double theta = 1.57;
    double psi = 0.0;

    if (satellite.pointSatellite(phi, theta, psi)) {  // point satellite otherwise done
        mode_ = IDLE_MODE;
    }
}

void Computer::transmit() {
    // transmits payload

    payload_collection = true;

    usleep(1500000);  // wait for the payload data to be transmitted

    if (!still_transmitting) {
        mode_ = IDLE_MODE;
        payload_collection = false;
    }
}

void Computer::safe() {
    // Changes back to idle

    if (satellite.checkBattery()) {
        if (!orbit_insertion_complete) {
            mode_ = DETUMBLING_MODE;
        }
        else if (satellite.getTime() - start_time > MAX_LIFETIME) {
        mode_ = END_OF_LIFE;
        } 
        else {
            mode_ = IDLE_MODE;
        }
    }
}

void Computer::endOfLife() {
    // do nothing
}

int Computer::continuousWOD() {
    // exits function when stop_transmission

    while (!stop_continuousWOD) {
        if (WOD_transmit) {
            can_receive_WOD = false;
            satellite.wodTransmission();
            can_receive_WOD = true;
            std::cout << "WOD Transmission" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(30000));
        // std::this_thread::sleep_for(std::chrono::milliseconds(5000));

        // transmit every 30 seconds
    }

    return 0;
}

void Computer::commandReceive() {
    while (!stop_receive) {
        if (can_receive_payload && can_receive_WOD) {
            // Receive Data
            std::vector<uint8_t> msg = satellite.checkTransceiver();
            if (!satellite.receive) {
            } else {
                new_command = true;
                if (msg[2] == 0x4D) {
                    if (msg[3] <= 0x38 && msg[3] >= 0x30) {
                        command = msg[3];
                    } else {
                        command = CMD_SEND_MODE;
                    }
                } else if (msg[2] == 0x54 && msg[3] == 0x58) {
                    if (msg[4] == 1) {
                        command = CMD_WOD_ON;
                    } else if (msg[4] == 0) {
                        command = CMD_WOD_OFF;
                    } else {
                        command = CMD_SEND_WOD;
                    }
                } else if (msg[2] == 0x53 && msg[3] == 0x4F && msg[4] == 0x53) {
                    command = SOS;
                } else{
                    new_command = false; 
                }
            }
            // Check if theres a new command and update flag
            std::cout << "Command: " << command << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(30000));
        // std::this_thread::sleep_for(std::chrono::milliseconds(5000));

        // transmit every 30 seconds
    }

    // return 0;
}