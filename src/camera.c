/* CPURenderer/src/camera.c */
#include "camera.h"
#include "geom.h"
#include <math.h>

camera camera_literal(double pos_x, double pos_y, double pos_z, 
        double dir_x, double dir_y, double dir_z,
        double up_x, double up_y, double up_z, 
        double fov, double foc_l)
{
    camera c;
    c.pos = vec3d_literal(pos_x, pos_y, pos_z); 
    c.dir = vec3d_normalized(vec3d_literal(dir_x, dir_y, dir_z));
    c.up = vec3d_normalized(vec3d_literal(up_x, up_y, up_z));

    /* precalcualte camera right vector */
    c.right = vec3d_cross(c.dir, c.up);

    /* and precalcualte vector to the center of the screen (foc_len*dir) */
    c.foc_v = vec3d_scale(c.dir, foc_l);

    /* also precalcualte camera fov world height from fov and focal length:
     * (2 * focal_l * tg(fov/2)) / height */
    c.fov_h = 2.0 * foc_l * tan(deg2rad(fov)/2.0);

    return c;
}

ray get_camera_ray(const camera *c, size_t x, size_t y, image *img)
{
    double x_d, y_d;
    double pixel_size;
    ray r;

    /* calculate pixel center coord in pixels from screen center */
    x_d = (double)x - ((double)img->width)/2.0 + 0.5;
    y_d = (double)y - ((double)img->height)/2.0 + 0.5;

    pixel_size = c->fov_h / (double)img->height;
    
    /* using camera right and up directions as a basis, and camera dir as 
     * offset direction, calcualate direction to the point */
    r.orig = c->pos;
    r.dir = vec3d_sum3(
        vec3d_scale(c->right, x_d * pixel_size),
        vec3d_scale(c->up, y_d * pixel_size),
        c->foc_v
    );

    vec3d_normalize(&r.dir); /* don't forget to normalize dir */

    return r;
}
