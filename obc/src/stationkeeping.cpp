#include "Satellite.h"
#include "GPS.h"
#include "stationkeeping.h"

#include <unistd.h>
#include <math.h>

void ECEFtoECI(Vec3 inVec, Vec3 *outVec, float time)
{
    imu::Vector<3> signal;
    float JD2000 = UTCtoJD2000(time);
    float theta = 4.89496121274f + 6.30038809899f * JD2000;
    outVec->x = cosf(theta) * inVec.x - sinf(theta) * inVec.y;
    outVec->y = sinf(theta) * inVec.x + cosf(theta) * inVec.y;
    outVec->z = inVec.z;
}

void ECItoECEF(Vec3 inVec, Vec3 *outVec, float time)
{
    float JD2000 = UTCtoJD2000(time);
    float theta = 4.89496121274f + 6.30038809899f * JD2000;
    outVec->x = cosf(theta) * inVec.x + sinf(theta) * inVec.y;
    outVec->y = -sinf(theta) * inVec.x + cosf(theta) * inVec.y;
    outVec->z = inVec.z;
}

void ECEFtoNED(Vec3 inVec, Vec3 *outVec, float lat, float lon)
{
    float tau = lon * PIf / 180.0f;
    float mu = lat * PIf / 180.0f;
    outVec->x = -inVec.x * sinf(mu) * cosf(tau) - inVec.y * sinf(mu) * sinf(tau) + inVec.z * cosf(mu);
    outVec->y = -inVec.x * sinf(tau) + inVec.y * cosf(tau);
    outVec->z = -inVec.x * cosf(mu) * cosf(tau) - inVec.y * cosf(mu) * sinf(tau) - inVec.z * sinf(mu);
}

void NEDtoECEF(Vec3 inVec, Vec3 *outVec, float lat, float lon)
{
    float tau = lon * PIf / 180.0f;
    float mu = lat * PIf / 180.0f;
    outVec->x = -inVec.x * sinf(mu) * cosf(tau) - inVec.y * sinf(tau) - inVec.z * cosf(mu) * cosf(tau);
    outVec->y = -inVec.x * sinf(mu) * sinf(tau) + inVec.y * cosf(tau) - inVec.z * cosf(mu) * sinf(tau);
    outVec->z = inVec.x * cosf(mu) + -inVec.z * sinf(mu);
}

void printVec3(Vec3 vec)
{
    printf("[%f, %f, %f]\n", vec.x, vec.y, vec.z);
}

void getPrograde(Vec3 progradeVec, GPS::loc_t *llh1, GPS::loc_t *llh2)
{
    // Sleep for 0.1s for next position estimate

    llh2NED(llh2, &p2);
    llh2NED(llh1, &p1);

    progradeVec.x = p2.x - p1.x;
    progradeVec.y = p2.y - p1.y;
    progradeVec.z = p2.z - p1.z;

    normVec3(&progradeVec);
}

void llh2NED(GPS::loc_t *llh, Vec3 *outvec)
{
    storeVec.x = (llh->altitude + EARTH_RADIUS) * cosf(llh->latitude) * cosf(llh->longitude);
    storeVec.y = (llh->altitude + EARTH_RADIUS) * cosf(llh->latitude) * sinf(llh->longitude);
    storeVec.z = (llh->altitude + EARTH_RADIUS) * sinf(llh->latitude);

    NEDtoECEF(storeVec, outvec, llh->latitude, llh->longitude);
}

void normVec3(Vec3 *v)
{
    float n = sqrtf(powf(v->x, 2) + powf(v->y, 2) + powf(v->z, 2));
    if (n == 0.0f)
    {
        return;
    }
    v->x /= n;
    v->y /= n;
    v->z /= n;
}

void vec2Euler(Vec3 vec)
{
    vec.x = atan2(vec.z, vec.x);
    vec.y = atan2(vec.y, vec.x);
    vec.z = atan2(vec.z, vec.y);
}

// function llh2lgcv(LLH)
// % Latitude, Longitude and Range
// lambda = LLH(1);
// psi = LLH(2);
// R = LLH(3) + 6378137;

// % define ECEF of satellite
// ecef_sat = [Rcos(lambda)cos(psi); ...
//      Rcos(lambda)sin(psi); ...
//      Rsin(lambda)];

//  % radius of earth
// R_e = 6378137;

// % ECEF on the surface of the earth ("shadow" of the satellite)
// ecef_surface = [R_ecos(lambda)cos(psi); ...
//      R_ecos(lambda)sin(psi); ...
//      R_esin(lambda)];

//  % difference vector (from shadow to the satellite)
// ecef_diff = ecef_sat - ecef_surface;

// % Rotation matrix to convert NED to ECEF
// C = [-sin(lambda)cos(psi), -sin(psi), -cos(lambda)cos(psi); ...
//     -sin(lambda)sin(psi), cos(psi), -cos(lambda)sin(psi); ...
//     cos(lambda), 0, -sin(lambda)];

// % Invert (transpose) matrix and multiply to the difference vector to get
// % LGCV (NED) coordinated
// lgcv = C' * ecef_diff;

// end
