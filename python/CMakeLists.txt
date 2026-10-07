cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

# Python and pybind11 detection
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Build options
option(BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF)
option(BUILD_PYTHON_TESTS "Build Python test targets" ON)
option(BUILD_DOCUMENTATION "Build documentation with Sphinx" OFF)
option(INSTALL_HEADERS "Install C++ header files" ON)
option(INSTALL_CONFIG "Install configuration files" ON)
option(EXPORT_BINDINGS "Generate export configuration for downstream usage" ON)

# Platform-specific settings
if(WIN32)
    add_compile_definitions(_USE_MATH_DEFINES)
endif()

# Compiler warnings configuration
function(add_nlm_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endfunction()

# Determine source root using relative path
# Find the src directory relative to this CMakeLists.txt
get_filename_component(NLM_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
set(NLM_SOURCE_DIR "${NLM_ROOT_DIR}/src")

message(STATUS "NLM source directory: ${NLM_SOURCE_DIR}")

# Core library configuration
file(GLOB_RECURSE NLM_CORE_HEADERS "${NLM_SOURCE_DIR}/core/*.hpp" "${NLM_SOURCE_DIR}/core/*/*.hpp")
file(GLOB_RECURSE NLM_BRAIN_HEADERS "${NLM_SOURCE_DIR}/brain/*.hpp")
file(GLOB_RECURSE NLM_DYNAMICS_HEADERS "${NLM_SOURCE_DIR}/dynamics/*.hpp")
file(GLOB_RECURSE NLM_PLASTICITY_HEADERS "${NLM_SOURCE_DIR}/plasticity/*.hpp")
file(GLOB_RECURSE NLM_DEVELOPMENT_HEADERS "${NLM_SOURCE_DIR}/development/*.hpp")
file(GLOB_RECURSE NLM_NEUROMODULATION_HEADERS "${NLM_SOURCE_DIR}/neuromodulation/*.hpp")
file(GLOB_RECURSE NLM_MEMORY_HEADERS "${NLM_SOURCE_DIR}/memory/*.hpp")
file(GLOB_RECURSE NLM_COGNITION_HEADERS "${NLM_SOURCE_DIR}/cognition/*.hpp")
file(GLOB_RECURSE NLM_SENSORY_HEADERS "${NLM_SOURCE_DIR}/sensory/*.hpp")
file(GLOB_RECURSE NLM_PREDICTION_HEADERS "${NLM_SOURCE_DIR}/prediction/*.hpp")
file(GLOB_RECURSE NLM_MOTOR_HEADERS "${NLM_SOURCE_DIR}/motor/*.hpp")
file(GLOB_RECURSE NLM_ENVIRONMENT_HEADERS "${NLM_SOURCE_DIR}/environment/*.hpp")
file(GLOB_RECURSE NLM_EXPERIMENTS_HEADERS "${NLM_SOURCE_DIR}/experiments/*.hpp")
file(GLOB_RECURSE NLM_VISUALIZATION_HEADERS "${NLM_SOURCE_DIR}/visualization/*.hpp")
file(GLOB_RECURSE NLM_AGENT_HEADERS "${NLM_SOURCE_DIR}/agent/*.hpp")
file(GLOB_RECURSE NLM_WORLD_HEADERS "${NLM_SOURCE_DIR}/world/*.hpp")

# Include all header files for installation
list(APPEND NLM_ALL_HEADERS
    ${NLM_CORE_HEADERS} ${NLM_BRAIN_HEADERS} ${NLM_DYNAMICS_HEADERS}
    ${NLM_PLASTICITY_HEADERS} ${NLM_DEVELOPMENT_HEADERS} ${NLM_NEUROMODULATION_HEADERS}
    ${NLM_MEMORY_HEADERS} ${NLM_COGNITION_HEADERS} ${NLM_SENSORY_HEADERS}
    ${NLM_PREDICTION_HEADERS} ${NLM_MOTOR_HEADERS} ${NLM_ENVIRONMENT_HEADERS}
    ${NLM_EXPERIMENTS_HEADERS} ${NLM_VISUALIZATION_HEADERS} ${NLM_AGENT_HEADERS}
    ${NLM_WORLD_HEADERS}
)

# Core library
file(GLOB_RECURSE NLM_CORE_SOURCES "${NLM_SOURCE_DIR}/core/*.cpp" "${NLM_SOURCE_DIR}/core/*/*.cpp")
add_library(nlm_core ${NLM_CORE_SOURCES})
target_include_directories(nlm_core
    PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)
target_compile_features(nlm_core PUBLIC cxx_std_20)
add_nlm_warnings(nlm_core)

# Agent library
file(GLOB_RECURSE NLM_AGENT_SOURCES "${NLM_SOURCE_DIR}/agent/*.cpp")
add_library(nlm_agent ${NLM_AGENT_SOURCES})
target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_agent PUBLIC nlm_core)
add_nlm_warnings(nlm_agent)

# World library
file(GLOB_RECURSE NLM_WORLD_SOURCES "${NLM_SOURCE_DIR}/world/*.cpp")
add_library(nlm_world ${NLM_WORLD_SOURCES})
target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_world PUBLIC nlm_core)
add_nlm_warnings(nlm_world)

