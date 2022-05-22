#ifndef GPS_H
#define GPS_H

#include "UART.h"
#include <cstdint>
#include <inttypes.h>
#include <string.h>

#define GPS_CHANNEL 2
#define GPS_BAUD 9600 

#define _EMPTY 0x00
#define NMEA_GPRMC 0x01
#define NMEA_GPRMC_STR "$GPRMC"
#define NMEA_GPGGA 0x02
#define NMEA_GPGGA_STR "$GPGGA"
#define NMEA_UNKNOWN 0x00
#define _COMPLETED 0x03

#define NMEA_CHECKSUM_ERR 0x80
#define NMEA_MESSAGE_ERR 0xC0

using namespace std;

class GPS:public UARTDevice{
    
    public:
        struct location {
            double latitude;
            double longitude;
            double altitude;
            string time; 
            string date; 
            int epoch;   // milliseconds since 01/01/2000 
        };
        typedef struct location loc_t;


        struct gpgga {
            // Latitude eg: 4124.8963 (XXYY.ZZKK.. DEG, MIN, SEC.SS)
            double latitude;
            // Latitude eg: N
            char lat;
            // Longitude eg: 08151.6838 (XXXYY.ZZKK.. DEG, MIN, SEC.SS)
            double longitude;
            // Longitude eg: W
            char lon;
            // Quality 0, 1, 2
            uint8_t quality;
            // Number of satellites: 1,2,3,4,5...
            uint8_t satellites;
            // Altitude eg: 280.2 (Meters above mean sea level)
            double altitude;

            //utc time
            string time;
        };
        typedef struct gpgga gpgga_t;

        struct gprmc {
            double speed;
            double course;
            string date;   // date ddmmyy
        };
        typedef struct gprmc gprmc_t;

        // DEFAULT is Channel 4, baud 9600
        GPS( );
        void print_GPS(); 
        void gps_off();
        void get_location(loc_t *coord);
        void gps_on();

    private:
        uint8_t get_NMEA_type(const char *message);
        uint8_t nmea_valid_checksum(const char *message);
        void gps_convert_deg_to_dec(double *latitude, char ns,  double *longitude, char we);
        double gps_deg_dec(double deg_point);
        void nmea_parse_gpgga(char *nmea, gpgga_t *loc);
        void nmea_parse_gprmc(char *nmea, gprmc_t *loc);
        int convertToEpoch(string date, string time);  // returns ms since 1/1/2000

};


#endif