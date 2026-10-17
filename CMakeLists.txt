cmake_minimum_required(VERSION 3.16)
project(NLMEnhancedBenchmark LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Error handling system files
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/core/ErrorHandling)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/core/Config)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/core/Logger)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/core/Random)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/core/SimulationClock)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/brain)

# Set compilation flags for better profiling
add_compile_options(
    -Wall
    -Wextra
    -O2
    -g
)

# Add sanitization flags for Debug build
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -g3 -fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
else()
    add_compile_options(-O3)
endif()

# Enhanced benchmark executable
set(BENCHMARK_SOURCES
    src/benchmark.cpp
)

# Add existing NLM library sources
list(APPEND BENCHMARK_SOURCES
    src/core/Config/Config.cpp
    src/core/Logger/Logger.cpp
    src/core/Random/Random.cpp
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/Brain.cpp
)

# Create enhanced benchmark executable
add_executable(nlm_enhanced_benchmark ${BENCHMARK_SOURCES})

# Set target properties
target_compile_options(nlm_enhanced_benchmark PRIVATE -Wall -Wextra -O2)

# Installation
install(TARGETS nlm_enhanced_benchmark
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Option to build with different profiling levels
option(BUILD_PERFORMANCE_PROFILER "Build with enhanced performance profiler" ON)
option(BUILD_MEMORY_PROFILER "Build with enhanced memory profiler" ON)
option(BUILD_PERFORMANCE_ANALYZER "Build with enhanced performance analyzer" ON)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
