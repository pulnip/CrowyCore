#pragma once

#include <algorithm>
#include <string>
#include <string_view>

namespace Crowy
{
    inline std::string toUpper(std::string_view view){
        std::string upper(view);
        std::transform(view.begin(), view.end(), upper.begin(),
            [](unsigned char c){ return std::toupper(c); }
        );
        return upper;
    }

    struct StringHash{
        using is_transparent = void;

        size_t operator()(std::string_view view) const{
            return std::hash<std::string_view>{}(view);
        }
    };
}