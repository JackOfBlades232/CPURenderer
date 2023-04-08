/* CPURenderer/src/debug.h */
#include "debug.h"
#include "geom.h"
#include "scene.h"

#include <stdio.h>

void print_vec(vec3d v)
{
    printf("(%lf, %lf, %lf)\n", v.x, v.y, v.z);
}

void print_material_info(material *m)
{
    printf("Ka: ");
    print_vec(m->ka);
    printf("Ke: ");
    print_vec(m->ke);
    printf("Kd: ");
    print_vec(m->kd);
    printf("Ks: ");
    print_vec(m->ks);
    printf("Ns: %lf, Ni: %lf\n", m->ns, m->ni);
    printf("al: ");
    print_vec(m->al);
}

void print_triangle_info(scene_obj *tr)
{
    if (tr->type != triangle)
        return;

    printf("Vertices:\n");
    print_vec(tr->data.tr.v1);
    print_vec(tr->data.tr.v2);
    print_vec(tr->data.tr.v3);
}

void print_light_info(light_src *l)
{
    printf("Pos: ");
    print_vec(l->pos);
    printf("Illum: ");
    print_vec(l->illum);
}
