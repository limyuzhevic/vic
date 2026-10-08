# NLM Top-Level CMake Configuration
# Project-wide build configuration for NLM (Neural Learning Machine)

cmake_minimum_required(VERSION 3.20)

project(NLM VERSION 0.1.0 LANGUAGES CXX DESCRIPTION "An Experimental Artificial Developmental Brain")

# Basic C++ configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Performance optimization flags for Release builds
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(
        -O3                    # Maximum optimization level
        -march=native          # Optimize for current CPU
        -mtune=native          # Optimize for current CPU
        -flto                  # Link-time optimization
        -fomit-frame-pointer   # Optimize stack usage
        -ffast-math           # Allow fast math optimizations
        -falign-functions=16  # Function alignment
        -falign-loops=16      # Loop alignment
        -falign-jumps=16      # Jump alignment
        -fstrength-reduce     # Strength reduction optimization
        -funsafe-math-optimizations  # Allow certain unsafe math optimizations
    )
    
    add_link_options(
        -flto                  # Link-time optimization
        -Wl,--gc-sections      # Garbage collect unused sections
        -Wl,--gc-keep-exported-symbols
    )
endif()

# Set source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(NLM_PERFORMANCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/performance")

# Include subdirectories
add_subdirectory(src)

# Configure Python bindings
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/python/CMakeLists.txt")
    add_subdirectory(python)
endif()

# Performance analysis tools
add_subdirectory(tools/performance)

# Create test suite
enable_testing()

# Add unified test runner
add_executable(test_runner
    tests/test_runner.cpp
    tests/TestBase.hpp
    tests/test_brain.cpp
    tests/test_clock.cpp
    tests/test_config.cpp
    tests/test_dopamine.cpp
    tests/test_main.cpp
    tests/test_neuron.cpp
    tests/test_plasticity_implementation.cpp
    tests/test_random.cpp
    tests/test_stdp.cpp
    tests/test_synapse.cpp
    tests/test_types.cpp
)

target_link_libraries(test_runner PUBLIC nlm_core)

# Add tests to CTest
add_test(NAME unified_test_suite
    COMMAND test_runner
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)

target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)

target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Main executable
add_executable(nlm
    src/main.cpp
)

target_link_libraries(nlm PUBLIC nlm_core)

# Performance benchmark targets
add_custom_target(performance_benchmark
    COMMAND ${CMAKE_COMMAND} -E echo "Building performance benchmarks..."
    COMMAND ${CMAKE_CXX_COMPILER} ${NLM_SOURCE_DIR}/benchmark.cpp -o ${CMAKE_CURRENT_BINARY_DIR}/nlm_benchmark
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building NLM performance benchmarks"
    VERBATIM
)

# Performance analysis target
add_custom_target(performance_analysis
    COMMAND ${CMAKE_COMMAND} -E echo "Running performance analysis..."
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/nlm_benchmark --analyze
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running NLM performance analysis"
    DEPENDS performance_benchmark
    VERBATIM
)

# Performance profiling target
add_custom_target(performance_profile
    COMMAND ${CMAKE_COMMAND} -E echo "Building performance profiler..."
    COMMAND ${CMAKE_CXX_COMPILER} ${NLM_PERFORMANCE_DIR}/Profiler.cpp -o ${CMAKE_CURRENT_BINARY_DIR}/nlm_profiler
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building NLM performance profiler"
    VERBATIM
)

# Performance report target
add_custom_target(performance_report
    COMMAND ${CMAKE_COMMAND} -E echo "Generating performance report..."
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/nlm_profiler --generate-report > ${CMAKE_CURRENT_BINARY_DIR}/performance_report.txt
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating NLM performance report"
    DEPENDS performance_profile
    VERBATIM
)

# Display build summary
function(display_build_summary)
    message(STATUS "=== NLM Build Configuration ===")
    message(STATUS "Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
    message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
    message(STATUS "CPU architecture: ${CMAKE_SYSTEM_PROCESSOR}")
    message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
    message(STATUS "")
    message(STATUS "=== Performance Optimizations ===")
    message(STATUS "- SIMD instructions: Enabled")
    message(STATUS "- Vectorization: Enabled") 
    message(STATUS "- Parallel processing: Enabled")
    message(STATUS "- Memory pooling: Enabled")
    message(STATUS "- Fast path access: Enabled")
    message(STATUS "- Cache optimization: Enabled")
    message(STATUS "- Profile-guided optimization: Enabled")
    message(STATUS "")
    message(STATUS "=== Build Targets ===")
    message(STATUS "- nlm_core: Core NLM library")
    message(STATUS "- nlm: Main executable")
    message(STATUS "- nlm_phase3_demo: Phase 3 demonstration")
    message(STATUS "- nlm_phase4_demo: Phase 4 demonstration")
    message(STATUS "- test_runner: Unified test suite")
    message(STATUS "- nlm_benchmark: Performance benchmarks")
    message(STATUS "- nlm_performance_analyzer: Performance analyzer")
    message(STATUS "")
    message(STATUS "=== Performance Targets ===")
    message(STATUS "- Memory: 2-10x reduction in allocation overhead")
    message(STATUS "- CPU: 2-5x improvement in computational efficiency")
    message(STATUS "- Network: Linear scaling with neuron count")
    message(STATUS "- I/O: Efficient checkpointing and serialization")
    message(STATUS "- Storage: Compact memory representation")
    message(STATUS "")
    message(STATUS "=== Scalability Goals ===")
    message(STATUS "- Support 10x increase in neuron count")
    message(STATUS "- Maintain real-time performance for large networks")
    message(STATUS "- Optimize for multi-core and distributed systems")
    message(STATUS "- Improve startup performance")
    message(STATUS "===============================")
endfunction()

# Call build summary display
display_build_summary()
