// Class for 12-bit ADC interfaced with photodiodes for sun sensor

#ifndef ADS1015_H
#define ADS1015_H

#include "I2C.h"
#include <stdint.h>
using namespace exploringBB;

#define ADC_ADDRESS1 0x4A
#define ADC_ADDRESS2 0x4B

/* CONNECTIONS
0x48 (1001000) ADR -> GND
0x49 (1001001) ADR -> VDD
0x4A (1001010) ADR -> SDA
0x4B (1001011) ADR -> SCL
*/

class ADS1015 : protected I2CDevice {
   public:
    enum ADDRESS_POINTER {
        CONVERSION = 0x00,
        CONFIG = 0x01,
        LO_THRESH = 0x02,
        HI_THRESH = 0x03
    };

    enum CONFIG_BITS {
        OS = 1 << 15,
        MUX = 0b111 << 12,
        PGA = 0b111 << 9,
        MODE = 1 << 8,
        DR = 0b111 << 5,
        COMP_MODE = 1 << 4,
        COMP_POL = 1 << 3,
        COMP_LAT = 1 << 2,
        COMP_QUE = 0b11
    };
    ADS1015(unsigned int I2CBus, unsigned int I2CAddress);
    uint16_t readRegisters(unsigned int registerAddress);
    int writeRegister(unsigned int registerAddress, uint16_t value);
    float getVoltage(int channel);
    void setFSR();
    ~ADS1015();

   private:
    unsigned int I2CBus, I2CAddress;
    float fsr_ = 0.; // Configurable with config register
};

#endif