#include "Temperature.h"

Temperature::Temperature(/* args */) {}

float Temperature::GetTemp(int number) {
    int raw_value = ReadAnalog(number);
    // Voltage value between 0-1.8V
    float voltage = raw_value/4095;
}

int Temperature::ReadAnalog(int number) {
    std::stringstream ss;
    ss << LDR_PATH << number << "_raw";
    std::fstream fs;
    fs.open(ss.str().c_str(), std::fstream::in);
    fs >> number;
    fs.close();
    return number;
}

Temperature::~Temperature() {}