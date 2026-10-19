cmake_minimum_required(VERSION 3.20)

# Include modern CMake setup utilities
include(cmake/NLMSetup.cmake)

# Project configuration
project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Neuromorphic Learning Machine"
    HOMEPAGE_URL "https://github.com/nlm-project/nlm"
)

# === Modern CMake Configuration ===

# Policy settings for modern CMake behavior
cmake_policy(SET CMP0077 NEW)  # option() honors normal variables
cmake_policy(SET CMP0092 NEW)  # MSVC runtime library selection

# === Build System Options ===

# Option to control build components
option(BUILD_DOCS "Build documentation" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_DEMOS "Build demo executables" ON)
option(BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF)
option(NLM_ENABLE_COVERAGE "Enable code coverage reporting" OFF)
option(NLM_ENABLE_SANITIZERS "Enable sanitizers" OFF)
option(NLM_ENABLE_PROFILING "Enable profiling support" OFF)

# Build type selection
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# === Platform Detection and Configuration ===

# Detect platform
if(WIN32)
    message(STATUS "Configuring for Windows platform")
    set(NLM_PLATFORM_WINDOWS 1)
elseif(APPLE)
    message(STATUS "Configuring for macOS platform")
    set(NLM_PLATFORM_MACOS 1)
elseif(UNIX)
    message(STATUS "Configuring for Unix/Linux platform")
    set(NLM_PLATFORM_UNIX 1)
endif()

# Platform-specific configurations
if(NLM_PLATFORM_WINDOWS)
    # Windows-specific configurations
    if(MSVC)
        # MSVC runtime library selection
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")
        else()
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
        endif()
    endif()
elseif(NLM_PLATFORM_UNIX)
    # Unix-specific configurations (Linux, macOS)
    # Position independent code for shared libraries
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    
    # macOS framework support
    if(NLM_PLATFORM_MACOS)
        find_library(COCOA_LIBRARY Cocoa)
        if(COCOA_LIBRARY)
            link_libraries(${COCOA_LIBRARY})
        endif()
    endif()
endif()

# === Source File Discovery ===

# Message for source file globbing warning
message(WARNING "Using file(GLOB) is not recommended by CMake for source files. "
                 "Consider explicitly listing source files in future versions.")

# Core library sources
file(GLOB CORE_SOURCES "src/core/*.cpp" "src/core/*/*.cpp")
file(GLOB BRAIN_SOURCES "src/brain/*.cpp")
file(GLOB DYNAMICS_SOURCES "src/dynamics/*.cpp")
file(GLOB PLASTICITY_SOURCES "src/plasticity/*.cpp")
file(GLOB DEVELOPMENT_SOURCES "src/development/*.cpp")
file(GLOB NEUROMODULATION_SOURCES "src/neuromodulation/*.cpp")
file(GLOB MEMORY_SOURCES "src/memory/*.cpp")
file(GLOB COGNITION_SOURCES "src/cognition/*.cpp")
file(GLOB SENSORY_SOURCES "src/sensory/*.cpp")
file(GLOB PREDICTION_SOURCES "src/prediction/*.cpp")
file(GLOB MOTOR_SOURCES "src/motor/*.cpp")
file(GLOB ENVIRONMENT_SOURCES "src/environment/*.cpp")
file(GLOB EXPERIMENTS_SOURCES "src/experiments/*.cpp")
file(GLOB VISUALIZATION_SOURCES "src/visualization/*.cpp")

# Agent system sources
file(GLOB AGENT_SOURCES "src/agent/*.cpp")

# World system sources
file(GLOB WORLD_SOURCES "src/world/*.cpp")

# Phase-specific sources
file(GLOB PHASE3_SOURCES "src/experiments/Phase3Experiment.cpp")
file(GLOB PHASE4_SOURCES "src/experiments/Phase4Experiment.cpp")

# Demo sources
file(GLOB DEMO_SOURCES "src/experiments/Phase3Demo.cpp" "src/experiments/Phase4Demo.cpp")

# Main application sources
file(GLOB MAIN_SOURCES "src/main.cpp")

# === Library Targets ===

# Core library
add_library(nlm_core
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
)

# Apply compile options to core library
setup_compile_options(nlm_core)

# Configure platform-specific properties
setup_platform(nlm_core)

# Core library include directories
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Core library compile definitions
target_compile_definitions(nlm_core PUBLIC
    $<IF:$<BOOL:${NLM_PLATFORM_WINDOWS}>,_WINDOWS,>
    $<IF:$<BOOL:${NLM_PLATFORM_MACOS}>,_APPLE,>
    $<IF:$<BOOL:${NLM_PLATFORM_UNIX}>,_UNIX,>
)

# Agent system library
add_library(nlm_agent ${AGENT_SOURCES})
setup_compile_options(nlm_agent)
setup_platform(nlm_agent)
target_include_directories(nlm_agent PUBLIC src)
add_nlm_dependency(nlm_agent nlm_core)

# World system library
add_library(nlm_world ${WORLD_SOURCES})
setup_compile_options(nlm_world)
setup_platform(nlm_world)
target_include_directories(nlm_world PUBLIC src)
add_nlm_dependency(nlm_world nlm_core)

# Phase 3 experiments library
add_library(nlm_phase3 ${PHASE3_SOURCES})
setup_compile_options(nlm_phase3)
setup_platform(nlm_phase3)
target_include_directories(nlm_phase3 PUBLIC src)
add_nlm_dependency(nlm_phase3 nlm_core nlm_agent nlm_world)

