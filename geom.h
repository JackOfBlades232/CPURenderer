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
} triangle_obj;

double deg2rad(double deg);

vec3d create_vec(double x, double y, double z);
vec3d sum(vec3d v1, vec3d v2);
vec3d sum3(vec3d v1, vec3d v2, vec3d v3);
vec3d subtr(vec3d v1, vec3d v2);
vec3d mul(vec3d v, double coeff);
vec3d powv(vec3d v, double p);
vec3d normalized(vec3d v);
vec3d prod(vec3d v1, vec3d v2);
double dot(vec3d v1, vec3d v2);
vec3d mulv(vec3d v1, vec3d v2);
vec3d divv(vec3d v1, vec3d v2);

sphere_obj create_sphere(double cx, double cy, double cz, double r);
vec3d get_sphere_normal(vec3d point, sphere_obj s);
int intersect_with_sphere(ray r, sphere_obj s, vec3d *out, double *dist);
            
vec3d get_triangle_normal(vec3d point, triangle_obj tr);
int intersect_with_triangle(ray r, triangle_obj s, vec3d *out, double *dist);

#endif 