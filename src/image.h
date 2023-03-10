/* CPURenderer/src/image.h */
#ifndef IMAGE_SENTRY
#define IMAGE_SENTRY

#include "geom.h"
#include <stddef.h>

typedef struct tag_image {
    size_t width, height;
    vec3d *content;
} image;

int alloc_image(image *img, size_t width, size_t height);
vec3d *img_pixel_at(image *img, size_t x, size_t y);
void set_img_pixel(image *img, vec3d color, size_t x, size_t y);
void post_process(image *img);
void free_image(image *img);

#endif
