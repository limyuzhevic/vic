cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find JSON and YAML libraries
find_package(nlohmann_json REQUIRED)
find_package(yaml-cpp REQUIRED)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Performance optimization options

# Enable/disable debug symbols
option(NLM_ENABLE_DEBUG "Enable debug symbols and assertions" OFF)

# SIMD optimization
option(NLM_ENABLE_SSE "Enable SSE instruction set optimizations" ON)
option(NLM_ENABLE_AVX "Enable AVX instruction set optimizations (overrides SSE)" OFF)
option(NLM_ENABLE_NEON "Enable NEON instruction set optimizations (ARM)" OFF)

# Memory optimization
option(NLM_ENABLE_MEMORY_POOLS "Enable memory pools for better allocation performance" ON)
option(NLM_ENABLE_SPARSE_CONNECTIVITY "Enable sparse connectivity for large networks" ON)

# Parallel processing
option(NLM_ENABLE_OPENMP "Enable OpenMP parallel processing" OFF)
option(NLM_ENABLE_TBB "Enable Intel TBB parallel processing" OFF)

# Feature toggles
option(NLM_ENABLE_VISUALIZATION "Enable visualization support" OFF)
option(NLM_ENABLE_PERFORMANCE_METRICS "Enable detailed performance metrics" ON)

# Optimization level for Release builds
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -ffast-math" CACHE STRING "C++ compiler flags for Release build")

# Optimization level for Debug builds  
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG" CACHE STRING "C++ compiler flags for Debug build")

# Apply performance flags based on options
if(NLM_ENABLE_DEBUG)
    message(STATUS "Debug build enabled with symbols and assertions")
    add_compile_options(-g -O0 -DDEBUG)
else()
    message(STATUS "Release build enabled with optimizations")
    add_compile_options(${CMAKE_CXX_FLAGS_RELEASE})
endif()

if(NLM_ENABLE_SSE AND NOT NLM_ENABLE_AVX AND NOT NLM_ENABLE_NEON)
    add_compile_options(-msse -msse2 -msse3 -mssse3 -mavx -mavx2 -mpclmul -maes)
    message(STATUS "SSE/AVX optimizations enabled")
endif()

if(NLM_ENABLE_AVX)
    add_compile_options(-mavx -mavx2 -mpclmul -maes)
    message(STATUS "AVX optimizations enabled")
endif()

if(NLM_ENABLE_NEON)
    add_compile_options(-mfpu=neon -mvectorize)
    message(STATUS "NEON optimizations enabled")
endif()

if(NLM_ENABLE_OPENMP)
    find_package(OpenMP REQUIRED)
    if(OPENMP_FOUND)
        target_link_libraries(nlm_core PUBLIC OpenMP::OpenMP_CXX)
        message(STATUS "OpenMP parallel processing enabled")
    endif()
endif()

if(NLM_ENABLE_TBB)
    find_package(TBB REQUIRED)
    if(TBB_FOUND)
        target_link_libraries(nlm_core PUBLIC TBB::tbb)
        message(STATUS "TBB parallel processing enabled")
    endif()
endif()

# Memory pool optimization flags
if(NLM_ENABLE_MEMORY_POOLS)
    add_compile_options(-fipa-pta -fgcse -floop-interchange -ftree-loop-distribution)
    message(STATUS "Memory pools optimization enabled")
endif()

if(NLM_ENABLE_SPARSE_CONNECTIVITY)
    add_compile_options(-fipa-pta -fkeyword-abi-only)
    message(STATUS "Sparse connectivity optimization enabled")
endif()

# Performance profiling
option(NLM_ENABLE_PROFILING "Enable performance profiling" OFF)
if(NLM_ENABLE_PROFILING)
    add_compile_options(-pg -g)
    link_libraries(-pg -g)
    message(STATUS "Performance profiling enabled")
endif()

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo nlm_phase6_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
