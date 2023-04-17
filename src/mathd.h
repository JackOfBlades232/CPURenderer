/* CPURenderer/src/mathd.h */
#ifndef MATHD_SENTRY
#define MATHD_SENTRY

#include "geom.h"

#define EPSILON 0.000001

#define max(a, b) (a > b ? a : b)
#define max3(a, b, c) (max(a, max(b, c)))
#define min(a, b) (a < b ? a : b)
#define min_non_neg(a, b) ((a < 0 || b < 0) ? max(a, b) : min(a, b))
#define sign(a) (a > EPSILON ? 1 : (a < -EPSILON ? -1 : 0))

void swap_dbl(double *a, double *b);
int solve_qe(double a, double b, double c, double *r1, double *r2);
int dbl_is_zero(double x);

#endif 
