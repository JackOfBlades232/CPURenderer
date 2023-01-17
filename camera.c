/* CPURenderer/camera.c */
#include "camera.h"
#include "geom.h"
#include <math.h>

#include <stdio.h>

camera create_camera(double pos_x, double pos_y, double pos_z, 
        double dir_x, double dir_y, double dir_z,
        double up_x, double up_y, double up_z, 
        double fov, double foc_l)
{
    camera c;
    c.pos.x = pos_x;
    c.pos.y = pos_y;
    c.pos.z = pos_z;
    c.dir.x = dir_x;
    c.dir.y = dir_y;
    c.dir.z = dir_z;
    c.up.x = up_x;
    c.up.y = up_y;
    c.up.z = up_z;
    c.fov = fov;
    c.foc_l = foc_l;
    return c;
}

ray trace_camera_ray(const camera *c, size_t x, size_t y, image *img)
{
    double x_d, y_d;
    double pixel_size;
    vec3d c_right;
    ray res;

    x_d = (double)x - ((double)img->width)/2.0 + 0.5;
    y_d = (double)y - ((double)img->height)/2.0 + 0.5;

    pixel_size = (2.0 * c->foc_l * tan(deg2rad(c->fov)/2.0)) /
                 (double)img->height;

    c_right = prod(c->dir, c->up);
    
    res.orig = c->pos;
    res.dir = sum3(
        mul(c_right, x_d * pixel_size),
        mul(c->up, y_d * pixel_size),
        mul(c->dir, c->foc_l)
    );

    res.dir = normalized(res.dir);
    
    return res;
}
