/* CPURenderer/camera.h */
#ifndef CAMERA_SENTRY
#define CAMERA_SENTRY

#include "geom.h"
#include "image.h"

typedef struct tag_camera {
    vec3d pos, dir, up;
    double fov, foc_l;
} camera;

camera create_camera(double pos_x, double pos_y, double pos_z, 
        double dir_x, double dir_y, double dir_z,
        double up_x, double up_y, double up_z, 
        double fov, double foc_l);

ray get_camera_ray(const camera *c, size_t x, size_t y, image *img);

#endif 
