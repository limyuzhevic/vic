cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Build Python bindings directly
pybind11_add_module(pynlm python/bindings.cpp)
target_include_directories(pynlm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

# Create a simple test program
test_executable("test_bindings" "tests/test_brain.cpp")