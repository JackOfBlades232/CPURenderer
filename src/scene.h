/* CPURenderer/src/scene.h */
#ifndef SCENE_SENTRY
#define SCENE_SENTRY

#include "geom.h"
#include "camera.h"
#include "image.h"
#include "render_options.h"
#include <math.h>

typedef struct tag_material {
    vec3d ka, kd, ks, ke;
    double ns, ni;
    double tr;
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

typedef struct tag_bvh_tree_node {
    struct tag_bvh_tree_node *l, *r;
    bounds b;
    size_t first_obj_offset, obj_cnt;
} bvh_tree_node;

typedef struct tag_scene {
    light_src *lights;
    scene_obj *objects;
    size_t lights_cnt, objects_cnt;
    bvh_tree_node *bvh_root;
} scene;

material material_literal(double ka_x, double ka_y, double ka_z,
                          double ke_x, double ke_y, double ke_z,
                          double kd_x, double kd_y, double kd_z,
                          double ks_x, double ks_y, double ks_z,
                          double ns, double ni, double tr,
                          double al_x, double al_y, double al_z);

material material_zero();

scene *create_scene();
void destroy_scene(scene *s);

int render(const scene *s, const camera *c, render_options ropts, image *img);

#endif 
