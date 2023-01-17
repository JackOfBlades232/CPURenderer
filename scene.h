/* CPURenderer/scene.h */
#ifndef SCENE_SENTRY
#define SCENE_SENTRY

#include "geom.h"
#include "camera.h"
#include "bitmap.h"
#include <stddef.h>

typedef struct tag_material {
    vec3d ka, kd, ks, ke;
    double ns, ni;
    vec3d al;
} material;

typedef struct tag_light_src {
    vec3d pos;
    vec3d illum;
} light_src;

typedef struct tag_scene_obj {
    object_type type;
    material mat;
    union {
        sphere_obj s;
        triangle_obj tr;
    } data;
} scene_obj;

typedef struct tag_scene {
    light_src *lights;
    scene_obj *objects;
    size_t lights_cnt, objects_cnt;
} scene;

int render(const scene *s, const camera *c, bitmap_t *bm);

#endif 
