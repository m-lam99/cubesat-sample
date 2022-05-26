// Payload class to control data collection and transmission of payload data

#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <stdint.h>

#include "AS7263.h"
#include "GPS.h"

class Payload {
   public:
    struct payload_data {
        uint16_t R;
        uint16_t S;
        uint16_t T;
        uint16_t U;
        uint16_t V;
        uint16_t W;
        int16_t lat;
        int16_t lon;
        int16_t alt;
    };

    typedef struct payload_data payload_data_t;
    Payload(GPS* gps);
    payload_data_t getData();
    ~Payload();

   private:
    AS7263 sensor_;
    GPS* gps_;
    payload_data_t data_buf_;
    int valid_;
};

#endif