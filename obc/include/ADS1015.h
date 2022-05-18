// Class for 12-bit ADC interfaced with photodiodes for sun sensor

#ifndef ADS1015_H
#define ADS1015_H

#include "I2C.h"
#include <stdint.h>
using namespace exploringBB;

#define ADC_ADDRESS1 0x4A;
#define ADC_ADDRESS2 0x4B;

class ADS1015 : protected I2CDevice {
   public:
    enum ADDRESS_POINTER {
        CONVERSION = 0x00,
        CONFIG = 0x01,
        LO_THRESH = 0x02,
        HI_THRESH = 0x03
    };
    ADS1015(unsigned int I2CBus, unsigned int I2CAddress);
    uint16_t readRegisters(unsigned int registerAddress);
    int writeRegister(unsigned int registerAddress, uint16_t value);
    float getVoltage();
    ~ADS1015();

   private:
    unsigned int I2CBus, I2CAddress;
    float fsr_ = 2.048; // Configurable with config register
};

#endif