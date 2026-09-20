# ==============================================================================
# NLM Build Configuration Templates
# Templates for different build scenarios and environments
# ==============================================================================

# Development Build Template
# Used for active development and testing
# cmake -S . -B build -D BUILD_TEMPLATE=development
# ==============================================================================

set(BUILD_TEMPLATE "development" CACHE STRING "Build template (development/testing/production)")

if(BUILD_TEMPLATE STREQUAL "development")
    # Development-specific settings
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type for development")
    set(NLM_ENABLE_COVERAGE ON CACHE BOOL "Enable code coverage in development")
    set(NLM_ENABLE_SANITIZERS ON CACHE BOOL "Enable memory sanitizers in development")
    set(NLM_ENABLE_ASAN ON CACHE BOOL "Enable AddressSanitizer in development")
    set(NLM_ENABLE_UBSAN ON CACHE BOOL "Enable UndefinedBehaviorSanitizer in development")
    set(NLM_BUILD_WARNINGS_AS_ERRORS OFF CACHE BOOL "Warnings as errors in development")
    set(NLM_BUILD_VERBOSE_LOGGING ON CACHE BOOL "Verbose logging in development")
    set(NLM_ENABLE_PROFILING OFF CACHE BOOL "Profiling disabled in development")
    
    # Development-specific optimization
    add_compile_options(-g -O0 -DDEBUG)
    message(STATUS "Development build template configured")
endif()

# Testing Build Template
# Used for continuous integration and testing environments
# cmake -S . -B build -D BUILD_TEMPLATE=testing
# ==============================================================================

if(BUILD_TEMPLATE STREQUAL "testing")
    # Testing-specific settings
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Build type for testing")
    set(NLM_ENABLE_COVERAGE ON CACHE BOOL "Enable code coverage for testing")
    set(NLM_ENABLE_SANITIZERS ON CACHE BOOL "Enable memory sanitizers for testing")
    set(NLM_ENABLE_ASAN ON CACHE BOOL "Enable AddressSanitizer for testing")
    set(NLM_ENABLE_UBSAN ON CACHE BOOL "Enable UndefinedBehaviorSanitizer for testing")
    set(NLM_BUILD_WARNINGS_AS_ERRORS ON CACHE BOOL "Warnings as errors for testing")
    set(NLM_BUILD_VERBOSE_LOGGING OFF CACHE BOOL "Verbose logging disabled for testing")
    set(NLM_ENABLE_PROFILING ON CACHE BOOL "Enable profiling for performance testing")
    
    # Testing-specific flags
    add_compile_options(-O2 -g -DNDEBUG)
    
    # Enable testing targets
    enable_testing()
    message(STATUS "Testing build template configured")
endif()

# Production Build Template
# Used for final release builds
# cmake -S . -B build -D BUILD_TEMPLATE=production
# ==============================================================================

if(BUILD_TEMPLATE STREQUAL "production")
    # Production-specific settings
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type for production")
    set(NLM_ENABLE_COVERAGE OFF CACHE BOOL "Coverage disabled for production")
    set(NLM_ENABLE_SANITIZERS OFF CACHE BOOL "Sanitizers disabled for production")
    set(NLM_ENABLE_ASAN OFF CACHE BOOL "AddressSanitizer disabled for production")
    set(NLM_ENABLE_UBSAN OFF CACHE BOOL "UndefinedBehaviorSanitizer disabled for production")
    set(NLM_BUILD_WARNINGS_AS_ERRORS ON CACHE BOOL "Warnings as errors for production")
    set(NLM_BUILD_VERBOSE_LOGGING OFF CACHE BOOL "Verbose logging disabled for production")
    set(NLM_ENABLE_PROFILING OFF CACHE BOOL "Profiling disabled for production")
    set(NLM_ENABLE_CUDA OFF CACHE BOOL "CUDA disabled for production (use build-specific)")
    set(NLM_ENABLE_MPI OFF CACHE BOOL "MPI disabled for production (use build-specific)")
    
    # Production-specific optimizations
    add_compile_options(-O3 -DNDEBUG)
    
    # Strip debug information
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s" CACHE STRING "Production flags")
    message(STATUS "Production build template configured")
endif()

# Performance Build Template
# Used for high-performance computing builds
# cmake -S . -B build -D BUILD_TEMPLATE=performance
# ==============================================================================

