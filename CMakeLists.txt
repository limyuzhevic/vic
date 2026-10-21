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

# Performance optimizations for NLM brain simulation
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Enable compiler optimizations
    add_compile_options(-O3)
    
    # Optimize for cache locality and vectorization
    add_compile_options(-march=native)
    add_compile_options(-mtune=native)
    
    # Enable loop optimizations
    add_compile_options(-funroll-loops)
    add_compile_options(-fomit-frame-pointer)
    
    # Vectorization optimizations
    add_compile_options(-ffast-math)
    add_compile_options(-ftree-vectorize)
    
    # Compiler-specific optimizations
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        add_compile_options(-falign-functions=16)
        add_compile_options(-falign-loops=16)
        add_compile_options(-fstrict-aliasing)
        add_compile_options(-fexpensive-optimizations)
    endif()
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core library - only include files that exist
add_library(nlm_core STATIC
    src/brain/Brain.cpp
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/dynamics/SpikeSystem.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/PlasticityRule.cpp
    src/plasticity/StructuralPlasticity.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Main executable
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)