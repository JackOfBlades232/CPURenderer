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

int render_obj_to_png(const char *obj_path, const char *png_save_path,
        camera *c, size_t res_x, size_t res_y, render_options ropts)
{
    image img;
    file_read_result *fres;
    scene *s;

    fres = read_scene_from_files(obj_path);
    if (fres == NULL)
        return 1;
    s = create_scene_for_read_res(fres, ropts);
    alloc_image(&img, res_x, res_y);

    render(s, c, ropts, &img);
    save_img_to_png(&img, png_save_path);

    free_image(&img);
    destroy_scene(s);
    free(fres->mat_buf);
    free(fres);
    return 0;
}

int test_shading_parts()
{
    render_options ropts = { rmode_full, 1, { 1, bvhs_middle } };
    camera c = camera_from_look_at(0, 0, 0, 0, 0, -1, 90, 1);
    return render_obj_to_png("./ftests/shading_parts/scene.obj", 
            "shading_parts.png", &c, 640, 480, ropts);
}

int test_triangle()
{
    render_options ropts = { rmode_full, 1, { 1, bvhs_middle } };
    camera c = camera_from_look_at(0, 2, 0, 0, 0, 0, 90, 1);
    return render_obj_to_png("./ftests/triangle/scene.obj", 
            "triangle.png", &c, 640, 480, ropts);
}

int test_classic_box_first()
{
    render_options ropts = { rmode_full, 4, { 1, bvhs_middle } };
    camera c = camera_from_look_at(-0.5, 1.5, 0.98, 0, 1, 0, 90, 1);
    return render_obj_to_png("./ftests/classic_box/scene.obj", 
            "classic_box_1.png", &c, 500, 500, ropts);
}

int test_classic_box_second()
{
    render_options ropts = { rmode_full, 4, { 1, bvhs_middle } };
    camera c = camera_from_look_at(-0.9, 1.9, -1, 0, 0, 0, 90, 1);
    return render_obj_to_png("./ftests/classic_box/scene.obj", 
            "classic_box_2.png", &c, 500, 500, ropts);
}

int test_box()
{
    render_options ropts = { rmode_full, 4, { 1, bvhs_middle } };

    camera c = camera_from_look_at(0, 0.7, 1.75, 0, 0.7, 0, 60, 1);
    return render_obj_to_png("./ftests/box/scene.obj", 
            "box.png", &c, 640, 480, ropts);
}

int test_distorted_box()
{
    render_options ropts = { rmode_full, 4, { 1, bvhs_middle } };

    camera c = camera_from_look_at(-0.5, 1.5, 1.98, 0, 1, 0, 90, 1);
    return render_obj_to_png("./ftests/distorted_box/scene.obj", 
            "distorted_box.png", &c, 500, 500, ropts);
}

int test_mirrors()
{
    render_options ropts = { rmode_full, 9, { 1, bvhs_middle } };
    camera c = camera_from_look_at(2, 1.5, -0.1, 1, 1.2, -2.8, 90, 1);
    return render_obj_to_png("./ftests/mirrors/scene.obj", 
            "mirrors.png", &c, 800, 600, ropts);
}

int test_deer()
{
    render_options ropts = { rmode_full, 1, { 1, bvhs_middle } };
    camera c = camera_from_look_at(100, 200, 150, 0, 100, 0, 90, 1);
    return render_obj_to_png("./ftests/deer/scene.obj", 
            "deer.png", &c, 500, 500, ropts);
}

int main()
{
    /*
    */
    printf("Shading parts: %s\n",
            test_shading_parts() == 0 ? "passed" : "failed");
    printf("Triangle: %s\n",
            test_triangle() == 0 ? "passed" : "failed");

    printf("Classic box 1: %s\n",
           test_classic_box_first() == 0 ? "passed" : "failed");
    printf("Classic box 2: %s\n",
           test_classic_box_second() == 0 ? "passed" : "failed");

    printf("Distorted box: %s\n",
            test_distorted_box() == 0 ? "passed" : "failed");
    
    printf("Box: %s\n", test_box() == 0 ? "passed" : "failed");

    printf("Mirrors: %s\n", test_mirrors() == 0 ? "passed" : "failed");

    printf("Deer: %s\n",
            test_deer() == 0 ? "passed" : "failed");
    /*
    */
}
