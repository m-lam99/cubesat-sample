/* A Simple GPIO application
 * Written by Derek Molloy for the book "Exploring BeagleBone: Tools and
 * Techniques for Building with Embedded Linux" by John Wiley & Sons, 2018
 * ISBN 9781119533160. Please see the file README.md in the repository root
 * directory for copyright and GNU GPLv3 license information.            */

#include <unistd.h>  //for usleep

#include <iostream>
#include <unistd.h> //for usleep
#include <iomanip> // for setprecision

#include "ADS1015.h"
#include "GPIO.h"
#include "I2C.h"
#include "INA219.h"
<<<<<<< HEAD
#include "GPS.h"
=======
#include "AS7263.h"
>>>>>>> main

#include "BNO055.h"

// For the PWM
#include "PWM.h"

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
    INA219 sensor1(1, 0x45);
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
        std::cout << (int)sensor.getVoltage(0) << ", " << (int)sensor.getVoltage(1) << std::endl;
        usleep(500000);
    }
}

void testAS7263() {
    AS7263 sensor(2, AS7263_ADDRESS);
   sensor.Test();
    for (int i = 0; i < 10; i++) {
        sensor.takeMeasurements();
        std::cout << "calibrated val: " << sensor.getCalibratedR() << std::endl;
        std::cout << "meas val: " << sensor.getR() << std::endl;
        usleep(500000);
    }
}

void testPWM(){
   // 
    PWM pwm("pwm-4:1");  // P9_42 MUST be loaded as a slot before use
    pwm.setPeriod(100000);         // Set the period in ns
    std::cout << "set period" << std::endl; 
     pwm.setDutyCycle(100.0f);       // Set the duty cycle as a percentage
    std::cout << "set duty" << std::endl; 

    pwm.setPolarity(PWM::ACTIVE_LOW);  // using active low PWM
     std::cout << "set polarity" << std::endl; 

    pwm.run();                     // start the PWM output
    std::cout << "RUN" << std::endl; 

    while(1);

    return; 
}

int main() {
    //  testINA219();
    //testBNO055();
    testPWM();
    //testADS1015();

    // testADS1015();
    testAS7263();
    return 0;
}