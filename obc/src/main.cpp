/* A Simple GPIO application
* Written by Derek Molloy for the book "Exploring BeagleBone: Tools and
* Techniques for Building with Embedded Linux" by John Wiley & Sons, 2018
* ISBN 9781119533160. Please see the file README.md in the repository root
* directory for copyright and GNU GPLv3 license information.            */

#include <iostream>
#include <unistd.h> //for usleep
#include "GPIO.h"
#include "I2C.h"
#include "PWM.h"

void testGPIO(){
   GPIO outGPIO(59), inGPIO(46);

   // Basic Output - Flash the LED 10 times, once per second
   outGPIO.setDirection(OUTPUT);
    for (int i=0; i<10; i++){
        outGPIO.setValue(HIGH);
        usleep(500000); //micro-second sleep 0.5 seconds
        outGPIO.setValue(LOW);
        usleep(500000);
   }
   // Basic Input example
   inGPIO.setDirection(INPUT);
   std::cout << "The value of the input is: "<< inGPIO.getValue() << std::endl;

   // Fast write to GPIO 1 million times
    outGPIO.streamOpen();
    for (int i=0; i<1000000; i++){
        outGPIO.streamWrite(HIGH);
        outGPIO.streamWrite(LOW);
    }
    outGPIO.streamClose();
}

void testPWM(){
    /* PWM overlay name does not seem correct */ 
    // try PWMX-00A0.dtbo - seen in /lib/firmware  where x = 0/1/2/3/
    PWM pwm("pwm_test_P9_42.12");  // P9_42 MUST be loaded as a slot before use
    pwm.setPeriod(100000);         // Set the period in ns
    pwm.setDutyCycle(25.0f);       // Set the duty cycle as a percentage
    pwm.setPolarity(PWM::ACTIVE_LOW);  // using active low PWM
    pwm.run();                     // start the PWM output
    std::cout << "DUTY CYCLE: " << std::setprecision(2) << pwm.getDutyCyclePercent() << std::endl; 
    std::cout << "Period: " << pwm.getPeriod() << std::endl; 
    pwm.stop(); 

    return; 
}



void testI2C(){
   
}

int main(){
   testPWM(); 
   return 0;
}