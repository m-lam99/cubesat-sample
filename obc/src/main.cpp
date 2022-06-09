/* A Simple GPIO application
 * Written by Derek Molloy for the book "Exploring BeagleBone: Tools and
 * Techniques for Building with Embedded Linux" by John Wiley & Sons, 2018
 * ISBN 9781119533160. Please see the file README.md in the repository root
 * directory for copyright and GNU GPLv3 license information.            */

#include <unistd.h> //for usleep

#include <iostream>
#include <unistd.h> //for usleep
#include <iomanip>  // for setprecision

#include "Test.h"
#include "Computer.h"
#include <vector>

int main()
{
    // testPWM(PWM_0A);
    // testGPIO();
    // wodTest();

    // GPS gps;
    // AS7263 sensor_(2, AS7263_ADDRESS);
    // Payload payload(&gps);
    // Transceiver transceiver_();
    // gps_(gps), sensor_(2, AS7263_ADDRESS)
    // Computer beaglebone;
    // beaglebone.mode_ = TRANSMIT_MODE;
    // beaglebone.runSatellite();
    // Satellite satellite;
    // beaglebone.mode_=IDLE_MODE;
    
    // can test without this 
    usleep(10000000); 
    // Test test;

    // test.testGPIO();
    //test.runTests();

    // Computer beagle;
    // beagle.littleRun();
    // beagle.runSatellite();

    return 0;
}
