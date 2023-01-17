/* CPURenderer/geom.c */
#include "mathd.h"
#include "geom.h"
#include <math.h>

double deg2rad(double deg)
{
    return deg * M_PI / 180.0;
}

vec3d create_vec(double x, double y, double z)
{
    vec3d v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

vec3d sum(vec3d v1, vec3d v2)
{
    return create_vec(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z);
}

vec3d sum3(vec3d v1, vec3d v2, vec3d v3)
{
    return sum(sum(v1, v2), v3);
}

vec3d subtr(vec3d v1, vec3d v2)
{
    return create_vec(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z);
}

vec3d mul(vec3d v, double coeff)
{
    return create_vec(v.x*coeff, v.y*coeff, v.z*coeff);
}

vec3d powv(vec3d v1, double p)
{
    return create_vec(pow(v1.x, p), pow(v1.y, p), pow(v1.z, p));
}

vec3d normalized(vec3d v)
{
    double inv_len = 1.0 / sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    return mul(v, inv_len);
}

vec3d prod(vec3d v1, vec3d v2)
{
    return create_vec(
        v1.y*v2.z - v1.z*v2.y,
        v1.z*v2.x - v1.x*v2.z,
        v1.x*v2.y - v1.y*v2.x
    );
}

double dot(vec3d v1, vec3d v2)
{
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

vec3d mulv(vec3d v1, vec3d v2)
{
    return create_vec(v1.x*v2.x, v1.y*v2.y, v1.z*v2.z);
}

vec3d divv(vec3d v1, vec3d v2)
{
    return create_vec(v1.x/v2.x, v1.y/v2.y, v1.z/v2.z);
}

sphere_obj create_sphere(double cx, double cy, double cz, double r)
{
    sphere_obj s;
    s.center = create_vec(cx, cy, cz);
    s.rad = r;
    return s;
}

vec3d get_sphere_normal(vec3d point, sphere_obj s)
{
    return normalized(subtr(point, s.center));
}

int intersect_with_sphere(ray r, sphere_obj s, vec3d *out, double *dist)
{
    double a, b, c, root1, root2;
    int num_roots;

    a = r.dir.x*r.dir.x + r.dir.y*r.dir.y + r.dir.z*r.dir.z;
    b = 2 * (
                r.dir.x*(r.orig.x-s.center.x) +
                r.dir.y*(r.orig.y-s.center.y) + 
                r.dir.z*(r.orig.z-s.center.z)
            );
    c = (r.orig.x-s.center.x)*(r.orig.x-s.center.x) + 
        (r.orig.y-s.center.y)*(r.orig.y-s.center.y) + 
        (r.orig.z-s.center.z)*(r.orig.z-s.center.z) -
        s.rad*s.rad;

    num_roots = solve_qe(a, b, c, &root1, &root2);

    if (num_roots == 0)
        return 0;
    else if (root1 >= 0) {
        *out = sum(r.orig, mul(r.dir, root1));
        *dist = root1;
        return 1;
    } else if (root2 >= 0 && num_roots == 2) {
        *out = sum(r.orig, mul(r.dir, root2));
        *dist = root2;
        return 1;
    }

    return 0;
}

vec3d get_triangle_normal(vec3d point, triangle_obj tr)
{
    return create_vec(0, 0, 0);
}

int intersect_with_triangle(ray r, triangle_obj s, vec3d *out, double *dist)
{
    return 0;
}
