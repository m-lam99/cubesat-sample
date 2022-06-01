// ----- Main file ------ //

// This algorithm aims to run a double proportional control algorithm in quaternion space. Wish us luck.

// POINTS FOR INTEGRATION:
// - need three integration points
// - will be fed target pointing position in euler angles (main.cpp)
// - will need constant readings from IMU (both in controller.cpp) for 
//  current position in quaternions and current angular velocity
// - code also includes several 'print to terminal' checks that can be removed

#include"controller.h"
#include <iostream>

int main(void)
{

    // we receive a target orientation in euler angles
    double references[3] = {0, 1.5708, 0};

    // create controller object and run algorithm
    CControl newController(references);
    newController.runControlAlgorithm();

    return 0;
}