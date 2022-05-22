#include "Satellite.h"

Satellite::Satellite() : wod_() {}

Satellite::~Satellite() {}

int Satellite::detumbling() {}

int Satellite::payloadDataCollection() {}

int Satellite::payloadDataTransmission() {}

int Satellite::checkBattery() {}

int Satellite::checkOrbit() {}

int Satellite::orbitCorrection() {}

int Satellite::wodTransmission() {
    wod_data_.push_back(wod_.GetData());
    return 1;
}

int Satellite::deployment() {}

int Satellite::checkTransceiver() {}

int Satellite::initialize() {}