#include "WholeOrbit.h"


WholeOrbit::WholeOrbit(): 
    current_sensor_batt_(1, INA219_ADDRESS_BATT),
    current_sensor_3v3_(1, INA219_ADDRESS_3V3),
    current_sensor_5v_(1, INA219_ADDRESS_5V) {
}

WholeOrbit::~WholeOrbit()
{
}
