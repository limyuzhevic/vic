# NLM Build Configuration Module
# Comprehensive build-time configuration for NLM

# CMake version requirements
cmake_minimum_required(VERSION 3.16)

# NLM Build Configuration Module
# Provides centralized configuration options for NLM build system

# ==============================================================================
# BUILD TYPE CONFIGURATION
# ==============================================================================

# Set available build types
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel" CACHE STRING "Available build types")

# Default build type if none specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# ==============================================================================
# FEATURE OPTIONS
# ==============================================================================

# SIMD (Single Instruction, Multiple Data) optimizations
option(NLM_ENABLE_SIMD "Enable SIMD optimizations (AVX2, SSE, etc.)" ON)

# OpenMP parallelization
option(NLM_ENABLE_OPENMP "Enable OpenMP parallelization" ON)

# CUDA acceleration
option(NLM_ENABLE_CUDA "Enable CUDA acceleration (requires CUDA toolkit)" OFF)

# MPI parallelization
option(NLM_ENABLE_MPI "Enable MPI parallelization" OFF)

# Profiling support
option(NLM_ENABLE_PROFILING "Enable profiling support (gprof, perf)" OFF)

# Code coverage
option(NLM_ENABLE_COVERAGE "Enable code coverage reporting" OFF)

# Memory sanitizers for debugging
option(NLM_ENABLE_SANITIZERS "Enable memory error sanitizers" OFF)

# AddressSanitizer
option(NLM_ENABLE_ASAN "Enable AddressSanitizer for memory error detection" OFF)

# UndefinedBehaviorSanitizer
option(NLM_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer for undefined behavior detection" OFF)

# ==============================================================================
# PERFORMANCE OPTIONS
# ==============================================================================

# Optimization level (0-3)
set(NLM_BUILD_OPTIMIZATION_LEVEL "2" CACHE STRING "Optimization level (0-3)")

# Treat warnings as errors
option(NLM_BUILD_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

# Verbose build logging
option(NLM_BUILD_VERBOSE_LOGGING "Enable verbose build logging" OFF)

# ==============================================================================
# DEPENDENCY MANAGEMENT
# ==============================================================================

# Function to validate and find CUDA
function(validate_cuda)
    if(NLM_ENABLE_CUDA)
        find_package(CUDA REQUIRED)
        message(STATUS "CUDA support enabled")
    endif()
endfunction()

# Function to validate and find MPI
function(validate_mpi)
    if(NLM_ENABLE_MPI)
        find_package(MPI REQUIRED)
        message(STATUS "MPI support enabled")
    endif()
endfunction()

# Function to validate and find OpenMP
function(validate_openmp)
    if(NLM_ENABLE_OPENMP)
        find_package(OpenMP REQUIRED)
        message(STATUS "OpenMP support enabled")
    endif()
endfunction()

# Function to validate dependencies
function(validate_dependencies)
    validate_cuda()
    validate_mpi()
    validate_openmp()
endfunction()

# ==============================================================================
# PLATFORM DETECTION AND OPTIMIZATIONS
# ==============================================================================

# Detect platform
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(NLM_PLATFORM "Linux" CACHE STRING "Target platform")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(NLM_PLATFORM "Windows" CACHE STRING "Target platform")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(NLM_PLATFORM "MacOS" CACHE STRING "Target platform")
else()
    set(NLM_PLATFORM "Unknown" CACHE STRING "Target platform")
endif()

# Platform-specific optimizations
function(apply_platform_optimizations)
    if(NLM_PLATFORM STREQUAL "Linux")
        add_compile_options(-march=native -mtune=native)
        if(NLM_ENABLE_OPENMP)
            target_link_libraries(nlm_core PUBLIC OpenMP::OpenMP_CXX)
        endif()
    elseif(NLM_PLATFORM STREQUAL "Windows")
        if(MSVC)
            add_compile_options(/permissive- /arch:AVX2)
        endif()
    elseif(NLM_PLATFORM STREQUAL "MacOS")
        add_compile_options(-march=native)
    endif()
endfunction()

# ==============================================================================
# SIMD CONFIGURATION
# ==============================================================================

function(configure_simd)
    if(NLM_ENABLE_SIMD)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            add_compile_options(-mavx2 -mfma)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma")
        elseif(MSVC)
            add_compile_options(/arch:AVX2)
        endif()
        add_definitions(-DNLM_ENABLE_SIMD=1)
        message(STATUS "SIMD optimizations enabled")
    endif()
endfunction()

# ==============================================================================
# SANITIZER CONFIGURATION
# ==============================================================================

function(configure_sanitizers)
    if(NLM_ENABLE_SANITIZERS)
        if(NLM_ENABLE_ASAN)
            add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
            add_link_options(-fsanitize=address)
        endif()
        if(NLM_ENABLE_UBSAN)
            add_compile_options(-fsanitize=undefined -fno-sanitize=alignment)
            add_link_options(-fsanitize=undefined)
        endif()
        message(STATUS "Memory sanitizers enabled")
    endif()
endfunction()

# ==============================================================================
# BUILD TYPE CONFIGURATION
# ==============================================================================

function(configure_build_type)
    string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_UPPER)
    if(BUILD_TYPE_UPPER STREQUAL "DEBUG")
        add_compile_options(-g -O0 -DDEBUG)
        if(NLM_ENABLE_COVERAGE)
            add_compile_options(--coverage)
            add_link_options(--coverage)
        endif()
    elseif(BUILD_TYPE_UPPER STREQUAL "RELEASE")
        add_compile_options(-O3 -DNDEBUG)
        if(NLM_ENABLE_PROFILING)
            add_compile_options(-pg)
            add_link_options(-pg)
        endif()
    elseif(BUILD_TYPE_UPPER STREQUAL "RELWITHDEBINFO")
        add_compile_options(-O2 -g -DNDEBUG)
    elseif(BUILD_TYPE_UPPER STREQUAL "MINSIZEREL")
        add_compile_options(-Os -DNDEBUG)
    endif()
endfunction()

# ==============================================================================
# CONFIGURATION FILE GENERATION
# ==============================================================================

# Set output directory for generated configuration files
set(NLM_CONFIG_OUTPUT_DIR "${CMAKE_BINARY_DIR}/config" CACHE PATH "Output directory for generated config files")
file(MAKE_DIRECTORY "${NLM_CONFIG_OUTPUT_DIR}")

# Generate build configuration header
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/nlm_build_config.h.in"
    "${NLM_CONFIG_OUTPUT_DIR}/nlm_build_config.h"
)

