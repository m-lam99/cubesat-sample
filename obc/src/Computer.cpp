#include "Computer.h"

#include <unistd.h>

#include <chrono>
#include <future>
#include <thread>
#include <atomic>

using namespace std;
atomic<bool> stop_payloadTransmit{true};
atomic<bool> WOD_transmit{true};
atomic<bool> stop_transmit{true};
atomic<bool> stop_receive{true};
atomic<bool> can_receive{true};

atomic<bool> still_transmitting{true};

// atomic<bool> collect_data{false};
atomic<bool> payload_collection{false};
atomic<bool> stop_continuousWOD{true};

Computer::Computer()
    : satellite(),
      test(),
      mode_(START_MODE),
      test_mode(Test::T_IMU),
      orbit_insertion_complete(false),
      is_deployed(true)
    {
    cout << "constructing" << endl;
    // Dummy time
    start_time = 0;
    new_command = false;
    cout << "Computer initialised" << endl;
}

Computer::~Computer() { stop_continuousWOD = true; }


int Computer::runSatellite() {

    // Set Start
    start_time = satellite.getTime(); 

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
            case TEST_MODE:
                runTest(); 
            default:
                break;
        }
        if(mode_ = END_OF_LIFE){
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

void Computer::littleRun(){
     // Start WOD transmission
    thread tWODtransmit(&Computer::continuousWOD, this);
    thread tPayloadTransmit(&Computer::payloadTransmit, this);
    thread tCommandReceive(&Computer::commandReceive, this);

    stop_payloadTransmit = false;
    stop_continuousWOD = false;  // start wod
    stop_receive = false;

    std::cout << "RUNNING SATELLITE" << std::endl;
    while (1) {

        if (new_command) {
            // change mode
           cout << "New command received" << endl;
            commandHandling();
            new_command = false;
        }

        mode_ = satellite.getMode();

        switch (mode_) {
            case START_MODE:
                //
                cout << "Entering start" << endl; 
                break;
            case DETUMBLING_MODE:
                cout << "Entering Detumbling" << endl; 
                break;
            case DEPLOYMENT_MODE:
                cout << "Entering Deployment" << endl; 
                break;
            case IDLE_MODE:
                cout << "Entering Idle" << endl; 
                break;
            case NORMAL_MODE:
                cout << "Entering Normal" << endl; 
                break;
            case STATION_KEEPING_MODE:
                cout << "Entering Station Keeping" << endl; 
                break;
            case TRANSMIT_MODE:
                transmit();
                break;
            case SAFE_MODE:
                cout << "Entering Safe Mode" << endl; 
                break;
            case END_OF_LIFE:
                cout << "Entering end of life" << endl; 
                break;
            default:
                break;
        }
        usleep(500000);

    }
    stop_continuousWOD = true;
    stop_payloadTransmit = true;
    stop_receive = true;

    tCommandReceive.join();
    tWODtransmit.join();
    tPayloadTransmit.join();

}

void Computer::runTest(){
    WOD_transmit = false; 
    payload_collection = false; 
    can_receive = true; 

    switch (test_mode)
    {
        case Test::T_EXIT:
            break; 
        case Test::T_GPIO:
            test.testGPIO(); 
            break; 
        case Test::T_CURRENT:
            test.testINA219(); 
            break;
        case Test::T_ADC:
            test.testADS1015(); 
            break;
        case Test::T_IR:
            test.testAS7263(); 
            break;
        case Test::T_PWM:
            test.testPWM(); 
            break;
        case Test::T_IMU: 
            test.testBNO055(); 
            break; 
        case Test::T_WOD:
            test.wodTest(); 
            break; 
        case Test::T_WOD_ENCODE:
            test.wodTestEncoded(); 
            break; 
        case Test::T_PROP:
            test.testPropulsion(); 
            break;
        case Test::T_TTC: 
            test.testTransceiver(); 
            break;
        case Test::T_24V:
            test.test24V(); 
            break;
        case Test::T_BURN:
            test.testDeployment(); 
            break;
        case Test::T_DETUM:
            test.testDetumble(); 
            break;
        case Test::T_POINT:
            test.testPoint(); 
            break;
        case Test::T_WOD_TRANSMIT:
            test.transmitWOD(); 
            break;
        case Test::T_PAYLOAD:
            test.testPayload(); 
            break;
        default:
            break;
    }

    if(test_mode == Test::T_EXIT){
        WOD_transmit = true; 
        mode_ = IDLE_MODE;
    }
}

void Computer::commandHandling(){

    if(command <= 0x39 && command >= 0x31){
        if(mode_ == TRANSMIT_MODE && command !=TRANSMIT_MODE){
            payload_collection = false;
            can_receive = true;
            // change of mode
        }

        mode_ = command; 
        cout << "Command " << command << "received" << endl; 

    }
    else if (command == CMD_SEND_WOD){
        can_receive= false; 
        std::vector<uint8_t> message ={WOD_transmit};
        satellite.transmitMessage(message);
        can_receive= true;
        cout << "Send WOD command received" << endl; 

    }
    else if (command == CMD_WOD_OFF){
        cout << "WOD off command received" << endl; 
        WOD_transmit = false;
        can_receive = true;
    }
    else if (command == CMD_WOD_ON){
        WOD_transmit = true;
        cout << "WOD on command received" << endl; 
    }
    else if (command == CMD_SEND_MODE){
        can_receive = false; 
        std::vector<uint8_t> message = {mode_};
        satellite.transmitMessage(message);
        can_receive  = true;
        cout << "SEND mode command received" << endl; 

    }
    else if (command == SOS){
        mode_ = SAFE_MODE;
        cout << "SOS received" << endl;
    }
    else if(command == CMD_TEST){
        mode_ = TEST_MODE; 
    }
    return; 
}
void Computer::start() {
    cout << "START mode" << endl; 
    mode_ = DETUMBLING_MODE;
}

void Computer::detumbling() {

    cout << "Detumbling mode" << endl; 

    // POLL DETUMBLING ALGO UNTIL AT REST
    while (satellite.detumbling() == 1) {
        if(new_command){
            break;
        }
        continue;
    }
    
    // testing will be done by timing how long sat takes to
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
    // deploy things only once 
    if(!is_deployed){
        satellite.deployment();

    }
    is_deployed = true; 
}

void Computer::idle() {
    // do nothing
    cout << "Idle mode" << endl; 
    uint32_t operational_time = satellite.getTime() - start_time;
    
    if (operational_time > MAX_LIFETIME) {
        mode_ = END_OF_LIFE;
    } else if (satellite.checkOrbit() == 0) {  // in a bad orbit
        mode_ = STATION_KEEPING_MODE;
    } else if (payload_collection && satellite.checkDayTime()) {
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
    double theta = 0.0;
    double psi = -1.57;

    while (!satellite.pointSatellite(phi, theta, psi)) {
        if(new_command){
            break;
        }
        continue;
    }
    
    mode_ = IDLE_MODE;

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
            can_receive = false;
            satellite.wodTransmission();
            can_receive = true;
            std::cout << "WOD Transmission" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(30000));

        // transmit every 30 seconds
    }

    return 0;
}

