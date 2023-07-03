/* CPURenderer/src/bvh_funcs.h */
#ifndef BVH_SENTRY
#define BVH_SENTRY

#include "scene.h"
#include "geom.h"

void construct_scene_bvh_tree(scene *s, bvh_options opts);
// @TODO: factor out traversal to here

#endif 
