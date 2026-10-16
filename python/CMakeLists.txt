# Modern Python bindings CMakeLists.txt for NLM
# Uses target-based approach with dependency management

cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings VERSION 0.1.0 LANGUAGES CXX)

# Import modern CMake utilities from parent project
include(../cmake/NLMConfig.cmake)

# Validate Python bindings configuration
if(NOT BUILD_PYTHON_BINDINGS)
    message(STATUS "Python bindings disabled")
    return()
endif()

# Find Python and pybind11 dependencies
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Configure Python-specific settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type for Python bindings" FORCE)
endif()

# Use parent project's core libraries
add_library(nlm_core INTERFACE)
target_link_libraries(nlm_core INTERFACE ../nlm_core)

add_library(nlm_agent INTERFACE)
target_link_libraries(nlm_agent INTERFACE ../nlm_agent)

add_library(nlm_world INTERFACE)
target_link_libraries(nlm_world INTERFACE ../nlm_world)

# Phase libraries (conditional)
if(BUILD_EXPERIMENTS)
    add_library(nlm_phase3 INTERFACE)
    target_link_libraries(nlm_phase3 INTERFACE ../nlm_phase3)
    
    add_library(nlm_phase4 INTERFACE)
    target_link_libraries(nlm_phase4 INTERFACE ../nlm_phase4)
    
    add_library(nlm_phase5 INTERFACE)
    target_link_libraries(nlm_phase5 INTERFACE ../nlm_phase5)
    
    add_library(nlm_phase6 INTERFACE)
    target_link_libraries(nlm_phase6 INTERFACE ../nlm_phase6)
endif()

# Create Python module using modern CMake target approach
pybind11_add_module(pynlm bindings.cpp)

# Link with parent project targets
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)

if(BUILD_EXPERIMENTS)
    target_link_libraries(pynlm PRIVATE nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6)
endif()

# Set target properties for optimal Python integration
target_compile_features(pynlm PRIVATE cxx_std_20)
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Compiler-specific optimizations
if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive- /WX)
else()
    target_compile_options(pynlm PRIVATE -Wall -Wextra -Wpedantic -fvisibility=hidden)
endif()

# Set Python module output name
set_target_properties(pynlm PROPERTIES
    OUTPUT_NAME "pynlm"
    PREFIX ""
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
)

# Install Python module
install(TARGETS pynlm
    LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES}
    ARCHIVE DESTINATION ${PYTHON_SITE_PACKAGES}
    RUNTIME DESTINATION ${PYTHON_SITE_PACKAGES}
)

# Install requirements file for pip
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt
    DESTINATION .
    OPTIONAL
)

# Add Python-specific tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    add_subdirectory(tests)
endif()

# Performance monitoring for Python bindings
if(BUILD_PERFORMANCE_TESTS)
    add_custom_target(pynlm_performance_benchmark
        COMMAND ${Python_EXECUTABLE} -c "import pynlm; import time; start = time.time(); for i in range(1000): pynlm.some_function(); end = time.time(); print(f'Python bindings performance: {end-start:.3f}s for 1000 iterations')"
        COMMENT "Running Python bindings performance benchmark"
        VERBATIM
    )
endif()

message(STATUS "Python bindings configured with pybind11 ${pybind11_VERSION}")
message(STATUS "Python executable: ${Python_EXECUTABLE}")
message(STATUS "Python include: ${Python_INCLUDE_DIRS}")
