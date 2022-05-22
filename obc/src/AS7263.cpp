#include "AS7263.h"

#include <unistd.h>

#include <iostream>

using namespace exploringBB;

AS7263::AS7263(unsigned int I2CBus, unsigned int I2CAddress)
    : I2CDevice(I2CBus, I2CAddress) {
    this->I2CAddress = I2CAddress;
    this->I2CBus = I2CBus;
}

void AS7263::Test() {
    uint8_t device_no = readVirtualReg(VIRTUAL_REG::HW_VERSION);
    std::cout << (int)device_no << std::endl;
}

void AS7263::writeVirtualReg(unsigned int virtual_reg, uint8_t data) {
    volatile uint8_t status;
    while (1) {
        // Read slave I²C status to see if the write buffer is ready.
        status = readRegister(REGISTERS::STATUS);
        if ((status & AS7263_TX_VALID) == 0)
            // No inbound TX pending at slave.  Okay to write now.
            break;
    }
    // Send the virtual register address (setting bit 7 to indicate a pending
    // write).
    writeRegister(REGISTERS::WRITE, (virtual_reg | 0x80));
    while (1) {
        // Read the slave I2C status to see if the write buffer is ready.
        status = readRegister(REGISTERS::STATUS);
        if ((status & AS7263_TX_VALID) == 0)
            // No inbound TX pending at slave.  Okay to write data now.
            break;
    }
    // Send the data to complete the operation.
    writeRegister(REGISTERS::WRITE, data);
}

uint8_t AS7263::readVirtualReg(unsigned int virtual_reg) {
    volatile uint8_t status, d;

    while (1) {
        // Read slave I2C status to see if the read buffer is ready.
        status = readRegister(REGISTERS::STATUS);
        if ((status & AS7263_TX_VALID) == 0)
            // No inbound TX pending at slave.  Okay to write now.
            break;
    }
    // Send the virtual register address (setting bit 7 to indicate a pending
    // write).
    writeRegister(REGISTERS::WRITE, virtual_reg);
    while (1) {
        // Read the slave I²C status to see if our read data is available.
        status = readRegister(REGISTERS::STATUS);
        if ((status & AS7263_RX_VALID) != 0)
            // Read data is ready.
            break;
    }

    
    // Read the data to complete the operation.
    d = readRegister(REGISTERS::READ);
    std::cout << d << std::endl;
    return d;
}