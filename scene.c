/* CPURenderer/scene.c */
#include "scene.h"
#include "bitmap.h"
#include "geom.h"
#include "camera.h"
#include <float.h>

#include <stdio.h>

static int intersect_ray(ray r, scene_obj obj, vec3d *out, double *dist)
{
    switch (obj.type) {
        case sphere:
            return intersect_with_sphere(r, obj.data.s, out, dist);
        case triangle:
            return intersect_with_triangle(r, obj.data.tr, out, dist);
    }

    return 0;
}

static scene_obj *find_closest_object(ray r, scene *s, vec3d *pos_out)
{
    scene_obj *obj_p, *closest;
    double dist, min_dist;
    vec3d pos;
    int intersects;

    closest = NULL;
    min_dist = DBL_MAX;

    for (obj_p = s->objects; obj_p - s->objects < s->objects_cnt; obj_p++) {
        intersects = intersect_ray(r, *obj_p, &pos, &dist); 
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

int render(scene *s, camera *c, bitmap_t *bm)
{
    size_t x, y;
    ray r;

    for (x = 0; x < bm->width; x++)
        for (y = 0; y < bm->height; y++) {
            scene_obj *obj = NULL;
            vec3d intersection;

            r = trace_camera_ray(c, x, y, bm);
            obj = find_closest_object(r, s, &intersection);
                        
            /* testing */
            if (obj != NULL) 
                set_pixel(bm, x, y, 255, 0, 0);
            else
                set_pixel(bm, x, y, 0, 0, 0);
        }
                

    return 1;
}
