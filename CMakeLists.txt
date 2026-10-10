cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# Modern CMake policies
cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0077 NEW)
cmake_policy(SET CMP0091 NEW)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Compiler optimization flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -DNDEBUG -march=native -flto)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -g -DDEBUG)
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_options(-O2 -g -DNDEBUG)
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    add_compile_options(-Os -DNDEBUG)
endif()

# Platform-specific configurations
if(WIN32)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Feature options
option(NLM_ENABLE_MEMORY_SYSTEMS "Enable memory system components" ON)
option(NLM_ENABLE_NEUROMODULATION "Enable neuromodulation systems" ON)
option(NLM_ENABLE_COGNITION "Enable cognitive components" ON)
option(NLM_ENABLE_DEVELOPMENT "Enable development systems" ON)
option(NLM_ENABLE_PREDICTION "Enable prediction systems" ON)
option(NLM_ENABLE_PYTHON_BINDINGS "Build Python bindings" ON)
option(NLM_ENABLE_TESTS "Build tests" ON)
option(NLM_ENABLE_PERFORMANCE_OPTIMIZATIONS "Enable performance optimizations" ON)
option(NLM_ENABLE_VISUALIZATION "Enable visualization components" ON)
option(NLM_ENABLE_MONITORING "Enable performance monitoring" ON)
option(NLM_ENABLE_MONITORS_DOC "Generate monitoring documentation" ON)

# Installation directories
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation prefix")
set(INCLUDE_INSTALL_DIR "include" CACHE PATH "Include directory for installation")
set(LIB_INSTALL_DIR "lib" CACHE PATH "Library directory for installation")
set(BIN_INSTALL_DIR "bin" CACHE PATH "Binary directory for installation")

# Project configuration
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/configs/nlm_config.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/nlm_config.hpp"
    @ONLY
)

# Create interface library for headers
add_library(nlm_headers INTERFACE)
target_include_directories(nlm_headers
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
)

# Add subdirectories in logical order
add_subdirectory(src)
if(NLM_ENABLE_TESTS)
    add_subdirectory(tests)
endif()
if(NLM_ENABLE_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
if(NLM_ENABLE_MONITORING)
    add_subdirectory(monitor)
endif()

# Plugin system (optional, can be enabled with NLM_ENABLE_PLUGINS)
option(NLM_ENABLE_PLUGINS "Enable plugin system" ON)
if(NLM_ENABLE_PLUGINS)
    add_subdirectory(plugin)
endif()

# Export configuration for find_package
install(EXPORT nlmTargets
    FILE nlmConfig.cmake
    NAMESPACE NLM::
    DESTINATION ${LIB_INSTALL_DIR}/cmake/nlm
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/nlmConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/nlmConfig.cmake"
    INSTALL_DESTINATION ${LIB_INSTALL_DIR}/cmake/nlm
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/nlmConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/nlmConfigVersion.cmake"
    DESTINATION ${LIB_INSTALL_DIR}/cmake/nlm
)

# Documentation generation
find_program(SPHINX_BUILD sphinx-build)
if(SPHINX_BUILD)
    add_custom_target(docs
        COMMAND ${SPHINX_BUILD} -b html ${CMAKE_CURRENT_SOURCE_DIR}/docs ${CMAKE_CURRENT_BINARY_DIR}/docs/html
        COMMENT "Building documentation"
        VERBATIM
    )
endif()

# Package statistics
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/nlm_package_info.txt"
    "NLM Build Configuration:\n"
    "  Build type: ${CMAKE_BUILD_TYPE}\n"
    "  Memory systems: ${NLM_ENABLE_MEMORY_SYSTEMS}\n"
    "  Neuromodulation: ${NLM_ENABLE_NEUROMODULATION}\n"
    "  Cognition: ${NLM_ENABLE_COGNITION}\n"
    "  Development: ${NLM_ENABLE_DEVELOPMENT}\n"
    "  Prediction: ${NLM_ENABLE_PREDICTION}\n"
    "  Python bindings: ${NLM_ENABLE_PYTHON_BINDINGS}\n"
    "  Tests: ${NLM_ENABLE_TESTS}\n"
    "  Performance optimizations: ${NLM_ENABLE_PERFORMANCE_OPTIMIZATIONS}\n"
    "  Visualization: ${NLM_ENABLE_VISUALIZATION}\n"
    "  Monitoring: ${NLM_ENABLE_MONITORING}\n"
    "  Documentation: ${NLM_ENABLE_MONITORS_DOC}\n"
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/nlm_package_info.txt"
    DESTINATION ${LIB_INSTALL_DIR}/cmake/nlm
)
