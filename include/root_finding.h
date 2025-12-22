#pragma once

#include <assert.h>
#include <math.h>
#ifdef PRINT_ROOT_FINDING
#include <stdio.h>
#endif

static inline double bisection(double(*f)(double), double a, double b,
    const double TOL, const int n0
){
    double fa=f(a), fb=f(b);
    int inc=fa<0;

    assert(a < b);
    assert(TOL > 0);
    assert(n0 > 0);
    assert(fa*fb < 0);

    double p=(a+b)/2;

#ifdef PRINT_ROOT_FINDING
    puts("_n|______________an|______________bn|______________pn|___________f(pn)");
#endif
    for(int i=1; i<n0; ++i){
        double fp=f(p);
        if((fabs(b-a) < TOL) || (fp == 0))
            break;

#ifdef PRINT_ROOT_FINDING
        printf("%2d|%16.10lf|%16.10lf|%16.10lf|%16.10lf\n", i, a, b, p, fp);
#endif

        if(fp < 0){
            if(inc) a=p;
            else b=p;
        }
        else{
            if(inc) b=p;
            else a=p;
        }
        p=(a+b)/2;
    }

    return p;
}

static inline double secant(double(*f)(double), double p0, double p1,
    const double TOL, const int n0
){
    assert(TOL > 0);
    assert(n0 > 0);

#ifdef PRINT_ROOT_FINDING
    puts("_n|______________pn|___________f(pn)");
#endif
    for(int i=2; i<n0; ++i){
        if(fabs(p1-p0) < TOL)
            break;

        double fp0=f(p0), fp1=f(p1);
        double p=(p0*fp1 - p1*fp0)/(fp1-fp0);
        double fp=f(p);
#ifdef PRINT_ROOT_FINDING
        printf("%2d|%16.10lf|%16.10lf\n", i, p, fp);
#endif

        if(fp == 0)
            break;

        p0=p1;
        p1=p;
    }

    return (p0+p1)/2;
}

static inline double newton(double(*f0)(double), double(*f1)(double),
    double x0, double x1,
    const double TOL, const int N0
){
    double p0 = (x0 + x1)/2;

#ifdef PRINT_ROOT_FINDING
    puts("_n|______________pn|____abs(pn-pn-1)");
    printf(" 0|%16.9lf|\n", p0);
#endif
    for(int i=1; i<N0; ++i){
        double p = p0 - f0(p0)/f1(p0);

        if((fabs(p-p0) < TOL) || (f0(p) == 0))
            break;

#ifdef PRINT_ROOT_FINDING
        printf("%2d|%16.9lf|%16.9lf\n", i, p, fabs(p-p0));
#endif
        p0=p;
    }

    return p0;
}
