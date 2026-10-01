cmake_minimum_required(VERSION 3.16)

project(pynlm_bindings LANGUAGES CXX)

# Find Python and pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

# Include the main project
add_subdirectory(..)

# Create Python module
pybind11_add_module(pynlm bindings.cpp)

target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Compiler-specific optimizations
if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
else()
    target_compile_options(pynlm PRIVATE -std=c++20 -O2 -DNDEBUG)
endif()

# Installation
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)