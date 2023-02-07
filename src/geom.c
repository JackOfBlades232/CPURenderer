/* CPURenderer/src/geom.c */
#include "mathd.h"
#include "geom.h"
#include <math.h>

#include <stdio.h>

double deg2rad(double deg)
{
    return deg * M_PI / 180.0;
}

vec3d vec3d_literal(double x, double y, double z)
{
    vec3d v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

vec3d vec3d_zero()
{
    return vec3d_literal(0, 0, 0);
}

vec3d vec3d_one()
{
    return vec3d_literal(1, 1, 1);
}

vec3d vec3d_neg(vec3d v)
{
    return vec3d_literal(-v.x, -v.y, -v.z);
}

vec3d vec3d_sum(vec3d v1, vec3d v2)
{
    return vec3d_literal(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z);
}

vec3d vec3d_sum3(vec3d v1, vec3d v2, vec3d v3)
{
    return vec3d_literal(v1.x+v2.x+v3.x, v1.y+v2.y+v3.y, v1.z+v2.z+v3.z);
}

vec3d vec3d_sub(vec3d v1, vec3d v2)
{
    return vec3d_literal(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z);
}

vec3d vec3d_scale(vec3d v, double coeff)
{
    return vec3d_literal(v.x*coeff, v.y*coeff, v.z*coeff);
}

vec3d vec3d_pow(vec3d v1, double p)
{
    return vec3d_literal(pow(v1.x, p), pow(v1.y, p), pow(v1.z, p));
}

vec3d vec3d_cross(vec3d v1, vec3d v2)
{
    return vec3d_literal(
        v1.y*v2.z - v1.z*v2.y,
        v1.z*v2.x - v1.x*v2.z,
        v1.x*v2.y - v1.y*v2.x
    );
}

double vec3d_dot(vec3d v1, vec3d v2)
{
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

double vec3d_sq_len(vec3d v)
{
    return vec3d_dot(v, v);
}

double vec3d_len(vec3d v)
{
    return sqrt(vec3d_sq_len(v));
}

vec3d vec3d_normalized(vec3d v)
{
    return vec3d_scale(v, 1.0 / vec3d_len(v));
}

void vec3d_normalize(vec3d *v)
{
    double inv_len = 1.0 / vec3d_len(*v);
    v->x /= inv_len;
    v->y /= inv_len;
    v->z /= inv_len;
}

vec3d vec3d_mul(vec3d v1, vec3d v2)
{
    return vec3d_literal(v1.x*v2.x, v1.y*v2.y, v1.z*v2.z);
}

vec3d vec3d_div(vec3d v1, vec3d v2)
{
    return vec3d_literal(v1.x/v2.x, v1.y/v2.y, v1.z/v2.z);
}

vec3d vec3d_reflect(vec3d v, vec3d normal)
{
    double n_v;
    n_v = vec3d_dot(v, normal);
    return vec3d_sub(vec3d_scale(normal, 2.0*n_v), v);
}

sphere_obj sphere_literal(double cx, double cy, double cz, double r)
{
    sphere_obj s;
    s.center = vec3d_literal(cx, cy, cz);
    s.rad = r;
    return s;
}

vec3d get_sphere_normal(vec3d point, const sphere_obj *s)
{
    return vec3d_normalized(vec3d_sub(point, s->center));
}

int intersect_with_sphere(ray r, const sphere_obj *s, vec3d *out, double *dist)
{
    vec3d vec_to_s_center;
    double a, b, c, root1, root2;
    int num_roots;

    /* Solving ||(orig + k*dir) - center||^2 = rad^2 */

    /* First, calculate coeff for this quadr equation */
    vec_to_s_center = vec3d_sub(r.orig, s->center);

    a = vec3d_sq_len(r.dir);
    b = 2.0 * vec3d_dot(r.dir, vec_to_s_center);
    c = vec3d_sq_len(vec_to_s_center) - s->rad*s->rad;

    /* Then, solve and choose minimal non-vec3d_negative root, if it exists */
    num_roots = solve_qe(a, b, c, &root1, &root2);

    if (num_roots == 0)
        return 0;
    else if (num_roots == 1)
        *dist = root1;
    else
        *dist = min_non_neg(root1, root2);

    if (*dist >= EPSILON) {
        *out = vec3d_sum(r.orig, vec3d_scale(r.dir, *dist));
        return 1;
    }

    return 0;
}

static vec3d calculate_triangle_normal(const triangle_obj *tr)
{
    return vec3d_normalized(
            vec3d_cross(
                vec3d_sub(tr->v2, tr->v1), 
                vec3d_sub(tr->v3, tr->v1)
                )
            );
}

triangle_obj trianlge_literal(double x1, double y1, double z1,
                             double x2, double y2, double z2,
                             double x3, double y3, double z3)
{
    triangle_obj tr;
    tr.v1 = vec3d_literal(x1, y1, z1);
    tr.v2 = vec3d_literal(x2, y2, z2);
    tr.v3 = vec3d_literal(x3, y3, z3);
    tr.normal = calculate_triangle_normal(&tr);
    return tr;
}

vec3d get_triangle_normal(vec3d point, const triangle_obj *tr,
        vec3d view_point)
{
    /* return the normal for the side facing the viewer */
    return vec3d_dot(tr->normal, vec3d_sub(view_point, point)) > 0.0 ?
        tr->normal :
        vec3d_neg(tr->normal);
}

int intersect_with_triangle(ray r, const triangle_obj *tr,
        vec3d *out, double *dist)
{
    vec3d edge1, edge2;
    vec3d h, s, q;
    double a, f, u, v;
    double t;

    edge1 = vec3d_sub(tr->v2, tr->v1);
    edge2 = vec3d_sub(tr->v3, tr->v1);

    /* Check if ray is parallel to triangle plane (or, if normal to plane 
     * of ray dir and second edge is perpendicular to first edge */

    h = vec3d_cross(r.dir, edge2); /* perp to plane of ray dir and edge 1 */
    a = vec3d_dot(edge1, h); /* cosine of the angle (scaled by lengths) */
    if (dbl_is_zero(a)) /* if cosine == 0, then parallel => no intersetion */
        return 0;

    /* Now, we solve the linear system (-ray.dir, edge1, edge2)(t, u, v) = 
     * ray.orig - v1 to obtain uv-coords of intersection 
     * and t -- it's ray coord. We solve it by Cramer Rule. */ 

    /* For the point to be in the triangle, uv-coords must be >= 0 and their
     * sum must be no more than 1 */

    f = 1.0/a;
    s = vec3d_sub(r.orig, tr->v1);
    u = f * vec3d_dot(s, h);
    if (u < 0.0 || u > 1.0) 
        return 0;

    q = vec3d_cross(s, edge1);
    v = f * vec3d_dot(r.dir, q);
    if (v < 0.0 || u+v > 1.0)
        return 0;

    t = f * vec3d_dot(edge2, q);
    if (t > EPSILON) {
        *dist = t;
        *out = vec3d_sum(r.orig, vec3d_scale(r.dir, t));
        return 1;
    } else
        return 0;
}
