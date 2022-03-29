#include "INA219.h"
#include <math.h>
#include <iostream>

INA219::INA219(unsigned int I2CBus, unsigned int I2CAddress):
    I2CDevice(I2CBus, I2CAddress){
    this->I2CAddress = I2CAddress;
    this->I2CBus = I2CBus;
    // Set to default configs - perhaps change later
    this->writeRegister(0x399F, REG_CONFIG);
}

float INA219::DetermineCurrentLSB(float max_expected_amps, float max_possible_amps){
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