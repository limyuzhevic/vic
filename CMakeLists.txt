cmake_minimum_required(VERSION 3.20)

# Project configuration
project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Neural Learning Machine - Experimental Artificial Developmental Brain"
    LANGUAGES CXX
)

# Set C++ standard requirements
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate version header
include(CMakePackageConfigHelpers)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/Config/NLMVersion.hpp.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/Config/NLMVersion.hpp"
    @ONLY
)

# Install version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Platform detection
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Building for Linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Building for macOS")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Building for Windows")
else()
    message(STATUS "Building for ${CMAKE_SYSTEM_NAME}")
endif()

# Build configuration options
option(BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF)
option(NLM_BUILD_EXAMPLES "Build example executables" ON)
option(NLM_BUILD_TESTS "Build test suite" ON)
option(NLM_BUILD_DOCS "Build documentation" OFF)
option(NLM_ENABLE_PROFILE "Enable profiling support" OFF)
option(NLM_ENABLE_ASSERTIONS "Enable runtime assertions" ON)

# Compiler-specific configurations
if(MSVC)
    add_compile_options(/W4 /WX /permissive-)
    add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Wshadow
        -Wformat=2
        -Wunused
        -Wconversion
        -Wsign-conversion
    )
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-Wdouble-promotion -Wnull-dereference)
    endif()
endif()

# Optimization levels by build type
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

if(NLM_ENABLE_ASSERTIONS)
    add_compile_options(-DNLM_ENABLE_ASSERTIONS)
endif()

if(NLM_ENABLE_PROFILE)
    add_compile_options(-DNLM_ENABLE_PROFILE)
endif()

# Installation directories
include(GNUInstallDirs)

# Library installation paths
set(NLM_LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
set(NLM_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR})

# Create a config file for find_package support
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nlm
)

# Source file globbing (modern CMake approach)
file(GLOB_RECURSE CORE_SOURCES "src/core/*.cpp")
file(GLOB_RECURSE BRAIN_SOURCES "src/brain/*.cpp")
file(GLOB_RECURSE DYNAMICS_SOURCES "src/dynamics/*.cpp")
file(GLOB_RECURSE PLASTICITY_SOURCES "src/plasticity/*.cpp")
file(GLOB_RECURSE DEVELOPMENT_SOURCES "src/development/*.cpp")
file(GLOB_RECURSE NEUROMODULATION_SOURCES "src/neuromodulation/*.cpp")
file(GLOB_RECURSE MEMORY_SOURCES "src/memory/*.cpp")
file(GLOB_RECURSE COGNITION_SOURCES "src/cognition/*.cpp")
file(GLOB_RECURSE SENSORY_SOURCES "src/sensory/*.cpp")
file(GLOB_RECURSE PREDICTION_SOURCES "src/prediction/*.cpp")
file(GLOB_RECURSE MOTOR_SOURCES "src/motor/*.cpp")
file(GLOB_RECURSE ENVIRONMENT_SOURCES "src/environment/*.cpp")
file(GLOB_RECURSE EXPERIMENTS_SOURCES "src/experiments/*.cpp")
file(GLOB_RECURSE VISUALIZATION_SOURCES "src/visualization/*.cpp")

# Agent system files
file(GLOB_RECURSE AGENT_SOURCES "src/agent/*.cpp")
file(GLOB_RECURSE WORLD_SOURCES "src/world/*.cpp")

# Configure source lists based on build options
if(NOT BUILD_SHARED_LIBS)
    # Static library configuration
    add_library(nlm_core STATIC
        ${CORE_SOURCES}
        ${BRAIN_SOURCES}
        ${DYNAMICS_SOURCES}
        ${PLASTICITY_SOURCES}
        ${DEVELOPMENT_SOURCES}
        ${NEUROMODULATION_SOURCES}
        ${MEMORY_SOURCES}
        ${COGNITION_SOURCES}
        ${SENSORY_SOURCES}
        ${PREDICTION_SOURCES}
        ${MOTOR_SOURCES}
        ${ENVIRONMENT_SOURCES}
        ${EXPERIMENTS_SOURCES}
        ${VISUALIZATION_SOURCES}
        ${AGENT_SOURCES}
        ${WORLD_SOURCES}
    )
