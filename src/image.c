/* CPURenderer/image.c */
#include "image.h"
#include "bitmap.h"
#include "geom.h"
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#define GAMMA_CORR_POW 1.0/2.2

image *create_image(size_t width, size_t height)
{
    image *img;
    img = malloc(sizeof(image));
    img->width = width;
    img->height = height;
    img->content = malloc(width * height * sizeof(vec3d));
    return img;
}

static vec3d *img_pixel_at(image *img, size_t x, size_t y)
{
    return img->content + y*img->width + x;
}

void set_img_pixel(image *img, vec3d color, size_t x, size_t y)
{
    *img_pixel_at(img, x, y) = color;
}

static vec3d tone_map(vec3d color)
{
    return vec3d_div(color, vec3d_sum(color, vec3d_literal(1, 1, 1)));
}

static vec3d gamma_correct(vec3d color)
{
    return vec3d_pow(color, GAMMA_CORR_POW);
}

static vec3d process_color(vec3d color)
{
    return gamma_correct(tone_map(color));
}

void post_process(image *img)
{
    size_t x, y;

    for (x = 0; x < img->width; x++)
        for (y = 0; y < img->height; y++) {
            vec3d *colorp = img_pixel_at(img, x, y);
            *colorp = process_color(*colorp);
        }
}

int copy_image_to_bitmap(image *src_img, bitmap_t *dest_bm)
{
    size_t x, y;

    if (src_img->width != dest_bm->width || src_img->height != dest_bm->height)
        return 0;

    for (x = 0; x < src_img->width; x++)
        for (y = 0; y < src_img->height; y++) {
            vec3d *colorp;
            pixel_t pix;

            colorp = img_pixel_at(src_img, x, y);
            pix = create_pixel(
                (uint8_t)(colorp->x * 255.0),
                (uint8_t)(colorp->y * 255.0),
                (uint8_t)(colorp->z * 255.0)
            );
            set_pixel(dest_bm, x, dest_bm->height - y - 1, pix);
        }

    return 1;
}

void destroy_image(image *img)
{
    free(img->content);
    free(img);
}
