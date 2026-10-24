cmake_minimum_required(VERSION 3.16)

# Python bindings for NLM
# This builds the pynlm Python module as a pybind11 extension
# NOTE: This CMakeLists.txt assumes that the parent directory's CMakeLists.txt
# has already been processed and created the required nlm_core, nlm_agent, and nlm_world libraries.

# Find Python and pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Create the Python module extension
pybind11_add_module(pynlm bindings.cpp)

# Link with the core libraries (these must have been defined by the parent CMakeLists.txt)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)

# Include source directory for headers
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# MSVC-specific settings
if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

# Install the Python module
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

# Add tests if enabled
option(NLM_BUILD_PYTHON_TESTS "Build Python tests" OFF)
if(NLM_BUILD_PYTHON_TESTS)
    enable_testing()
    add_test(NAME pynlm_test COMMAND python -c "import pynlm; print('pynlm module loaded successfully')")
endif()
