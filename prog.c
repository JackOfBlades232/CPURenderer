/* testing */
#include "geom.h"
#include "scene.h"
#include "bitmap.h"
#include "image.h"
#include "camera.h"
#include "png_save.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <stdio.h>

uint8_t random_val()
{
    return (uint8_t)(256.0*rand()/(RAND_MAX+1.0));
}

pixel_t random_pixel()
{
    pixel_t pixel;

    pixel.r = random_val();
    pixel.g = random_val();
    pixel.b = random_val();

    return pixel;
}

int create_noise_img()
{
    bitmap_t bm;
    int i, j;

    bm.width = 1920;
    bm.height = 1080;
    bm.pixels = calloc(bm.width * bm.height, sizeof(pixel_t));

    for (i = 0; i < bm.height; i++)
        for (j = 0; j < bm.width; j++)
            *pixel_at(&bm, j, i) = random_pixel();
  
    return save_bitmap_to_png(&bm, "./test.png");
}

void test_camera_tracing()
{
    camera c;
    image img;
    size_t i, j;

    img.width = 19;
    img.height = 11;
    c = create_camera(0, 0, 0, 0, 0, -1, 0, 1, 0, 90, 1);

    for (i = 0; i < img.width; i++)
        for (j = 0; j < img.height; j++) {
            ray r = trace_camera_ray(&c, i, j, &img);
            printf("(%ld, %ld): ro (%lf, %lf, %lf), rd (%lf, %lf, %lf)\n",
                    i, j, r.orig.x, r.orig.y, r.orig.z, 
                    r.dir.x, r.dir.y, r.dir.z); 
        }
}

int test_sphere()
{
    int status = 0;
    bitmap_t bm;
    scene s;
    camera c;

    bm.width = 1920;
    bm.height = 1080;
    bm.pixels = calloc(bm.width * bm.height, sizeof(pixel_t));

    s.objects = malloc(sizeof(scene_obj));
    s.objects_cnt = 1;
    s.objects[0].type = sphere;
    s.objects[0].data.s = create_sphere(1, 1, -2.5, 0.5);
    s.objects[0].mat.ka = create_vec(0, 1, 1);

    c = create_camera(0, 0, 0, 0, 0, -1, 0, 1, 0, 90, 1);

    status = render(&s, &c, &bm);

    return status ? save_bitmap_to_png(&bm, "./test.png") : 10;
}

int main()
{
    return test_sphere();
}
