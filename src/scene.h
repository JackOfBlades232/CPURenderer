/* CPURenderer/src/scene.h */
#ifndef SCENE_SENTRY
#define SCENE_SENTRY

#include "geom.h"
#include "camera.h"
#include "image.h"
#include <math.h>

enum { max_depth = 2 };

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
    material *mat;
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

material material_literal(double ka_x, double ka_y, double ka_z,
                          double ke_x, double ke_y, double ke_z,
                          double kd_x, double kd_y, double kd_z,
                          double ks_x, double ks_y, double ks_z,
                          double ns, double ni,
                          double al_x, double al_y, double al_z);

scene *create_scene();
void destroy_scene(scene *s);

int render(const scene *s, const camera *c, image *img);

#endif 
