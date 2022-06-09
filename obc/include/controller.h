//--- P2 Controller ---//
//   header file   //

#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include <iostream>
#include <cmath>
#include "utility/imumaths.h"


    class CControl
    {
        public:

        double gain = 1;
        int coils = 197;
        double area = 63.6172512352;

        // initiate the controller object using new target orientation
        CControl(double phi, double theta, double psi);

        // run controller
        imu::Vector<3> runControlAlgorithm(imu::Quaternion measurements, imu::Vector<3> velocities);

        // Change pointing direction 
        void changeTarget(double phi, double theta, double psi); 
        
        bool getTolerance(void);

        imu::Vector<3> detumble(imu::Vector<3> rps, imu::Vector<3> mags);

        private:

        // controller gains
        double gainQ, gainW;


        // target orientation quaternion
        //double qRef[4];
        imu::Quaternion qRef;

        // measured orientation quaternion
        imu::Quaternion qM;

        // measured orientation quaternion conjugate
        imu::Quaternion qMConj;

        // measured angular velocities
        double angVels[3];

        // error quaternion
        //double qErr[4];
        imu::Quaternion qErr;

        // control signal ouputted to system as Euler angles
        //double output[3];
        imu::Vector<3> output;

        // tolerance value
        double tolerance;

        // convert Euler angles to quaternions
        void eulerToQuat(double phi, double theta, double psi);

        // calculate error using Kronecker product
        void calcKronecker(imu::Quaternion p, imu::Quaternion q);

        // check if closest rotation
        bool isClosest(void);

        // calculate control signal and store as 3 by 1 matrix
        void getControlSignal(void);

        // convert quaternion to euler angle
        void quatToEuler(double q[4]);

    };

#endif