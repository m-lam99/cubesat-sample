//--- P2 Controller ---//
//   header file   //

#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include <iostream>
#include <cmath>

    class CControl
    {
        public:

        // initiate the controller object using new target orientation
        CControl(double target[3]);

        // run controller
        int runControlAlgorithm(void);

        private:

        // controller gains
        double gainQ, gainW;

        // target orientation quaternion
        double qRef[4];

        // measured orientation quaternion
        double qM[4];

        // measured orientation quaternion conjugate
        double qMConj[4];

        // measured angular velocities
        double angVels[3];

        // error quaternion
        double qErr[4];

        // control signal ouputted to system
        double output[3];

        // tolerance value
        double tolerance;

        // convert Euler angles to quaternions
        void eulerToQuat(double angles[3]);

        // calculate error using Kronecker product
        void calcKronecker(double p[4], double q[4]);

        // check if closest rotation
        bool isClosest(void);

        // calculate control signal and store as 3 by 1 matrix
        void getControlSignal(void);
    };

#endif