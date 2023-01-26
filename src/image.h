/* CPURenderer/image.h */
#ifndef IMAGE_SENTRY
#define IMAGE_SENTRY

#include "bitmap.h"
#include "geom.h"
#include <stddef.h>

typedef struct tag_image {
  size_t width, height;
  vec3d *content;
} image;

image *create_image(size_t width, size_t height);
void set_img_pixel(image *img, vec3d color, size_t x, size_t y);
void post_process(image *img);
int copy_image_to_bitmap(image *src_img, bitmap_t *dest_bm);
void destroy_image(image *img);

#endif
