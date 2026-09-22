cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type with defaults
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Modern CMake: Find dependencies
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Set up source directories
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Helper function for setting up targets with consistent properties
function(setup_target target_name)
    target_compile_features(${target_name} PRIVATE cxx_std_20)
    
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /permissive- /W4)
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    
    target_include_directories(${target_name} PRIVATE
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    )
endfunction()

# Modular Python bindings setup
# Instead of duplicating source files, we link against the already-built libraries
# This assumes the main NLM libraries are built and available

# Create an imported target for the main NLM library
if(NOT TARGET nlm_core)
    # If nlm_core doesn't exist (not built in the main project), we need to build it
    add_library(nlm_core STATIC)
    target_include_directories(nlm_core PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    )
    target_compile_features(nlm_core PUBLIC cxx_std_20)
endif()

# Create pybind11 module with proper modular dependencies
pybind11_add_module(pynlm bindings.cpp)
setup_target(pynlm)

# Link against the modular NLM libraries
# This creates a clean dependency chain and allows for better incremental builds
target_link_libraries(pynlm PRIVATE
    nlm_core
)

# Export target for find_package usage
install(TARGETS pynlm
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/pynlm
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/pynlm
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/pynlm
)

# Package configuration for find_package support
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/pynlmConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/pynlmConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pynlm
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/pynlmConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/pynlmConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/pynlmConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pynlm
)

# Additional Python-specific configuration
option(PYTHON_BUILD_SHARED "Build shared Python bindings" ON)
if(PYTHON_BUILD_SHARED)
    set_target_properties(pynlm PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Set output name for easier Python module loading
set_target_properties(pynlm PROPERTIES
    OUTPUT_NAME "pynlm"
    PREFIX ""
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
)

# Set C++ standard properties for pybind11
target_compile_features(pynlm PRIVATE cxx_std_20)

# Add pybind11 specific compile options
if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
else()
    target_compile_options(pynlm PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Documentation for Python module
set_target_properties(pynlm PROPERTIES
    EXCLUDE_FROM_ALL FALSE
    DEBUG_POSTFIX ""
)
