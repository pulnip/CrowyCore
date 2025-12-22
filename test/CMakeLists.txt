enable_testing()

# target for visulize Numerical Method
add_executable(CrowyNumericalMethodVisualize
    root_finding_visualize.c
)

target_link_libraries(CrowyNumericalMethodVisualize
PRIVATE
    Crowy::NumericalMethod
)

include(GoogleTest)

# target for test Numerical Method
add_executable(CrowyNumericalMethodTest
    root_finding_test.cpp
)

target_link_libraries(CrowyNumericalMethodTest
PRIVATE
    Crowy::NumericalMethod
    GTest::gtest_main
    GTest::gmock_main
)

add_test(
    NAME Crowy-NumericalMethod-Test
    COMMAND $<TARGET_FILE:CrowyNumericalMethodTest>
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
set_tests_properties(Crowy-NumericalMethod-Test PROPERTIES LABELS "unit;core")

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

add_test(
    NAME Crowy-Core-Test
    COMMAND $<TARGET_FILE:CrowyCoreTest>
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
set_tests_properties(Crowy-Core-Test PROPERTIES LABELS "unit;core")
