#include "ADS1015.h"

#include <unistd.h>

#include <bitset>
#include <iostream>

using namespace exploringBB;

ADS1015::ADS1015(unsigned int I2CBus, unsigned int I2CAddress)
    : I2CDevice(I2CBus, I2CAddress)
{
    this->I2CAddress = I2CAddress;
    this->I2CBus = I2CBus;

    // PGA +-4.096 V
    writeRegister(ADDRESS_POINTER::CONFIG, 0x4683);
    setFSR();
    std::cout << "ADC INITIALISED" << std::endl; 
}

int ADS1015::writeRegister(unsigned int registerAddress, uint16_t value)
{
    unsigned char buffer[3];
    buffer[0] = registerAddress;
    buffer[1] = value >> 8;
    buffer[2] = value & 0xff;

    if (::write(this->file, buffer, 3) != 3)
    {
        perror("I2C: Failed write to the device\n");
        return 1;
    }

    return 0;
}

uint16_t ADS1015::readRegisters(unsigned int registerAddress)
{
    this->write(registerAddress);
    unsigned char buffer[2];
    if (::read(this->file, buffer, 1) != 1)
    {
        perror("I2C: Failed to set register.\n");
        return 1;
    }

    if (::read(this->file, buffer, 2) != 2)
    {
        perror("I2C: Failed to read register value.\n");
        return 1;
    }
    return (buffer[0] << 8) | buffer[1];
}

float ADS1015::getVoltage(int channel)
{
    // writeRegister(ADDRESS_POINTER::CONFIG, 0b0100010010000011);
    uint16_t config = readRegisters(ADDRESS_POINTER::CONFIG);

    switch (channel)
    {
    case 0:
        writeRegister(ADDRESS_POINTER::CONFIG, config | (0b100 << 12));
        break;
    case 1:
        writeRegister(ADDRESS_POINTER::CONFIG, config | (0b101 << 12));
        break;
    case 2:
        writeRegister(ADDRESS_POINTER::CONFIG, config | (0b110 << 12));
        break;
    default:
        perror("No >:(");
        break;
    }
    int test = config | (0b100 << 12);
    // std::bitset<16> x(test);
    // std::cout << "config " << x << std::endl;
    uint16_t bit_val = readRegisters(ADDRESS_POINTER::CONVERSION);
    // std::cout << "bit " << bit_val;

    // Bit shift to right
    bit_val = bit_val >> 4;

    // std::cout << "    bit shifted " << bit_val << std::endl;

    float voltage;
    // Negative
    if (bit_val & 1 << 12)
    { // bit shifted value
        voltage = ((float)(-(~bit_val)) - 1.0) / (float)(1 << 11) * fsr_;
    }
    else
    {
        voltage = (float)bit_val / (float)(1 << 11) * fsr_;
    }

    return voltage;
}

void ADS1015::setFSR()
{
    int fsr_bits = (readRegisters(ADDRESS_POINTER::CONFIG) & CONFIG_BITS::PGA) >> 9;

    switch (fsr_bits)
    {
    case 0b000:
        fsr_ = 6.144;
        break;
    case 0b001:
        fsr_ = 4.096;
        break;
    case 0b010:
        fsr_ = 2.048;
        break;
    case 0b011:
        fsr_ = 1.024;
        break;
    case 0b100:
        fsr_ = 0.512;
        break;
    default:
        fsr_ = 0.256;
        break;
    }

    std::cout << fsr_ << std::endl;
}

ADS1015::~ADS1015() {}