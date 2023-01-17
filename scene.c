/* CPURenderer/scene.c */
#include "scene.h"
#include "bitmap.h"
#include "image.h"
#include "geom.h"
#include "camera.h"
#include "lighting.h"
#include <float.h>

static int intersect_ray(ray r, const scene_obj *obj, vec3d *out, double *dist)
{
    switch (obj->type) {
        case sphere:
            return intersect_with_sphere(r, obj->data.s, out, dist);
        case triangle:
            return intersect_with_triangle(r, obj->data.tr, out, dist);
    }

    return 0;
}

static vec3d get_normal(vec3d point, const scene_obj *obj)
{
    switch (obj->type) {
        case sphere:
            return get_sphere_normal(point, obj->data.s);
        case triangle:
            return get_triangle_normal(point, obj->data.tr);
    }

    return create_vec(0, 0, 0);
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

int render(const scene *s, const camera *c, bitmap_t *bm)
{
    image *imgp;
    size_t x, y;
    ray r;

    imgp = create_image(bm->width, bm->height);

    for (x = 0; x < imgp->width; x++)
        for (y = 0; y < imgp->height; y++) {
            scene_obj *obj = NULL;
            vec3d intersection;

            r = trace_camera_ray(c, x, y, imgp);
            obj = find_closest_object(r, s, &intersection);
                        
            if (obj != NULL) {
                vec3d normal;
                vec3d color;

                normal = get_normal(intersection, obj);
                color = shade(intersection, normal, c->pos, obj, s);
                set_img_pixel(imgp, color, x, y);
            } else
                set_img_pixel(imgp, create_vec(0, 0, 0), x, y);
        }

    post_process(imgp);
    copy_image_to_bitmap(imgp, bm);

    destroy_image(imgp);

    return 1;
}
