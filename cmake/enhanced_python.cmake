# Enhanced Python Bindings Configuration
# This CMake configuration adds advanced Python bindings to NLM

# Check for Python and pybind11
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

# Enhanced Python bindings configuration
set(NLM_PYTHON_BINDINGS_VERSION "2.0.0-enhanced")
set(NLM_PYTHON_BINDINGS_DESCRIPTION "Advanced Python bindings for NLM neural simulation framework")

# Python module target
pybind11_add_module(pynlm_enhanced
    python/bindings.cpp
    python/BindingsEnhancer.cpp
    python/AdvancedSensoryProcessor.cpp
    python/AdvancedVisualizationSystem.cpp
    python/AdvancedExperimentationFramework.cpp
    python/CommandLineTools.cpp
    python/MemoryManagementTools.cpp
    python/SerializationManager.cpp
    python/DebugTools.cpp
    python/main.cpp
)

target_include_directories(pynlm_enhanced PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/python
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(pynlm_enhanced PRIVATE
    nlm_core
    pybind11::module
)

# Set Python module properties
set_target_properties(pynlm_enhanced PROPERTIES
    OUTPUT_NAME "pynlm_enhanced"
    VERSION ${NLM_PYTHON_BINDINGS_VERSION}
    SOVERSION "2"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Install the enhanced Python module
install(TARGETS pynlm_enhanced
    LIBRARY DESTINATION ${Python3_SITEARCH}
    ARCHIVE DESTINATION ${Python3_ARCHIVEDIR}
    RUNTIME DESTINATION ${Python3_BINDIR}
)

# Create convenience target for building enhanced bindings
add_custom_target(build_enhanced_python
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target pynlm_enhanced
    COMMENT "Building enhanced Python bindings for NLM"
    VERBATIM
)

# Add test for enhanced Python bindings
enable_testing()

add_test(NAME test_enhanced_python_bindings
    COMMAND ${Python3_EXECUTABLE} -c "import pynlm_enhanced; print('Enhanced Python bindings loaded successfully')"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Create a configuration file for enhanced bindings
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config/enhanced_python_config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/enhanced_python_config.cmake
    @ONLY
)

# Documentation for enhanced Python bindings
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/enhanced_python_doc.md.in
    ${CMAKE_CURRENT_BINARY_DIR}/enhanced_python_doc.md
    @ONLY
)

# Summary of changes
message(STATUS "Enhanced Python Bindings Configuration:")
message(STATUS "  Version: ${NLM_PYTHON_BINDINGS_VERSION}")
message(STATUS "  Description: ${NLM_PYTHON_BINDINGS_DESCRIPTION}")
message(STATUS "  Python Module: pynlm_enhanced")
message(STATUS "  Source Files: ${CMAKE_CURRENT_SOURCE_DIR}/python/*.cpp")
message(STATUS "  Headers: ${CMAKE_CURRENT_SOURCE_DIR}/python/*.hpp")
message(STATUS "  Installation: ${Python3_SITEARCH}/pynlm_enhanced*.so")

# Add dependency checking
include(CheckCXXCompilerFlag)

# Check for required compiler features
check_cxx_compiler_flag(-std=c++20 HAS_CXX20)
if(NOT HAS_CXX20)
    message(FATAL_ERROR "C++20 compiler support is required")
endif()

# Check for required Python modules
execute_process(
    COMMAND ${Python3_EXECUTABLE} -c "import numpy; import matplotlib; import pandas; print('OK')"
    RESULT_VARIABLE PYTHON_DEPS_RESULT
    OUTPUT_VARIABLE PYTHON_DEPS_OUTPUT
    ERROR_VARIABLE PYTHON_DEPS_ERROR
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

if(PYTHON_DEPS_RESULT AND NOT PYTHON_DEPS_RESULT EQUAL 0)
    message(WARNING "Python dependencies not fully available")
    message(WARNING "Required: numpy, matplotlib, pandas")
endif()

# Create a summary report
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/enhanced_python_summary.txt
"Enhanced Python Bindings Summary
===============================

Version: ${NLM_PYTHON_BINDINGS_VERSION}
Description: ${NLM_PYTHON_BINDINGS_DESCRIPTION}

Modules Included:
1. AdvancedSensoryProcessor - Machine learning utilities for feature extraction
2. VisualizationSystem - Comprehensive visualization tools
3. AdvancedExperimentationFramework - Automated experiment running
4. CommandLineTools - Command-line interface utilities
5. MemoryManagementTools - Memory management and optimization
6. SerializationManager - Advanced serialization with version control
7. DebugTools - Debug and analysis tools

Build Configuration:
- Target: pynlm_enhanced
- Output: ${Python3_SITEARCH}/pynlm_enhanced*.so
- Dependencies: nlm_core, pybind11

Installation:
- Install command: make install
- Python module location: ${Python3_SITEARCH}

Testing:
- Tests enabled: ${CMAKE_CURRENT_BINARY_DIR}/test_enhanced_python_bindings

Usage:
1. Install the module: make install
2. Import in Python: import pynlm_enhanced as nlm
3. Use advanced features: nlm.AdvancedSensoryProcessor, nlm.VisualizationSystem, etc.

License: MIT
Copyright: (c) 2026 NLM Research Team
")

message(STATUS "Enhanced Python bindings configuration complete")
message(STATUS "Summary written to: ${CMAKE_CURRENT_BINARY_DIR}/enhanced_python_summary.txt")