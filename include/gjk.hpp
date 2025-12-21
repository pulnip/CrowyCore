#pragma once

#include "math.hpp"

namespace Crowy
{
    // gjk2d
    bool gjk(const Vec2 poly1[], int num_vert1,
        const Vec2 poly2[], int num_vert2);
    // gjk3d
    bool gjk(const Vec3 poly1[], int num_vert1,
        const Vec3 poly2[], int num_vert2);
}
