/* CPURenderer/src/bvh_funcs.c */
#include "bvh_funcs.h"
#include "geom.h"
#include <stdlib.h>

#include "debug.h"
#include <stdio.h>

typedef struct tag_object_info {
    bounds b;
    vec3d c;
} object_info;

static bounds get_bounds(const scene_obj *obj)
{
    switch (obj->type) {
        case sphere:
            return get_sphere_bounds(&(obj->data.s));
        case triangle:
            return get_triangle_bounds(&(obj->data.tr));
    }

    bounds b = { { 0, 0, 0 }, { 0, 0, 0} };
    return b;
}

static vec3d get_centroid(const scene_obj *obj)
{
    switch (obj->type) {
        case sphere:
            return get_sphere_centroid(&(obj->data.s));
        case triangle:
            return get_triangle_centroid(&(obj->data.tr));
    }

    return vec3d_zero();
}

void construct_scene_bvh_tree(scene *s, bvh_options opts)
{
    if (!opts.use_bvh) {
        s->bvh_root = NULL;
        return;
    }

    object_info *obj_infos = malloc(s->objects_cnt * sizeof(*obj_infos));
    for (size_t i = 0; i < s->objects_cnt; i++) {
        scene_obj *objp = s->objects + i;
        object_info info = obj_infos[i];
        info.b = get_bounds(objp);
        info.c = get_centroid(objp);

        // @DEBUG
        /*
        if (objp->type == triangle)
            print_triangle_info(objp);
        else {
            printf("Sphere\n center: ");
            print_vec(objp->data.s.center);
            printf("radius: %lf\n", objp->data.s.rad);
        }

        printf("Bounds: \n");
        print_vec(info.b.min);
        print_vec(info.b.max);
        printf("Centroid: \n");
        print_vec(info.c);
        printf("\n\n");
        */
    }

    // @TEST
    s->bvh_root = NULL;

cleanup:
    free(obj_infos);
}
