#include "GPS.h"
#include <iostream>
#include <string.h>

using namespace std;

// DEFAULT is Channel 4, baud 9600
GPS::GPS(unsigned int channel, unsigned int baud):
    UARTDevice(channel, baud){
   
}

void GPS::print_GPS(){
    
    char buffer[256]; 
    readln(buffer);
    cout << buffer << endl;

}

