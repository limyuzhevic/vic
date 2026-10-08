// NLM Source Files CMake Configuration
// This file defines the source files for the NLM core library

# Set source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Collect all C++ source files
file(GLOB_RECURSE NLM_SOURCES
    "${NLM_SOURCE_DIR}/*.cpp"
    "${NLM_SOURCE_DIR}/*.cxx"
    "${NLM_SOURCE_DIR}/*.cc"
)

# Filter out test files (kept in tests/ directory)
list(FILTER NLM_SOURCES EXCLUDE REGEX ".*/tests/.*")

# Core components
file(GLOB_RECURSE CORE_SOURCES
    "${NLM_SOURCE_DIR}/core/*.cpp"
    "${NLM_SOURCE_DIR}/core/*.cxx"
    "${NLM_SOURCE_DIR}/core/*.cc"
)

file(GLOB_RECURSE BRAIN_SOURCES
    "${NLM_SOURCE_DIR}/brain/*.cpp"
    "${NLM_SOURCE_DIR}/brain/*.cxx"
    "${NLM_SOURCE_DIR}/brain/*.cc"
)

file(GLOB_RECURSE DYNAMICS_SOURCES
    "${NLM_SOURCE_DIR}/dynamics/*.cpp"
    "${NLM_SOURCE_DIR}/dynamics/*.cxx"
    "${NLM_SOURCE_DIR}/dynamics/*.cc"
)

file(GLOB_RECURSE MEMORY_SOURCES
    "${NLM_SOURCE_DIR}/memory/*.cpp"
    "${NLM_SOURCE_DIR}/memory/*.cxx"
    "${NLM_SOURCE_DIR}/memory/*.cc"
)

file(GLOB_RECURSE NEUROMODULATION_SOURCES
    "${NLM_SOURCE_DIR}/neuromodulation/*.cpp"
    "${NLM_SOURCE_DIR}/neuromodulation/*.cxx"
    "${NLM_SOURCE_DIR}/neuromodulation/*.cc"
)

file(GLOB_RECURSE PLASTICITY_SOURCES
    "${NLM_SOURCE_DIR}/plasticity/*.cpp"
    "${NLM_SOURCE_DIR}/plasticity/*.cxx"
    "${NLM_SOURCE_DIR}/plasticity/*.cc"
)

file(GLOB_RECURSE PREDICTION_SOURCES
    "${NLM_SOURCE_DIR}/prediction/*.cpp"
    "${NLM_SOURCE_DIR}/prediction/*.cxx"
    "${NLM_SOURCE_DIR}/prediction/*.cc"
)

file(GLOB_RECURSE COGNITION_SOURCES
    "${NLM_SOURCE_DIR}/cognition/*.cpp"
    "${NLM_SOURCE_DIR}/cognition/*.cxx"
    "${NLM_SOURCE_DIR}/cognition/*.cc"
)

file(GLOB_RECURSE AGENT_SOURCES
    "${NLM_SOURCE_DIR}/agent/*.cpp"
    "${NLM_SOURCE_DIR}/agent/*.cxx"
    "${NLM_SOURCE_DIR}/agent/*.cc"
)

file(GLOB_RECURSE WORLD_SOURCES
    "${NLM_SOURCE_DIR}/world/*.cpp"
    "${NLM_SOURCE_DIR}/world/*.cxx"
    "${NLM_SOURCE_DIR}/world/*.cc"
)

file(GLOB_RECURSE ENVIRONMENT_SOURCES
    "${NLM_SOURCE_DIR}/environment/*.cpp"
    "${NLM_SOURCE_DIR}/environment/*.cxx"
    "${NLM_SOURCE_DIR}/environment/*.cc"
)

file(GLOB_RECURSE PERFORMANCE_SOURCES
    "${NLM_SOURCE_DIR}/performance/*.cpp"
    "${NLM_SOURCE_DIR}/performance/*.cxx"
    "${NLM_SOURCE_DIR}/performance/*.cc"
)

# Create the main NLM core library
add_library(nlm_core STATIC
    ${CORE_SOURCES}
    ${BRAIN_SOURCES}
    ${DYNAMICS_SOURCES}
    ${MEMORY_SOURCES}
    ${NEUROMODULATION_SOURCES}
    ${PLASTICITY_SOURCES}
    ${PREDICTION_SOURCES}
    ${COGNITION_SOURCES}
    ${AGENT_SOURCES}
    ${WORLD_SOURCES}
    ${ENVIRONMENT_SOURCES}
    ${PERFORMANCE_SOURCES}
)

# Include directories for NLM core
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Set compiler options for performance
target_compile_options(nlm_core PRIVATE
    -O2                    # Optimized build
    -march=native          # Optimize for current CPU
    -mtune=native          # Optimize for current CPU
    -ffast-math           # Allow fast math optimizations
    -fomit-frame-pointer  # Optimize stack usage
    -flto                 # Link-time optimization
)

# Set language standards
target_compile_features(nlm_core PUBLIC cxx_std_20)

# Set linker options for performance
target_link_libraries(nlm_core PUBLIC
    ${CMAKE_CXX_FLAGS_RELEASE}
)

# Performance optimization macros
target_compile_definitions(nlm_core PUBLIC
    NLM_PERFORMANCE_PROFILING
    NLM_VECTORIZATION_ENABLED
    NLM_SIMD_ENABLED
    NLM_PARALLEL_PROCESSING
)

# Install rules
install(TARGETS nlm_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${NLM_SOURCE_DIR}/core/ DESTINATION include/nlm/core
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

install(DIRECTORY ${NLM_SOURCE_DIR}/brain/ DESTINATION include/nlm/brain
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

install(DIRECTORY ${NLM_SOURCE_DIR}/performance/ DESTINATION include/nlm/performance
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

# Add performance testing executable
add_executable(nlm_performance_test
    tests/performance_test.cpp
)

target_link_libraries(nlm_performance_test PUBLIC nlm_core)

target_compile_options(nlm_performance_test PRIVATE
    -O2
    -march=native
    -mtune=native
)

# Add benchmark executable
add_executable(nlm_benchmark
    tests/benchmark.cpp
)

target_link_libraries(nlm_benchmark PUBLIC nlm_core)

target_compile_options(nlm_benchmark PRIVATE
    -O2
    -march=native
    -mtune=native
)

# Performance analysis tools
add_executable(nlm_performance_analyzer
    tools/performance_analyzer.cpp
)

target_link_libraries(nlm_performance_analyzer PUBLIC nlm_core)

# Add tests
target_include_directories(tests PUBLIC ${NLM_SOURCE_DIR})
