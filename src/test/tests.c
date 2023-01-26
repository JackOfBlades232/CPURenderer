/* testing */
#include "../geom.h"
#include "../scene.h"
#include "../bitmap.h"
#include "../image.h"
#include "../camera.h"
#include "../png_save.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "../debug.h"

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

    bm.width = 640;
    bm.height = 480;
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
    c = camera_literal(0, 0, 0, 0, 0, -1, 0, 1, 0, 90, 1);

    for (i = 0; i < img.width; i++)
        for (j = 0; j < img.height; j++) {
            ray r = get_camera_ray(&c, i, j, &img);
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

    bm.width = 640;
    bm.height = 480;
    bm.pixels = calloc(bm.width * bm.height, sizeof(pixel_t));

    s.objects = malloc(sizeof(scene_obj));
    s.objects_cnt = 1;
    s.objects[0].type = sphere;
    s.objects[0].data.s = sphere_literal(1, 1, -2.5, 0.5);
    s.objects[0].mat.ka = vec3d_literal(0, 1, 1);

    c = camera_literal(0, 0, 0, 0, 0, -1, 0, 1, 0, 90, 1);

    status = render(&s, &c, &bm);

    return status ? save_bitmap_to_png(&bm, "./test.png") : 10;
}

int test_3_spheres()
{
    int status = 0;
    bitmap_t bm;
    scene s;
    camera c;

    bm.width = 640;
    bm.height = 480;
    bm.pixels = calloc(bm.width * bm.height, sizeof(pixel_t));

    s.objects = malloc(3 * sizeof(scene_obj));
    s.objects_cnt = 3;
    
    s.objects[0].type = sphere;
    s.objects[0].data.s = sphere_literal(0, 0, -0.5, 0.15);
    s.objects[0].mat.ka = vec3d_literal(0, 0, 0);
    s.objects[0].mat.ke = vec3d_literal(0, 0, 0);
    s.objects[0].mat.kd = vec3d_literal(0.5, 0, 0);
    s.objects[0].mat.ks = vec3d_literal(0, 0, 0);

    s.objects[1].type = sphere;
    s.objects[1].data.s = sphere_literal(-0.35, 0, -0.5, 0.15);
    s.objects[1].mat.ka = vec3d_literal(0.5, 0, 0);
    s.objects[1].mat.ke = vec3d_literal(0, 0, 0);
    s.objects[1].mat.kd = vec3d_literal(0, 0, 0);
    s.objects[1].mat.ks = vec3d_literal(0, 0, 0);

    s.objects[2].type = sphere;
    s.objects[2].data.s = sphere_literal(0.4, 0, -0.5, 0.15);
    s.objects[2].mat.ka = vec3d_literal(0.05, 0, 0);
    s.objects[2].mat.ke = vec3d_literal(0, 0, 0);
    s.objects[2].mat.kd = vec3d_literal(0, 0, 0);
    s.objects[2].mat.ks = vec3d_literal(0.5, 0, 0);
    s.objects[2].mat.ns = 500;

    s.lights_cnt = 1;
    s.lights = malloc(sizeof(light_src));

    s.lights[0].pos = vec3d_literal(-0.2, 0, 0.5);
    s.lights[0].illum = vec3d_literal(0.5, 0.5, 0.5);

    c = camera_literal(0, 0, 0, 0, 0, -1, 0, 1, 0, 90, 1);

    status = render(&s, &c, &bm);

    return status ? save_bitmap_to_png(&bm, "./test.png") : 10;
}

int test_triangle()
{
    int status = 0;
    bitmap_t bm;
    scene s;
    camera c;

    bm.width = 640;
    bm.height = 480;
    bm.pixels = calloc(bm.width * bm.height, sizeof(pixel_t));

    s.objects = malloc(sizeof(scene_obj));
    s.objects_cnt = 1;
    
    s.objects[0].type = triangle;
    s.objects[0].data.tr = trianlge_literal(-1, 0, 0, 0, -5, -5, 1, 0, 0);
    s.objects[0].mat.ka = vec3d_literal(0, 0, 0);
    s.objects[0].mat.ke = vec3d_literal(0, 0, 0);
    s.objects[0].mat.kd = vec3d_literal(0, 0, 1);
    s.objects[0].mat.ks = vec3d_literal(0, 1, 0);
    s.objects[0].mat.ns = 1000;

    s.lights_cnt = 1;
    s.lights = malloc(sizeof(light_src));

    s.lights[0].pos = vec3d_literal(0, 2, -0.5);
    s.lights[0].illum = vec3d_literal(1, 1, 1);

    c = camera_literal(0, 2, 0, 0, -1, 0, 0, 0, -1, 90, 1);

    status = render(&s, &c, &bm);

    return status ? save_bitmap_to_png(&bm, "./test.png") : 10;
}

int main()
{
    return test_3_spheres();
}
