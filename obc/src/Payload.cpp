#include "Payload.h"

#include <iostream>

Payload::Payload(GPS* gps) : gps_(gps), sensor_(2, AS7263_ADDRESS) {
    // Check if correct device
    std::cout << sensor_.readVirtualReg(AS7263::VIRTUAL_REG::HW_VERSION) << std::endl;
    if (sensor_.readVirtualReg(AS7263::VIRTUAL_REG::HW_VERSION) != 0x40) {
        valid_ = 0;
    } else {
        valid_ = 1;
        std::cout << "PAyload initition" << std::endl;
        sensor_.initialise();
    }
}

Payload::payload_data_t Payload::getData() {
    if (valid_) {
        sensor_.takeMeasurements();
        // Automatically returns 0 if particular channel is not working
        data_buf_.R = sensor_.getCalibratedR();
        data_buf_.S = sensor_.getCalibratedS();
        data_buf_.T = sensor_.getCalibratedT();
        data_buf_.U = sensor_.getCalibratedU();
        data_buf_.V = sensor_.getCalibratedV();
        data_buf_.W = sensor_.getCalibratedW();
        valid_ = sensor_.getR();
    }

    GPS::loc_t loc_data;
    gps_->get_location(&loc_data);
    
    data_buf_.lat = loc_data.latitude;
    data_buf_.lon = loc_data.longitude;
    data_buf_.alt = loc_data.altitude;
    
    // data_buf_.lat = (int16_t)(loc_data.latitude * 32767 / 90);
    // data_buf_.lon = (int16_t)(loc_data.longitude * 32767 / 180);
    // data_buf_.alt = ((int16_t)(loc_data.altitude - 250000) * (32767 / 50000));
    
    return data_buf_;
}

Payload::~Payload() {}
