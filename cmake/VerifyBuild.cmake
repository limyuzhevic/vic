# CMake script to verify build integrity
# This script checks for common build issues and validates the configuration

cmake_minimum_required(VERSION 3.16)

message("Verifying NLM build integrity...")

# Check if source files exist
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    message(FATAL_ERROR "Source file src/main.cpp not found!")
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/core/Config/Config.hpp")
    message(FATAL_ERROR "Header file src/core/Config/Config.hpp not found!")
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/core/Logger/Logger.hpp")
    message(FATAL_ERROR "Header file src/core/Logger/Logger.hpp not found!")
endif()

# Check for required C++ standard features
if(CMAKE_CXX_STANDARD LESS 20)
    message(WARNING "C++ standard is less than C++20. Some features may not be available.")
endif()

# Check for compiler features
if(MSVC)
    if(NOT MSVC_VERSION GREATER 1927)
        message(WARNING "MSVC version may not support all C++20 features.")
    endif()
else()
    # Check for C++20 support
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(-std=c++20 HAS_CXX20)
    if(NOT HAS_CXX20)
        message(FATAL_ERROR "Compiler does not support C++20!")
    endif()
endif()

# Check for required libraries
if(NOT Python_FOUND)
    message(FATAL_ERROR "Python not found! Required for pybind11 bindings.")
endif()

if(NOT pybind11_FOUND)
    message(FATAL_ERROR "pybind11 not found! Required for Python bindings.")
endif()

# Check build configurations
if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Warning: Build type is not Release. Consider using Release for better performance.")
endif()

message("Build integrity check completed successfully!")
