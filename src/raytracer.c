/* CPURenderer/raytracer.c */
#include "raytracer.h"
#include "mathd.h"
#include <math.h>
#include <float.h>

static int intersect_ray(ray r, const scene_obj *obj, vec3d *out, double *dist)
{
    switch (obj->type) {
        case sphere:
            return intersect_with_sphere(r, &(obj->data.s), out, dist);
        case triangle:
            return intersect_with_triangle(r, &(obj->data.tr), out, dist);
    }

    return 0;
}

static vec3d get_normal(vec3d point, const scene_obj *obj, vec3d view_point)
{
    switch (obj->type) {
        case sphere:
            return get_sphere_normal(point, &(obj->data.s));
        case triangle:
            return get_triangle_normal(point, &(obj->data.tr), view_point);
    }

    return vec3d_literal(0, 0, 0);
}

static scene_obj *find_closest_object(ray r, const scene *s, vec3d *pos_out)
{
    scene_obj *obj_p, *closest;
    double dist, min_dist;
    vec3d pos;
    int intersects;

    closest = NULL;
    min_dist = DBL_MAX;

    for (obj_p = s->objects; obj_p - s->objects < s->objects_cnt; obj_p++) {
        intersects = intersect_ray(r, obj_p, &pos, &dist); 
        if (!intersects)
            continue;

        if (dist < min_dist) {
            min_dist = dist;
            closest = obj_p;
            *pos_out = pos;
        }
    }

    return closest;
}

static int light_is_seen(vec3d point, const light_src *l,
        const scene *s, const scene_obj *obj)
{
    const scene_obj *objp;
    ray lr;
    vec3d intersection;
    double dist, dist_to_light;

    lr.orig = point;
    lr.dir = vec3d_sub(l->pos, point);
    dist_to_light = sqrt(vec3d_dot(lr.dir, lr.dir));
    lr.dir = vec3d_normalized(lr.dir);
    
    for (objp = s->objects; objp - s->objects < s->objects_cnt; objp++) {
        if (objp == obj)
            continue;
        
        if (intersect_ray(lr, objp, &intersection, &dist) &&
                dist <= dist_to_light)
            return 0;
    }

    return 1;
}

static vec3d diffuse_illum(const scene_obj *obj, const light_src *l, 
        vec3d point, vec3d normal)
{
    vec3d vl;
    double scal;

    vl = vec3d_normalized(vec3d_sub(l->pos, point));
    scal = max(0, vec3d_dot(vl, normal));

    return vec3d_scale(vec3d_mul(obj->mat->kd, l->illum), scal);
}

static vec3d specular_illum(const scene_obj *obj, const light_src *l,
        vec3d point, vec3d normal, vec3d view_point)
{
    vec3d vl, vr, ve;
    double scal;

    vl = vec3d_normalized(vec3d_sub(l->pos, point));
    vr = vec3d_reflect(vl, normal);

    ve = vec3d_normalized(vec3d_sub(view_point, point));
    scal = max(0, vec3d_dot(vr, ve));

    return vec3d_scale(
                vec3d_mul(obj->mat->ks, l->illum), 
                pow(scal, obj->mat->ns)
            );
}

static vec3d vec3d_reflect_illum(const scene_obj *obj, const scene *s, 
        vec3d point, vec3d normal, vec3d view_point, int cur_depth)
{
    return vec3d_zero();
}

static vec3d refract_illum(const scene_obj *obj, const scene *s, 
        vec3d point, vec3d normal, vec3d view_point, int cur_depth)
{
    return vec3d_zero();
}

static vec3d shade(vec3d point, vec3d normal, vec3d view_point,
        const scene_obj *obj, const scene *s, int cur_depth)
{
    vec3d color;
    int i;

    color = vec3d_sum(obj->mat->ka, obj->mat->ke); /* ambient */

    /* diffuse and specular over all lights */
    for (i = 0; i < s->lights_cnt; i++) {
        light_src *l = s->lights + i;
        if (light_is_seen(point, l, s, obj)) {
            color = vec3d_sum3(
                color, 
                diffuse_illum(obj, l, point, normal),
                specular_illum(obj, l, point, normal, view_point)
            );
        }
    }

    if (cur_depth < max_depth) {
        color = vec3d_sum3(
            color,
            vec3d_reflect_illum(obj, s, point, normal, view_point, cur_depth),
            refract_illum(obj, s, point, normal, view_point, cur_depth)
        );
    }

    return color;
}

vec3d trace_ray(ray r, const scene *s, const camera *c, int cur_depth)
{
    scene_obj *hit_obj;
    vec3d hit_point, hit_normal;

    hit_obj = find_closest_object(r, s, &hit_point);

    if (!hit_obj)
        return vec3d_zero();

    hit_normal = get_normal(hit_point, hit_obj, c->pos);
    return shade(hit_point, hit_normal, c->pos, hit_obj, s, cur_depth);
}
