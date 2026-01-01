#pragma once

#include <cstddef>
#include <limits>

namespace Crowy
{
    using Index = size_t;
    constexpr auto INVALID_INDEX = std::numeric_limits<Index>::max();
}