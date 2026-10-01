# NLM CMake Configuration
# Modern C++20 build system with optimized settings

cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type optimization
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Optimization flags for Release build
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -ffast-math -funroll-loops")

# NLM-specific compile definitions
add_compile_definitions(NLM_LOG_LEVEL=Info)

# Include directories
include_directories(src)
include_directories(src/core)
include_directories(src/core/Config)
include_directories(src/core/Logger)
include_directories(src/core/Random)
include_directories(src/core/SimulationClock)
include_directories(src/core/Factory)
include_directories(src/core/TestRunner)
include_directories(src/core/Optimizations)
include_directories(src/brain)
include_directories(src/memory)
include_directories(src/plasticity)
include_directories(src/neuromodulation)
include_directories(src/cognition)
include_directories(src/sensory)
include_directories(src/motor)
include_directories(src/environment)
include_directories(src/agent)
include_directories(src/world)
include_directories(src/development)
include_directories(src/prediction)
include_directories(src/experiments)
include_directories(src/tests)

# Source files
file(GLOB_RECURSE NLM_SOURCES
    src/core/*.cpp
    src/brain/*.cpp
    src/memory/*.cpp
    src/plasticity/*.cpp
    src/neuromodulation/*.cpp
    src/cognition/*.cpp
    src/sensory/*.cpp
    src/motor/*.cpp
    src/environment/*.cpp
    src/agent/*.cpp
    src/world/*.cpp
    src/development/*.cpp
    src/prediction/*.cpp
    src/experiments/*.cpp
)

# Filter out test files for main executable
list(FILTER NLM_SOURCES EXCLUDE REGEX "/tests/.*\.cpp$" )

# Create main executable
add_executable(nlm_phase2 src/main.cpp ${NLM_SOURCES})

# Target properties for optimized build
set_target_properties(nlm_phase2 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    OPTIMIZE_FOR_SPEED ON
    USE_FOLDERS OFF
)

# Installation rules
install(TARGETS nlm_phase2 DESTINATION bin)
install(DIRECTORY src/core/Config/ DESTINATION include/nlm/core/Config)
install(DIRECTORY src/core/Logger/ DESTINATION include/nlm/core/Logger)
install(DIRECTORY src/brain/ DESTINATION include/nlm/brain)

# Python bindings with modern pybind11
find_package(pybind11 REQUIRED)

# Python module sources
file(GLOB PYTHON_SOURCES src/python/*.cpp)

pybind11_add_module(pynlm src/python/bindings.cpp ${NLM_SOURCES})
target_link_libraries(pynlm PRIVATE pybind11::module)
set_target_properties(pynlm PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/python
    PREFIX ""
)

# Test targets
enable_testing()

# Performance tests
add_executable(test_performance
    src/benchmarks/PerformanceBenchmark.cpp
    ${NLM_SOURCES}
)
target_link_libraries(test_performance nlm_phase2)

# Memory tests
add_executable(test_memory
    src/benchmarks/MemoryBenchmark.cpp
    ${NLM_SOURCES}
)
target_link_libraries(test_memory nlm_phase2)

# Integration tests
add_executable(test_integration
    src/benchmarks/IntegrationBenchmark.cpp
    ${NLM_SOURCES}
)
target_link_libraries(test_integration nlm_phase2)

# Python tests
add_custom_target(python_tests
    COMMAND python -m pytest tests/python/
    DEPENDS pynlm
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Build targets
add_custom_target(quick_build
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug -DQUICK_BUILD=ON ..
    COMMAND cmake --build . --target nlm_phase2
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_target(full_build
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ..
    COMMAND cmake --build . --parallel
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Documentation
add_custom_target(docs
    COMMAND doxygen Doxyfile
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

message(STATUS "NLM build system configured successfully!")
message(STATUS "Main executable: nlm_phase2")
message(STATUS "Python module: pynlm")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")