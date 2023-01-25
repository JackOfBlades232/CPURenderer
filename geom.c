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

vec3d zero_vec()
{
    return create_vec(0, 0, 0);
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

double sq_len(vec3d v)
{
    return dot(v, v);
}

double len(vec3d v)
{
    return sqrt(sq_len(v));
}

vec3d mulv(vec3d v1, vec3d v2)
{
    return create_vec(v1.x*v2.x, v1.y*v2.y, v1.z*v2.z);
}

vec3d divv(vec3d v1, vec3d v2)
{
    return create_vec(v1.x/v2.x, v1.y/v2.y, v1.z/v2.z);
}

vec3d reflect(vec3d v, vec3d normal)
{
    double n_v;
    n_v = dot(v, normal);
    return subtr(mul(normal, 2.0*n_v), v);
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

    /* Solving ||(orig + k*dir) - center||^2 = rad^2 */

    /* First, calculate coeff for this quadr equation */
    a = sq_len(r.dir);
    b = 2.0 * dot(r.dir, subtr(r.orig, s.center));
    c = sq_len(subtr(r.orig, s.center)) - s.rad*s.rad;

    /* Then, solve and choose minimal non-negative root, if it exists */
    num_roots = solve_qe(a, b, c, &root1, &root2);

    if (num_roots == 0)
        return 0;
    else if (num_roots == 1)
        *dist = root1;
    else
        *dist = min_non_neg(root1, root2);

    if (*dist >= 0) {
        *out = sum(r.orig, mul(r.dir, *dist));
        return 1;
    }

    return 0;
}

vec3d get_triangle_normal(vec3d point, triangle_obj tr)
{
    return create_vec(0, 0, 0);
}

int intersect_with_triangle(ray r, triangle_obj tr, vec3d *out, double *dist)
{
    return 0;
}
