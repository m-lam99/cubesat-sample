#include "Satellite.h"
#include <unistd.h>
#include <iostream>


Satellite::Satellite()
    : wod_(&gps_, mode_),
      adc1_(2, ADC_ADDRESS1),
      adc2_(2, ADC_ADDRESS2),
      imu_(2, 0x28),
      gps_(),
      payload_(&gps_),
      outGPIO(23) //change pin number!!!!!!!! 
      {
          outGPIO.setDirection(OUTPUT);
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
}

int Satellite::checkBattery() {}

int Satellite::checkOrbit() {}

int Satellite::orbitCorrection() {}

int Satellite::wodCollection() {
    wod_data_.push(wod_.GetData());
    return 1;
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

    // ax25::ByteArray* encodedMsg = ax25::encode(&message_);
    // if (encodedMsg != NULL) {
    //     // int success = sendMessage(encodedMsg);
    //     wod_data_.pop();
    //     return 1;
    // } else {
    //     return 0;
    // }
}

int Satellite::deployment() {}

int Satellite::checkTransceiver() {}

int Satellite::propulsion(std::vector<int> array) {
    
    int n = array.size();
    for (int i = 0; i < n; ++i) {
        
        if (i % 2 == 0) {   
            // Turn on
            outGPIO.setValue(HIGH);
        } else {
            // Turn off
            outGPIO.setValue(LOW);
        }
        usleep(array[i]*1000000);

        std::cout << array[i] << std::endl;
        
    }
    return 1;
}