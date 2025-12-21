cmake_minimum_required(VERSION 3.28)

project(CrowyCore LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

if(DEFINED CROWY_ENABLE_TEST)
    set(CROWYCORE_ENABLE_TEST ${CROWY_ENABLE_TEST})
else()
    option(CROWYCORE_ENABLE_TEST "Enable Tests" ON)
endif()

add_library(CrowyCore STATIC)

target_sources(CrowyCore
PRIVATE
    src/gjk.cpp
)

target_include_directories(CrowyCore
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_library(Crowy::Core ALIAS CrowyCore)

if(CROWYCORE_ENABLE_TEST)
    add_subdirectory(test)
endif()