/* CPURenderer/src/raytracer.c */
#include "raytracer.h"
#include "geom.h"
#include "mathd.h"
#include <math.h>
#include <float.h>

static int intersect_ray(ray r, const scene_obj *obj, vec3d *out, double *dist)
{
    switch (obj->type) {
        case sphere:
            return intersect_with_sphere(r, &(obj->data.s), out, dist);
        case triangle: // @HACK
            return intersect_with_triangle(
                    r, (triangle_obj *) &(obj->data.tr), out, dist);
    }

    return 0;
}

static vec3d get_normal(vec3d point, const scene_obj *obj, vec3d view_point)
{
    switch (obj->type) {
        case sphere:
            return get_sphere_normal(point, &(obj->data.s));
        case triangle:
            return get_triangle_normal(point, &(obj->data.tr), view_point);
    }

    return vec3d_zero();
}

static scene_obj *find_closest_object(ray r, const scene *s,
                                      vec3d *pos_out, double *dist)
{
    scene_obj *obj_p, *closest;
    double cur_dist;
    vec3d pos;

    closest = NULL;
    *dist = DBL_MAX;

    for (obj_p = s->objects; obj_p - s->objects < s->objects_cnt; obj_p++) {
        if (!intersect_ray(r, obj_p, &pos, &cur_dist))
            continue;

        if (cur_dist < *dist) {
            *dist = cur_dist;
            closest = obj_p;
            *pos_out = pos;
        }
    }

    return closest;
}

static int light_is_seen(vec3d point, const light_src *l, const scene *s)
{
    const scene_obj *objp;
    ray lr;
    vec3d intersection;
    double dist, dist_to_light;

    lr.orig = point;
    lr.dir = vec3d_sub(l->pos, point);
    dist_to_light = vec3d_len(lr.dir);
    vec3d_normalize(&lr.dir);
    
    for (objp = s->objects; objp - s->objects < s->objects_cnt; objp++) {
        if (intersect_ray(lr, objp, &intersection, &dist) &&
                dist <= dist_to_light)
            return 0;
    }

    return 1;
}

static vec3d diffuse_illum(const scene_obj *obj, const light_src *l, 
        vec3d point, vec3d normal)
{
    vec3d vl;
    double scal;

    /* formula: kd * illum * max(0, cos(angle from vec_to_light to normal)) */

    vl = vec3d_normalized(vec3d_sub(l->pos, point));
    scal = max(0, vec3d_dot(vl, normal));

    return vec3d_scale(
                vec3d_mul(obj->mat->kd, l->illum), 
                scal * obj->mat->al.x
            );
}

static vec3d specular_illum(const scene_obj *obj, const light_src *l,
        vec3d point, vec3d normal, vec3d view_point)
{
    vec3d vl, vr, ve;
    double scal;

    /* formula: ks * illum * (max(0, cos(angle from reflected vec from light 
     * to vector to viewer))) ^ ns */

    vl = vec3d_normalized(vec3d_sub(l->pos, point));
    vr = vec3d_reflect(vl, normal);

    ve = vec3d_normalized(vec3d_sub(view_point, point));
    scal = max(0, vec3d_dot(vr, ve));

    return vec3d_scale(
                vec3d_mul(obj->mat->ks, l->illum), 
                pow(scal, obj->mat->ns) * obj->mat->al.x
            );
}

static vec3d reflect_illum(const scene_obj *obj, const scene *s, 
                           vec3d point, vec3d normal, vec3d view_point,
                           int cur_depth, render_options ropts)
{
    if (dbl_is_zero(obj->mat->al.y))
        return vec3d_zero();

    vec3d ve = vec3d_normalized(vec3d_sub(view_point, point));
    if (vec3d_dot(normal, ve) < EPSILON)
        return vec3d_zero();

    ray refl_r;
    refl_r.orig = point;
    refl_r.dir = vec3d_reflect(ve, normal);
    
    return vec3d_scale(
                trace_ray(refl_r, s, cur_depth+1, ropts),
                obj->mat->al.y
            );
}

