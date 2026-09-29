# Python bindings CMakeLists.txt
# This file builds the Python bindings for NLM
# It assumes the core libraries (nlm_core, nlm_agent, nlm_world) are already built
# Note: In a real setup, this should be part of the main CMake project
# or use add_subdirectory to share the same build context

cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Use the source directory from the parent project
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Python bindings depend on the core libraries - assume they are already built
# Note: In practice, you'd want to either:
# 1. Use add_subdirectory(../) to build everything in one CMake project
# 2. Or use find_package to locate the already-built libraries

pybind11_add_module(pynlm bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)