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

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Performance optimizations
add_library(nlm_performance STATIC
    src/performance/SIMDOptimizations.cpp
)
target_include_directories(nlm_performance PUBLIC src)
target_link_libraries(nlm_performance PUBLIC nlm_core)

# Core configuration library
add_library(nlm_config STATIC
    src/core/Config/Config.cpp
    src/core/Config/NLMConfig.cpp
)
target_include_directories(nlm_config PUBLIC src)
target_link_libraries(nlm_config PUBLIC nlm_core nlm_performance)

# Logger system
add_library(nlm_logger STATIC
    src/core/Logger/Logger.cpp
)
target_include_directories(nlm_logger PUBLIC src)
target_link_libraries(nlm_logger PUBLIC nlm_core)

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core nlm_config nlm_performance nlm_logger)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core nlm_config nlm_performance nlm_logger)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)
target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world nlm_config nlm_performance nlm_logger)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)
target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_config nlm_performance nlm_logger)

# Main executable
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core nlm_config nlm_performance nlm_logger)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_config nlm_performance nlm_logger)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_config nlm_performance nlm_logger)
target_include_directories(nlm_phase4_demo PUBLIC src)

# Phase 5 library (if it exists)
if(TARGET nlm_phase5)
    target_link_libraries(nlm_phase4_demo PUBLIC nlm_phase5)
endif()

# Phase 6 demo executable
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_config nlm_performance nlm_logger)
if(TARGET nlm_phase5)
    target_link_libraries(nlm_phase6_demo PUBLIC nlm_phase5)
endif()
target_include_directories(nlm_phase6_demo PUBLIC src)

# Unified Phase Test Framework
add_library(nlm_phase_tests STATIC
    src/experiments/UnifiedPhaseTestFramework.cpp
)
target_include_directories(nlm_phase_tests PUBLIC src)
target_link_libraries(nlm_phase_tests PUBLIC nlm_core nlm_agent nlm_world nlm_config nlm_performance nlm_logger)

# Testing framework
enable_testing()

# Find GTest
find_package(GTest REQUIRED)

# Add tests target
add_executable(test_experiments
    tests/test_experiments.cpp
)
target_link_libraries(test_experiments PUBLIC nlm_core nlm_agent nlm_world nlm_config nlm_performance nlm_logger GTest::gtest GTest::gtest_main)
add_test(NAME experiments_test COMMAND test_experiments)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    nlm_phase3_demo nlm_phase4_demo nlm_phase6_demo nlm_config nlm_performance nlm_logger nlm_phase_tests
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)