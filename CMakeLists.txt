#!/usr/bin/env cmake -E cmake_minimum_required(VERSION 3.10)
project(nlm_phase2 LANGUAGES CXX)

# Set minimum CMake version and C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include all source files
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# Create executable for Phase 2 tests
add_executable(nlm_phase2 ${SRC_FILES})

# Link libraries (assuming nlm is a library in the project)
# Uncomment and adjust as needed:
# target_link_libraries(nlm_phase2 nlm)

# Create test executables for standalone testing
add_executable(test_basic_connectivity test_basic_connectivity.cpp)
add_executable(test_plasticity_learning test_plasticity_learning.cpp)
add_executable(test_stdp_verification test_stdp_verification.cpp)

# Set test properties
set_target_properties(test_basic_connectivity PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)
set_target_properties(test_plasticity_learning PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)
set_target_properties(test_stdp_verification PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

# Add tests
add_test(NAME test_basic_connectivity COMMAND test_basic_connectivity)
add_test(NAME test_plasticity_learning COMMAND test_plasticity_learning)
add_test(NAME test_stdp_verification COMMAND test_stdp_verification)

# Install target
install(TARGETS nlm_phase2 DESTINATION bin)