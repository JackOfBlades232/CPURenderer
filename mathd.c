/* CPURenderer/mathd.c */
#include "mathd.h"
#include <math.h>

double max(double a, double b)
{
    return a > b ? a : b;
}

double min(double a, double b)
{
    return a < b ? a : b;
}

double min_non_neg(double a, double b)
{
    return a < 0 || b < 0 ? max(a, b) : min(a, b);
}

void swap_dbl(double *a, double *b)
{
    double tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

static double discrim(double a, double b, double c)
{
    return b*b - 4*a*c;
}

int solve_qe(double a, double b, double c, double *r1, double *r2)
{
    double d;
    d = discrim(a, b, c);
    if (d < 0)
        return 0;
    else if (d == 0) {
        *r1 = -b/(2*a);
        return 1;
    } else {
        d = sqrt(d);
        *r1 = (-b-d)/(2*a);
        *r2 = (-b+d)/(2*a);
        return 2;
    }
}

int dbl_is_zero(double x)
{
    return x > -EPSILON && x < EPSILON;
}

static double det3(vec3d c1, vec3d c2, vec3d c3)
{
    return dot(c1, prod(c2, c3));
}

int solve_3_linear_eq(vec3d c1, vec3d c2, vec3d c3, vec3d b, vec3d *sol)
{
    double main_det;

    main_det = det3(c1, c2, c3);
    if (dbl_is_zero(main_det))
        return 0;

    sol->x = det3(b, c2, c3) / main_det;
    sol->y = det3(c1, b, c3) / main_det;
    sol->z = det3(c1, c2, b) / main_det;

    return 1;
}
