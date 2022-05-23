#include "WholeOrbit.h"
#include <algorithm>
#include <cmath>


WholeOrbit::WholeOrbit(): 
    current_sensor_batt_(1, INA219_ADDRESS_BATT),
    current_sensor_3v3_(1, INA219_ADDRESS_3V3),
    current_sensor_5v_(1, INA219_ADDRESS_5V),
    thermistor_comms_(1),
    thermistor_eps_(2),
    thermistor_batt_(0) {

        // Do some config stuff for current sensors
}

WholeOrbit::wod_t WholeOrbit::GetData(){
    // UUI8 = min(0, max(28-1, floor( (20 * U) - 60) ) ) 
    float voltage_batt_f = current_sensor_batt_.busVoltage();
    wod_.voltage_batt = std::min(0, std::max(1<<8 -1, (int)floor((20*voltage_batt_f) - 60)));

    // IUI8 = min(0, max(28-1, floor( 127 * I ) + 127) )
    float current_batt_f = current_sensor_batt_.current();
    wod_.current_batt = std::min(0, std::max(1<<8 - 1, (int)floor(127*current_batt_f)+127));

    // IUI8 = min(0, max(28-1, floor( 40 * I ) ) )
    float current_3v3_f = current_sensor_3v3_.current();
    wod_.current_3v3 = std::min(0, std::max(1<<8 -1, (int)floor(40*current_3v3_f)));

    float current_5v_f = current_sensor_5v_.current();
    wod_.current_5v = std::min(0, std::max(1<<8 -1, (int)floor(40*current_5v_f)));

    //TUI8 = min(0, max(28-1, floor( (4 * T) + 60) ) ) 
    float temp_comms_f = thermistor_comms_.GetTemp();
    wod_.temp_comms = std::min(0, std::max(1<<8 -1, (int)floor((4*temp_comms_f) + 60)));

    float temp_eps_f = thermistor_eps_.GetTemp();
    wod_.temp_eps = std::min(0, std::max(1<<8 -1, (int)floor((4*temp_eps_f) + 60)));

    float temp_batt_f = thermistor_batt_.GetTemp();
    wod_.temp_batt = std::min(0, std::max(1<<8 -1, (int)floor((4*temp_batt_f) + 60)));
    
    return wod_;
}

WholeOrbit::~WholeOrbit()
{
}
