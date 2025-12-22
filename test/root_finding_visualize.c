#define PRINT_ROOT_FINDING
#include "root_finding.h"

static double f1(double x){
    return (x + 4)*x*x - 10;
}

static double f2(double x){
    return x-pow(3, -x);
}
static double f2_derived(double x){
    return 1+log(3)*pow(3, -x);
}

int main(int argc, char* argv[]){
    bisection(f1, 1, 2, 0.0005, 20);
    secant(f1, 1, 2, 0.0005, 20);
    newton(f2, f2_derived, 0, 1, 0.00000001, 20);

    return 0;
}
