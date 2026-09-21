cmake_minimum_required(VERSION 3.16)

# Project declaration
project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Neural Learning Machine - C++ neural simulation framework"
)

# Configure Python bindings
set(PYTHON_BINDINGS ON CACHE BOOL "Enable Python bindings support")

# Build type configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Multi-configuration support
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel" CACHE STRING "Available build types")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose build type" FORCE)
endif()

# Compiler warnings and features
if(MSVC)
    add_compile_options(/W4 /permissive-)
    add_definitions(-DNOMINMAX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Werror=return-type)
    add_definitions(-D_GLIBCXX_ASSERTIONS)
endif()

# Installation directories
include(GNUInstallDirs)

# Install prefix configuration
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "Installation prefix" FORCE)
endif()

# Feature options
option(BUILD_PYTHON_BINDINGS "Build Python bindings using pybind11" ON)
option(BUILD_TESTS "Build C++ test suite" ON)
option(BUILD_DEMOS "Build demo applications" ON)
option(BUILD_DOCS "Generate documentation with Doxygen" OFF)
option(NLM_INSTALL_PYTHON "Install Python package to system" ON)
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

# Import core module
add_subdirectory(src)

# Import tests if enabled
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Import Python bindings if enabled
if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# Package configuration
include(CMakePackageConfigHelpers)

# Create version configuration
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Create package configuration file
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

# Export targets for find_package
export(EXPORT NLMTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/NLMTargets.cmake"
)

# Install package configuration files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

# Install exported targets
install(EXPORT NLMTargets
    FILE NLMTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

# Generate documentation if enabled
if(BUILD_DOCS)
    find_package(Doxygen QUIET)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating Doxygen documentation"
            VERBATIM
        )
    endif()
endif()