# Generate feature configuration header
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/nlm_features.h.in"
    "${NLM_CONFIG_OUTPUT_DIR}/nlm_features.h"
)

# Generate CMake configuration summary
function(generate_build_summary)
    file(WRITE "${NLM_CONFIG_OUTPUT_DIR}/build_summary.txt" "")
    file(APPEND "${NLM_CONFIG_OUTPUT_DIR}/build_summary.txt" "NLM Build Configuration Summary\n")
    file(APPEND "${NLM_CONFIG_OUTPUT_DIR}/build_summary.txt" "===================================\n")
    file(APPEND "${NLM_CONFIG_OUTPUT_DIR}/build_summary.txt" "Build Type: ${CMAKE_BUILD_TYPE}\n")
    file(APPEND "${NLM_CONFIG_OUTPUT_DIR}/build_summary.txt" "Platform: ${NLM_PLATFORM}\n")
    file(APPEND "${NLM_CONFIG_OUTPUT_DIR}/build_summary.txt" "SIMD: ${NLM_ENABLE_SIMD}\n")
    file(APPEND "${NLM_CONFIG_OUTPUT_DIR}/build_summary.txt" "OpenMP: ${NLM_ENABLE_OPENMP}\n")
    file(APPEND "${NLM_CONFIG_OUTPUT_DIR}/build_summary.txt" "CUDA: ${NLM_ENABLE_CUDA}\n")
    file(APPEND "${NLM_CONFIG_OUTPUT_DIR}/build_summary.txt" "MPI: ${NLM_ENABLE_MPI}\n")
    file(APPEND "${NLM_CONFIG_OUTPUT_DIR}/build_summary.txt" "Profiling: ${NLM_ENABLE_PROFILING}\n")
    file(APPEND "${NLM_CONFIG_OUTPUT_DIR}/build_summary.txt" "Coverage: ${NLM_ENABLE_COVERAGE}\n")
    file(APPEND "${NLM_CONFIG_OUTPUT_DIR}/build_summary.txt" "Sanitizers: ${NLM_ENABLE_SANITIZERS}\n")
endfunction()

# ==============================================================================
# VALIDATION AND FINALIZATION
# ==============================================================================

function(finalize_configuration)
    # Apply platform optimizations
    apply_platform_optimizations()
    
    # Configure SIMD
    configure_simd()
    
    # Configure sanitizers
    configure_sanitizers()
    
    # Configure build type
    configure_build_type()
    
    # Generate build summary
    generate_build_summary()
    
    # Validate warnings as errors
    if(NLM_BUILD_WARNINGS_AS_ERRORS)
        if(MSVC)
            add_compile_options(/WX)
        else()
            add_compile_options(-Werror)
        endif()
    endif()
    
    # Log configuration summary
    if(NLM_BUILD_VERBOSE_LOGGING)
        message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
        message(STATUS "Platform: ${NLM_PLATFORM}")
        message(STATUS "Features: SIMD=${NLM_ENABLE_SIMD}, OpenMP=${NLM_ENABLE_OPENMP}, CUDA=${NLM_ENABLE_CUDA}")
        message(STATUS "Performance: Optimization level=${NLM_BUILD_OPTIMIZATION_LEVEL}")
        message(STATUS "Debug: Warnings as errors=${NLM_BUILD_WARNINGS_AS_ERRORS}, Sanitizers=${NLM_ENABLE_SANITIZERS}")
    endif()
endfunction()