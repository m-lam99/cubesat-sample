// Class to handle data included in WOD packet

#ifndef WHOLE_ORBIT_H
#define WHOLE_ORBIT_H

#include <stdint.h>

#include "INA219.h"
#include "Temperature.h"

class WholeOrbit {
   public:
    WholeOrbit();
    void GetData();
    ~WholeOrbit();

   private:
    uint32_t time_;
    uint8_t mode_;
    uint8_t voltage_batt_ = 0;
    uint8_t current_batt_ = 0;
    uint8_t current_3v3_ = 0;
    uint8_t current_5v_ = 0;
    uint8_t temp_comms_ = 0;
    uint8_t temp_eps_ = 0;
    uint8_t temp_batt_ = 0;

    INA219 current_sensor_batt_;
    INA219 current_sensor_3v3_;
    INA219 current_sensor_5v_;

    Temperature thermistor_comms_;
    Temperature thermistor_eps_;
    Temperature thermistor_batt_;
};

#endif