# Phase 4 experiments library
add_library(nlm_phase4 ${PHASE4_SOURCES})
setup_compile_options(nlm_phase4)
setup_platform(nlm_phase4)
target_include_directories(nlm_phase4 PUBLIC src)
add_nlm_dependency(nlm_phase4 nlm_core nlm_agent nlm_world)

# === Executable Targets ===

# Main executable
add_executable(nlm ${MAIN_SOURCES})
setup_compile_options(nlm)
setup_platform(nlm)
add_nlm_dependency(nlm nlm_core)

# Phase 3 demo executable
if(BUILD_DEMOS)
    add_executable(nlm_phase3_demo src/experiments/Phase3Demo.cpp)
    setup_compile_options(nlm_phase3_demo)
    setup_platform(nlm_phase3_demo)
    add_nlm_dependency(nlm_phase3_demo nlm_core nlm_agent nlm_world nlm_phase3)
endif()

# Phase 4 demo executable
if(BUILD_DEMOS)
    add_executable(nlm_phase4_demo src/experiments/Phase4Demo.cpp)
    setup_compile_options(nlm_phase4_demo)
    setup_platform(nlm_phase4_demo)
    add_nlm_dependency(nlm_phase4_demo nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
endif()

# === Testing Infrastructure ===

if(BUILD_TESTS)
    enable_testing()
    
    # Add test directory
    add_subdirectory(tests)
    
    # Create a test that runs all tests
    add_custom_target(run_all_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS nlm_test
        COMMENT "Running all tests..."
    )
endif()

# === Installation Configuration ===

# Install targets with component-based installation
install_component(nlm_core Runtime)
install_component(nlm_agent Runtime)
install_component(nlm_world Runtime)
install_component(nlm_phase3 Runtime)
install_component(nlm_phase4 Runtime)

if(BUILD_DEMOS)
    install(TARGETS nlm_phase3_demo nlm_phase4_demo
        RUNTIME DESTINATION bin
        COMPONENT Runtime
    )
endif()

# Install headers
install(DIRECTORY src/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
    PATTERN "*.hpp"
    COMPONENT Development
)

# Install configuration files
set(INSTALL_CONFIG_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/NLM")

# Generate and install CMake configuration files
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    INSTALL_DESTINATION ${INSTALL_CONFIG_DIR}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    DESTINATION ${INSTALL_CONFIG_DIR}
    COMPONENT Development
)

install(EXPORT NLMTargets
    FILE NLMTargets.cmake
    NAMESPACE NLM::
    DESTINATION ${INSTALL_CONFIG_DIR}
    COMPONENT Development
)

# === Documentation Generation ===

if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_INPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
        set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs")
        
        configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Doxyfile.in"
            "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
            @ONLY
        )
        
        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
        
        add_custom_target(docs_open
            COMMAND ${CMAKE_COMMAND} -E echo "Documentation generated at: ${DOXYGEN_OUTPUT_DIR}/index.html"
            COMMAND ${CMAKE_COMMAND} -E echo "To open documentation, run: xdg-open ${DOXYGEN_OUTPUT_DIR}/index.html"
            DEPENDS docs
            COMMENT "Opening documentation"
        )
        
        install(DIRECTORY ${DOXYGEN_OUTPUT_DIR}
            DESTINATION share/doc/nlm
            COMPONENT Documentation
        )
    endif()
endif()

# === Packaging Support (CPack) ===

include(CPack)

set(CPACK_PACKAGE_NAME "NLM")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION "Neuromorphic Learning Machine")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A neuromorphic learning machine library")
set(CPACK_PACKAGE_CONTACT "contact@nlm-project.org")
set(CPACK_PACKAGE_VENDOR "NLM Project")

# Source package configuration
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_SOURCE_GENERATOR "TGZ")

# Binary package configuration
set(CPACK_GENERATOR "TGZ;DEB;RPM")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "NLM Project")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Neuromorphic Learning Machine library")
set(CPACK_RPM_PACKAGE_RELEASE "1")

# Package components
set(CPACK_COMPONENTS_ALL
    Runtime
    Development
    Documentation
)

# Component definitions
set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "Runtime Files")
set(CPACK_COMPONENT_RUNTIME_DESCRIPTION "Runtime libraries and executables")

set(CPACK_COMPONENT_DEVELOPMENT_DISPLAY_NAME "Development Files")
set(CPACK_COMPONENT_DEVELOPMENT_DESCRIPTION "Header files and CMake configuration")

set(CPACK_COMPONENT_DOCUMENTATION_DISPLAY_NAME "Documentation")
set(CPACK_COMPONENT_DOCUMENTATION_DESCRIPTION "API documentation")

# === IDE Integration ===

# Generate project files for major IDEs
if(MSVC)
    # Visual Studio project generation
    set(CMAKE_PROJECT_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateVSProject.cmake")
endif()

# === Summary ===

message(STATUS "NLM CMake configuration complete.")
message(STATUS "Use 'cmake -S . -B build -D...options...' to configure the project.")
message(STATUS "Use 'cmake --build build' to build the project.")
message(STATUS "Use 'cmake --build build --target docs' to build documentation.")
message(STATUS "Use 'cmake --build build --target run_all_tests' to run tests.")
