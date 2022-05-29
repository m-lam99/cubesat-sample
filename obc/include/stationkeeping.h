#ifndef util_h
#define util_h

#define PIf 3.14159265358979f
#define J2000UTC 946684800.0f + 12 * 3600.0f
#define SAMPLING_FREQ 22.22f
// #define deltaT (1 / SAMPLING_FREQ) // 100Hz sampling frequency
#include <stdio.h>

struct vec3
{
    float x;
    float y;
    float z;
};

typedef struct vec3 Vec3;

Vec3 storeVec;
GPS::loc_t *llh1;
GPS::loc_t *llh2;
Vec3 p2;
Vec3 p1;

struct mat3
{
    float a11;
    float a12;
    float a13;
    float a21;
    float a22;
    float a23;
    float a31;
    float a32;
    float a33;
};

struct jacobian
{
    float a11;
    float a12;
    float a13;
    float a14;
    float a21;
    float a22;
    float a23;
    float a24;
    float a31;
    float a32;
    float a33;
    float a34;
};

void printVec3(Vec3 vec);

static inline float UTCtoJD2000(float time)
{
    return (time - J2000UTC) / 24.0f / 3600.0f;
}

void ECEFtoECI(Vec3 inVec, Vec3 *outVec, float time);
void ECItoECEF(Vec3 inVec, Vec3 *outVec, float time);

void ECEFtoNED(Vec3 inVec, Vec3 *outVec, float lat, float lon);

void NEDtoECEF(Vec3 inVec, Vec3 *outVec, float lat, float lon);

static inline float det3(struct mat3 A)
{
    return A.a11 * (A.a33 * A.a22 - A.a32 * A.a23) - A.a21 * (A.a33 * A.a22 - A.a32 * A.a13) +
           A.a31 * (A.a23 * A.a12 - A.a22 * A.a13);
}

static inline void vecmult3(struct mat3 A, Vec3 x, Vec3 *result)
{
    result->x = A.a11 * x.x + A.a12 * x.y + A.a13 * x.z;
    result->y = A.a21 * x.x + A.a22 * x.y + A.a23 * x.z;
    result->z = A.a31 * x.x + A.a32 * x.y + A.a33 * x.z;
}

static inline void invMat3(struct mat3 A, struct mat3 *outMat)
{
    float det = det3(A);
    outMat->a11 = 1 / det * (A.a33 * A.a22 - A.a32 * A.a23);
    outMat->a12 = 1 / det * (-(A.a33 * A.a12 - A.a32 * A.a13));
    outMat->a13 = 1 / det * (A.a23 * A.a12 - A.a22 * A.a13);
    outMat->a21 = 1 / det * (-(A.a33 * A.a21 - A.a31 * A.a23));
    outMat->a22 = 1 / det * (A.a33 * A.a11 - A.a31 * A.a13);
    outMat->a23 = 1 / det * (-(A.a23 * A.a11 - A.a21 * A.a13));
    outMat->a31 = 1 / det * (A.a32 * A.a21 - A.a31 * A.a22);
    outMat->a32 = 1 / det * (-(A.a32 * A.a11 - A.a31 * A.a12));
    outMat->a33 = 1 / det * (A.a22 * A.a11 - A.a21 * A.a12);
}

struct quaternion
{
    float q1;
    float q2;
    float q3;
    float q4;
};

typedef struct quaternion Quaternion;

// global variables
extern Quaternion q_est;
extern Quaternion q_est_dot;

// Multiply two quaternions and return a copy of the result, prod = L * R
Quaternion quat_mult(Quaternion q_L, Quaternion q_R);

// Multiply a reference of a quaternion by a scalar, q = s*q
static inline void quat_scalar(Quaternion *q, float scalar)
{
    q->q1 *= scalar;
    q->q2 *= scalar;
    q->q3 *= scalar;
    q->q4 *= scalar;
}

// Adds two quaternions together and the sum is the pointer to another quaternion, Sum = L + R
static inline void quat_add(Quaternion *Sum, Quaternion L, Quaternion R)
{
    Sum->q1 = L.q1 + R.q1;
    Sum->q2 = L.q2 + R.q2;
    Sum->q3 = L.q3 + R.q3;
    Sum->q4 = L.q4 + R.q4;
}

// Subtracts two quaternions together and the sum is the pointer to another quaternion, sum = L - R
static inline void quat_sub(Quaternion *Sum, Quaternion L, Quaternion R)
{
    Sum->q1 = L.q1 - R.q1;
    Sum->q2 = L.q2 - R.q2;
    Sum->q3 = L.q3 - R.q3;
    Sum->q4 = L.q4 - R.q4;
}

// the conjugate of a quaternion is it's imaginary component sign changed  q* = [s, -v] if q = [s, v]
static inline Quaternion quat_conjugate(Quaternion q)
{
    q.q2 = -q.q2;
    q.q3 = -q.q3;
    q.q4 = -q.q4;
    return q;
}

// norm of a quaternion is the same as a complex number
// sqrt( q1^2 + q2^2 + q3^2 + q4^2)
// the norm is also the sqrt(q * conjugate(q)), but thats a lot of operations in the quaternion multiplication
float quat_Norm(Quaternion q);

// Normalizes pointer q by calling quat_Norm(q),
static inline void quat_Normalization(Quaternion *q)
{
    float norm = quat_Norm(*q);
    if (norm == 0.0f)
    {
        return;
    }
    q->q1 /= norm;
    q->q2 /= norm;
    q->q3 /= norm;
    q->q4 /= norm;
}

static inline void printQuaternion(Quaternion q)
{
    printf("%f %f %f %f\n", q.q1, q.q2, q.q3, q.q4);
}

static inline void jacobianMult(Vec3 f, struct jacobian J, Quaternion *q_est_dot)
{
    q_est_dot->q1 = J.a11 * f.x + J.a21 * f.y + J.a31 * f.z;
    q_est_dot->q2 = J.a12 * f.x + J.a22 * f.y + J.a32 * f.z;
    q_est_dot->q3 = J.a13 * f.x + J.a23 * f.y + J.a33 * f.z;
    q_est_dot->q4 = J.a14 * f.x + J.a24 * f.y + J.a34 * f.z;
}

void normVec3(Vec3 *v);

#endif