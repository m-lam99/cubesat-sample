#include "GPS.h"
#include <iostream>
#include <string.h>
#include <math.h>

using namespace std;

// DEFAULT is Channel 4, baud 9600
GPS::GPS( ):
    UARTDevice(GPS_CHANNEL, GPS_BAUD){
   
}

void GPS::gps_on( ){
   this->init();
   this->config();
   cout << "GPS on" << endl; 
}

void GPS::print_GPS(){
    cout << "PRINT GPS" << endl; 
    loc_t data;

    while (1) {
        get_location(&data);
        printf("%lf %lf %lf %lf\n", data.time, data.latitude, data.longitude, data.altitude);
    }

}

void GPS::get_location(loc_t *coord){

    uint8_t status = _EMPTY;
    while(status != _COMPLETED) {
        gpgga_t gpgga;
        char buffer[256];

        readln(buffer);
        switch (get_NMEA_type(buffer)) {
            case NMEA_GPGGA:
                nmea_parse_gpgga(buffer, &gpgga);

                gps_convert_deg_to_dec(&(gpgga.latitude), gpgga.lat, &(gpgga.longitude), gpgga.lon);

                coord->latitude = gpgga.latitude;
                coord->longitude = gpgga.longitude;
                coord->altitude = gpgga.altitude;
                coord->time = gpgga.time; 

                status |= NMEA_GPGGA;
                status = _COMPLETED;
                break;
            default:
                break;
        }
    }
}

// Convert lat e lon to decimals (from deg)
void GPS::gps_convert_deg_to_dec(double *latitude, char ns,  double *longitude, char we)
{
    double lat = (ns == 'N') ? *latitude : -1 * (*latitude);
    double lon = (we == 'E') ? *longitude : -1 * (*longitude);

    *latitude = gps_deg_dec(lat);
    *longitude = gps_deg_dec(lon);
}

double GPS::gps_deg_dec(double deg_point)
{
    double ddeg;
    double sec = modf(deg_point, &ddeg)*60;
    int deg = (int)(ddeg/100);
    int min = (int)(deg_point-(deg*100));

    double absdlat = round(deg * 1000000.);
    double absmlat = round(min * 1000000.);
    double absslat = round(sec * 1000000.);

    return round(absdlat + (absmlat/60) + (absslat/3600)) /1000000;
}

uint8_t GPS::get_NMEA_type(const char *message)
{
    uint8_t checksum = 0;
    if ((checksum = nmea_valid_checksum(message)) != _EMPTY) {
        return checksum;
    }

    if (strstr(message, NMEA_GPGGA_STR) != NULL) {
        return NMEA_GPGGA;
    }

    if (strstr(message, NMEA_GPRMC_STR) != NULL) {
        return NMEA_GPRMC;
    }

    return NMEA_UNKNOWN;
}

void GPS::nmea_parse_gpgga(char *nmea, gpgga_t *loc)
{
    char *p = nmea;

    p = strchr(p, ',')+1; //skip time
    loc->time = atof(p);

    p = strchr(p, ',')+1;
    loc->latitude = atof(p);

    p = strchr(p, ',')+1;
    switch (p[0]) {
        case 'N':
            loc->lat = 'N';
            break;
        case 'S':
            loc->lat = 'S';
            break;
        case ',':
            loc->lat = '\0';
            break;
    }

    p = strchr(p, ',')+1;
    loc->longitude = atof(p);

    p = strchr(p, ',')+1;
    switch (p[0]) {
        case 'W':
            loc->lon = 'W';
            break;
        case 'E':
            loc->lon = 'E';
            break;
        case ',':
            loc->lon = '\0';
            break;
    }

    p = strchr(p, ',')+1;
    loc->quality = (uint8_t)atoi(p);

    p = strchr(p, ',')+1;
    loc->satellites = (uint8_t)atoi(p);

    p = strchr(p, ',')+1;

    p = strchr(p, ',')+1;
    loc->altitude = atof(p);
}


uint8_t GPS::nmea_valid_checksum(const char *message) {
    uint8_t checksum= (uint8_t)strtol(strchr(message, '*')+1, NULL, 16);

    char p;
    uint8_t sum = 0;
    ++message;
    while ((p = *message++) != '*') {
        sum ^= p;
    }

    if (sum != checksum) {
        return NMEA_CHECKSUM_ERR;
    }

    return _EMPTY;
}

void GPS::gps_off(){
    close(); 
    cout << "GPS OFF" << endl; 
}