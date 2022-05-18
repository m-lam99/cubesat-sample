#ifndef GPS_H
#define GPS_H

#include "UART.h"
#include <cstdint>

class GPS:public UARTDevice{
    
    public:
        GPS(unsigned int channel, unsigned int baud);
        void print_GPS(); 

    private:
};


#endif