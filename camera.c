/* CPURenderer/camera.c */
#include "camera.h"
#include "geom.h"
#include <math.h>

camera create_camera(double pos_x, double pos_y, double pos_z, 
        double dir_x, double dir_y, double dir_z,
        double up_x, double up_y, double up_z, 
        double fov, double foc_l)
{
    camera c;
    c.pos = create_vec(pos_x, pos_y, pos_z);
    c.dir = create_vec(dir_x, dir_y, dir_z);
    c.up = create_vec(up_x, up_y, up_z);
    c.fov = fov;
    c.foc_l = foc_l;
    return c;
}

ray trace_camera_ray(const camera *c, size_t x, size_t y, image *img)
{
    double x_d, y_d;
    double pixel_size;
    vec3d c_right;
    ray r;

    /* calculate pixel center coord in pixels from screen center */
    x_d = (double)x - ((double)img->width)/2.0 + 0.5;
    y_d = (double)y - ((double)img->height)/2.0 + 0.5;

    /* calcualte world pixel size from fov and focal length:
     * (2 * focal_l * tg(fov/2)) / height */
    pixel_size = (2.0 * c->foc_l * tan(deg2rad(c->fov)/2.0)) /
                 (double)img->height;

    /* calculate right of the camera */
    c_right = prod(c->dir, c->up);
    
    /* using camera right and up directions as a basis, and camera dir as 
     * offset direction, calcualate direction to the point */
    r.orig = c->pos;
    r.dir = sum3(
        mul(c_right, x_d * pixel_size),
        mul(c->up, y_d * pixel_size),
        mul(c->dir, c->foc_l)
    );

    r.dir = normalized(r.dir); /* don't forget to normalize dir */
    
    return r;
}
