# NLM Python Bindings - Enhanced CMake Configuration
# Modern CMake configuration for pybind11 integration with better
# dependency management and cross-platform support.

cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Python bindings for NLM (Neural Learning Machine)"
)

# Set minimum Python version
set(PYTHON_MODULE_VERSION 3.8)

# Find Python interpreter and development components
find_package(Python ${PYTHON_MODULE_VERSION} REQUIRED COMPONENTS Interpreter Development)

# Try to find pybind11 - prefer CONFIG mode for modern usage
find_package(pybind11 CONFIG REQUIRED)
if(NOT pybind11_FOUND)
    # Fallback to module mode if CONFIG not available
    find_package(pybind11 REQUIRED)
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Output directories for Python module
set(PYTHON_MODULE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/python_module)
set(PYTHON_MODULE_INSTALL_DIR ${PYTHON_MODULE_OUTPUT_DIR})

# Configure Python module properties
pybind11_add_module(pynlm
    bindings.cpp
    src/bindings/PyBrain.cpp
    src/bindings/PyConfig.cpp
    src/bindings/PyNeuron.cpp
    src/bindings/PySynapse.cpp
    src/bindings/PyBrain.cpp
    src/bindings/PyEnvironment.cpp
    src/bindings/PyAgent.cpp
    src/bindings/PyWorld.cpp
)

# Target include directories
target_include_directories(pynlm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${pybind11_INCLUDE_DIR}
)

# Link against NLM core libraries
target_link_libraries(pynlm PRIVATE
    nlm_core
    nlm_agent
    nlm_world
)

# Python module specific compile options
target_compile_options(pynlm PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Platform-specific optimizations
if(WIN32)
    target_compile_options(pynlm PRIVATE /WX)
    target_link_options(pynlm PRIVATE /SUBSYSTEM:WINDOWS)
elseif(APPLE)
    target_link_options(pynlm PRIVATE -undefined dynamic_lookup)
elseif(UNIX)
    target_link_options(pynlm PRIVATE -fPIC)
endif()

# Python module configuration
pybind11_setup_pybind()

# Installation configuration
install(TARGETS pynlm
    LIBRARY DESTINATION ${PYTHON_MODULE_INSTALL_DIR}
    ARCHIVE DESTINATION ${PYTHON_MODULE_INSTALL_DIR}
    RUNTIME DESTINATION ${PYTHON_MODULE_INSTALL_DIR}
)

# Package data configuration
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../configs/default.cfg
    DESTINATION ${PYTHON_MODULE_INSTALL_DIR}/configs
)

# Create a setup.py helper for Python installation
set(SETUP_PY_CONTENT
"from setuptools import setup, find_packages
from setuptools.extension import Extension
from pybind11.setup_helpers import Pybind11Extension, build_ext
import os

# Get the pybind11 include directory
try:
    import pybind11
    pybind11_include = pybind11.get_include()
except ImportError:
    pybind11_include = ""

# Define the extension module
ext_modules = [
    Pybind11Extension(
        'pynlm._core',
        sorted(glob.glob('lib/*.so') + glob.glob('lib/*.pyd')),
        include_dirs=[
            '${CMAKE_CURRENT_SOURCE_DIR}/../src',
            pybind11_include,
        ],
        cxx_std=20,
        extra_compile_args=['-O3'],
        language='c++',
    ),
]

setup(
    name='pynlm',
    version='${PROJECT_VERSION}',
    author='NLM Authors',
    description='Python bindings for NLM (Neural Learning Machine)',
    long_description='',
    packages=find_packages(),
    ext_modules=ext_modules,
    install_requires=[],
    setup_requires=['pybind11>=2.11.0'],
    python_requires='>=3.8',
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
        'Programming Language :: C++',
    ],
    zip_safe=False,
)
")

# Write setup.py to build directory
file(WRITE ${CMAKE_BINARY_DIR}/setup.py "${SETUP_PY_CONTENT}")

# Create Python package structure
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/pynlm)

# Create __init__.py for the package
set(INIT_PY_CONTENT
"import sys
import os

# Add the built module to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import core functionality
try:
    from ._core import *
    __all__ = ['createBrain', 'createDefaultConfig', 'Brain', 'Config']
except ImportError as e:
    # Fallback for testing or development
    import warnings
    warnings.warn(f"Failed to import pynlm core module: {e}")
    # Define placeholder classes for development
    class Brain:
        def __init__(self, config=None):
            pass
    class Config:
        def __init__(self):
            pass
    def createBrain(config=None):
        return Brain(config)
    def createDefaultConfig():
        return Config()

# Define package metadata
__version__ = '${PROJECT_VERSION}'
__author__ = 'NLM Authors'
__description__ = 'Python bindings for NLM (Neural Learning Machine)'

# Package metadata
__all__ = ['__version__', '__author__', '__description__', '__all__']
")

# Write __init__.py
file(WRITE ${CMAKE_BINARY_DIR}/pynlm/__init__.py "${INIT_PY_CONTENT}")

# Create a simpler import for testing and development
set(C_TEST_HELPER_CONTENT
"// Helper functions for C++ testing of Python bindings
#ifndef NLM_PYTHON_TEST_HELPER_H
#define NLM_PYTHON_TEST_HELPER_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>
#include <pybind11/eigen.h>
#include <pybind11/numpy.h>
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"

namespace py = pybind11;

// Type conversion helpers
inline py::array_t<float> to_numpy_array(const std::vector<float>& vec) {
    py::array_t<float> array(vec.size());
    auto buffer = array.request();
    float* ptr = static_cast<float*>(buffer.ptr);
    std::copy(vec.begin(), vec.end(), ptr);
    return array;
}

inline std::vector<float> from_numpy_array(py::array_t<float> array) {
    auto buffer = array.request();
    float* ptr = static_cast<float*>(buffer.ptr);
    size_t size = buffer.size;
    return std::vector<float>(ptr, ptr + size);
}

// Python exception handling
class ConfigValidationError : public std::runtime_error {
public:
    explicit ConfigValidationError(const std::string& msg) : std::runtime_error(msg) {}
};

#endif // NLM_PYTHON_TEST_HELPER_H
")

# Write C test helper
file(WRITE ${CMAKE_BINARY_DIR}/include/nlm_python_test_helper.h "${C_TEST_HELPER_CONTENT}")

# Configure module for development and testing
set_target_properties(pynlm PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    OUTPUT_NAME "_core"
    PREFIX ""
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
)

# Add custom target for building Python module
add_custom_target(pynlm_python
    COMMAND ${Python_EXECUTABLE} ${CMAKE_BINARY_DIR}/setup.py build_ext --inplace
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Building Python module for NLM"
    VERBATIM
)

# Add dependency check for cross-compilation
if(CMAKE_CROSSCOMPILING)
    message(WARNING "Cross-compiling Python bindings - ensure Python development headers are available")
endif()

# Generate Python module metadata
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/pynlmConfig.cmake.in
    ${CMAKE_BINARY_DIR}/pynlmConfig.cmake
    @ONLY
)

# Export Python module configuration
export(EXPORT pynlmTargets
    FILE ${CMAKE_BINARY_DIR}/pynlmTargets.cmake
)

message(STATUS "Python bindings configuration:")
message(STATUS "  Python executable: ${Python_EXECUTABLE}")
message(STATUS "  Python include dir: ${Python_INCLUDE_DIRS}")
message(STATUS "  Python library dir: ${Python_LIBRARIES}")
message(STATUS "  pybind11 include: ${pybind11_INCLUDE_DIR}")
message(STATUS "  Module output: ${PYTHON_MODULE_OUTPUT_DIR}")
