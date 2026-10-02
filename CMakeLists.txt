# NLM Project CMake Configuration
# Build system for Neural Learning Machine (NLM) Project

cmake_minimum_required(VERSION 3.10)
project(NLM VERSION 2.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Project configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Compiler optimizations
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
    add_compile_options(-O3 -march=native -fomit-frame-pointer -flto)
    add_link_options(-flto -Wl,--gc-sections)
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Source files
file(GLOB_RECURSE SRC_FILES
    "src/**/*.cpp"
    "src/**/*.cc"
)

# Header files
file(GLOB_RECURSE HDR_FILES
    "src/**/*.hpp"
    "src/**/*.h"
)

# Create executable
add_executable(nlm ${SRC_FILES} ${HDR_FILES})

# Target properties
target_compile_definitions(nlm PRIVATE _USE_MATH_DEFINES _CRT_SECURE_NO_WARNINGS)
target_link_libraries(nlm PUBLIC stdc++)

# Installation
install(TARGETS nlm
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION bin
)

# Documentation
install(DIRECTORY docs/ DESTINATION share/doc/nlm)

# Configuration files
install(DIRECTORY configs/ DESTINATION etc/nlm)

# Add tests
enable_testing()

# Add example targets
add_executable(nlm_demo examples/nlm_demo.cpp)
target_link_libraries(nlm_demo nlm)

# Add benchmark target
add_executable(nlm_benchmark src/benchmark.cpp)
target_link_libraries(nlm_benchmark nlm)

# Add experiment targets
add_executable(phase2_experiment src/experiments/Phase2Experiment.cpp)
target_link_libraries(phase2_experiment nlm)

# Add installation rules for examples
install(TARGETS nlm_demo nlm_benchmark phase2_experiment
    RUNTIME DESTINATION bin/examples
)

# Installation for headers
install(DIRECTORY include/nlm DESTINATION include/nlm)

# Build options
option(BUILD_EXAMPLES "Build example programs" ON)
option(BUILD_BENCHMARKS "Build benchmarks" ON)
option(BUILD_TESTS "Build tests" ON)
option(ENABLE_PERFORMANCE_MONITORING "Enable performance monitoring" ON)
option(ENABLE_VECTORIZATION "Enable compiler vectorization" ON)
option(ENABLE_MEMORY_POOLING "Enable memory pooling optimizations" ON)

# Performance optimizations
if(ENABLE_VECTORIZATION)
    target_compile_options(nlm PRIVATE -march=native -O3)
endif()

if(ENABLE_MEMORY_POOLING)
    target_compile_definitions(nlm PRIVATE NLM_ENABLE_MEMORY_POOLING)
endif()

if(ENABLE_PERFORMANCE_MONITORING)
    target_compile_definitions(nlm PRIVATE NLM_PERFORMANCE_MONITORING)
endif()

# Target-specific optimizations
add_custom_target(performance_optimize
    COMMAND ${CMAKE_COMMAND} -E echo "Optimizing build for performance..."
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Test target
add_test(NAME nlm_basic
    COMMAND nlm
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Custom targets for different build types
add_custom_target(debug
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ..
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building in Debug mode"
)

add_custom_target(release
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ..
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building in Release mode"
)

add_custom_target(optimize
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release -DENABLE_VECTORIZATION=ON -DENABLE_MEMORY_POOLING=ON ..
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building with maximum optimizations"
)

# Summary message
message(STATUS "NLM Project Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Examples: ${BUILD_EXAMPLES}")
message(STATUS "  Benchmarks: ${BUILD_BENCHMARKS}")
message(STATUS "  Tests: ${BUILD_TESTS}")
message(STATUS "  Performance Monitoring: ${ENABLE_PERFORMANCE_MONITORING}")
message(STATUS "  Vectorization: ${ENABLE_VECTORIZATION}")
message(STATUS "  Memory Pooling: ${ENABLE_MEMORY_POOLING}")

# Export configuration
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlm_config.cmake
    @ONLY
)

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(EXPORT nlmTargets
    FILE nlmConfig.cmake
    NAMESPACE nlm::
    DESTINATION lib/cmake/nlm
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfigVersion.cmake
    DESTINATION lib/cmake/nlm
)
