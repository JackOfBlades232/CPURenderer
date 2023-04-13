/* CPURenderer/src/image.c */
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

static vec3d process_color(vec3d color, render_mode rmode, double img_stat)
{

    switch (rmode) {
        case rmode_full:
            return gamma_correct(tone_map(color));

        case rmode_depth:
            if (vec3d_is_zero(color))
                return vec3d_one();
            else
                return vec3d_scale(color, img_stat);

        case rmode_normal:
            if (vec3d_is_zero(color))
                return color;
            else
                return vec3d_sum(vec3d_scale(color, 0.5), 
                                 vec3d_scale(vec3d_one(), 0.5));

        default:
            return vec3d_zero();
    }
}

double get_max_depth(image *img)
{
    size_t x, y;
    double max_depth = .0;

    for (x = 0; x < img->width; x++)
        for (y = 0; y < img->height; y++) {
            vec3d *colorp = img_pixel_at(img, x, y);
            if (max_depth < colorp->x)
                max_depth = colorp->x;
        }

    return max_depth;
}

void post_process(image *img, render_mode rmode)
{
    size_t x, y;
    double img_stat = 1.;

    switch (rmode) {
        case rmode_full:
            // @TODO Should implement C^2 for tone mapping
            break;

        case rmode_depth:
            img_stat = 1. / get_max_depth(img);
            break;

        default:
            break;
    }

    for (x = 0; x < img->width; x++)
        for (y = 0; y < img->height; y++) {
            vec3d *colorp = img_pixel_at(img, x, y);
            *colorp = process_color(*colorp, rmode, img_stat);
        }
}

void free_image(image *img)
{
    free(img->content);
}