void Computer::commandReceive() {

    while (!stop_receive) {
        if (can_receive) {
            // Receive Data
            std::vector<uint8_t> msg = satellite.checkTransceiver();
            if (!satellite.receive) {
                // Check if theres a new command and update flag
                std::cout << "No Command received" << std::endl;
            } else {
                new_command = true;
                // Check if theres a new command and update flag
                std::cout << "Command: " << command << std::endl;
                if (msg[0] == 0x4D) {
                    if (msg[1] <= 0x38 && msg[1] >= 0x30) {
                        command = msg[1];
                    } else {
                        command = CMD_SEND_MODE;
                    }
                }
                else if(msg[0] == 0x54){
                    command = CMD_TEST;
                    test_mode = msg[1];
                }
                else if (msg[0] == 0x54 && msg[1] == 0x58) {
                    if (msg[2] == 1) {
                        command = CMD_WOD_ON;
                    } else if (msg[2] == 0) {
                        command = CMD_WOD_OFF;
                    } else {
                        command = CMD_SEND_WOD;
                    }
                } else if (msg[0] == 0x53 && msg[1] == 0x4F && msg[2] == 0x53) {
                    command = SOS;
                } else{
                    new_command = false; 
                }
            }
            
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        // std::this_thread::sleep_for(std::chrono::milliseconds(5000));

        // transmit every 30 seconds
    }

    // return 0;
}

int Computer::payloadTransmit() {
    // exits function when stop_transmission

    while (!stop_payloadTransmit) {
        if (payload_collection) {
            can_receive = false;
            still_transmitting = satellite.payloadDataTransmission();
            can_receive= true;
            // still_transmitting = true;
            std::cout << "TRANSMITTING" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // transmit every seconds
    }

    return 0;
}
