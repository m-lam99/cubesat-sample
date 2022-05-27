/* A Simple GPIO application
 * Written by Derek Molloy for the book "Exploring BeagleBone: Tools and
 * Techniques for Building with Embedded Linux" by John Wiley & Sons, 2018
 * ISBN 9781119533160. Please see the file README.md in the repository root
 * directory for copyright and GNU GPLv3 license information.            */

#include <unistd.h> //for usleep

#include <iostream>
#include <unistd.h> //for usleep
#include <iomanip>  // for setprecision

// #include "ADS1015.h"
// #include "GPIO.h"
// #include "I2C.h"
// #include "INA219.h"
// #include "GPS.h"
// #include "AS7263.h"
// #include "transceiver.h"
// #include "BNO055.h"
// #include "Satellite.h"
#include "Computer.h"

// For the PWM
//#include "PWM.h"

#include <vector>

using namespace exploringBB;

void testGPIO() {
    GPIO outGPIO(23), inGPIO(46);

    // Basic Output - Flash the LED 10 times, once per second
    outGPIO.setDirection(OUTPUT);
    for (int i = 0; i < 10; i++)
    {
        outGPIO.setValue(HIGH);
        usleep(500000); // micro-second sleep 0.5 seconds
        outGPIO.setValue(LOW);
        usleep(500000);
    }
    // Basic Input example
    inGPIO.setDirection(INPUT);
    std::cout << "The value of the input is: " << inGPIO.getValue()
              << std::endl;

    // Fast write to GPIO 1 million times
    outGPIO.streamOpen();
    for (int i = 0; i < 1000000; i++)
    {
        outGPIO.streamWrite(HIGH);
        outGPIO.streamWrite(LOW);
    }
    outGPIO.streamClose();
}

void testINA219()
{
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

void testADS1015()
{
    ADS1015 adc1(1, ADC_ADDRESS1);
    ADS1015 adc2(2, ADC_ADDRESS1);

    for (int i = 0; i < 10; i++)
    {
        std::cout << (int)adc1.getVoltage(0) << ", " << (int)adc1.getVoltage(1) << std::endl;
        usleep(500000);
    }

    /*
    adc1:
        0: +x
        1: +y
        2: +z
    adc2:
        0: -x
        1: -y
        2: -z
    */

    float voltages[6];
    for (int i = 0; i < 3; i++)
    {
        voltages[i] = adc1.getVoltage(i);
        voltages[i + 3] = adc2.getVoltage(i);
    }

    for (int i = 0; i < 6; i++)
    {
        std::cout << voltages[i] << std::endl;
    }
}

void testAS7263()
{
    AS7263 sensor(2, AS7263_ADDRESS);
    sensor.Test();
    for (int i = 0; i < 10; i++)
    {
        sensor.takeMeasurements();
        std::cout << "calibrated val: " << sensor.getCalibratedR() << std::endl;
        std::cout << "meas val: " << sensor.getR() << std::endl;
        usleep(500000);
    }
}

void testPWM(string pwm_channel){
   // 
    PWM pwm(pwm_channel);  // P9_42 MUST be loaded as a slot before use
    pwm.setPeriod(3345678);         // Set the period in ns
    pwm.setDutyCycle(50.0f);       // Set the duty cycle as a percentage

    pwm.setPolarity(PWM::ACTIVE_LOW);  // using active low PWM
    pwm.run();                     // start the PWM output
    std::cout << "PWM active : " <<  pwm.getPeriod() << std::endl; 
    // pwm.stop();  // to discontinue the pwm signal 
    usleep(1000000*10);
    pwm.stop();
    return; 
}

void testBNO055()
{
    BNO055 bno(2, BNO055_ADDRESS_A);
    std::cout << "Orientation Sensor Raw Data Test" << std::endl;
    if (!bno.begin())
    {
        /* There was a problem detecting the BNO055 ... check your connections */
        std::cout << "Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!" << std::endl;
        while (1)
            ;
    }

    usleep(1000);

    /* System Status */
    int8_t status = bno.getSystemStatus();
    std::cout << "System status: " << (int)status << std::endl;

    /* Display the current temperature */
    int8_t temp = bno.getTemp();
    std::cout << "Current Temperature: " << (int)temp << " C" << std::endl;

    std::cout << "Calibration status values: 0=uncalibrated, 3=fully calibrated" << std::endl;

    bno.setExtCrystalUse(true);

    while (1)
    {

        // imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
        // std::cout << "X: " << euler.x() <<  " Y: " << euler.y() << " Z: "
	    //	<< euler.z() << "\t\t";

        // Display Quaternions
        // imu::Quaternion quat = bno.getQuat();
        // std::cout << "qW: " << quat.w() << " qX: " << quat.x() << " qY: " << quat.y() << " qZ: " << quat.z() << "\t\t";

        /* Display calibration status for each sensor. */
        uint8_t system, gyro, accel, mag = 0;
        bno.getCalibration(&system, &gyro, &accel, &mag);
        imu::Vector<3> euler = bno.getVector(BNO055::VECTOR_EULER);


	    /* Display the floating point data */
	    std::cout << "X: " << euler.x() <<  " Y: " << euler.y() << " Z: "
	  	<< euler.z() << "\t\t";

        std::cout << "CALIBRATION: Sys=" << (int)system << " Gyro=" << (int)gyro
                  << " Accel=" << (int)accel << " Mag=" << (int)mag << std::endl;

        usleep(10000 * BNO055_SAMPLERATE_DELAY_MS);
    }
}

void testGPS()
{

    GPS gps;
    gps.gps_on();
    for (int i = 0; i < 1000; i++)
    {
        gps.print_GPS();
        usleep(200);
    }
    gps.gps_off();
    return;
}

void wodTest() {
    Satellite NICE;
    std::cout << "test" << std::endl;
    NICE.wodCollection();
    // NICE.wodTransmission();
    
}


void testPropulsion(){
    Satellite NICE; 
    std::vector<int> prop_vals{1, 3, 9, 5, 6, 2};
    NICE.propulsion(prop_vals);
}

void testTransceiver(){
    Transceiver comms;

    comms.TestTransceiver();
    // while(1){
    //     std::vector<uint8_t> message = {'T', 'e', 's', 't'};
    //     comms.TransmitMessage(message);
    //     usleep(1000000);
    // }
    // std::vector<uint8_t> message = {'T', 'e', 's', 't'};
    // comms.TransmitMessage(message);
    // usleep(1000000);
    while(1){
        comms.SendCommand(comms.CMD_RECEIVE_MODE_CONFIG);
        std::vector<uint8_t> message = comms.ReceiveData();
        std::cout << "MESSAGE: "; 

       for (unsigned int i = 0; i < comms.MAX_BYTES_AX25; i++)
            {
                if(message[0] == '#' && message[1] == 'R' && i>3 && i<18){
                    std::cout << message[i]; 

                }
            }
        std::cout << std::endl; 
        usleep(1000000);
    }
    
}

int main() {
    // testPWM(PWM_0A);
    // testGPIO();
    //wodTest();
   
   GPS gps; 
    Payload payload(&gps); 
    Transceiver transceiver_();
    // gps_(gps), sensor_(2, AS7263_ADDRESS)
    // Computer beaglebone; 
    // beaglebone.mode_ = TRANSMIT_MODE; 
    // beaglebone.runSatellite(); 
    
    // usleep(20000000); 
    // beaglebone.mode_=IDLE_MODE; 
    return 0;
}