// Class to handle data included in WOD packet

#ifndef WHOLE_ORBIT_H
#define WHOLE_ORBIT_H

#include <stdint.h>

#include "GPS.h"
#include "INA219.h"
#include "Temperature.h"

class WholeOrbit {
   public:
    struct wod{
        uint32_t time;
        uint8_t mode;
        uint8_t voltage_batt;
        uint8_t current_batt;
        uint8_t current_3v3;
        uint8_t current_5v;
        uint8_t temp_comms;
        uint8_t temp_eps;
        uint8_t temp_batt;
    };

    struct wod_float{
        int time;
        int mode;
        float voltage_batt;
        float current_batt;
        float current_3v3;
        float current_5v;
        float temp_comms;
        float temp_eps;
        float temp_batt;
    };

    typedef struct wod wod_t;
    typedef struct wod_float wod_float_t;
    WholeOrbit(GPS* gps, int mode, INA219* current_sensor_batt);
    wod_t GetData();
    wod_float_t GetDataFloat();
    void ChangeMode(int mode);
    ~WholeOrbit();

   private:
    GPS* gps_;

    int mode_ = 0;

    wod_t wod_;
    wod_float_t wod_float_;

    INA219* current_sensor_batt_;
    INA219 current_sensor_3v3_;
    INA219 current_sensor_5v_;

    Temperature thermistor_comms_;
    Temperature thermistor_eps_;
    Temperature thermistor_batt_;
    
    // 1 if valid, 0 if invalid
    int current_sensor_batt_valid_;
    int current_sensor_3v3_valid_;
    int current_sensor_5v_valid_;
    
    
};

#endif