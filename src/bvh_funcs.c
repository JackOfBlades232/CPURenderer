/* CPURenderer/src/bvh_funcs.c */
#include "bvh_funcs.h"
#include "geom.h"
#include "mathd.h"
#include <stdlib.h>

#include "debug.h"
#include <stdio.h>

typedef struct tag_object_info {
    bounds b;
    vec3d c;
    size_t idx;
} object_info;

static bounds get_bounds(const scene_obj *obj)
{
    switch (obj->type) {
        case sphere:
            return get_sphere_bounds(&(obj->data.s));
        case triangle:
            return get_triangle_bounds(&(obj->data.tr));
    }

    return bounds_from_point(vec3d_zero());
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

size_t partition_by_middle(object_info *obj_infos, size_t start, size_t end,
                           bounds cb, dim3d dim)
{
    if (start >= end+1)
        return start;

    vec3d middle_dummy = bounds_center(cb);

    size_t l = start,
           r = end-1;
    int cmp_res;
    while (l < r) {
        cmp_res = vec3d_dim_comp(middle_dummy, obj_infos[l].c, dim);
        if (cmp_res > 0) {
            object_info tmp = obj_infos[r];
            obj_infos[r] = obj_infos[l];
            obj_infos[l] = tmp;
            r--;
        } else
            l++;
    }

    return cmp_res <= 0 ? l : l+1;
}

static bvh_tree_node *recursive_bvh(object_info *obj_infos, 
                                    size_t start, size_t end, 
                                    scene_obj *ordered_objects, 
                                    size_t *ordered_write_idx, 
                                    scene_obj *objects, 
                                    bvh_options opts)
{
    if (start >= end)
        return NULL;
    
    bvh_tree_node *node = malloc(sizeof(*node));
    node->l = NULL;
    node->r = NULL;

    node->b = obj_infos[start].b;
    for (size_t i = start+1; i < end; i++)
        node->b = bounds_union(node->b, obj_infos[i].b);

    printf("\nNum objects: %ld\n", end-start);
    printf("------------------------------\nBounds:\n");
    print_vec(node->b.min);
    print_vec(node->b.max);
    /*
    */

    if (end == start+1) {
        // @TODO: check writes range?
        node->first_obj_offset = *ordered_write_idx;
        for (size_t i = start; i < end; i++)
            ordered_objects[(*ordered_write_idx)++] = objects[obj_infos[i].idx];
        node->obj_cnt = *ordered_write_idx - node->first_obj_offset;

        return node;
    }

    bounds cb = bounds_from_point(obj_infos[start].c);
    for (size_t i = start+1; i < end; i++)
        cb = bounds_add_point(cb, obj_infos[i].c);

    printf("\n------------------------------\nCentroid bounds:\n");
    print_vec(cb.min);
    print_vec(cb.max);

    dim3d split_dim = bounds_max_dim(cb);
    double c_spread = bounds_dim_spread(cb, split_dim);

    if (c_spread < EPSILON) {
        node->first_obj_offset = *ordered_write_idx;
        for (size_t i = start; i < end; i++)
            ordered_objects[(*ordered_write_idx)++] = objects[obj_infos[i].idx];
        node->obj_cnt = *ordered_write_idx - node->first_obj_offset;

        return node;
    }
    
    size_t mid;
    switch (opts.split_mode) {
        case bvhs_middle:
            mid = partition_by_middle(obj_infos, start, end, cb, split_dim);
            break;

        case bvhs_equal:
            // @TODO: impl
            break;

        case bvhs_sah:
            // @TODO: impl
            break;
    }

    printf("Kidz:\n");
    node->l = recursive_bvh(obj_infos, start, mid,
                            ordered_objects, ordered_write_idx, objects, opts);
    node->r = recursive_bvh(obj_infos, mid, end,
                            ordered_objects, ordered_write_idx, objects, opts);

    return node;
}

void construct_scene_bvh_tree(scene *s, bvh_options opts)
{
    if (!opts.use_bvh 
            || opts.split_mode != bvhs_middle) { //  @TEST
        s->bvh_root = NULL;
        return;
    }

    object_info *obj_infos = malloc(s->objects_cnt * sizeof(*obj_infos));
    for (size_t i = 0; i < s->objects_cnt; i++) {
        scene_obj *objp = s->objects + i;
        object_info *infop = obj_infos + i;
        infop->b = get_bounds(objp);
        infop->c = get_centroid(objp);
        infop->idx = i;

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

    scene_obj *ordered_objects = 
        malloc(s->objects_cnt * sizeof(*ordered_objects));
    size_t ordered_write_idx = 0;

    s->bvh_root = recursive_bvh(obj_infos, 0, s->objects_cnt,
                                ordered_objects, &ordered_write_idx,
                                s->objects, opts);

    scene_obj *tmp = s->objects;
    s->objects = ordered_objects;
    free(tmp);

    free(obj_infos);
}
