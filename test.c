/* CPURenderer/src/test/tests.c */
#include "src/geom.h"
#include "src/scene.h"
#include "src/image.h"
#include "src/camera.h"
#include "src/png_save.h"
#include "src/file_reader.h"
#include <pngconf.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "src/debug.h"

static render_mode rmode = rmode_full;

double random_val()
{
    return rand()/(RAND_MAX+1.0);
}

vec3d random_vec()
{
    return vec3d_literal(random_val(), random_val(), random_val());
}

int create_noise_img()
{
    image img;
    size_t i, j;

    alloc_image(&img, 640, 480);

    for (i = 0; i < img.height; i++)
        for (j = 0; j < img.width; j++)
            set_img_pixel(&img, random_vec(), j, i);
  
    save_img_to_png(&img, "./test_noise.png");
    free_image(&img);
    return 0;
}

void test_camera_tracing()
{
    camera c;
    image img;
    size_t i, j;

    img.width = 19;
    img.height = 11;
    c = camera_from_look_at(0, 0, 0, 0, 0, -1, 90, 1);

    for (i = 0; i < img.width; i++)
        for (j = 0; j < img.height; j++) {
            ray r = get_camera_ray(&c, i, j, &img);
            if (i < 0)
                printf("(%ld, %ld): ro (%lf, %lf, %lf), rd (%lf, %lf, %lf)\n",
                        i, j, r.orig.x, r.orig.y, r.orig.z, 
                        r.dir.x, r.dir.y, r.dir.z);
        }
}

int test_sphere()
{
    image img;
    scene s;
    camera c;
    material m;

    alloc_image(&img, 640, 480);

    m = material_literal(0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    s.objects = malloc(sizeof(scene_obj));
    s.objects_cnt = 1;
    s.objects[0].type = sphere;
    s.objects[0].data.s = sphere_literal(1, 1, -2.5, 0.5);
    s.objects[0].mat = &m;

    c = camera_from_look_at(0, 0, 0, 0, 0, -1, 90, 1);

    render(&s, &c, rmode, &img);

    save_img_to_png(&img, "./test_sphere.png");
    free(s.objects);
    free_image(&img);
    return 0;
}

int test_3_spheres()
{
    image img;
    scene s;
    camera c;
    material m1, m2, m3;

    alloc_image(&img, 640, 480);

    m1 = material_literal(0, 0, 0, 0, 0, 0, 0.5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    m2 = material_literal(0.5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    m3 = material_literal(0.05, 0, 0, 0, 0, 0, 0, 0, 0,
            0.5, 0, 0, 500, 0, 0, 0, 0);

    s.objects = malloc(3 * sizeof(scene_obj));
    s.objects_cnt = 3;
    
    s.objects[0].type = sphere;
    s.objects[0].data.s = sphere_literal(0, 0, -0.5, 0.15);
    s.objects[0].mat = &m1;

    s.objects[1].type = sphere;
    s.objects[1].data.s = sphere_literal(-0.35, 0, -0.5, 0.15);
    s.objects[1].mat = &m2;

    s.objects[2].type = sphere;
    s.objects[2].data.s = sphere_literal(0.4, 0, -0.5, 0.15);
    s.objects[2].mat = &m3;

    s.lights_cnt = 1;
    s.lights = malloc(sizeof(light_src));

    s.lights[0].pos = vec3d_literal(-0.2, 0, 0.5);
    s.lights[0].illum = vec3d_literal(0.5, 0.5, 0.5);

    c = camera_from_look_at(0, 0, 0, 0, 0, -1, 90, 1);

    render(&s, &c, rmode, &img);

    save_img_to_png(&img, "./test_3_spheres.png");
    free(s.objects);
    free_image(&img);
    return 0;
}

int test_triangle_simple()
{
    image img;
    scene s;
    camera c;
    material m;

    alloc_image(&img, 640, 480);

    m = material_literal(0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1000, 0, 0, 0, 0);

    s.objects = malloc(sizeof(scene_obj));
    s.objects_cnt = 1;
    
    s.objects[0].type = triangle;
    s.objects[0].data.tr = trianlge_literal(-1, 0, 0, 0, -5, -5, 1, 0, 0);
    s.objects[0].mat = &m;

    s.lights_cnt = 1;
    s.lights = malloc(sizeof(light_src));

    s.lights[0].pos = vec3d_literal(0, 2, -0.5);
    s.lights[0].illum = vec3d_literal(1, 1, 1);

    c = camera_from_look_at(0, 2, 0, 0, 0, 0, 90, 1);

    render(&s, &c, rmode, &img);

    save_img_to_png(&img, "./test_triangle.png");
    free(s.objects);
    free_image(&img);
    return 0;
}

int test_gen_case(const char *obj_path, const char *png_save_path,
        camera *c, size_t res_x, size_t res_y)
{
    image img;
    file_read_result *fres;
    scene *s;

    fres = read_scene_from_files(obj_path);
    if (fres == NULL)
        return 1;

    s = create_scene_for_read_res(fres);

    alloc_image(&img, res_x, res_y);

    render(s, c, rmode, &img);

    save_img_to_png(&img, png_save_path);

    destroy_scene(s);
    free(fres->mat_buf);
    free(fres);
    free_image(&img);
    return 0;
}

int test_shading_parts()
{
    camera c = camera_from_look_at(0, 0, 0, 0, 0, -1, 90, 1);
    return test_gen_case("./ftests/shading_parts/scene.obj", 
            "shading_parts.png", &c, 640, 480);
}

int test_triangle()
{
    camera c = camera_from_look_at(0, 2, 0, 0, 0, 0, 90, 1);
    return test_gen_case("./ftests/triangle/scene.obj", 
            "triangle.png", &c, 640, 480);
}

int test_classic_box_first()
{
    camera c = camera_from_look_at(-0.5, 1.5, 0.98, 0, 1, 0, 90, 1);
    return test_gen_case("./ftests/classic_box/scene.obj", 
            "classic_box_1.png", &c, 500, 500);
}

int test_classic_box_second()
{
    camera c = camera_from_look_at(-0.9, 1.9, -1, 0, 0, 0, 90, 1);
    return test_gen_case("./ftests/classic_box/scene.obj", 
            "classic_box_2.png", &c, 500, 500);
}

int main()
{
    printf("Shading parts: %s\n",
            test_shading_parts() == 0 ? "passed" : "failed");
    printf("Triangle: %s\n",
            test_triangle() == 0 ? "passed" : "failed");

    printf("Classic box 1: %s\n",
           test_classic_box_first() == 0 ? "passed" : "failed");
    printf("Classic box 2: %s\n",
           test_classic_box_second() == 0 ? "passed" : "failed");
}
