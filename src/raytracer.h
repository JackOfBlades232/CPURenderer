/* CPURenderer/raytracer.h */
#ifndef RAYTRACER_SENTRY
#define RAYTRACER_SENTRY

#include "geom.h"
#include "scene.h"

vec3d trace_ray(ray r, const scene *s, const camera *c, int cur_depth);

#endif 
