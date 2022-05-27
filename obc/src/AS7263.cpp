#include "AS7263.h"

#include <unistd.h>
#include <cstring>

#include <iostream>

using namespace exploringBB;

AS7263::AS7263(unsigned int I2CBus, unsigned int I2CAddress)
    : I2CDevice(I2CBus, I2CAddress) {
    this->I2CAddress = I2CAddress;
    this->I2CBus = I2CBus;

    setBulbCurrent(0b00); //Set to 12.5mA (minimum)
	disableBulb(); //Turn off to avoid heating the sensor

	setIntegrationTime(50); //50 * 2.8ms = 140ms. 0 to 255 is valid.
							//If you use Mode 2 or 3 (all the colors) then integration time is double. 140*2 = 280ms between readings.

	setGain(3); //Set gain to 64x

	setMeasurementMode(3); //One-shot reading of VBGYOR
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
    return (int)d;
}

void AS7263::enableBulb() {
    // Read, mask/set, write
    uint8_t value = readVirtualReg(VIRTUAL_REG::LED_CONTROL);
    value |= (1 << 3);  // Set the bit
    writeVirtualReg(VIRTUAL_REG::LED_CONTROL, value);
}

// Disable the onboard 5700k or external incandescent bulb
void AS7263::disableBulb() {
    // Read, mask/set, write
    uint8_t value = readVirtualReg(VIRTUAL_REG::LED_CONTROL);
    value &= ~(1 << 3);  // Clear the bit
    writeVirtualReg(VIRTUAL_REG::LED_CONTROL, value);
}

void AS7263::setBulbCurrent(uint8_t current) {
    if (current > 0b11) current = 0b11;  // Limit to two bits

    // Read, mask/set, write
    uint8_t value = readVirtualReg(VIRTUAL_REG::LED_CONTROL);  // Read
    value &= 0b11001111;      // Clear ICL_DRV bits
    value |= (current << 4);  // Set ICL_DRV bits with user's choice
    writeVirtualReg(VIRTUAL_REG::LED_CONTROL, value);  // Write
}

void AS7263::setIntegrationTime(uint8_t integrationValue) {
    writeVirtualReg(VIRTUAL_REG::INT_T, integrationValue);  // Write
}

//Sets the gain value
//Gain 0: 1x (power-on default)
//Gain 1: 3.7x
//Gain 2: 16x
//Gain 3: 64x
void AS7263::setGain(uint8_t gain)
{
	if (gain > 0b11) gain = 0b11;

	//Read, mask/set, write
	uint8_t value = readVirtualReg(VIRTUAL_REG::CONTROL_SETUP); //Read
	value &= 0b11001111; //Clear GAIN bits
	value |= (gain << 4); //Set GAIN bits with user's choice
	writeVirtualReg(VIRTUAL_REG::CONTROL_SETUP, value); //Write
}

//Sets the measurement mode
//Mode 0: Continuous reading of VBGY (7262) / STUV (7263)
//Mode 1: Continuous reading of GYOR (7262) / RTUX (7263)
//Mode 2: Continuous reading of all channels (power-on default)
//Mode 3: One-shot reading of all channels
void AS7263::setMeasurementMode(uint8_t mode)
{
	if (mode > 0b11) mode = 0b11;

	//Read, mask/set, write
	uint8_t value = readVirtualReg(VIRTUAL_REG::CONTROL_SETUP); //Read
	value &= 0b11110011; //Clear BANK bits
	value |= (mode << 2); //Set BANK bits with user's choice
	writeVirtualReg(VIRTUAL_REG::CONTROL_SETUP, value); //Write
}

//Tells IC to take measurements and polls for data ready flag
void AS7263::takeMeasurements()
{
	clearDataAvailable(); //Clear DATA_RDY flag when using Mode 3

						  //Goto mode 3 for one shot measurement of all channels
	setMeasurementMode(3);

	//Wait for data to be ready
	while (dataAvailable() == false) usleep(POLLING_DELAY);

	//Readings can now be accessed via getViolet(), getBlue(), etc
}

//Clears the DRDY flag
//Normally this should clear when data registers are read
void AS7263::clearDataAvailable()
{
	uint8_t value = readVirtualReg(VIRTUAL_REG::CONTROL_SETUP);
	value &= ~(1 << 1); //Set the DATA_RDY bit
	writeVirtualReg(VIRTUAL_REG::CONTROL_SETUP, value);
}


//Checks to see if DRDY flag is set in the control setup register
bool AS7263::dataAvailable()
{
	uint8_t value = readVirtualReg(VIRTUAL_REG::CONTROL_SETUP);
	return (value & (1 << 1)); //Bit 1 is DATA_RDY
}


//Given an address, read four uint8_ts and return the floating point calibrated value
float AS7263::getCalibratedValue(uint8_t calAddress)
{
	uint8_t b0, b1, b2, b3;
	b0 = readVirtualReg(calAddress + 0);
	b1 = readVirtualReg(calAddress + 1);
	b2 = readVirtualReg(calAddress + 2);
	b3 = readVirtualReg(calAddress + 3);

	//Channel calibrated values are stored big-endian
	uint32_t calBytes = 0;
	calBytes |= ((uint32_t)b0 << (8 * 3));
	calBytes |= ((uint32_t)b1 << (8 * 2));
	calBytes |= ((uint32_t)b2 << (8 * 1));
	calBytes |= ((uint32_t)b3 << (8 * 0));

	return (convertBytesToFloat(calBytes));
}

//Given 4 uint8_ts returns the floating point value
float AS7263::convertBytesToFloat(uint32_t myLong)
{
	float myFloat;
	std::memcpy(&myFloat, &myLong, 4); //Copy uint8_ts into a float
	return (myFloat);
}

//Turns on bulb, takes measurements, turns off bulb
void AS7263::takeMeasurementsWithBulb()
{
	//enableIndicator(); //Tell the world we are taking a reading. 
	//The indicator LED is red and may corrupt the readings

	enableBulb(); //Turn on bulb to take measurement

	takeMeasurements();

	disableBulb(); //Turn off bulb to avoid heating sensor
				   //disableIndicator();
}


//A the 16-bit value stored in a given channel registerReturns 
int AS7263::getChannel(uint8_t channelRegister)
{
	int colorData = readVirtualReg(channelRegister) << 8; //High uint8_t
	colorData |= readVirtualReg(channelRegister + 1); //Low uint8_t
	return(colorData);
}