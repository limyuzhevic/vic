# CMakeLists.txt for the improvements test
# This builds and runs the improvement tests

cmake_minimum_required(VERSION 3.16)
project(NLM_IMPROVEMENTS_TEST VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." )  # We are in a test directory, source is in parent

# Find the core NLM library
find_library(NLM_CORE_LIB nlm_core PATHS "${CMAKE_CURRENT_SOURCE_DIR}/../build" "${CMAKE_CURRENT_SOURCE_DIR}/../../build")

# Include directories
target_include_directories(NLM_IMPROVEMENTS_TEST
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
        "${CMAKE_CURRENT_SOURCE_DIR}/../src"
)

# Test executable
add_executable(test_improvements
    test_improvements.cpp
)

# Link with NLM core library
target_link_libraries(test_improvements PRIVATE nlm_core)

# Add test target
add_test(NAME improvements_test COMMAND test_improvements)

# Installation (optional)
install(TARGETS test_improvements RUNTIME DESTINATION bin)
