/* CPURenderer/src/geom.h */
#ifndef GEOM_SENTRY
#define GEOM_SENTRY

typedef enum tag_dim3d { x, y, z } dim3d;

typedef enum tag_object_type { sphere, triangle } object_type;

typedef struct tag_vec3d {
    double x, y, z;
} vec3d;

typedef struct tag_ray {
    vec3d orig, dir;
} ray;

typedef struct tag_sphere_obj {
    vec3d center;
    double rad;
} sphere_obj;

typedef struct tag_triangle_obj {
    vec3d v1, v2, v3;
    vec3d normal;
    vec3d vn1, vn2, vn3;
    vec3d last_hit_barycentric;
    int has_vn;
} triangle_obj;

typedef struct tag_bounds {
    vec3d min, max;
} bounds;

double deg2rad(double deg);

vec3d vec3d_literal(double x, double y, double z);
vec3d vec3d_zero();
vec3d vec3d_one();
int vec3d_is_zero(vec3d v);
vec3d vec3d_neg(vec3d v);
vec3d vec3d_sum(vec3d v1, vec3d v2);
vec3d vec3d_sum3(vec3d v1, vec3d v2, vec3d v3);
vec3d vec3d_sub(vec3d v1, vec3d v2);
vec3d vec3d_scale(vec3d v, double coeff);
vec3d vec3d_pow(vec3d v, double p);
vec3d vec3d_normalized(vec3d v);
void vec3d_normalize(vec3d *v);
vec3d vec3d_cross(vec3d v1, vec3d v2);
double vec3d_dot(vec3d v1, vec3d v2);
double vec3d_sq_len(vec3d v);
double vec3d_len(vec3d v);
vec3d vec3d_mul(vec3d v1, vec3d v2);
vec3d vec3d_div(vec3d v1, vec3d v2);

double vec3d_get_dim(vec3d v, dim3d dim);
int vec3d_dim_comp(vec3d v1, vec3d v2, dim3d dim);

vec3d vec3d_reflect(vec3d v, vec3d normal);

bounds bounds_from_point(vec3d point);
bounds bounds_union(bounds b1, bounds b2);
bounds bounds_add_point(bounds b, vec3d point);
dim3d bounds_max_dim(bounds b);
double bounds_dim_spread(bounds b, dim3d dim);
vec3d bounds_center(bounds b);
double bounds_area(bounds b);

int intersect_with_bounds(ray r, bounds b, double *dist);

sphere_obj sphere_literal(double cx, double cy, double cz, double r);
vec3d get_sphere_normal(vec3d point, const sphere_obj *s);
int intersect_with_sphere(ray r, const sphere_obj *s, 
                          vec3d *out, double *dist);
bounds get_sphere_bounds(const sphere_obj *s);
vec3d get_sphere_centroid(const sphere_obj *s);
            
triangle_obj trianlge_literal(double x1, double y1, double z1,
                              double x2, double y2, double z2,
                              double x3, double y3, double z3);
vec3d get_triangle_normal(vec3d point, const triangle_obj *tr, 
                          vec3d view_point);
int intersect_with_triangle(ray r, triangle_obj *tr, vec3d *out, double *dist);
bounds get_triangle_bounds(const triangle_obj *tr);
vec3d get_triangle_centroid(const triangle_obj *tr);

#endif 
