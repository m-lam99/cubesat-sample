// Class to read temperature from thermistor through analog input

#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <sstream>
#include <string>
#include <fstream>
#define LDR_PATH "/sys/bus/iio/devices/iio:device0/in_voltage"

class Temperature {
   private:
    /* data */
   public:
    Temperature(/* args */);
    float GetTemp(int number);
    int ReadAnalog(int number);
    ~Temperature();
};

#endif