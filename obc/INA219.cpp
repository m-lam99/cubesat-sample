#include "INA219.h"
#include <math.h>
#include <iostream>
#include <unistd.h>

INA219::INA219(unsigned int I2CBus, unsigned int I2CAddress):
    I2CDevice(I2CBus, I2CAddress){
    this->I2CAddress = I2CAddress;
    this->I2CBus = I2CBus;
    // Set to default configs - perhaps change later
    this->writeRegister(0x399F, REGISTERS::CONFIG);
}

int INA219::writeRegister(unsigned int registerAddress, uint16_t value){
   unsigned char buffer[3];
   buffer[0] = registerAddress;
   buffer[1] = value >> 8;
   buffer[2] = value & 0xff;

   if(::write(this->file, buffer, 2)!=2){
      perror("I2C: Failed write to the device\n");
      return 1;
   }
   return 0;
}

uint16_t INA219::readRegister(unsigned int registerAddress){
   this->write(registerAddress);
   unsigned char buffer[2];
   if(::read(this->file, buffer, 1)!=1){
      perror("I2C: Failed to read in the value.\n");
      return 1;
   }
   return (buffer[0] << 8) | buffer[1];
}


void INA219::configure(int voltage_range, int gain, int bus_adc, int shunt_adc){
	voltage_range_ = voltage_range;
	gain_ = gain;

	uint16_t calibration = (voltage_range_ << CONFIG_BITS::BRNG | gain_ << CONFIG_BITS::PG | 
							bus_adc << CONFIG_BITS::BADC | shunt_adc << CONFIG_BITS::SADC);

	writeRegister(REGISTERS::CONFIG, calibration);
}

float INA219::determineCurrentLSB(float max_expected_amps, float max_possible_amps){
    float current_lsb;

	float nearest = roundf(max_possible_amps * 1000.0) / 1000.0;
	if (max_expected_amps > nearest) {
		char buffer[65];
		std::cout << "Expected current " << max_expected_amps << " A is greater than max possible current " 
            << max_possible_amps << std::endl;
	}

	if (max_expected_amps < max_possible_amps) {
		current_lsb = max_expected_amps / CURRENT_LSB_FACTOR;
	} else {
		current_lsb = max_possible_amps / CURRENT_LSB_FACTOR;
	}
	
	if (current_lsb < min_device_current_lsb_) {
		current_lsb = min_device_current_lsb_;
	}
	return current_lsb;
}