/* CPURenderer/mathd.h */
#ifndef MATHD_SENTRY
#define MATHD_SENTRY

#include "geom.h"

#define EPSILON 0.000001

double max(double a, double b);
double min(double a, double b);
double min_non_neg(double a, double b);
void swap_dbl(double *a, double *b);
int solve_qe(double a, double b, double c, double *r1, double *r2);
int solve_3_linear_eq(vec3d c1, vec3d c2, vec3d c3, vec3d b, vec3d *sol);
int dbl_is_zero(double x);

#endif 