if(BUILD_TEMPLATE STREQUAL "performance")
    # Performance-specific settings
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Build type for performance")
    set(NLM_ENABLE_COVERAGE OFF CACHE BOOL "Coverage disabled for performance")
    set(NLM_ENABLE_SANITIZERS OFF CACHE BOOL "Sanitizers disabled for performance")
    set(NLM_BUILD_WARNINGS_AS_ERRORS OFF CACHE BOOL "Warnings as errors disabled for performance")
    set(NLM_BUILD_VERBOSE_LOGGING OFF CACHE BOOL "Verbose logging disabled for performance")
    set(NLM_ENABLE_PROFILING ON CACHE BOOL "Enable profiling for performance analysis")
    
    # Enable performance features
    set(NLM_ENABLE_SIMD ON CACHE BOOL "SIMD enabled for performance")
    set(NLM_ENABLE_OPENMP ON CACHE BOOL "OpenMP enabled for performance")
    
    # Performance-specific optimizations
    add_compile_options(-O3 -march=native -mtune=native -DNDEBUG)
    
    # MPI for distributed computing
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(NLM_ENABLE_MPI ON CACHE BOOL "MPI enabled for performance")
    endif()
    
    message(STATUS "Performance build template configured")
endif()

# Cross-compilation Support
# Support for building on different platforms
# ==============================================================================

# Set CMAKE_TOOLCHAIN_FILE for cross-compilation
if(DEFINED ENV{CROSS_COMPILE})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{CROSS_COMPILE}-gcc.cmake" CACHE FILE "Toolchain file for cross-compilation")
endif()

# Architecture-specific settings
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64|ARM")
    set(NLM_ARCHITECTURE "ARM" CACHE STRING "Target architecture")
    add_compile_options(-march=armv8-a)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86|AMD64|amd64")
    set(NLM_ARCHITECTURE "x86_64" CACHE STRING "Target architecture")
    add_compile_options(-march=x86-64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "ppc|PPC")
    set(NLM_ARCHITECTURE "POWERPC" CACHE STRING "Target architecture")
    add_compile_options(-mcpu=powerpc64)
else()
    set(NLM_ARCHITECTURE "GENERIC" CACHE STRING "Target architecture")
endif()

# Memory model configuration
if(NLM_ARCHITECTURE STREQUAL "x86_64")
    # x86_64 specific memory optimizations
    add_compile_options(-mlzcnt -mpopcnt -mavx512f)
elseif(NLM_ARCHITECTURE STREQUAL "ARM")
    # ARM specific optimizations
    add_compile_options(-mfpu=neon -mfp16-format=ieee)
endif()

# Configuration validation
function(validate_build_configuration)
    message(STATUS "Validating build configuration...")
    
    # Check for conflicting options
    if(NLM_ENABLE_ASAN AND NLM_ENABLE_COVERAGE)
        message(WARNING "AddressSanitizer and coverage both enabled - may impact performance")
    endif()
    
    if(NLM_ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "RELEASE")
        message(WARNING "Sanitizers enabled in release build - may impact performance")
    endif()
    
    # Validate optimization level
    if(NLM_BUILD_OPTIMIZATION_LEVEL GREATER 3)
        message(FATAL_ERROR "Optimization level must be between 0 and 3")
    endif()
    
    # Check for required dependencies
    if(NLM_ENABLE_CUDA AND NOT CUDA_FOUND)
        message(FATAL_ERROR "CUDA requested but CUDA toolkit not found")
    endif()
    
    if(NLM_ENABLE_MPI AND NOT MPI_FOUND)
        message(FATAL_ERROR "MPI requested but MPI not found")
    endif()
    
    message(STATUS "Build configuration validation complete")
endfunction()

