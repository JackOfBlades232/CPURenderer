/* CPURenderer/src/raytracer.h */
#ifndef RAYTRACER_SENTRY
#define RAYTRACER_SENTRY

#include "geom.h"
#include "scene.h"
#include "render_mode.h"

vec3d trace_ray(ray r, const scene *s, int cur_depth, render_options ropts);

#endif 