else()
    # Shared library configuration
    add_library(nlm_core SHARED
        ${CORE_SOURCES}
        ${BRAIN_SOURCES}
        ${DYNAMICS_SOURCES}
        ${PLASTICITY_SOURCES}
        ${DEVELOPMENT_SOURCES}
        ${NEUROMODULATION_SOURCES}
        ${MEMORY_SOURCES}
        ${COGNITION_SOURCES}
        ${SENSORY_SOURCES}
        ${PREDICTION_SOURCES}
        ${MOTOR_SOURCES}
        ${ENVIRONMENT_SOURCES}
        ${EXPERIMENTS_SOURCES}
        ${VISUALIZATION_SOURCES}
        ${AGENT_SOURCES}
        ${WORLD_SOURCES}
    )
    set_target_properties(nlm_core PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION ${PROJECT_VERSION}
    )
endif()

# Target include directories
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:${NLM_INCLUDE_INSTALL_DIR}>
)

# Core component libraries
if(NOT BUILD_SHARED_LIBS)
    add_library(nlm_agent STATIC ${AGENT_SOURCES})
    add_library(nlm_world STATIC ${WORLD_SOURCES})
    add_library(nlm_phase3 STATIC ${EXPERIMENTS_SOURCES})
    add_library(nlm_phase4 STATIC ${EXPERIMENTS_SOURCES})
else()
    add_library(nlm_agent SHARED ${AGENT_SOURCES})
    add_library(nlm_world SHARED ${WORLD_SOURCES})
    add_library(nlm_phase3 SHARED ${EXPERIMENTS_SOURCES})
    add_library(nlm_phase4 SHARED ${EXPERIMENTS_SOURCES})
endif()

# Library dependencies
target_link_libraries(nlm_agent PUBLIC nlm_core)
target_link_libraries(nlm_world PUBLIC nlm_core)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable
add_executable(nlm src/main.cpp)
target_link_libraries(nlm PUBLIC nlm_core)

# Demo executables (conditional)
if(NLM_BUILD_EXAMPLES)
    add_executable(nlm_phase3_demo src/experiments/Phase3Demo.cpp)
    target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
    
    add_executable(nlm_phase4_demo src/experiments/Phase4Demo.cpp)
    target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
endif()

# Testing setup
if(NLM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
    
    # Add test targets
    add_test(NAME nlm_basic_test COMMAND nlm --demo basic)
    add_test(NAME nlm_learning_test COMMAND nlm --demo learning)
    add_test(NAME nlm_stdp_test COMMAND nlm --demo stdp)
    add_test(NAME nlm_phase3_demo_test COMMAND nlm_phase3_demo)
    add_test(NAME nlm_phase4_demo_test COMMAND nlm_phase4_demo)
endif()

# Installation rules
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${NLM_LIB_INSTALL_DIR}
    LIBRARY DESTINATION ${NLM_LIB_INSTALL_DIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(DIRECTORY src/
    DESTINATION ${NLM_INCLUDE_INSTALL_DIR}/nlm
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN ".*\.svn" EXCLUDE
)

# Install configuration files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nlm
)

# Install executables
install(TARGETS nlm
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    NAMELINK_COMPONENT nlm_core
)

if(NLM_BUILD_EXAMPLES)
    install(TARGETS nlm_phase3_demo nlm_phase4_demo
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

# Create a config file for finding NLM
include(CMakeFindDependencyList)

# Python bindings configuration
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/python/CMakeLists.txt")
    add_subdirectory(python)
endif()

# Documentation (future)
if(NLM_BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN "${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in")
        set(DOXYGEN_OUT "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        
        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
    endif()
endif()

# Summary
message(STATUS "NLM Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Shared Libraries: ${BUILD_SHARED_LIBS}")
message(STATUS "  Examples: ${NLM_BUILD_EXAMPLES}")
message(STATUS "  Tests: ${NLM_BUILD_TESTS}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")

# Export targets for downstream projects
export(TARGETS nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    FILE "${CMAKE_CURRENT_BINARY_DIR}/NLMTargets.cmake")

# Help target
add_custom_target(help
    COMMAND echo "Available targets:"
    COMMAND ${CMAKE_MAKE_PROGRAM} help
    COMMENT "Show available build targets"
    VERBATIM
)
