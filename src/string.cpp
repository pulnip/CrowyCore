#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "string.hpp"

namespace Crowy
{
    std::string toUpper(std::string_view view){
        std::string upper(view);
        std::transform(view.begin(), view.end(), upper.begin(),
            [](unsigned char c){ return std::toupper(c); }
        );
        return upper;
    }

    std::string readFileAsString(const std::filesystem::path& path){
        std::ifstream file(path);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    std::vector<uint8_t> readFileAsBinary(const std::filesystem::path& path){
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if(!file.is_open())
            throw std::runtime_error("Failed to open file: " + path.string());

        auto size = file.tellg();
        file.seekg(0);

        std::vector<uint8_t> buffer(size);
        file.read(reinterpret_cast<char*>(buffer.data()), size);

        return buffer;
    }
}