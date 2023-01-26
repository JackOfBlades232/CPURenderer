/* CPURenderer/lighting.c */
#include "lighting.h"
#include "geom.h"
#include "mathd.h"
#include "scene.h"
#include <math.h>

static int light_is_seen(vec3d point, const light_src *l,
        const scene *s, const scene_obj *obj)
{
    const scene_obj *objp;
    ray lr;
    vec3d intersection;
    double dist, dist_to_light;

    lr.orig = point;
    lr.dir = subtr(l->pos, point);
    dist_to_light = sqrt(dot(lr.dir, lr.dir));
    lr.dir = normalized(lr.dir);
    
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

    vl = normalized(subtr(l->pos, point));
    scal = max(0, dot(vl, normal));

    return mul(mulv(obj->mat.kd, l->illum), scal);
}

static vec3d specular_illum(const scene_obj *obj, const light_src *l,
        vec3d point, vec3d normal, vec3d view_point)
{
    vec3d vl, vr, ve;
    double scal;

    vl = normalized(subtr(l->pos, point));
    vr = reflect(vl, normal);

    ve = normalized(subtr(view_point, point));
    scal = max(0, dot(vr, ve));

    return mul(mulv(obj->mat.ks, l->illum), pow(scal, obj->mat.ns));
}

static vec3d reflect_illum(const scene_obj *obj, const scene *s, 
        vec3d point, vec3d normal, vec3d view_point, int cur_depth)
{
    return zero_vec();
}

static vec3d refract_illum(const scene_obj *obj, const scene *s, 
        vec3d point, vec3d normal, vec3d view_point, int cur_depth)
{
    return zero_vec();
}

vec3d shade(vec3d point, vec3d normal, vec3d view_point,
        const scene_obj *obj, const scene *s, int cur_depth)
{
    vec3d color;
    int i;

    color = sum(obj->mat.ka, obj->mat.ke); /* ambient */

    /* diffuse and specular over all lights */
    for (i = 0; i < s->lights_cnt; i++) {
        light_src *l = s->lights + i;
        if (light_is_seen(point, l, s, obj)) {
            color = sum3(
                color, 
                diffuse_illum(obj, l, point, normal),
                specular_illum(obj, l, point, normal, view_point)
            );
        }
    }

    if (cur_depth < max_depth) {
        color = sum3(
            color,
            reflect_illum(obj, s, point, normal, view_point, cur_depth),
            refract_illum(obj, s, point, normal, view_point, cur_depth)
        );
    }

    return color;
}
