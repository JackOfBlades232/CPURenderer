/* CPURenderer/scene.c */
#include "scene.h"
#include "geom.h"
#include "camera.h"
#include "lighting.h"
#include <float.h>

#include "debug.h"

int intersect_ray(ray r, const scene_obj *obj, vec3d *out, double *dist)
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

int render(const scene *s, const camera *c, image *img)
{
    size_t x, y;
    ray r;
    vec3d color;

    for (x = 0; x < img->width; x++)
        for (y = 0; y < img->height; y++) {
            r = get_camera_ray(c, x, y, img);
            color = trace_ray(r, s, c, 0);

            /* Need to negate image inversion */
            set_img_pixel(img, color, x, img->height-y-1);
        }

    post_process(img);

    return 1;
}
