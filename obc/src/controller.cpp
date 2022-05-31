//--- Implementation of Controller class ---//

#include <unistd.h>
#include <vector>

#include "controller.h"
#include "PWM.h"
#include "BNO055.h"

CControl::CControl(double phi, double theta, double psi)
{
    // convert to quaternion and store
    eulerToQuat(phi, theta, psi);

    // set controller gains
    gainQ = 20;
    gainW = 4;

    // set tolerance value
    tolerance = 0.2;
}

void CControl::changeTarget(double phi, double theta, double psi){
    eulerToQuat(phi, theta, psi); 
}
// Convert Eluer angles to quaternions
void CControl::eulerToQuat(double phi, double theta, double psi)
{
    // extract angle values
    // double phi = angles[0];
    // double theta = angles[1];
    // double psi = angles[2];
    
    // calculate terms
    double a, b, c, d;
    a = cos(phi/2) * cos(theta/2) * cos(psi/2) + sin(phi/2) * sin(theta/2) * sin(psi/2);
    b = sin(phi/2) * cos(theta/2) * cos(psi/2) - cos(phi/2) * sin(theta/2) * sin(psi/2);
    c = cos(phi/2) * sin(theta/2) * cos(psi/2) + sin(phi/2) * cos(theta/2) * sin(psi/2);
    d = cos(phi/2) * cos(theta/2) * sin(psi/2) - sin(phi/2) * sin(theta/2) * cos(psi/2);

    // store values in reference variable
    qRef = imu::Quaternion(a,b,c,d);
    // qRef[0] = a;
    // qRef[1] = b;
    // qRef[2] = c;
    // qRef[3] = d;
}

// P2 control algorithm
imu::Vector<3> CControl::runControlAlgorithm(imu::Quaternion measurements, imu::Vector<3> velocities)
{
    // SIMULATION MODE - update measurements from code
    // if in real time mode, get measurements in each iteration

    // get current state measurement in quaternions and store as well as conjugate
    // DUMMY VALUE
    // double measurements[4] = {-0.163513, -0.00360107, 0.0134277, -0.98645};

    imu::Quaternion qMConj = measurements.conjugate();

    // for (int i = 0; i < 4; i++)
    // {
    //     qM[i] = measurements[i];
    //     if (i > 0)
    //     {
    //         qMConj[i] = -measurements[i];
    //     } else {
    //         qMConj[i] = measurements[i];
    //     }
    // }

    // get current angular velocity measurements and store in variable
    // DUMMY VALUE assuming constant maximum speed required
    // double velocities[3] = {0.02, 0.0017, 0.014};
    for (int i = 0; i < 3; i++)
    {
        angVels[i] = velocities[i];
    }

    // calculate error quaternion as Kronecker product of reference and conjugate 
    // measurement quaternions, and store in variable
    calcKronecker(qRef, qMConj);

    // check if eror calculation uses closest rotation
    // adjust error quaternion if we do not have closest rotation
    if (!isClosest())
    {
        qErr = qErr.conjugate();
        // for (int i = 1; i <= 3; i++)
        // {
        //     qErr[i] = -qErr[i];
        // }
    }

    // obtain the control signal and send to system
    getControlSignal();
    for (int i = 0; i < 3; i++)
    {
        std::cout << output[i] << std::endl;
    }
    std::cout << "---" << std::endl;

    // NOT PART OF ALGORITHM
    // get new measurement
    // will need IMU reading
    // for (int i = 1; i < 4; i++)
    // {
    //     qM[i] = qM[i] + output[i-1];
    //     qMConj[i] = -qM[i];
    // }


    return output;
}

// calculate Kronecker product of two quaternions
void CControl::calcKronecker(imu::Quaternion p, imu::Quaternion q)
{
    // create variables to store product values
    double a, b, c, d;

    // calculate first term
    a = p.w()*q.w() - p.x()*q.x() - p.y()*q.y() - p.z()*q.z();

    // second term
    b = p.w()*q.x() + p.x()*q.w() + p.y()*q.z() - p.z()*q.y();

    // third term
    c = p.w()*q.y() - p.x()*q.z() + p.y()*q.w() + p.z()*q.x();

    // fourth term
    d = p.w()*q.z() + p.x()*q.y() - p.y()*q.x() - p.z()*q.w();

    // store Kronecker product quaternion as error value
    qErr = imu::Quaternion(a,b,c,d);
}

// check if closest rotation used
bool CControl::isClosest(void)
{
    bool result;

    // if first error term is negative, then desired orientation is more than
    // pi radians away, and so we do not have the closest rotation
    if (qErr.w() < 0)
    {
        result = false;
    } else {
        result = true;
    }

    return result;
}

// calculate control signal and store in object variable
void CControl::getControlSignal(void)
{
    // create temporary variables to store matrxi information
    double a[3], b[3];

    // obtain first term in algorithm
    a[0] = qErr.x() * gainQ;
    a[1] = qErr.y() * gainQ;
    a[2] = qErr.z() * gainQ;

    // for (int i = 0; i < 3; i++)
    // {
    //     a[i] = gainQ * qErr[i+1];
    // }

    // obtain second term in algorithm
    for (int i = 0; i < 3; i++)
    {
        b[i] = gainW * angVels[i];
    }

    // calculate final control signal in quaternion form
    double controlQuat[4];
    controlQuat[0] = qErr.w();
    for (int i = 0; i < 3; i++)
    {
        controlQuat[i+1] = -a[i] - b[i];
    }

    // obtain control signal as Euler angles
    quatToEuler(controlQuat);
}

// convert control signal from quaternion form to Euler angles
void CControl::quatToEuler(double q[4])
{
    output[0] = atan2(2*(q[0]*q[1] + q[2]*q[3]), pow(q[0],2.0) - pow(q[1],2.0) - pow(q[2],2.0) - pow(q[3],2.0));
    output[1] = asin(2*(q[0]*q[2] - q[3]*q[1]));
    output[2] = atan2(2*(q[0]*q[3] + q[1]*q[2]), pow(q[0],2) + pow(q[1],2) + pow(q[2],2) + pow(q[3],2));
}

bool CControl::getTolerance(void) 
{
    if (qErr.w() < tolerance) {
        return true;
    } else {
        return false;
    }
}

void CControl::detumble(imu::Vector<3> rps, imu::Vector<3> mags) {
    // %%%Bfield is in teslas - 40000 nT = 4e4e-9 = 4e-5 ~= 1e-5
    // %%%pqr is in rad/s --    0.1 rad/s  = 1e-1
    // %%% pqr*Bfield = 1e-1*1e-5 = 1e-6
    // %%% pqr*Bfield / (n*A) = 6e=7
    // %%% muB = n*i*A
    // n = number of turns
    // A = surfaced enclosed by turn of coil
    // current = k*cross(pqr, magfieldxyz)/(n*A);
}