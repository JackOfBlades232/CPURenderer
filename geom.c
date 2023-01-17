/* CPURenderer/geom.c */
#include "geom.h"
#include <math.h>

double deg2rad(double deg)
{
    return deg * M_PI / 180.0;
}

vec3d normalized(vec3d v)
{
    double inv_len = 1.0 / sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    v.x *= inv_len;
    v.y *= inv_len;
    v.z *= inv_len;
    return v;
}

vec3d sum(vec3d v1, vec3d v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
    return v1;
}

vec3d sum3(vec3d v1, vec3d v2, vec3d v3)
{
    return sum(sum(v1, v2), v3);
}

vec3d mul(vec3d v, double coeff)
{
    v.x *= coeff;
    v.y *= coeff;
    v.z *= coeff;
    return v;
}

vec3d prod(vec3d v1, vec3d v2)
{
    vec3d res;
    res.x = v1.y*v2.z - v1.z*v2.y;
    res.y = v1.z*v2.x - v1.x*v2.z;
    res.z = v1.x*v2.y - v1.y*v2.x;
    return res;
}

double dot(vec3d v1, vec3d v2)
{
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}
