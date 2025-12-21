enable_testing()

add_executable(CrowyCoreTest
    dynamic_vector_test.cpp
    gjk2d_test.cpp
    gjk3d_test.cpp
    math_test.cpp
    obb_test.cpp
)

target_link_libraries(CrowyCoreTest
PRIVATE
    Crowy::Core
    GTest::gtest_main
    GTest::gmock_main
)

include(GoogleTest)
add_test(
    NAME Crowy-Core-Test
    COMMAND $<TARGET_FILE:CrowyCoreTest>
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
set_tests_properties(Crowy-Core-Test PROPERTIES LABELS "unit;core")