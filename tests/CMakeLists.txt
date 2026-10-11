cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Build type detection for tests
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific test optimizations
if(MSVC)
    add_compile_options(/W4)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        add_compile_options(/Od /Zi /debug)
    elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        add_compile_options(/O2 /fp:fast)
    endif()
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        add_compile_options(-O0 -g -ggdb)
    elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        add_compile_options(-O3 -march=native)
    endif()
endif()

# Create test executable
add_executable(nlm_test
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
