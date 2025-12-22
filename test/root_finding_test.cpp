#include <gtest/gtest.h>
#include "root_finding.h"

constexpr auto epsilon = 1e-2;

static double f1(double x){
    return (x + 4)*x*x - 10;
}

static double f2(double x){
    return x-pow(3, -x);
}
static double f2_derived(double x){
    return 1+log(3)*pow(3, -x);
}

TEST(ROOT_FINDING, BISECTION_METHOD){
    auto found = bisection(f1, 1, 2, 0.0005, 20);
    auto exact = 1.3652300134;

    EXPECT_LT(std::fabs(found-exact), epsilon);
    EXPECT_LT(std::fabs(f1(found)), epsilon);
}

TEST(ROOT_FINDING, SECANT_METHOD){
    auto found = secant(f1, 1, 2, 0.0005, 20);
    auto exact = 1.3652300134;

    EXPECT_LT(std::fabs(found-exact), epsilon);
    EXPECT_LT(std::fabs(f1(found)), epsilon);
}

TEST(ROOT_FINDING, NEWTON_METHOD){
    auto found = newton(f2, f2_derived, 0, 1, 0.00000001, 20);
    auto exact = 0.54780548;

    EXPECT_LT(std::fabs(found-exact), epsilon);
    EXPECT_LT(std::fabs(f2(found)), epsilon);
}
