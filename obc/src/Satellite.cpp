#include "Satellite.h"

#include <unistd.h>
#include <iostream>
#include <string>

#include <iostream>


Satellite::Satellite()
    : current_sensor_batt_(1, INA219_ADDRESS_BATT),
      gps_(),
      wod_(&gps_, mode_, &current_sensor_batt_),
      adc1_(2, ADC_ADDRESS1),
      adc2_(2, ADC_ADDRESS2),
      imu_(2, 0x28),
      payload_(&gps_),
      prop_GPIO_(23),
      burn_GPIO_(59),
      transceiver_()
{
    if (prop_GPIO_.setDirection(OUTPUT) == -1){
        prop_valid_ = 0;
    } else {
        prop_valid_ = 1;
    }

    if (burn_GPIO_.setDirection(OUTPUT) == -1){
        burn_valid_ = 0;
    } else {
        burn_valid_ = 1;
        burn_GPIO_.setValue(LOW);
    }
}

Satellite::~Satellite() {}

int Satellite::detumbling() {}


int Satellite::payloadDataCollection() {
    payload_data_.push(payload_.getData());
    return 1;
}

int Satellite::payloadDataTransmission() {
    Payload::payload_data_t data_packet = payload_data_.front();

    unsigned char data_transmit[sizeof(data_packet)];
    memcpy(data_transmit, &data_packet, sizeof(data_packet));

    message_.payload = data_transmit;
    message_.npayload = 18;
    message_.source = srcaddr;
    message_.destination = destaddr;
    message_.dataType = 1;
    message_.commandResponse = 0;
    message_.controlType = 0;
    message_.sendSequence = 0;
    message_.receiveSequence = 0;

    encodedMsg_ = ax25::encode(&message_);
    if (encodedMsg_ != NULL) {
        // int success = sendMessage(encodedMsg);

        wod_data_.pop();
        return 1;
    } else {
        return 0;
    }
}

int Satellite::checkBattery() {
    if (current_sensor_batt_.busVoltage() < BATTERY_THRESHOLD){
        // GO INTO IDLE MODE
        return 0;
    }
    return 1;
}

int Satellite::checkOrbit() {}

int Satellite::orbitCorrection() {}

int Satellite::wodCollection() {
    wod_data_.push(wod_.GetData());
    std::cout << (int)wod_data_.back().current_3v3 << std::endl;
    return 1;
}

uint32_t Satellite::getTime(){ // from gps 
    GPS::loc_t* location_data;
    gps_.get_location(location_data); 
    return location_data->epoch; 
}

int Satellite::wodTransmission() {
    WholeOrbit::wod_t data_packet = wod_data_.front();

    unsigned char data_transmit[sizeof(data_packet)];
    memcpy(data_transmit, &data_packet, sizeof(data_packet));

    message_.payload = data_transmit;  // double check
    message_.npayload = 12;
    message_.source = srcaddr;
    message_.destination = destaddr;
    message_.dataType = 0;
    message_.commandResponse = 0;
    message_.controlType = 0;
    message_.sendSequence = 0;
    message_.receiveSequence = 0;

    encodedMsg_ = ax25::encode(&message_);

    std::cout << "WOD transmit" << std::endl; 
    if (encodedMsg_ != NULL) {
        // Transmit message
        //transceiver_.TransmitMessage(encodedMsg_); 

        // int success = sendMessage(encodedMsg);
        wod_data_.pop();
        return 1;
    } else {
        return 0;
    }

}

int Satellite::deployment() {
    if (burn_GPIO_.setValue(HIGH) == -1 || !burn_valid_){
        return 0;
    } else {
        usleep(1000000);
        burn_GPIO_.setValue(LOW);
        return 1;
    }
}

int Satellite::checkDayTime(){
    int isDaytime = 1; 

    // All channels 
    float max_voltage = 0.0; 

    for(int i = 0; i < 3; i++){
        if(adc1_.getVoltage(i) > max_voltage){
            max_voltage = adc1_.getVoltage(i);
        }

        if(adc2_.getVoltage(i) > max_voltage){
            max_voltage = adc2_.getVoltage(i);
        }

    }

    if(max_voltage < SUN_SENOR_THRESHOLD){
        isDaytime = 0; 
    }

    return isDaytime; 

}

std::vector<uint8_t> Satellite::checkTransceiver() {
    // checks for messages 
     transceiver_.SendCommand(transceiver_ .CMD_RECEIVE_MODE_CONFIG);
        std::vector<uint8_t> message = transceiver_.ReceiveData();
        std::cout << "MESSAGE: "; 

       for (unsigned int i = 0; i < transceiver_.MAX_BYTES_AX25; i++)
            {
                if(message[0] == '#' && message[1] == 'R' && i>3 && i<18){
                    std::cout << message[i]; 

                }
            }
        std::cout << std::endl; 
    
    // UNPACK message 
    
    return message; 

}

int Satellite::propulsion(std::vector<int> array) {
    int n = array.size();
    for (int i = 0; i < n; ++i) {
        if (i % 2 == 0) {
            // Turn on
            if (prop_GPIO_.setValue(HIGH) == -1 || !prop_valid_){
                return 0;
            }
        } else {
            // Turn off
            if (prop_GPIO_.setValue(LOW) == -1 || !prop_valid_){
                return 0;
            }
        }
        usleep(array[i] * 1000000);

        // std::cout << array[i] << std::endl;
    }
    return 1;
}

