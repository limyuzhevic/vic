cmake_minimum_required(VERSION 3.20)

# NLM - Neural Learning Machine
# Modern CMake Build Configuration (Target-Based)
# Version: 0.1.0

project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "An experimental artificial developmental brain implementing real spiking neural computation"
)

# ==============================================================================
# C++ Standard and Compiler Configuration
# ==============================================================================

# C++20 standard with strict compliance
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default build type is Release with optimization
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type specified, using Release")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
endif()

# ==============================================================================
# Modern CMake Options
# ==============================================================================

option(BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF)
option(NLM_BUILD_TESTS "Build test executables" ON)
option(NLM_BUILD_PYTHON "Build Python bindings" OFF)
option(NLM_ENABLE_SANITIZERS "Enable memory sanitizers and debugging" ON)
option(NLM_ENABLE_PROFILING "Enable performance profiling" OFF)
option(NLM_ENABLE_EXPERIMENTS "Build experimental features" ON)
option(NLM_ENABLE_CCACHE "Use CCache for faster rebuilds" ON)

# ==============================================================================
# Build Optimizations and Validation
# ==============================================================================

# CCache integration
if(NLM_ENABLE_CCACHE AND CMAKE_VERSION VERSION_LESS "3.13")
    message(WARNING "CCache support requires CMake 3.13+")
endif()

if(NLM_ENABLE_CCACHE)
    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_FOUND})
        set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_FOUND})
        message(STATUS "Using CCache for faster rebuilds")
    else()
        message(WARNING "CCache not found, rebuild speeds may be reduced")
    endif()
endif()

# Build type validation
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT CMAKE_BUILD_TYPE STREQUAL "Release" AND
   NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" AND NOT CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    message(FATAL_ERROR "Invalid build type: ${CMAKE_BUILD_TYPE}")
endif()

# Build type specific settings
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Build type: Debug (with debug symbols)")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Build type: Release (with optimizations)")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    message(STATUS "Build type: Release with Debug Info")
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    message(STATUS "Build type: MinSizeRel (minimum size, maximum speed)")
endif()

# ==============================================================================
# Compiler Options and Warnings
# ==============================================================================

if(MSVC)
    # Microsoft Visual C++ options
    add_compile_options(/W4 /permissive-)
    add_definitions(-DNOMINMAX)
    # Disable specific warnings that aren't relevant
    add_compile_options(/WX /EHsc)
else()
    # GCC/Clang options
    add_compile_options(-Wall -Wextra -Wpedantic)
    add_compile_options(-Wshadow -Wnon-virtual-dtor -Wold-style-cast)
    add_compile_options(-Wcast-align -Wnull-dereference -Wdouble-promotion)
    add_compile_options(-Wformat=2 -Wimplicit-fallthrough)
    add_compile_options(-Wlogical-op -Wduplicated-cond -Wduplicated-branches)
    add_compile_options(-Wmissing-include-dirs -Wmisleading-indentation)
    add_compile_options(-Wnoexcept -Wnested-namespaces -Wnull-dereference)
    add_compile_options(-Wno-redundant-decls -Wreturn-type -Wsubobject-linkage)
    add_compile_options(-Wuseless-cast -Wredundant-decls)
endif()

# ==============================================================================
# Memory Sanitizers and Debugging
# ==============================================================================

if(NLM_ENABLE_SANITIZERS AND NOT MSVC)
    message(STATUS "Memory sanitizers enabled")
    
    # AddressSanitizer for memory corruption detection
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
    
    # LeakSanitizer for memory leak detection
    add_compile_options(-fsanitize=leak)
    add_link_options(-fsanitize=leak)
    
    # UndefinedBehaviorSanitizer for runtime errors
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)
    
    # ThreadSanitizer for data race detection
    add_compile_options(-fsanitize=thread)
    add_link_options(-fsanitize=thread)
    
    # Additional sanitizers for certain flags
    if(DEFINED ENV{ASAN_OPTIONS})
        add_compile_options(-fsanitize=address,$ENV{ASAN_OPTIONS})
    endif()
else()
    message(STATUS "Memory sanitizers disabled")
endif()

# ==============================================================================
# Platform-Specific Configurations
# ==============================================================================

# Windows specific settings
if(WIN32)
    message(STATUS "Platform: Windows")
    
    # Disable Windows-specific warnings
    add_compile_options(/WX)
    
    # Windows-specific defines
    add_definitions(-D_WIN32_WINNT=0x0601)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    
    # Set appropriate subsystem for executables
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)  # For DLLs
    
    # Windows path separators
    string(REPLACE "/" "\\\\" CMAKE_SEP "//")
