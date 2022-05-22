#include "Satellite.h"

Satellite::Satellite()
    : wod_(), adc1_(2, ADC_ADDRESS1), adc2_(2, ADC_ADDRESS2), imu_(2, 0x28) {}

Satellite::~Satellite() {}

int Satellite::detumbling() {}

int Satellite::payloadDataCollection() {}

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
    //transmit(data_packet)
    wod_data_.pop();
    
}

int Satellite::deployment() {}

int Satellite::checkTransceiver() {}

int Satellite::initialize() {}