static vec3d refract_illum(const scene_obj *obj, const scene *s, 
                           vec3d point, vec3d normal, vec3d view_point,
                           int cur_depth, render_options ropts)
{
    if (obj->mat->ni < EPSILON)
        return vec3d_zero();

    vec3d ve = vec3d_normalized(vec3d_sub(view_point, point));
    double view_cos = vec3d_dot(normal, ve);

    if (dbl_is_zero(view_cos)) // ray horizontal
        return vec3d_zero();

    int we_are_outside_obj = view_cos >= EPSILON;

    double sin_coeff, al2, tr;
    if (we_are_outside_obj) {
        tr = obj->mat->tr;
        al2 = obj->mat->al.z;

        // @HACK so as not to make unnecessary calculations
        if (dbl_is_zero(tr) || dbl_is_zero(al2))
            return vec3d_zero();

        // @SPEED: we could bake 1/ni in material
        sin_coeff = 1.0 / obj->mat->ni;
    } else {
        tr = 1.;
        al2 = 1.;
        sin_coeff = obj->mat->ni;
    }

    // refracted ray, formulas: 
    // if outside obj (coords in tangent/normal plane):
    //  sin(phi_2) =  -sin(phi_1) / ni (Snell's law)
    //  cos(phi_2) = -sqrt(1 - sin**2) (normal points outside, ray -- inside)
    // else:
    //  sin(phi_2) =  -sin(phi_1) * ni (inverted -- outside ni is 1.)
    //  cos(phi_2) = sqrt(1 - sin**2) (ray and normal point outside)
    //
    // ray_dir = normal * cos + tangent * sin
    // tangent = view_dir - proj(view_dir onto normal)
    ray refr_r;
    refr_r.orig = point;

    double refr_sin_coord = -sin_coeff * vec3d_len(vec3d_cross(normal, ve));

    // if not -1 <= sine <= 0, (asb sine > 1 or < 0), consider refr failed
    if (refr_sin_coord > -EPSILON || refr_sin_coord < -1. + EPSILON)
        return vec3d_zero();

    double refr_cos_coord = 
        (we_are_outside_obj ? -1 : 1) * sqrt(1 - refr_sin_coord*refr_sin_coord);

    vec3d tangent = vec3d_normalized(
            vec3d_sub(ve, vec3d_scale(normal, view_cos)));
    
    // Sum of two normalized vectors with sin-cos coeffs is already normalized
    refr_r.dir = vec3d_sum(
            vec3d_scale(normal, refr_cos_coord),
            vec3d_scale(tangent, refr_sin_coord)
            );

    // multipliers: albedo_2 and transparency
    return vec3d_scale(
            trace_ray(refr_r, s, cur_depth+1, ropts), 
            tr * al2
            );
}

static vec3d shade(vec3d point, vec3d normal, vec3d view_point,
                   const scene_obj *obj, const scene *s,
                   int cur_depth, render_options ropts)
{
    vec3d color;
    int i;

    color = vec3d_sum(obj->mat->ka, obj->mat->ke); /* ambient */

    /* diffuse and specular over all lights */
    if (!dbl_is_zero(obj->mat->al.x)) {
        for (i = 0; i < s->lights_cnt; i++) {
            light_src *l = s->lights + i;
            if (light_is_seen(point, l, s)) {
                color = vec3d_sum3(
                    color, 
                    diffuse_illum(obj, l, point, normal),
                    specular_illum(obj, l, point, normal, view_point)
                );
            }
        }
    }

    /* if not reached max recursion depth, collect illumination from refletion
     * and refraction */
    if (cur_depth < ropts.max_rec_depth) {
        color = vec3d_sum3(
            color,
            reflect_illum(obj, s, point, normal, view_point, cur_depth, ropts),
            refract_illum(obj, s, point, normal, view_point, cur_depth, ropts)
        );
    }

    return color;
}

vec3d trace_ray(ray r, const scene *s, int cur_depth, render_options ropts)
{
    scene_obj *hit_obj;
    vec3d hit_point, hit_normal;
    double dist;

    // Avoid hitting yourself
    r.orig = vec3d_sum(r.orig, vec3d_scale(r.dir, EPSILON));

    hit_obj = find_closest_object(r, s, &hit_point, &dist);
    if (!hit_obj)
        return vec3d_zero();
    hit_normal = get_normal(hit_point, hit_obj, r.orig);

    switch (ropts.mode) {
        case rmode_full:
            return shade(hit_point, hit_normal, r.orig,
                         hit_obj, s, cur_depth, ropts);
        case rmode_depth:
            return vec3d_scale(vec3d_one(), dist);
        case rmode_normal:
            return hit_normal;;
        default:
            return vec3d_zero();
    }
}
