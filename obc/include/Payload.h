// Payload class to control data collection and transmission of payload data

#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <stdint.h>

#include "AS7263.h"
#include "GPS.h"

class Payload {
   public:
    struct payload_data {
        float R;
        float S;
        float T;
        float U;
        float V;
        float W;
        double lat;
        double lon;
        double alt;
    };

    typedef struct payload_data payload_data_t;

    struct payload_data_encoded {
        int16_t R;
        int16_t S;
        int16_t T;
        int16_t U;
        int16_t V;
        int16_t W;
        int16_t lat;
        int16_t lon;
        int16_t alt;
    };

    typedef struct payload_data_encoded payload_data_enc_t;
    Payload(GPS* gps);
    payload_data_t getData();
    payload_data_enc_t getDataEncoded();
    ~Payload();

   private:
    AS7263 sensor_;
    GPS* gps_;
    payload_data_t data_buf_;
    payload_data_enc_t data_buf_encoded_;
    int valid_;
};

#endif