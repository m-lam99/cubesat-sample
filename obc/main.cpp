/* A Simple GPIO application
* Written by Derek Molloy for the book "Exploring BeagleBone: Tools and
* Techniques for Building with Embedded Linux" by John Wiley & Sons, 2018
* ISBN 9781119533160. Please see the file README.md in the repository root
* directory for copyright and GNU GPLv3 license information.            */

#include <iostream>
#include <unistd.h> //for usleep
#include "GPIO.h"
#include "I2C.h"
#include "INA219.h"

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

void testINA219(){
   INA219 sensor(1, INA219_ADDRESS);
   sensor.configure(INA219::VOLTAGE_RANGE::FSR_32, INA219::PGA_GAIN::GAIN_8_320MV, 0, 0);
   
}

int main(){

   return 0;
}