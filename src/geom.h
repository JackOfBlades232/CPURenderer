/* CPURenderer/geom.h */
#ifndef GEOM_SENTRY
#define GEOM_SENTRY

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
} triangle_obj;

double deg2rad(double deg);

vec3d create_vec(double x, double y, double z);
vec3d zero_vec();
vec3d neg(vec3d v);
vec3d sum(vec3d v1, vec3d v2);
vec3d sum3(vec3d v1, vec3d v2, vec3d v3);
vec3d subtr(vec3d v1, vec3d v2);
vec3d mul(vec3d v, double coeff);
vec3d powv(vec3d v, double p);
vec3d normalized(vec3d v);
vec3d prod(vec3d v1, vec3d v2);
double dot(vec3d v1, vec3d v2);
double sq_len(vec3d v);
double len(vec3d v);
vec3d mulv(vec3d v1, vec3d v2);
vec3d divv(vec3d v1, vec3d v2);

vec3d reflect(vec3d v, vec3d normal);

sphere_obj create_sphere(double cx, double cy, double cz, double r);
vec3d get_sphere_normal(vec3d point, sphere_obj s);
int intersect_with_sphere(ray r, sphere_obj s, vec3d *out, double *dist);
            
triangle_obj create_triangle(double x1, double y1, double z1,
                             double x2, double y2, double z2,
                             double x3, double y3, double z3);
vec3d get_triangle_normal(vec3d point, triangle_obj tr, vec3d view_point);
int intersect_with_triangle(ray r, triangle_obj s, vec3d *out, double *dist);

#endif 
