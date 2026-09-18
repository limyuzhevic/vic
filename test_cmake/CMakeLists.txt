cmake_minimum_required(VERSION 3.16)
project(NLMBuildTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Test build with NLM core
include_directories(../src)

add_executable(test_build test_build.cpp)
target_link_libraries(test_build nlm_core)