else()
    message(STATUS "Platform: Unix-like (Linux/macOS)")
    
    # Unix-like specific settings
    add_definitions(-D_POSIX_C_SOURCE=200809L)
    
    # RPATH configuration
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

# macOS specific settings
if(APPLE)
    message(STATUS "Platform: macOS")
    
    # macOS-specific flags
    add_compile_options(-Wno-poison-system-directories)
    add_definitions(-D_GNU_SOURCE)
    
    # Framework search paths
    set(CMAKE_FRAMEWORK_PATH "${CMAKE_INSTALL_PREFIX}/Frameworks")
endif()

# ==============================================================================
# Target-Based Architecture
# ==============================================================================

# Core library sources
file(GLOB_RECURSE CORE_SOURCES
    "src/core/**/*.cpp"
    "src/core/**/*.cc"
)

file(GLOB_RECURSE BRAIN_SOURCES
    "src/brain/*.cpp"
    "src/brain/*.cc"
)

file(GLOB_RECURSE DYNAMICS_SOURCES
    "src/dynamics/*.cpp"
    "src/dynamics/*.cc"
)

file(GLOB_RECURSE PLASTICITY_SOURCES
    "src/plasticity/*.cpp"
    "src/plasticity/*.cc"
)

file(GLOB_RECURSE DEVELOPMENT_SOURCES
    "src/development/*.cpp"
    "src/development/*.cc"
)

file(GLOB_RECURSE NEUROMODULATION_SOURCES
    "src/neuromodulation/*.cpp"
    "src/neuromodulation/*.cc"
)

file(GLOB_RECURSE MEMORY_SOURCES
    "src/memory/*.cpp"
    "src/memory/*.cc"
)

file(GLOB_RECURSE COGNITION_SOURCES
    "src/cognition/*.cpp"
    "src/cognition/*.cc"
)

file(GLOB_RECURSE SENSORY_SOURCES
    "src/sensory/*.cpp"
    "src/sensory/*.cc"
)

file(GLOB_RECURSE PREDICTION_SOURCES
    "src/prediction/*.cpp"
    "src/prediction/*.cc"
)

file(GLOB_RECURSE MOTOR_SOURCES
    "src/motor/*.cpp"
    "src/motor/*.cc"
)

file(GLOB_RECURSE ENVIRONMENT_SOURCES
    "src/environment/*.cpp"
    "src/environment/*.cc"
)

file(GLOB_RECURSE EXPERIMENTS_SOURCES
    "src/experiments/*.cpp"
    "src/experiments/*.cc"
)

file(GLOB_RECURSE VISUALIZATION_SOURCES
    "src/visualization/*.cpp"
    "src/visualization/*.cc"
)

# Create core library target
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

target_include_directories(nlm_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        src
)

target_compile_features(nlm_core PUBLIC cxx_std_20)
target_compile_options(nlm_core
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Agent system target
file(GLOB_RECURSE AGENT_SOURCES
    "src/agent/*.cpp"
    "src/agent/*.cc"
)

add_library(nlm_agent
    ${AGENT_SOURCES}
)

target_include_directories(nlm_agent
    PUBLIC
        src
)

target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system target
file(GLOB_RECURSE WORLD_SOURCES
    "src/world/*.cpp"
    "src/world/*.cc"
)

add_library(nlm_world
    ${WORLD_SOURCES}
)

target_include_directories(nlm_world
    PUBLIC
        src
)

target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments (if enabled)
if(NLM_ENABLE_EXPERIMENTS)
    file(GLOB_RECURSE PHASE3_SOURCES
        "src/experiments/Phase3Experiment.cpp"
    )
    
    add_library(nlm_phase3
        ${PHASE3_SOURCES}
    )
    
    target_include_directories(nlm_phase3 PUBLIC src)
    target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)
endif()

# Phase 4 experiments (if enabled)
if(NLM_ENABLE_EXPERIMENTS)
    file(GLOB_RECURSE PHASE4_SOURCES
        "src/experiments/Phase4Experiment.cpp"
    )
    
    add_library(nlm_phase4
        ${PHASE4_SOURCES}
    )
    
    target_include_directories(nlm_phase4 PUBLIC src)
    target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)
endif()

# ==============================================================================
# Executable Targets
# ==============================================================================

# Main NLM executable
add_executable(nlm
    src/main.cpp
)

