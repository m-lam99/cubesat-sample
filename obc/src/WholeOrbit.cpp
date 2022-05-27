#include "WholeOrbit.h"
#include <algorithm>
#include <cmath>
#include <iostream>


WholeOrbit::WholeOrbit(GPS* gps, uint8_t mode, INA219* current_sensor_batt): 
    gps_(gps),
    mode_(mode),
    current_sensor_batt_(current_sensor_batt),
    current_sensor_3v3_(1, INA219_ADDRESS_3V3),
    current_sensor_5v_(1, INA219_ADDRESS_5V),
    thermistor_comms_(1),
    thermistor_eps_(2),
    thermistor_batt_(0) {
        
        
        if (current_sensor_batt_->open() == 1){
            current_sensor_batt_valid_ = 0;
        }
        else {
            current_sensor_batt_valid_ = 1;
            // Do some config stuff for current sensors
            current_sensor_batt_valid_ = current_sensor_batt_->writeRegister(INA219::REGISTERS::CALIBRATION, 4096);

        }
        
        if (current_sensor_3v3_.open() == 1){
            current_sensor_3v3_valid_ = 0;
        } else {
            current_sensor_3v3_valid_ = 1;
            current_sensor_3v3_valid_ = current_sensor_3v3_.writeRegister(INA219::REGISTERS::CALIBRATION, 4096);

        }
        
        if (current_sensor_5v_.open() == 1){
            current_sensor_5v_valid_ = 0;
        }
        else {
            current_sensor_5v_valid_ = 1;
            current_sensor_5v_valid_ = current_sensor_5v_.writeRegister(INA219::REGISTERS::CALIBRATION, 4096);
        }
        
        std::cout << "WHOLE ORBIT INITIALISED: " << current_sensor_3v3_valid_ << current_sensor_batt_valid_ << current_sensor_5v_valid_ << std::endl;

        
}

WholeOrbit::wod_t WholeOrbit::GetData(){

    // Time
    GPS::loc_t* location_data;
    gps_->get_location(location_data);
    uint32_t time = location_data->epoch;

    // std::cout << time << ", ";

    // Mode
    wod_.mode = mode_;

    std::cout << (int)mode_ << std::endl;

    
    float voltage_batt_f;
    float current_batt_f;
    if (current_sensor_batt_valid_){
        voltage_batt_f = current_sensor_batt_->busVoltage();
        current_batt_f = current_sensor_batt_->current();
    } else {
        voltage_batt_f = 0.;
        current_batt_f = 0.;
    }
    
    // UUI8 = min(0, max(28-1, floor( (20 * U) - 60) ) ) 
    wod_.voltage_batt = std::max(0, std::min(1<<8 -1, (int)floor((20*voltage_batt_f) - 60)));

    // std::cout << voltage_batt_f << ", ";

    // IUI8 = min(0, max(28-1, floor( 127 * I ) + 127) )
    wod_.current_batt = std::max(0, std::min(1<<8 - 1, (int)floor(127*current_batt_f)+127));

    // std::cout << current_batt_f << ", ";

    float current_3v3_f;
    if (current_sensor_3v3_valid_){
        current_3v3_f = current_sensor_3v3_.current();
    } else {
        current_3v3_f = 0.;
    }

    // IUI8 = min(0, max(28-1, floor( 40 * I ) ) )
    wod_.current_3v3 = std::max(0, std::min(1<<8 -1, (int)floor(40*current_3v3_f)));

    //std::cout << current_3v3_f << ", ";

    float current_5v_f;
    if (current_sensor_5v_valid_){
        current_5v_f = current_sensor_5v_.current();
    } else {
        current_5v_f = 0.;
    }

    wod_.current_5v = std::max(0, std::min(1<<8 -1, (int)floor(40*current_5v_f)));

    //std::cout << current_5v_f << ", ";

    //TUI8 = min(0, max(28-1, floor( (4 * T) + 60) ) ) 
    float temp_comms_f = thermistor_comms_.GetTemp();
    wod_.temp_comms = std::max(0, std::min(1<<8 -1, (int)floor((4*temp_comms_f) + 60)));

    // std::cout << temp_comms_f << ", ";

    float temp_eps_f = thermistor_eps_.GetTemp();
    wod_.temp_eps = std::max(0, std::min(1<<8 -1, (int)floor((4*temp_eps_f) + 60)));

    // std::cout << temp_eps_f << ", ";

    float temp_batt_f = thermistor_batt_.GetTemp();
    wod_.temp_batt = std::max(0, std::min(1<<8 -1, (int)floor((4*temp_batt_f) + 60)));
    
    std::cout << temp_batt_f << ", " << (int)wod_.temp_batt << std::endl;

    return wod_;
}

void WholeOrbit::ChangeMode(int mode){
    mode_ = mode;
}

WholeOrbit::~WholeOrbit()
{
}
