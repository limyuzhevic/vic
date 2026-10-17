# Benchmarks CMakeLists.txt
# Build system for NLM benchmarks

cmake_minimum_required(VERSION 3.16)
project(NLMBenchmarks VERSION 0.1.0 LANGUAGES CXX)

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

# Find NLM core library
find_package(nlm_core REQUIRED)

# Add benchmark executable
add_executable(nlm_benchmarks
    Benchmarks.cpp
)

target_link_libraries(nlm_benchmarks PUBLIC nlm_core)

# Add benchmark to tests
enable_testing()
add_test(NAME nlm_benchmarks
    COMMAND nlm_benchmarks
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Install benchmarks (optional)
install(TARGETS nlm_benchmarks
    RUNTIME DESTINATION bin
)