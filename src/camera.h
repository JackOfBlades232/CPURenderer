/* CPURenderer/src/camera.h */
#ifndef CAMERA_SENTRY
#define CAMERA_SENTRY

#include "geom.h"
#include "image.h"

typedef struct tag_camera {
    vec3d pos;
    vec3d dir, up, right;
    vec3d foc_v;
    double fov_h;
} camera;

// Currently no tilt, may add as angle in degrees
camera camera_from_look_at(double pos_x, double pos_y, double pos_z, 
        double at_x, double at_y, double at_z,
        double fov, double foc_l);

ray get_camera_ray(const camera *c, size_t x, size_t y, image *img);

#endif 