# Python module
pybind11_add_module(pynlm python/bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm PRIVATE ${NLM_SOURCE_DIR})
add_nlm_warnings(pynlm)

if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

# Configuration file for Python package
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/pynlm-config.cmake"
    @ONLY
)

# Installation targets
include(GNUInstallDirs)

# Install Python module
install(TARGETS pynlm
    LIBRARY DESTINATION ${PYTHON_SITEARCH}
    ARCHIVE DESTINATION ${PYTHON_LIBDIR}
    RUNTIME DESTINATION ${PYTHON_BINDIR}
)

# Install headers if requested
if(INSTALL_HEADERS)
    install(DIRECTORY ${NLM_SOURCE_DIR}/
        DESTINATION include/nlm
        FILES_MATCHING PATTERN "*.hpp"
        PATTERN "*.h"
    )
endif()

# Install configuration files
if(INSTALL_CONFIG)
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/pynlm-config.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pynlm
    )
endif()

# Generate and install export configuration
if(EXPORT_BINDINGS)
    export(TARGETS nlm_core nlm_agent nlm_world pynlm
        FILE "${CMAKE_CURRENT_BINARY_DIR}/pynlm-targets.cmake"
    )
    
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/pynlm-targets.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pynlm
    )
endif()

# Package configuration
include(CPack)

set(CPACK_PACKAGE_NAME "pynlm")
set(CPACK_PACKAGE_VERSION "0.1.0")
set(CPACK_PACKAGE_DESCRIPTION "Python bindings for NLM (Neural Learning Machine)")
set(CPACK_PACKAGE_CONTACT "nlm-project@nlm.org")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/nlm-project/nlm")
set(CPACK_PACKAGE_VENDOR "NLM Project")

if(WIN32)
    set(CPACK_GENERATOR "ZIP")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

# Package dependencies
if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    if(COCOA_LIBRARY)
        set(CPACK_DEPENDENCIES ${CPACK_DEPENDENCIES} ${COCOA_LIBRARY})
    endif()
endif()

# Python package metadata
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml.in"
    "${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml"
    DESTINATION ${PYTHON_SITEARCH}/pynlm
)

# Documentation target
if(BUILD_DOCUMENTATION)
    find_program(SPHINX_BUILD sphinx-build)
    if(SPHINX_BUILD)
        add_custom_target(docs
            COMMAND ${SPHINX_BUILD} -b html
                "${CMAKE_CURRENT_SOURCE_DIR}/docs"
                "${CMAKE_CURRENT_BINARY_DIR}/docs/html"
            COMMENT "Building documentation with Sphinx"
            VERBATIM
        )
        add_custom_target(docs_clean
            COMMAND ${SPHINX_BUILD} -b html
                -E "${CMAKE_CURRENT_SOURCE_DIR}/docs"
                "${CMAKE_CURRENT_BINARY_DIR}/docs/html"
            COMMENT "Cleaning documentation build directory"
            VERBATIM
        )
    else()
        message(WARNING "SPHINX_BUILD not found. Documentation target will not be available.")
    endif()
endif()

# Test targets
if(BUILD_PYTHON_TESTS)
    enable_testing()
    
    # Find Python test files
    file(GLOB PYTHON_TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.py")
    
    if(PYTHON_TEST_FILES)
        add_test(NAME python_bindings_test
            COMMAND ${Python_EXECUTABLE} -m pytest ${PYTHON_TEST_FILES}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        set_tests_properties(python_bindings_test PROPERTIES
            TIMEOUT 300
            LABELS "python;bindings;test"
        )
    endif()
endif()

# Summary output
message(STATUS "NLM Python Bindings Configuration")
message(STATUS "  Source directory: ${NLM_SOURCE_DIR}")
message(STATUS "  Python interpreter: ${Python_EXECUTABLE}")
message(STATUS "  Python include path: ${Python_INCLUDE_DIRS}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Build shared libraries: ${BUILD_SHARED_LIBS}")
message(STATUS "  Install headers: ${INSTALL_HEADERS}")
message(STATUS "  Install configuration: ${INSTALL_CONFIG}")
message(STATUS "  Export bindings: ${EXPORT_BINDINGS}")
message(STATUS "  Python tests: ${BUILD_PYTHON_TESTS}")
message(STATUS "  Documentation: ${BUILD_DOCUMENTATION}")

# Provide useful variables to downstream
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/pynlm-config-version.cmake"
    VERSION ${CPACK_PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/pynlm-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pynlm
)
