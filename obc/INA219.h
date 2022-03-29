#ifndef INA219_H
#define INA219_H

#include "I2C.h"

#define ADDRESS 0x40

#define REG_CONFIG 0x00
#define REG_SHUNTVOLTAGE 0x01
#define REG_BUSVOLTAGE 0x02
#define REG_POWER 0x03
#define REG_CURRENT 0x04
#define REG_CALIBRATION 0x05

#define CURRENT_LSB_FACTOR 32770

class INA219:protected I2CDevice{
    
    public:
        INA219(unsigned int I2CBus, unsigned int I2CAddress);
        float DetermineCurrentLSB(float max_expected_amps, float max_possible_amps);
    private:
        unsigned int I2CBus, I2CAddress;
        float min_device_current_lsb_;
};


#endif