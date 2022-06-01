//---- B-Dot Algorithm ----//
// Header file

#ifndef _BDOT_H
#define _BDOT_H

#include <iostream>
#include <cmath>

    class CDetumbler
    {
        public:

        // initiate detumbler object
        CDetumbler(void);

        // run B-Dot controller
        double* runController(void);

        private:

        // proportional gain value
        double gain;

        // number of coils
        int coils;

        // cross-sectional area of magnetorquers in mm
        double area;

        // variable to hold magnetic field readings
        double magField[3];

        // variable to hold angular velocity readings
        double angVel[3];

        // variable to hold the calculated values for each current value
        double outputCurrents[3];

        // obtain IMU data
        void getData(void);
    };

#endif