# NLM Python Integration Module
# Provides Python integration and dependency management

# Find Python first
find_package(Python REQUIRED COMPONENTS Interpreter Development)

# Set pybind11_DIR to point to pybind11's CMake config
# This is for pybind11 installed via pip
if(NOT pybind11_DIR)
    set(pybind11_DIR "/usr/local/lib/python3.10/dist-packages/pybind11/share/cmake/pybind11")
endif()

# Include pybind11's CMake module
find_package(pybind11 REQUIRED)

# Verify pybind11 is available
if(TARGET pybind11::module)
    message(STATUS "Found pybind11: ${pybind11_VERSION}")
else()
    message(FATAL_ERROR "pybind11 not found. Python bindings cannot be enabled.")
endif()
