/* CPURenderer/src/scene.c */
#include "scene.h"
#include "geom.h"
#include "camera.h"
#include "raytracer.h"
#include <stdlib.h>

#include "debug.h"

material material_literal(double ka_x, double ka_y, double ka_z,
                          double ke_x, double ke_y, double ke_z,
                          double kd_x, double kd_y, double kd_z,
                          double ks_x, double ks_y, double ks_z,
                          double ns, double ni,
                          double al_x, double al_y, double al_z)
{
    material m;
    m.ka = vec3d_literal(ka_x, ka_y, ka_z);
    m.ke = vec3d_literal(ke_x, ke_y, ke_z);
    m.kd = vec3d_literal(kd_x, kd_y, kd_z);
    m.ks = vec3d_literal(ks_x, ks_y, ks_z);
    m.ns = ns;
    m.ni = ni;
    m.al = vec3d_literal(al_x, al_y, al_z);
    return m;
}

scene *create_scene()
{
    return malloc(sizeof(scene));
}

void destroy_scene(scene *s)
{
    free(s->objects);
    free(s->lights);
    free(s);
}

int render(const scene *s, const camera *c, image *img)
{
    size_t x, y;
    ray r;
    vec3d color;

    for (x = 0; x < img->width; x++)
        for (y = 0; y < img->height; y++) {
            r = get_camera_ray(c, x, y, img);
            color = trace_ray(r, s, c, 0);

            /* Need to negate image inversion */
            set_img_pixel(img, color, x, img->height-y-1);
        }

    post_process(img);

    return 1;
}
