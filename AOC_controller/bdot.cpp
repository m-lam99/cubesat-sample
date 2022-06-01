// ---- BDot Implementation ---- //

#include "bdot.h"

// Initialise detumbler object and declare function parameters
CDetumbler::CDetumbler(void)
{
    gain = 1;
    area = 63.6172512352;
    coils = 197;
}

// run the control algorithm for detumbling mode
double* CDetumbler::runController(void)
{

    // get angular velocity and magnetic field readings
    // DUMMY VALUES, NEED TO CHANGE
    getData();

    // calculate current values
    for (int i = 0; i < 3; i++)
    {
        double temp;

        switch (i)
        {
        case 0:
            temp = (angVel[1]*magField[2])-(magField[1]*angVel[2]);
            break;

        case 1:
            temp = (angVel[2]*magField[0])-(magField[2]*angVel[0]);
            break;

        case 2:
            temp = (angVel[0]*magField[1])-(magField[0]*angVel[1]);
            break;
        }

        temp /= (coils * area);

        // store value in output variable
        outputCurrents[i] = temp;
    }

    // return outputs as array
    return outputCurrents;
}

// get raw IMU data for angular velocity and magnetic field strength
void CDetumbler::getData(void)
{
    // DUMMY VALUES
    // when integrating, need to change this to request input from IMU
    // NOTE TO PROGRAMMER - check the units of IMU data

    double rawAngVel[3] = {0.3, 0.2, -0.1};
    double rawMagField[3] = {0.03, 0.4, 0.008};

    for (int i = 0; i < 3; i++)
    {
        angVel[i] = rawAngVel[i];
        magField[i] = rawMagField[i];
    }
}