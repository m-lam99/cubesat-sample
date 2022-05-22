#include "Temperature.h"

Temperature::Temperature(int pin){
    pin_ = pin;
}

float Temperature::GetTemp() {
    int raw_value = ReadAnalog();
    // Voltage value between 0-1.8V
    float voltage = (float)raw_value/4095;
    float T = log(voltage/0.8962)/log(0.9629);
    return T;
}

int Temperature::ReadAnalog() {
    std::stringstream ss;
    ss << LDR_PATH << pin_ << "_raw";
    std::fstream fs;
    fs.open(ss.str().c_str(), std::fstream::in);
    fs >> pin_;
    fs.close();
    return pin_;
}

Temperature::~Temperature() {}