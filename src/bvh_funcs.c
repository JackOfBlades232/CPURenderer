/* CPURenderer/src/bvh_funcs.c */
#include "bvh_funcs.h"
#include "geom.h"
#include "mathd.h"
#include <stdlib.h>
#include <float.h>

#include "debug.h"
#include <stdio.h>

typedef struct tag_object_info {
    bounds b;
    vec3d c;
    size_t idx;
} object_info;

typedef struct tag_bucket {
    bounds b;
    size_t obj_cnt;
    double upper_bound;
} bucket;

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

static size_t partition_by_value_dummy(object_info *obj_infos, 
                                       size_t start, size_t end,
                                       vec3d dummy, dim3d dim)
{
    if (start >= end+1)
        return start;

    size_t l = start,
           r = end-1;
    int cmp_res;
    while (l < r) {
        cmp_res = vec3d_dim_comp(dummy, obj_infos[l].c, dim);
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

static size_t partition_by_middle(object_info *obj_infos, 
                                  size_t start, size_t end,
                                  bounds cb, dim3d dim)
{
    vec3d middle_dummy = bounds_center(cb);
    return partition_by_value_dummy(obj_infos, start, end, middle_dummy, dim);
}

static size_t partition_by_sah(object_info *obj_infos, 
                               size_t start, size_t end,
                               bounds cb, dim3d dim,
                               bvh_options opts)
{
    if (end-start <= opts.max_objs_for_mid)
        return partition_by_middle(obj_infos, start, end, cb, dim);

    bucket buckets[opts.num_buckets];
    double lower_bound = vec3d_get_dim(cb.min, dim);
    double spread = bounds_dim_spread(cb, dim);
    double bucket_size = spread / opts.num_buckets;

    for (int i = 0; i < opts.num_buckets; i++) {
        buckets[i].b = bounds_from_point(vec3d_zero());
        buckets[i].obj_cnt = 0;
        buckets[i].upper_bound = lower_bound + bucket_size * (i+1);
    }

    int bucket_idx = 0;
    for (size_t i = start; i < end; i++) {
        object_info info = obj_infos[i];
        double c_dim = vec3d_get_dim(info.c, dim);

        while (c_dim >= buckets[bucket_idx].upper_bound + EPSILON)
            bucket_idx++;

        bucket *bucket_p = buckets + bucket_idx;
        if (bucket_p->obj_cnt == 0)
            bucket_p->b = info.b;
        else 
            bucket_p->b = bounds_union(bucket_p->b, info.b);

        bucket_p->obj_cnt++;
    }

    double best_sah = DBL_MAX;
    int best_idx = -1;

    bounds b1, b2;
    int c1 = 0, c2;
    for (bucket_idx = 0; bucket_idx < opts.num_buckets-1; bucket_idx ++) {
        if (buckets[bucket_idx].obj_cnt == 0)
            continue;

        if (c1 == 0) {
            b1 = buckets[bucket_idx].b;
            c1 = buckets[bucket_idx].obj_cnt;
        } else {
            b1 = bounds_union(b1, buckets[bucket_idx].b);
            c1 += buckets[bucket_idx].obj_cnt;
        }

        c2 = 0;
        for (int i = bucket_idx+2; i < opts.num_buckets; i++) {
            if (buckets[i].obj_cnt == 0)
                continue;
            if (c2 == 0) {
                b2 = buckets[i].b;
                c2 = buckets[i].obj_cnt;
            } else {
                b2 = bounds_union(b2, buckets[i].b);
                c2 += buckets[i].obj_cnt;
            }
        }

        double sa1 = bounds_area(b1);
        double sa2 = bounds_area(b2);
        double sa_tot = bounds_area(bounds_union(b1, b2)); // @TODO: clac once?
        double sah = opts.part_cost + opts.inter_cost*(sa1*c1 + sa2*c2) / sa_tot;

        if (sah < best_sah) {
            best_sah = sah;
            best_idx = bucket_idx;
        }
    }

    if (end-start <= opts.max_objs_in_leaf &&
            best_sah > opts.inter_cost * (end-start)) {
        return start;
    }

    double split_plane = buckets[best_idx].upper_bound;
    vec3d dummy = vec3d_literal(split_plane, split_plane, split_plane);
    return partition_by_value_dummy(obj_infos, start, end, dummy, dim);
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

    if (end <= start+1) {
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

        case bvhs_sah:
            mid = partition_by_sah(obj_infos, start, end, cb, split_dim, opts);
            break;
    }
    
    if (mid <= start) {
        // @TODO: factor out (?)
        node->first_obj_offset = *ordered_write_idx;
        for (size_t i = start; i < end; i++)
            ordered_objects[(*ordered_write_idx)++] = objects[obj_infos[i].idx];
        node->obj_cnt = *ordered_write_idx - node->first_obj_offset;

        return node;
    }

    node->l = recursive_bvh(obj_infos, start, mid,
                            ordered_objects, ordered_write_idx, objects, opts);
    node->r = recursive_bvh(obj_infos, mid, end,
                            ordered_objects, ordered_write_idx, objects, opts);

    return node;
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
        object_info *infop = obj_infos + i;
        infop->b = get_bounds(objp);
        infop->c = get_centroid(objp);
        infop->idx = i;
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
