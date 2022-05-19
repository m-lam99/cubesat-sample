/* A Simple GPIO application
 * Written by Derek Molloy for the book "Exploring BeagleBone: Tools and
 * Techniques for Building with Embedded Linux" by John Wiley & Sons, 2018
 * ISBN 9781119533160. Please see the file README.md in the repository root
 * directory for copyright and GNU GPLv3 license information.            */

#include <unistd.h>  //for usleep

#include <iostream>

#include "ADS1015.h"
#include "GPIO.h"
#include "I2C.h"
#include "INA219.h"

using namespace exploringBB;

void testGPIO() {
    GPIO outGPIO(59), inGPIO(46);

    // Basic Output - Flash the LED 10 times, once per second
    outGPIO.setDirection(OUTPUT);
    for (int i = 0; i < 10; i++) {
        outGPIO.setValue(HIGH);
        usleep(500000);  // micro-second sleep 0.5 seconds
        outGPIO.setValue(LOW);
        usleep(500000);
    }
    // Basic Input example
    inGPIO.setDirection(INPUT);
    std::cout << "The value of the input is: " << inGPIO.getValue()
              << std::endl;

    // Fast write to GPIO 1 million times
    outGPIO.streamOpen();
    for (int i = 0; i < 1000000; i++) {
        outGPIO.streamWrite(HIGH);
        outGPIO.streamWrite(LOW);
    }
    outGPIO.streamClose();
}

void testINA219() {
    INA219 sensor1(1, INA219_ADDRESS_3V3);
    // INA219 sensor2(1, INA219_ADDRESS2);
    // INA219 sensor3(1, INA219_ADDRESS3);
    // INA219 sensor4(1, INA219_ADDRESS4);
    sensor1.configure(INA219::VOLTAGE_RANGE::FSR_32,
                      INA219::PGA_GAIN::GAIN_8_320MV, 1, 3);
    // sensor2.configure(INA219::VOLTAGE_RANGE::FSR_32,
    // INA219::PGA_GAIN::GAIN_8_320MV, 1, 3);
    // sensor3.configure(INA219::VOLTAGE_RANGE::FSR_32,
    // INA219::PGA_GAIN::GAIN_8_320MV, 1, 3);
    // sensor4.configure(INA219::VOLTAGE_RANGE::FSR_32,
    // INA219::PGA_GAIN::GAIN_8_320MV, 1, 3);
    uint16_t value1 = sensor1.readRegisters(INA219::REGISTERS::CONFIG);
    // uint16_t value2 = sensor2.readRegister(INA219::REGISTERS::CONFIG);
    // uint16_t value3 = sensor3.readRegister(INA219::REGISTERS::CONFIG);
    // uint16_t value4 = sensor4.readRegister(INA219::REGISTERS::CONFIG);
    // sensor1.calibrate(26, 0.5, 3.2);
    sensor1.writeRegister(INA219::REGISTERS::CALIBRATION, 4096);
    // std::cout << value1 << std::endl;
    // std::cout << value2 << std::endl;
    // std::cout << value3 << std::endl;
    // std::cout << value4 << std::endl;
    std::cout << sensor1.busVoltage() << std::endl;
    std::cout << sensor1.current() << std::endl;
}

void testADS1015() {
    ADS1015 sensor(2, ADC_ADDRESS1);

    for (int i = 0; i < 10; i++) {
        std::cout << sensor.getVoltage(0) << std::endl;
        usleep(500000);
    }
}

int main() {
    testINA219();
    // GPIO outGPIO(59), inGPIO(46);

    // // Basic Output - Flash the LED 10 times, once per second
    // outGPIO.setDirection(OUTPUT);
    // for (int i=0; i<10; i++){
    //    outGPIO.setValue(HIGH);
    //    usleep(500000); //micro-second sleep 0.5 seconds
    //    outGPIO.setValue(LOW);
    //    usleep(500000);
    // }
    // // Basic Input example
    // inGPIO.setDirection(INPUT);
    // std::cout << "The value of the input is: "<< inGPIO.getValue() <<
    // std::endl;

    // // Fast write to GPIO 1 million times
    // outGPIO.streamOpen();
    // for (int i=0; i<1000000; i++){
    //    outGPIO.streamWrite(HIGH);
    //    outGPIO.streamWrite(LOW);
    // }
    // outGPIO.streamClose();

    return 0;
}