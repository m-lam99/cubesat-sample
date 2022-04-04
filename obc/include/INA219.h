#ifndef INA219_H
#define INA219_H

#include "I2C.h"
#include <cstdint>

// Need to change for different devices
#define INA219_ADDRESS 0x40

#define CURRENT_LSB_FACTOR 32770
#define SHUNT_MV_LSB 0.01
#define BUS_MV_LSB 4
#define R_SHUNT 0.1

class INA219:protected I2CDevice{
    
    public:
        // Register addresses for the INA219
        enum REGISTERS {
            CONFIG = 0x00,
            SHUNTVOLTAGE = 0x01,
            BUSVOLTAGE = 0x02,
            POWER = 0x03,
            CURRENT = 0x04,
            CALIBRATION = 0x05
        };

        // Bit 13 on config register
        enum VOLTAGE_RANGE {
            FSR_16 = 0,
            FSR_32 = 1
        };

        // Maximum shunt voltage, default is PGA 8
        // Bits 11-12 on config register
        enum PGA_GAIN {
            GAIN_1_40MV = 0,
            GAIN_2_80MV = 1,
            GAIN_4_160MV = 2,
            GAIN_8_320MV = 3
        };

        // Adjust Shunt ADC resolution or set number of samples used when averaging results
        // for shunt voltage register (0x01)
        // Bits 3-6 on Config register
        enum SADC {
            ADC_9BIT = 0, // Conversion time 84us
            ADC_10BIT = 1, // Conversion time 148us
            ADC_11BIT = 2, // Conversion time 276us
            ADC_12BIT_DEFAULT = 3, // Conversion time 532us
            ADC_12BIT = 9, // Conversion time 532us
            ADC_2SAMP = 10, // 2 samples at 12 bit, conversion time 1.06ms
            ADC_4SAMP = 11, // 4 samples at 12 bit, conversion time 2.13ms
            ADC_8SAMP = 12, // 8 samples at 12 bit, conversion time 8.51ms
            ADC_16SAMP = 13, // 16 samples at 12 bit, conversion time 17.02ms
            ADC_32SAMP = 14, // 32 samples at 12 bit, conversion time 34.05ms
            ADC_128SAMP = 15 // 128 samples at 12 bit, conversion time 68.10ms
        };

        // Operating mode
        // Bits 0-2 on config register
        enum MODE {
            POWER_DOWN = 0,
            SHUNT_TRIGGERED = 1,
            BUS_TRIGGERED = 2,
            SHUNT_BUS_TRIGGERED = 3,
            ADC_OFF = 4,
            SHUNT_CONTINUOUS = 5,
            BUS_CONTINUOUS = 6,
            SHUNT_BUS_CONTINUOUS = 7
        };

        enum CONFIG_BITS {
            RST = 15, // Reset
            BRNG = 13, // Bus voltage range
            PG = 11, // PGA
            BADC = 7, // Bus ADC Resolution/Averaging 
            SADC = 3, // Shunt ADC Resolution/Averaging
            MODE = 0
        };

        int writeRegister(unsigned int registerAddress, uint16_t value);
        uint16_t readRegister(unsigned int registerAddress);
        INA219(unsigned int I2CBus, unsigned int I2CAddress);
        void configure(int voltage_range, int gain, int bus_adc, int shunt_adc);
        float shuntVoltage();
        float busVoltage();
        void calibrate(int bus_volts_max, float max_expected_amps, float max_possible_amps);
        float determineCurrentLSB(float max_expected_amps, float max_possible_amps);
        float current();
        float power();
    private:
        unsigned int I2CBus, I2CAddress;
        float min_device_current_lsb_;
        float shunt_ohms_;
        float max_expected_amps_;
        int voltage_range_;
        int gain_;
        float current_lsb_;
        float power_lsb_;
};


#endif