/* CPURenderer/lighting.h */
#ifndef LIGHTING_SENTRY
#define LIGHTING_SENTRY

#include "geom.h"
#include "scene.h"

vec3d shade(vec3d point, vec3d normal, const scene_obj *obj, const scene *s);

#endif 
