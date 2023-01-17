/* CPURenderer/mathd.c */
#include "mathd.h"
#include <math.h>

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
        *r1 = -b / (2*a);
        return 1;
    } else {
        d = sqrt(d);
        *r1 = (-b-d)/(2*a);
        *r2 = (-b+d)/(2*a);
        if (*r1 > *r2) 
            swap_dbl(r1, r2);
        
        return 2;
    }
}

