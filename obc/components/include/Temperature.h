// Class to read temperature from thermistor through analog input

#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <sstream>
#include <string>
#include <fstream>
#include <cmath>

#define LDR_PATH "/sys/bus/iio/devices/iio:device0/in_voltage"

class Temperature {
   private:
    int pin_;
   public:
    Temperature(int pin);
    float GetTemp();
    int ReadAnalog();
    ~Temperature();
};

#endif