# Generate build configuration summary
function(generate_build_summary)
    set(BUILD_SUMMARY_FILE "${CMAKE_BINARY_DIR}/BUILD_SUMMARY.txt")
    file(WRITE "${BUILD_SUMMARY_FILE}" "NLM Build Configuration Summary\n")
    file(APPEND "${BUILD_SUMMARY_FILE}" "===============================\n")
    file(APPEND "${BUILD_SUMMARY_FILE}" "Build Template: ${BUILD_TEMPLATE}\n")
    file(APPEND "${BUILD_SUMMARY_FILE}" "Build Type: ${CMAKE_BUILD_TYPE}\n")
    file(APPEND "${BUILD_SUMMARY_FILE}" "Platform: ${NLM_PLATFORM}\n")
    file(APPEND "${BUILD_SUMMARY_FILE}" "Architecture: ${NLM_ARCHITECTURE}\n")
    file(APPEND "${BUILD_SUMMARY_FILE}" "SIMD: ${NLM_ENABLE_SIMD}\n")
    file(APPEND "${BUILD_SUMMARY_FILE}" "OpenMP: ${NLM_ENABLE_OPENMP}\n")
    file(APPEND "${BUILD_SUMMARY_FILE}" "CUDA: ${NLM_ENABLE_CUDA}\n")
    file(APPEND "${BUILD_SUMMARY_FILE}" "MPI: ${NLM_ENABLE_MPI}\n")
    file(APPEND "${BUILD_SUMMARY_FILE}" "Profiling: ${NLM_ENABLE_PROFILING}\n")
    file(APPEND "${BUILD_SUMMARY_FILE}" "Coverage: ${NLM_ENABLE_COVERAGE}\n")
    file(APPEND "${BUILD_SUMMARY_FILE}" "Sanitizers: ${NLM_ENABLE_SANITIZERS}\n")
    file(APPEND "${BUILD_SUMMARY_FILE}" "ASAN: ${NLM_ENABLE_ASAN}\n")
    file(APPEND "${BUILD_SUMMARY_FILE}" "UBSAN: ${NLM_ENABLE_UBSAN}\n")
    file(APPEND "${BUILD_SUMMARY_FILE}" "Warnings as Errors: ${NLM_BUILD_WARNINGS_AS_ERRORS}\n")
    file(APPEND "${BUILD_SUMMARY_FILE}" "Verbose Logging: ${NLM_BUILD_VERBOSE_LOGGING}\n")
    file(APPEND "${BUILD_SUMMARY_FILE}" "Optimization Level: ${NLM_BUILD_OPTIMIZATION_LEVEL}\n")
    
    message(STATUS "Build summary generated at ${BUILD_SUMMARY_FILE}")
endfunction()

# Final configuration setup
function(finalize_build)
    # Validate configuration
    validate_build_configuration()
    
    # Generate build summary
    generate_build_summary()
    
    # Apply platform optimizations
    if(NLM_PLATFORM STREQUAL "Linux")
        add_compile_options(-march=native -mtune=native)
    elseif(NLM_PLATFORM STREQUAL "Windows")
        if(MSVC)
            add_compile_options(/permissive- /arch:AVX2)
        endif()
    elseif(NLM_PLATFORM STREQUAL "MacOS")
        add_compile_options(-march=native)
    endif()
    
    # Apply SIMD configuration
    if(NLM_ENABLE_SIMD)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            add_compile_options(-mavx2 -mfma)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma")
        elseif(MSVC)
            add_compile_options(/arch:AVX2)
        endif()
        add_definitions(-DNLM_ENABLE_SIMD=1)
    endif()
    
    # Apply sanitizer configuration
    if(NLM_ENABLE_SANITIZERS)
        if(NLM_ENABLE_ASAN)
            add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
            add_link_options(-fsanitize=address)
        endif()
        if(NLM_ENABLE_UBSAN)
            add_compile_options(-fsanitize=undefined -fno-sanitize=alignment)
            add_link_options(-fsanitize=undefined)
        endif()
    endif()
    
    # Apply build type configuration
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
    
    # Apply warnings configuration
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
        message(STATUS "Architecture: ${NLM_ARCHITECTURE}")
        message(STATUS "Features: SIMD=${NLM_ENABLE_SIMD}, OpenMP=${NLM_ENABLE_OPENMP}, CUDA=${NLM_ENABLE_CUDA}")
        message(STATUS "Performance: Optimization level=${NLM_BUILD_OPTIMIZATION_LEVEL}")
        message(STATUS "Debug: Warnings as errors=${NLM_BUILD_WARNINGS_AS_ERRORS}, Sanitizers=${NLM_ENABLE_SANITIZERS}")
    endif()
    
    message(STATUS "Build configuration finalized successfully")
endfunction()