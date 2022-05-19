#ifndef GPS_H
#define GPS_H

#include "UART.h"
#include <cstdint>

#define GPS_CHANNEL 4
#define GPS_BAUD 9600 

class GPS:public UARTDevice{
    
    public:
         // DEFAULT is Channel 4, baud 9600
        GPS( );
        void print_GPS(); 
        void gps_off();

    private:

};


#endif