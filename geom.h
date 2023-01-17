/* CPURenderer/geom.h */
#ifndef GEOM_SENTRY
#define GEOM_SENTRY

typedef struct tag_vec3d {
    double x, y, z;
} vec3d;

typedef struct tag_ray {
    vec3d orig, dir;
} ray;

double deg2rad(double deg);

vec3d normalized(vec3d v);
vec3d sum(vec3d v1, vec3d v2);
vec3d sum3(vec3d v1, vec3d v2, vec3d v3);
vec3d mul(vec3d v, double coeff);
vec3d prod(vec3d v1, vec3d v2);
double dot(vec3d v1, vec3d v2);

#endif 
