cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Performance configuration options

# Enable all performance features (default: OFF for maximum compatibility)
option(NLM_PERFORMANCE_ENABLED "Enable performance optimizations" ON)

# Individual performance feature options (can be combined with build-type)
option(NLM_ENABLE_MEMORY_POOL "Enable memory pool allocators" ON)
option(NLM_ENABLE_SOA "Enable Structure-of-Arrays data layout" ON)
option(NLM_ENABLE_EVENT_QUEUE "Enable event-driven spike processing" ON)
option(NLM_ENABLE_SPARSE_CONNECTIVITY "Enable sparse connectivity" ON)
option(NLM_ENABLE_MULTITHREADING "Enable parallel processing" ON)
option(NLM_ENABLE_SIMD "Enable SIMD vectorization" ON)

# SIMD preference (platform-specific fallback available)
option(NLM_PREFER_AVX512 "Prefer AVX-512 instructions" OFF)
option(NLM_PREFER_AVX2 "Prefer AVX2 instructions" OFF)
option(NLM_PREFER_SSE "Prefer SSE instructions" OFF)

# Multithreading configuration
option(NLM_DYNAMIC_THREAD_COUNT "Auto-detect thread count" ON)
set(NLM_FIXED_THREAD_COUNT 0 CACHE STRING "Fixed thread count (0 = auto)")

# Memory pool configuration
set(NLM_MEMORY_POOL_PAGE_SIZE 4096 CACHE STRING "Memory pool page size in bytes")
set(NLM_MAX_POOL_SIZE 1048576 CACHE STRING "Maximum objects per pool")

# Event queue configuration
set(NLM_SPIKE_QUEUE_CAPACITY 16384 CACHE STRING "Spike event queue capacity")
set(NLM_DELAYED_SPIKE_BUCKETS 1024 CACHE STRING "Number of delayed spike buckets")

# Runtime configuration
option(NLM_RUNTIME_FEATURE_TOGGLE "Allow runtime feature toggling" ON)
option(NLM_PERFORMANCE_STATS "Enable performance statistics collection" ON)
option(NLM_PERFORMANCE_VALIDATION "Enable configuration validation" ON)

# Adaptive performance scaling
option(NLM_ADAPTIVE_PERFORMANCE "Enable runtime performance adaptation" OFF)
set(NLM_MIN_THREADS 1 CACHE STRING "Minimum thread count for adaptive scaling")
set(NLM_MAX_THREADS 64 CACHE STRING "Maximum thread count for adaptive scaling")
set(NLM_PERFORMANCE_ADAPTATION_INTERVAL 10000 CACHE STRING "Steps between performance adaptations")

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Check for missing source files
file(GLOB ALL_SOURCE_FILES
    src/core/Types/*.cpp
    src/core/Config/*.cpp
    src/core/Random/*.cpp
    src/core/Logger/*.cpp
    src/core/SimulationClock/*.cpp
    src/brain/*.cpp
    src/dynamics/*.cpp
    src/plasticity/*.cpp
    src/development/*.cpp
    src/neuromodulation/*.cpp
    src/memory/*.cpp
    src/cognition/*.cpp
    src/sensory/*.cpp
    src/prediction/*.cpp
    src/motor/*.cpp
    src/environment/*.cpp
    src/experiments/*.cpp
    src/visualization/*.cpp
    src/agent/*.cpp
    src/world/*.cpp
)

# Core library
add_library(nlm_core STATIC
    ${ALL_SOURCE_FILES}
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)
target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)
target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)

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

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
