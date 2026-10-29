cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Use the already-built NLM core library from the main project
# This eliminates source duplication and ensures consistency

pybind11_add_module(pynlm bindings.cpp)

target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)

target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Apply platform-specific compiler options
if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

# Add Python-specific optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(pynlm PRIVATE $<$<CXX_COMPILER_ID:GNU,Clang>:-O3>)
    target_compile_options(pynlm PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/O2>)
endif()

# Installation
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

# Generate Python package metadata
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml.in
    ${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml
    @ONLY
)

# Print build summary
message(STATUS "Python bindings build configuration:")
message(STATUS "  Python executable: ${Python_EXECUTABLE}")
message(STATUS "  pybind11 version: ${pybind11_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")