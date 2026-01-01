#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include "core_types.hpp"

namespace Crowy
{
    template<typename T>
    struct generic_handle{
        Index index = INVALID_INDEX;
        uint32_t generation = 0;

        bool isValid() const{
            return index != INVALID_INDEX;
        }

        static auto invalidHandle(){
            return generic_handle{};
        }

        friend bool operator==(const generic_handle& lhs, const generic_handle& rhs){
            return lhs.index == rhs.index && lhs.generation == rhs.generation;
        }
    };

    template<typename T>
    struct generic_handle_hash{
        size_t operator()(const generic_handle<T>& handle) const{
            return std::hash<Index>()(handle.index) ^ (std::hash<uint32_t>()(handle.generation) << 1);
        }
    };
}
