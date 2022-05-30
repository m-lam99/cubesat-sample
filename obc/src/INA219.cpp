#include "INA219.h"
#include <math.h>
#include <iostream>
#include <unistd.h>

using namespace exploringBB;

INA219::INA219(unsigned int I2CBus, unsigned int I2CAddress):
    I2CDevice(I2CBus, I2CAddress){
    this->I2CAddress = I2CAddress;
    this->I2CBus = I2CBus;
    std::cout << "INA219 Current sensor initialisied" << std::endl;
    // Set to default configs - perhaps change later
    // this->writeRegister(0x399F, REGISTERS::CONFIG);
}

int INA219::writeRegister(unsigned int registerAddress, uint16_t value){
   unsigned char buffer[3];
   buffer[0] = registerAddress;
   buffer[1] = value >> 8;
   buffer[2] = value & 0xff;

   if(::write(this->file, buffer, 3)!=3){
      perror("I2C: Failed write to the device\n");
      return 1;
   }

   return 0;
}

uint16_t INA219::readRegisters(unsigned int registerAddress){
   this->write(registerAddress);
   unsigned char buffer[2];
   if(::read(this->file, buffer, 1)!=1){
      perror("I2C: Failed to set register.\n");
      return 0;
   }
   
   if(::read(this->file, buffer, 2)!=2){
       perror("I2C: Failed to read register value.\n");
       return 0;
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

float INA219::shuntVoltage(){
   uint16_t shunt_voltage_bits = readRegister(REGISTERS::SHUNTVOLTAGE);
   float shunt_voltage;
   switch (gain_){
   case 1:
      if ((shunt_voltage_bits >> 12) & 0b1111){
         shunt_voltage = -(~(shunt_voltage_bits & 0xfff)+1)*SHUNT_MV_LSB;
      } else {
         shunt_voltage = shunt_voltage_bits*SHUNT_MV_LSB;
      }
      break;
   
   case 2:
      if ((shunt_voltage_bits >> 13) & 0b111){
         shunt_voltage = -(~(shunt_voltage_bits & 0x1fff)+1)*SHUNT_MV_LSB;
      } else {
         shunt_voltage = shunt_voltage_bits*SHUNT_MV_LSB;
      }
      break;

   case 4:
      if ((shunt_voltage_bits >> 14) & 0b11){
         shunt_voltage = -(~(shunt_voltage_bits & 0x3fff)+1)*SHUNT_MV_LSB;
      } else {
         shunt_voltage = shunt_voltage_bits*SHUNT_MV_LSB;
      }
      break;

   case 8:
      if ((shunt_voltage_bits >> 15)){
         shunt_voltage = -(~(shunt_voltage_bits & 0x7fff)+1)*SHUNT_MV_LSB;
      } else {
         shunt_voltage = shunt_voltage_bits*SHUNT_MV_LSB;
      }
      break;
   default:
      break;
   }
   return shunt_voltage;
}

float INA219::busVoltage(){
   uint16_t value = readRegisters(REGISTERS::BUSVOLTAGE);
   if (value & 0b1){
      std::cout << "Error: Power or Current out of range" << std::endl;
      return 0.;
   }
   else {
      return (int16_t)((value >> 3) * 4) * 0.001;
   }
}

void INA219::calibrate(int bus_volts_max, float max_expected_amps, float max_possible_amps){
   float shunt_voltage = shuntVoltage();
   current_lsb_ = determineCurrentLSB(max_expected_amps, max_possible_amps);
   uint16_t cal = trunc(0.04096/current_lsb_*R_SHUNT);
   writeRegister(REGISTERS::CALIBRATION, cal);

   power_lsb_ = 20*current_lsb_;
   uint16_t current = (shunt_voltage*cal)/4096; 
   writeRegister(REGISTERS::CURRENT, current);

   uint16_t power = (current*busVoltage())/5000;
   writeRegister(REGISTERS::POWER, power);
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
	return (float)current_lsb;
}

float INA219::current(){
   reset();
   valid_ = writeRegister(REGISTERS::CALIBRATION, 4096);
   // calibrate(26, 0.5, 3.2);
   // uint32_t ina219_currentDivider_mA = 10;
   // current_lsb_ = 10.0;
   
   if (valid_){
      return 0.f;
   } else {
         uint16_t current = readRegisters(REGISTERS::CURRENT);
      // if (current > 32767){
      //    current -= 65536;
      // }

      return (float)((float)readRegisters(REGISTERS::CURRENT)/(current_lsb_));
      
   }
   

}

float INA219::power(){
   int16_t power = (int16_t)readRegister(REGISTERS::POWER);
   return (float)power*power_lsb_*1000.0;
}

void INA219::reset(){
   int old_bits = readRegister(REGISTERS::CONFIG);
   int new_bits = 1 << CONFIG_BITS::RST | old_bits;
   writeRegister(REGISTERS::CONFIG, new_bits);
}