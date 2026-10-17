# Test Framework CMakeLists.txt
# This file defines the test framework library and test executables

cmake_minimum_required(VERSION 3.16)

# Test framework library
add_library(nlm_test_framework STATIC
    test_framework/TestSuite.cpp
)

target_include_directories(nlm_test_framework PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/test_framework
)

target_link_libraries(nlm_test_framework PUBLIC nlm_core)

# Test executables
add_executable(test_basic src/tests/test_basic.cpp)
add_executable(test_plasticity src/tests/test_plasticity.cpp)
add_executable(test_stdp src/tests/test_stdp.cpp)

foreach(test_target test_basic test_plasticity test_stdp)
    target_link_libraries(${test_target} PUBLIC nlm_core nlm_test_framework)
endforeach()

# Install test executables (future)
# install(TARGETS test_basic test_plasticity test_stdp
#     RUNTIME DESTINATION bin
# )
