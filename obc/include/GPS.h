#ifndef GPS_H
#define GPS_H

#include "UART.h"
#include <cstdint>

class GPS:public UARTDevice{
    
    public:
         // DEFAULT is Channel 4, baud 9600
        GPS(unsigned int channel, unsigned int baud);
        void print_GPS(); 

    private:
};


#endif