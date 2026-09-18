cmake_minimum_required(VERSION 3.16)
project(NLMBuildTest VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add source directory
include_directories(../src)

# Test build
add_executable(test_build test_build.cpp)

# Link against NLM core library
target_link_libraries(test_build nlm_core)

# Set test output directory
set_target_properties(test_build PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add test to run
add_test(NAME nlm_build_test COMMAND test_build)

# Message for user
message(STATUS "NLM Build Test Configured")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Source directory: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Build directory: ${CMAKE_BINARY_DIR}")
