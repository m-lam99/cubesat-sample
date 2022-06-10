#include "WholeOrbit.h"

#include <algorithm>
#include <cmath>
#include <iostream>

WholeOrbit::WholeOrbit(GPS* gps, int mode, INA219* current_sensor_batt)
    : gps_(gps),
    //   mode_(mode),
      current_sensor_batt_(current_sensor_batt),
      current_sensor_3v3_(1, INA219_ADDRESS_3V3),
      current_sensor_5v_(1, INA219_ADDRESS_5V),
      thermistor_comms_(1),
      thermistor_eps_(2),
      thermistor_batt_(0) {

    std::cout << "Beginning WOD initialise"<< std::endl;
    if (current_sensor_batt_->open() == 1) {
        current_sensor_batt_valid_ = 0;
    } else {
        current_sensor_batt_valid_ = 1;
        // Do some config stuff for current sensors
    //     current_sensor_batt_->writeRegister(
    //         INA219::REGISTERS::CALIBRATION, 4096);
    }

    if (current_sensor_3v3_.open() == 1) {
        current_sensor_3v3_valid_ = 0;
    } else {
        current_sensor_3v3_valid_ = 1;
        // current_sensor_3v3_.writeRegister(
        //     INA219::REGISTERS::CALIBRATION, 4096);
    }

    if (current_sensor_5v_.open() == 1) {
        current_sensor_5v_valid_ = 0;
    } else {
        current_sensor_5v_valid_ = 1;
    //     current_sensor_5v_.writeRegister(
    //         INA219::REGISTERS::CALIBRATION, 4096);
    }

    std::cout << "WHOLE ORBIT INITIALISED: " << current_sensor_3v3_valid_
              << current_sensor_batt_valid_ << current_sensor_5v_valid_
              << std::endl;
}

WholeOrbit::wod_t WholeOrbit::GetData() {
    // Time
    // std::cout << "test0" << std::endl;
    GPS::loc_t* location_data;
    // std::cout << "test1" << std::endl;
    gps_->get_location(location_data);
    // std::cout << "test2" << std::endl;
    uint32_t time = location_data->epoch;
    // std::cout << "test3" << std::endl;
    wod_.time = time;
    // std::cout << "Test4" << std::endl;

    // Mode
    wod_.mode = mode_;
    if (current_sensor_batt_valid_) {
        float voltage_batt_f = current_sensor_batt_->busVoltage();
        float current_batt_f = current_sensor_batt_->current();
        // UUI8 = min(0, max(28-1, floor( (20 * U) - 60) ) )
        wod_.voltage_batt = std::max(
            0, std::min(1 << 8 - 1, (int)floor((20 * voltage_batt_f) - 60)));

        // IUI8 = min(0, max(28-1, floor( 127 * I ) + 127) )
        wod_.current_batt = std::max(
            0, std::min(1 << 8 - 1, (int)floor(127 * current_batt_f) + 127));

    } else {
        wod_.voltage_batt = 0;
        wod_.current_batt = 0;
    }

    if (current_sensor_3v3_valid_) {
        float current_3v3_f = current_sensor_3v3_.current();
        // IUI8 = min(0, max(28-1, floor( 40 * I ) ) )
        wod_.current_3v3 =
            std::max(0, std::min(1 << 8 - 1, (int)floor(40 * current_3v3_f)));

    } else {
        wod_.current_3v3 = 0;
    }

    if (current_sensor_5v_valid_) {
        float current_5v_f = current_sensor_5v_.current();
        wod_.current_5v =
            std::max(0, std::min(1 << 8 - 1, (int)floor(40 * current_5v_f)));

    } else {
        wod_.current_5v = 0;
    }

    // TUI8 = min(0, max(28-1, floor( (4 * T) + 60) ) )
    float temp_comms_f = thermistor_comms_.GetTemp();
    wod_.temp_comms =
        std::max(0, std::min(1 << 8 - 1, (int)floor((4 * temp_comms_f) + 60)));

    float temp_eps_f = thermistor_eps_.GetTemp();
    wod_.temp_eps =
        std::max(0, std::min(1 << 8 - 1, (int)floor((4 * temp_eps_f) + 60)));

    float temp_batt_f = thermistor_batt_.GetTemp();
    wod_.temp_batt =
        std::max(0, std::min(1 << 8 - 1, (int)floor((4 * temp_batt_f) + 60)));

    return wod_;
}

WholeOrbit::wod_float_t WholeOrbit::GetDataFloat() {
    // Time
    GPS::loc_t location_data;
    std::cout << "getting data" << std::endl;
    gps_->get_location(&location_data);
    std::cout << "got gps data" << std::endl;

    uint32_t time = location_data.epoch;
    wod_float_.time = (int)time;

    // Mode
    wod_float_.mode = mode_;

    if (current_sensor_batt_valid_) {
        wod_float_.voltage_batt = current_sensor_batt_->busVoltage();
        wod_float_.current_batt = current_sensor_batt_->current();
    } else {
        wod_float_.voltage_batt = 0.f;
        wod_float_.current_batt = 0.f;
    }

    if (current_sensor_3v3_valid_) {
        wod_float_.current_3v3 = current_sensor_3v3_.current();
    } else {
        wod_float_.current_3v3 = 0.;
    }

    if (current_sensor_5v_valid_) {
        wod_float_.current_5v = current_sensor_5v_.current();
    } else {
        wod_float_.current_5v = 0.;
    }

    wod_float_.temp_comms = thermistor_comms_.GetTemp();

    wod_float_.temp_eps = thermistor_eps_.GetTemp();

    wod_float_.temp_batt = thermistor_batt_.GetTemp();

    return wod_float_;
}

void WholeOrbit::ChangeMode(int mode) { mode_ = mode; }

WholeOrbit::~WholeOrbit() {}
