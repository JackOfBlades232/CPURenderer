/* CPURenderer/src/debug.h */
#ifndef DEBUG_SENTRY
#define DEBUG_SENTRY

#include "geom.h"
#include "scene.h"

void print_vec(vec3d v);
void print_material_info(material *m);
void print_triangle_info(scene_obj *tr);
void print_light_info(light_src *l);

#endif 
