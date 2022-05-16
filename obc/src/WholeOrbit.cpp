#include "WholeOrbit.h"
#include <algorithm>
#include <cmath>


WholeOrbit::WholeOrbit(): 
    current_sensor_batt_(1, INA219_ADDRESS_BATT),
    current_sensor_3v3_(1, INA219_ADDRESS_3V3),
    current_sensor_5v_(1, INA219_ADDRESS_5V) {
}

void WholeOrbit::GetData(){
    // UUI8 = min(0, max(28-1, floor( (20 * U) - 60) ) ) 
    float voltage_batt_f = current_sensor_batt_.busVoltage();
    voltage_batt_ = std::min(0, std::max(1<<8 -1, (int)floor((20*voltage_batt_f) - 60)));

    // IUI8 = min(0, max(28-1, floor( 127 * I ) + 127) )
    float current_batt_f = current_sensor_batt_.current();
    current_batt_ = std::min(0, std::max(1<<8 - 1, (int)floor(127*current_batt_f)+127));

    // IUI8 = min(0, max(28-1, floor( 40 * I ) ) )
    float current_3v3_f = current_sensor_3v3_.current();
    current_3v3_ = std::min(0, std::max(1<<8 -1, (int)floor(40*current_3v3_f)));

    float current_5v_f = current_sensor_5v_.current();
    current_5v_ = std::min(0, std::max(1<<8 -1, (int)floor(40*current_5v_f)));
    
}

WholeOrbit::~WholeOrbit()
{
}
