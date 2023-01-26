/* CPURenderer/image.c */
#include "image.h"
#include "geom.h"
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#define GAMMA_CORR_POW 1.0/2.2

int alloc_image(image *img, size_t width, size_t height)
{
    img->width = width;
    img->height = height;
    img->content = malloc(width * height * sizeof(vec3d));
    return img->content != NULL;
}

vec3d *img_pixel_at(image *img, size_t x, size_t y)
{
    return img->content + y*img->width + x;
}

void set_img_pixel(image *img, vec3d color, size_t x, size_t y)
{
    *img_pixel_at(img, x, y) = color;
}

static vec3d tone_map(vec3d color)
{
    return vec3d_div(color, vec3d_sum(color, vec3d_one()));
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

void free_image(image *img)
{
    free(img->content);
}
