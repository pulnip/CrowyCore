cmake_minimum_required(VERSION 3.28)

project(CrowyCore LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

if(DEFINED CROWY_ENABLE_TEST)
    set(CROWYCORE_ENABLE_TEST ${CROWY_ENABLE_TEST})
else()
    option(CROWYCORE_ENABLE_TEST "Enable Tests" ON)
endif()

# target NumericalMethod
add_library(CrowyNumericalMethod INTERFACE)

target_include_directories(CrowyNumericalMethod
INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_library(Crowy::NumericalMethod ALIAS CrowyNumericalMethod)

# target Core
add_library(CrowyCore STATIC)

target_precompile_headers(CrowyCore
PUBLIC
    <algorithm>
    <array>
    <bit>
    <cassert>
    <chrono>
    <cmath>
    <cstddef>
    <cstdlib>
    <filesystem>
    <format>
    <functional>
    <iterator>
    <memory>
    <set>
    <stdexcept>
    <optional>
    <print>
    <ranges>
    <source_location>
    <string>
    <string_view>
    <thread>
    <type_traits>
    <unordered_map>
    <utility>
    <vector>
)

file(GLOB ALL_HEADERS
    "include/*.hpp"
)
file(GLOB ALL_SOURCES
    "src/*.cpp"
)

target_sources(CrowyCore
PUBLIC
    ${ALL_HEADERS}
PRIVATE
    ${ALL_SOURCES}
)

target_include_directories(CrowyCore
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(CrowyCore
PUBLIC
    Crowy::NumericalMethod
)

add_library(Crowy::Core ALIAS CrowyCore)

if(CROWYCORE_ENABLE_TEST)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(test)
endif()