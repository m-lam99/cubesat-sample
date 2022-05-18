#include "GPS.h"
#include <iostream>
#include <string.h>

using namespace std;

// DEFAULT is Channel 4, baud 9600
GPS::GPS( ):
    UARTDevice(GPS_CHANNEL, GPS_BAUD){
   
}

void GPS::print_GPS(){
    
    char buffer[256]; 
    readln(buffer);
    cout << buffer << endl;

}

