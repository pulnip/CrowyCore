#pragma once

#include "math.hpp"

namespace Crowy
{
    struct Ray{
        Vec3 point;
        Vec3 dir;
    };

    struct RaycastHit{
        Vec3 point;
        float distance;
    };
}