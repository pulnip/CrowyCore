#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace Crowy
{
    std::string toUpper(std::string_view);

    struct StringHash{
        using is_transparent = void;

        size_t operator()(std::string_view view) const{
            return std::hash<std::string_view>{}(view);
        }
    };

    std::string readFileAsString(const std::filesystem::path&);
    std::vector<uint8_t> readFileAsBinary(const std::filesystem::path&);
}