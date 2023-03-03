/* CPURenderer/src/debug.h */
#include "debug.h"

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
