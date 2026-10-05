# Optimized CMake Configuration for NLM (Neural Learning Machine)
# Phase 6: Integrated Artificial Brain

# Use modern CMake practices
cmake_minimum_required(VERSION 3.20)

# Project configuration
project(NLM
    VERSION 6.0.0
    LANGUAGES CXX
    DESCRIPTION "An experimental artificial developmental brain with integrated memory, prediction, and cognition systems"
    HOMEPAGE_URL "https://github.com/neural-learning-machine/nlm"
)

# Set C++ standard and compiler options
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler optimization flags based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -march=native -flto -fomit-frame-pointer -ffast-math)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_options(-O2 -g -march=native -flto)
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    add_compile_options(-Os -DNDEBUG -flto)
else()
    add_compile_options(-O0 -g -ggdb)
endif()

# Warning flags and diagnostic options
if(MSVC)
    add_compile_options(/W4 /WX /permissive-)
else()
    add_compile_options(
        -Wall -Wextra -Wpedantic -Werror
        -Wshadow -Wformat=2 -Wconversion
        -Wnull-dereference -Wdouble-promotion
        -Wlogical-op -Wuseless-cast
        -Wthread-safety -Wcovered-switch-default
    )
endif()

# Platform-specific optimizations
if(UNIX AND NOT APPLE)
    add_compile_options(-pthread -march=x86-64 -mtune-native)
    add_link_options(-pthread)
elseif(APPLE)
    add_compile_options(-march=native)
endif()

# Source directory setup
file(GLOB_RECURSE NLM_SOURCES
    "src/*.cpp"
    "src/**/*.cpp"
)

# Filter out test and benchmark files from main library
list(FILTER NLM_SOURCES EXCLUDE "*test*.cpp" EXCLUDE "*benchmark*.cpp")

# Core library - all essential NLM functionality
add_library(nlm_core STATIC ${NLM_SOURCES})

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Precompiled headers for faster compilation
if(NLM_ENABLE_PERF_OPTIMIZATIONS)
    target_precompiled_header(nlm_core PRIVATE
        "src/core/Logger/Logger.hpp"
        "src/core/Config/Config.hpp"
        "src/brain/Brain.hpp"
    )
endif()

# Configure C++ features
target_compile_features(nlm_core PRIVATE cxx_std_23)

# Phase 3 experiments (world interaction)
file(GLOB_PHASE3_SOURCES "src/experiments/Phase3*.cpp")
add_library(nlm_phase3 STATIC ${PHASE3_SOURCES})
target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core)

# Phase 4 experiments (cognition and prediction)
file(GLOB_PHASE4_SOURCES "src/experiments/Phase4*.cpp")
add_library(nlm_phase4 STATIC ${PHASE4_SOURCES})
target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_phase3)

# Phase 5 experiments (performance optimizations)
file(GLOB_PHASE5_SOURCES "src/experiments/Phase5*.cpp")
add_library(nlm_phase5 STATIC ${PHASE5_SOURCES})
target_include_directories(nlm_phase5 PUBLIC src)
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_phase3 nlm_phase4)

# Phase 6 experiments (integrated brain)
file(GLOB_PHASE6_SOURCES "src/experiments/Phase6*.cpp")
add_library(nlm_phase6 STATIC ${PHASE6_SOURCES})
target_include_directories(nlm_phase6 PUBLIC src)
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_phase3 nlm_phase4 nlm_phase5)

# Agent system (brain-controlled agents)
file(GLOB_AGENT_SOURCES "src/agent/*.cpp")
add_library(nlm_agent STATIC ${AGENT_SOURCES})
target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment and interaction)
file(GLOB_WORLD_SOURCES "src/world/*.cpp")
add_library(nlm_world STATIC ${WORLD_SOURCES})
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core nlm_agent)

# Main executable with Phase 6 integration
add_executable(nlm "src/main.cpp")
target_link_libraries(nlm PUBLIC nlm_core nlm_phase6)
set_target_properties(nlm PROPERTIES
    OUTPUT_NAME "nlm"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Demo executables
add_executable(nlm_phase3_demo "src/experiments/Phase3Demo.cpp")
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
set_target_properties(nlm_phase3_demo PROPERTIES OUTPUT_NAME "nlm_phase3_demo")

add_executable(nlm_phase4_demo "src/experiments/Phase4Demo.cpp")
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
set_target_properties(nlm_phase4_demo PROPERTIES OUTPUT_NAME "nlm_phase4_demo")

add_executable(nlm_phase6_demo "src/experiments/Phase6Demo.cpp")
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6)
set_target_properties(nlm_phase6_demo PROPERTIES OUTPUT_NAME "nlm_phase6_demo")

# Testing setup
enable_testing()

# Include test directories
add_subdirectory(tests)

# Include benchmarks directory (optional)
if(NLM_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

# Installation configuration
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES "src/core/Logger/Logger.hpp" DESTINATION include/nlm/core)
install(FILES "src/core/Config/Config.hpp" DESTINATION include/nlm/core)
install(FILES "src/brain/Brain.hpp" DESTINATION include/nlm)

# Package configuration for find_package support
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMPackageConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    INSTALL_DESTINATION lib/cmake/NLM
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    DESTINATION lib/cmake/NLM
)

# Build options
option(NLM_BUILD_TESTS "Build tests" ON)
option(NLM_BUILD_BENCHMARKS "Build benchmarks" ON)
option(NLM_BUILD_DOCS "Build documentation" OFF)
option(NLM_ENABLE_PERF_OPTIMIZATIONS "Enable performance optimizations" ON)
option(NLM_ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(NLM_ENABLE_SANITIZERS "Enable memory sanitizers" OFF)

# Performance optimizations
if(NLM_ENABLE_PERF_OPTIMIZATIONS)
    target_compile_options(nlm_core PRIVATE -flto -fomit-frame-pointer -march=native -ffast-math)
    target_link_options(nlm_core PRIVATE -flto -fuse-ld=gold)
endif()

# Coverage reporting
if(NLM_ENABLE_COVERAGE)
    target_compile_options(nlm_core PRIVATE --coverage)
    target_link_options(nlm_core PRIVATE --coverage)
endif()

# Memory sanitizers
if(NLM_ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(nlm_core PRIVATE -fsanitize=address,undefined)
    target_link_options(nlm_core PRIVATE -fsanitize=address,undefined)
endif()

# Summary message
message(STATUS "NLM Build Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Core library sources: ${NLM_SOURCES}")
message(STATUS "  Build tests: ${NLM_BUILD_TESTS}")
message(STATUS "  Benchmarks: ${NLM_BUILD_BENCHMARKS}")
message(STATUS "  Performance optimizations: ${NLM_ENABLE_PERF_OPTIMIZATIONS}")
message(STATUS "  Coverage reporting: ${NLM_ENABLE_COVERAGE}")
message(STATUS "  Memory sanitizers: ${NLM_ENABLE_SANITIZERS}")

# IDE integration
if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "nlm")
endif()

# Git integration for development workflow
find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --always
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_DESCRIBE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(GIT_DESCRIBE)
        message(STATUS "Git tag: ${GIT_DESCRIBE}")
    endif()
endif()