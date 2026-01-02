#pragma once

#include <algorithm>
#include <string>

namespace Crowy
{
    inline std::string toUpper(const std::string& text){
        std::string upper = text;
        std::transform(text.begin(), text.end(), upper.begin(),
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