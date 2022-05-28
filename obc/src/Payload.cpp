#include "Payload.h"
#include <iostream>

Payload::Payload(GPS* gps) : gps_(gps), sensor_(2, AS7263_ADDRESS) {
    // Check if correct device

    std::cout << "PAYLOAD BUS INITIALISE" << std::endl;
    if (sensor_.readVirtualReg(AS7263::VIRTUAL_REG::HW_VERSION) != 0x20){
        valid_ = 0;
    } else {
        valid_ = 1;
    }

    std::cout << "PAyload initition" << std::endl; 
}

Payload::payload_data_t Payload::getData(){
    if (valid_){
        // Automatically returns 0 if particular channel is not working
        data_buf_.R = sensor_.getR();
        data_buf_.S = sensor_.getS();
        data_buf_.T = sensor_.getT();
        data_buf_.U = sensor_.getU();
        data_buf_.V = sensor_.getV();
        data_buf_.W = sensor_.getW();
    }
    
    GPS::loc_t *loc_data;
    gps_->get_location(loc_data);

    data_buf_.lat = (int16_t)loc_data->latitude * 32767/90;
    data_buf_.lon = (int16_t)loc_data->longitude *32767/180;
    data_buf_.alt = ((int16_t)loc_data->altitude - 250000)*(32767/50000);

}

Payload::~Payload() {}
