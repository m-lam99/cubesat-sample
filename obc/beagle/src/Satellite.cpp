#include "Satellite.h"

Satellite::Satellite()
    : wod_(), adc1_(2, ADC_ADDRESS1), adc2_(2, ADC_ADDRESS2), imu_(2, 0x28) {}

Satellite::~Satellite() {}

int Satellite::detumbling() {}

int Satellite::payloadDataCollection() {
    
}

int Satellite::payloadDataTransmission() {}

int Satellite::checkBattery() {}

int Satellite::checkOrbit() {}

int Satellite::orbitCorrection() {}

int Satellite::wodCollection(){
    wod_data_.push(wod_.GetData());
    return 1;
}

int Satellite::wodTransmission() {
    WholeOrbit::wod_t data_packet = wod_data_.front();
    
    unsigned char data_transmit[sizeof(data_packet)];
    memcpy(data_transmit, &data_packet, sizeof(data_packet));
    

    message_.payload = data_transmit; // double check
    message_.npayload = 12;
    message_.source = srcaddr;
    message_.destination = destaddr;
    message_.dataType = 0;
    message_.commandResponse = 0;
    message_.controlType = 0;
    message_.sendSequence = 0;
    message_.receiveSequence = 0;

    //transmit(data_packet)
    ax25::ByteArray * encodedMsg = encode( & message_);
    if (encodedMsg != NULL) {
        //int success = sendMessage(encodedMsg);
    wod_data_.pop();
    }
    
}

int Satellite::deployment() {}

int Satellite::checkTransceiver() {}

int Satellite::initialize() {}