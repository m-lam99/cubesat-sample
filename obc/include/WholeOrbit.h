// Class to handle data included in WOD packet

#ifndef WHOLE_ORBIT_H
#define WHOLE_ORBIT_H

#include <stdint.h>
#include "INA219.h"

class WholeOrbit {
   private:
    uint32_t time_;
    bool mode_;
    uint8_t voltage_batt_;
    uint8_t current_batt_;
    uint8_t current_3v3_;
    uint8_t current_5v_;
    uint8_t temp_comms_;
    uint8_t temp_eps_;
    uint8_t temp_batt_;

    INA219 current_sensor_batt_;
    INA219 current_sensor_3v3_;
    INA219 current_sensor_5v_;

   public:
    WholeOrbit();
    ~WholeOrbit();
};

#endif