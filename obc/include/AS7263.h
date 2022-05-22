// Class for AS7263 NIR spectral sensor
// Payload subsystem on NICE

#ifndef AS7263_H
#define AS7263_H

#include "I2C.h"

using namespace exploringBB;

// Check if needs to be 0x93
#define AS7263_ADDRESS 0x49
#define AS7263_TX_VALID 0x02
#define AS7263_RX_VALID 0x01

class AS7263:protected I2CDevice {

    public:
        enum REGISTERS{
            STATUS = 0X00,
            WRITE = 0x01,
            READ = 0x02,
        };

        enum VIRTUAL_REG{
            HW_VERSION = 0x00,
            FW_VERSION = 0x02,
            CONTROL_SETUP = 0x04,
            INT_T = 0x05,
            DEVICE_TEMP = 0x06,
            LED_CONTROL = 0x07,
            R_HIGH = 0x08,
            R_LOW = 0x09,
            S_HIGH = 0x0A,
            S_LOW = 0x0B,
            T_HIGH = 0x0C,
            T_LOW = 0x0D,
            U_HIGH = 0x0E,
            U_LOW = 0x0F,
            V_HIGH = 0x10,
            V_LOW = 0x11,
            W_HIGH = 0x12,
            W_LOW = 0x13, 
            R_CAL = 0x14, 
            S_CAL = 0x18,
            T_CAL = 0x1C,
            U_CAL = 0x20,
            V_CAL = 0x24, 
            W_CAL = 0x28,
        };
        AS7263(unsigned int I2CBus, unsigned int I2CAddress);
        void Test();
        void writeVirtualReg(unsigned int registerAddress, uint8_t data);
        uint8_t readVirtualReg(unsigned int registerAddress);
        // ~AS7263();
    private:
        unsigned int I2CBus, I2CAddress;
};


#endif