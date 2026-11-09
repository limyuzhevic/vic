cmake_minimum_required(VERSION 3.16)
project(pynlm LANGUAGES CXX)

# Find Python and pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create the pynlm module
add_library(pynlm MODULE
    bindings.cpp
)

target_include_directories(pynlm PRIVATE
    ${PYTHON_INCLUDE_DIRS}
    ${pybind11_INCLUDE_DIRS}
    ../src/core
    ../src/brain
    ../src/agent
    ../src/cognition
    ../src/neuromodulation
    ../src/environment
    ../src/world
)

target_link_libraries(pynlm PRIVATE
    pybind11::module
)

# Set module properties
set_target_properties(pynlm PROPERTIES
    OUTPUT_NAME "pynlm"
    PREFIX ""
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(pynlm PRIVATE "PYBIND11_MODULE_EXPORT_SYM=__declspec(dllexport)")
endif()

# Installation
install(TARGETS pynlm DESTINATION ${PYTHON_SITE_PACKAGES})
