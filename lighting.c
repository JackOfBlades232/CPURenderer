/* CPURenderer/lighting.c */
#include "lighting.h"
#include "geom.h"

#include <stdio.h>

static int light_is_seen(vec3d point, const light_src *l, const scene_obj *obj)
{
    return 0;
}

static vec3d diffuse_illum(vec3d point, const light_src *l, vec3d normal)
{
    return create_vec(0, 0, 0);
}

static vec3d specular_illum(vec3d point, const light_src *l,
        vec3d normal, vec3d view_point)
{
    return create_vec(0, 0, 0);
}

vec3d shade(vec3d point, vec3d normal, vec3d view_point,
        const scene_obj *obj, const scene *s)
{
    vec3d color;
    int i;

    color = sum(obj->mat.ka, obj->mat.ke); /* ambient */

    /* diffuse and specular over all lights */
    for (i = 0; i < s->lights_cnt; i++) {
        light_src *l = s->lights + i;
        if (light_is_seen(point, l, obj)) {
            color = sum3(
                color, 
                diffuse_illum(point, l, normal),
                specular_illum(point, l, normal, view_point)
            );
        }
    }

    return color;
}
