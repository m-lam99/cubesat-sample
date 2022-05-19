#include "ADS1015.h"

#include <unistd.h>

#include <iostream>

using namespace exploringBB;

ADS1015::ADS1015(unsigned int I2CBus, unsigned int I2CAddress)
    : I2CDevice(I2CBus, I2CAddress) {
    this->I2CAddress = I2CAddress;
    this->I2CBus = I2CBus;
}

int ADS1015::writeRegister(unsigned int registerAddress, uint16_t value) {
    unsigned char buffer[3];
    buffer[0] = registerAddress;
    buffer[1] = value >> 8;
    buffer[2] = value & 0xff;

    if (::write(this->file, buffer, 3) != 3) {
        perror("I2C: Failed write to the device\n");
        return 1;
    }

    return 0;
}

uint16_t ADS1015::readRegisters(unsigned int registerAddress) {
    this->write(registerAddress);
    unsigned char buffer[2];
    if (::read(this->file, buffer, 1) != 1) {
        perror("I2C: Failed to set register.\n");
        return 1;
    }

    if (::read(this->file, buffer, 2) != 2) {
        perror("I2C: Failed to read register value.\n");
        return 1;
    }
    return (buffer[0] << 8) | buffer[1];
}

float ADS1015::getVoltage(int channel) {
    uint16_t config = readRegisters(ADDRESS_POINTER::CONFIG);

    switch (channel) {
        case 0:
            writeRegister(ADDRESS_POINTER::CONFIG, config || (0b100 << 12));
            break;
        case 1:
            writeRegister(ADDRESS_POINTER::CONFIG, config || (0b101 << 12));
            break;
        case 2:
            writeRegister(ADDRESS_POINTER::CONFIG, config || (0b110 << 12));
            break;
        default:
            perror("No >:(");
            break;
    }
    std::cout << "config" << readRegisters(ADDRESS_POINTER::CONFIG) << std::endl;
    uint16_t bit_val = readRegisters(ADDRESS_POINTER::CONVERSION);
    std::cout << "bit " << bit_val << std::endl;
    float voltage;
    // Negative
    if (bit_val & 1 << 16) {
        voltage = (-(~bit_val) - 1) / (1 << 11) * fsr_;
    } else {
        voltage = bit_val / (1 << 11) * fsr_;
    }

    return voltage;
}

ADS1015::~ADS1015() {}