target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
if(NLM_ENABLE_EXPERIMENTS)
    add_executable(nlm_phase3_demo
        src/experiments/Phase3Demo.cpp
    )
    
    target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
endif()

# Phase 4 demo executable
if(NLM_ENABLE_EXPERIMENTS)
    add_executable(nlm_phase4_demo
        src/experiments/Phase4Demo.cpp
    )
    
    target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
endif()

# ==============================================================================
# Python Bindings
# ==============================================================================

if(NLM_BUILD_PYTHON)
    enable_language(PYTHON)
    
    # Python binding configuration
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    
    # Include Python bindings CMakeLists.txt
    add_subdirectory(python)
endif()

# ==============================================================================
# Testing
# ==============================================================================

if(NLM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ==============================================================================
# Installation
# ==============================================================================

# Install libraries
install(TARGETS nlm_core nlm_agent nlm_world
    EXPORT NLMTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install experimental libraries
if(NLM_ENABLE_EXPERIMENTS)
    install(TARGETS nlm_phase3 nlm_phase4
        EXPORT NLMTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
endif()

# Install headers
install(DIRECTORY src/
    DESTINATION include/nlm
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*/.git/*" EXCLUDE
)

# Install executables
install(TARGETS nlm
    RUNTIME DESTINATION bin
)

if(NLM_ENABLE_EXPERIMENTS)
    install(TARGETS nlm_phase3_demo nlm_phase4_demo
        RUNTIME DESTINATION bin
    )
endif()

# ==============================================================================
# Package Management (CPack)
# ==============================================================================

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# Generate package configuration files
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    INSTALL_DESTINATION lib/cmake/NLM
)

# Generate and install CMake package files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    DESTINATION lib/cmake/NLM
)

install(EXPORT NLMTargets
    FILE NLMTargets.cmake
    NAMESPACE NLM::
    DESTINATION lib/cmake/NLM
)

# CPack configuration
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Neural Learning Machine - An experimental artificial developmental brain")
set(CPACK_PACKAGE_DESCRIPTION "NLM implements real spiking neural computation with:\n"
    "- Leaky Integrate-and-Fire (LIF) neurons\n"
    "- Event-driven spike propagation with synaptic delays\n"
    "- STDP and Hebbian plasticity\n"
    "- Structural plasticity (synaptogenesis/pruning)\n"
    "- Neuromodulation and reward-based learning\n"
    "- Working memory and episodic memory systems\n"
    "- Predictive coding and planning")

set(CPACK_PACKAGE_CONTACT "nlm@example.com")
set(CPACK_PACKAGE_VENDOR "NLM Development Team")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://nlm.example.com")

# Dependencies
if(NLM_BUILD_PYTHON)
    set(CPACK_PACKAGE_REQUIRES "Python")
endif()

# Generate package metadata files
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# Distribution formats
set(CPACK_GENERATOR "TGZ;DEB;RPM")

# Debian package configuration
if(DEB)
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "NLM Team")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_SECTION "science")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
endif()

# RPM package configuration
if(RPM)
    set(CPACK_RPM_PACKAGE_RELEASE "1")
    set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
    set(CPACK_RPM_PACKAGE_REQUIRES "gcc-c++")
endif()

# Packaging for Windows (NSIS installer)
if(WIN32)
    set(CPACK_GENERATOR "${CPACK_GENERATOR};NSIS")
    set(CPACK_PACKAGE_EXECUTABLES "nlm" "NLM")
    set(CPACK_NSIS_DISPLAY_NAME "NLM Neural Learning Machine")
    set(CPACK_NSIS_CONTACT "nlm@example.com")
endif()

# Packaging for macOS (PackageMaker)
if(APPLE)
    set(CPACK_GENERATOR "${CPACK_GENERATOR};PackageMaker")
    set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-macos")
endif()

# Common package settings
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}")
set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

include(CPack)

# ==============================================================================
# Build Information and Dependency Analysis
# ==============================================================================

# Generate build information
function(generate_build_info target_name)
    set(BUILD_INFO_FILE "${CMAKE_CURRENT_BINARY_DIR}/build_info/${target_name}_build_info.txt")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/build_info")
    
    file(WRITE ${BUILD_INFO_FILE}
        "NLM Build Information\n"
        "======================\n\n"
        "Project: NLM\n"
        "Version: ${PROJECT_VERSION}\n"
        "Build type: ${CMAKE_BUILD_TYPE}\n"
        "Compiler: ${CMAKE_CXX_COMPILER_ID}\n"
        "Compiler version: ${CMAKE_CXX_COMPILER_VERSION}\n"
        "Build time: ${CMAKE_TIMESTAMP}\n\n"
        "Options:\n"
        "  BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}\n"
        "  NLM_BUILD_TESTS: ${NLM_BUILD_TESTS}\n"
        "  NLM_BUILD_PYTHON: ${NLM_BUILD_PYTHON}\n"
        "  NLM_ENABLE_SANITIZERS: ${NLM_ENABLE_SANITIZERS}\n"
        "  NLM_ENABLE_PROFILING: ${NLM_ENABLE_PROFILING}\n"
        "  NLM_ENABLE_EXPERIMENTS: ${NLM_ENABLE_EXPERIMENTS}\n"
        "  NLM_ENABLE_CCACHE: ${NLM_ENABLE_CCACHE}\n"
    )
    
    add_custom_target(${target_name}_build_info ALL
        COMMAND ${CMAKE_COMMAND} -E echo "Build info generated: ${BUILD_INFO_FILE}"
        COMMENT "Generating build information for ${target_name}"
    )
endfunction()

# Generate build information for all targets
generate_build_info("nlm")
generate_build_info("nlm_core")

# ==============================================================================
# Dependency Scanning and Visualization
# ==============================================================================

# Target dependency graph (requires GraphViz)
if(DEFINED ENV{GRAPHVIZ_PATH})
    find_program(GraphViz_PATH dot PATHS $ENV{GRAPHVIZ_PATH})
    if(GraphViz_PATH)
        add_custom_target(dependencies
            COMMAND ${GraphViz_PATH} -Tpng nlm_targets.dot -o dependency_graph.png
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating dependency graph"
            VERBATIM
        )
        
        # Generate DOT file showing target dependencies
        file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/nlm_targets.dot"
            "digraph NLM_Dependencies {\n"
            "    rankdir=LR;\n"
            "    node [shape=box, style=filled];\n"
        )
        
        # Add target definitions
        file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/nlm_targets.dot"
            "    \"nlm_core\";\n"
            "    \"nlm_agent\";\n"
            "    \"nlm_world\";\n"
            "    \"nlm\";\n"
        )
        
        # Add dependency edges
        file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/nlm_targets.dot"
            "    \"nlm_agent\" -> \"nlm_core\";\n"
            "    \"nlm_world\" -> \"nlm_core\";\n"
            "    \"nlm\" -> \"nlm_core\";\n"
        )
        
        file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/nlm_targets.dot" "}\n")
    endif()
endif()

# ==============================================================================
# Utility Targets
# ==============================================================================

# Clean all build artifacts (custom target)
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/build_info
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/nlm_targets.dot
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/dependency_graph.png
    COMMENT "Cleaning build artifacts and generated files"
)

# Print configuration summary
add_custom_target(print_config
    COMMAND echo "=== NLM Build Configuration ==="
    COMMAND echo "Build type: ${CMAKE_BUILD_TYPE}"
    COMMAND echo "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}"
    COMMAND echo "Installation prefix: ${CMAKE_INSTALL_PREFIX}"
    COMMAND echo "Build directory: ${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND echo "Source directory: ${CMAKE_CURRENT_SOURCE_DIR}"
    COMMAND echo "Shared libraries: ${BUILD_SHARED_LIBS}"
    COMMAND echo "Build tests: ${NLM_BUILD_TESTS}"
    COMMAND echo "Build Python: ${NLM_BUILD_PYTHON}"
    COMMAND echo "Sanitizers: ${NLM_ENABLE_SANITIZERS}"
    COMMAND echo "Profiling: ${NLM_ENABLE_PROFILING}"
    COMMAND echo "Experiments: ${NLM_ENABLE_EXPERIMENTS}"
    COMMAND echo "CCache: ${NLM_ENABLE_CCACHE}"
    COMMENT "Displaying NLM build configuration summary"
)

# ==============================================================================
# Summary
# ==============================================================================

message(STATUS "NLM CMake Configuration Complete")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Build shared libraries: ${BUILD_SHARED_LIBS}")
message(STATUS "  Build tests: ${NLM_BUILD_TESTS}")
message(STATUS "  Build Python: ${NLM_BUILD_PYTHON}")
message(STATUS "  Sanitizers: ${NLM_ENABLE_SANITIZERS}")
message(STATUS "  Experiments: ${NLM_ENABLE_EXPERIMENTS}")
