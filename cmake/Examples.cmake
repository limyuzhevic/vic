# NLM CMake Configuration Examples
# This file provides examples of how to configure the NLM project

# === Example 1: Standard Development Build ===
cmake_minimum_required(VERSION 3.20)
project(NLM Examples)

# Standard development setup
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/dev_build.txt" "Development build configuration")

# === Example 2: Debug Build with Tests ===
cmake_minimum_required(VERSION 3.20)
project(NLM Debug With Tests)

set(CMAKE_BUILD_TYPE Debug)
option(BUILD_TESTS "Enable tests" ON)
option(BUILD_DOCS "Enable documentation" ON)
option(NLM_ENABLE_COVERAGE "Enable coverage" ON)

# === Example 3: Release Build for Production ===
cmake_minimum_required(VERSION 3.20)
project(NLM Release Production)

set(CMAKE_BUILD_TYPE Release)
option(BUILD_DOCS "Enable documentation" OFF)
option(BUILD_DEMOS "Enable demos" OFF)
option(NLM_ENABLE_PROFILING "Enable profiling" OFF)

# === Example 4: Cross-Platform Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Cross Platform)

# Platform-specific configuration
if(WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    if(COCOA_LIBRARY)
        link_libraries(${COCOA_LIBRARY})
    endif()
else()
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# === Example 5: High-Performance Build ===
cmake_minimum_required(VERSION 3.20)
project(NLM High Performance)

set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
option(NLM_ENABLE_PROFILING "Enable profiling" ON)
option(NLM_ENABLE_SANITIZERS "Enable sanitizers" ON)

# === Example 6: Minimal Build ===
cmake_minimum_required(VERSION 3.20)
project(NLM Minimal)

# Minimal configuration
option(BUILD_DOCS "Enable documentation" OFF)
option(BUILD_TESTS "Enable tests" OFF)
option(BUILD_DEMOS "Enable demos" OFF)
option(BUILD_EXAMPLES "Enable examples" OFF)

# Only core library
add_library(nlm_core src/core/*.cpp)

# === Example 7: Package Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Package)

# Package configuration
set(CPACK_PACKAGE_NAME "NLM")
set(CPACK_PACKAGE_VERSION "0.1.0")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "NLM Team")
set(CPACK_GENERATOR "TGZ;DEB;RPM")

include(CPack)

# === Example 8: IDE Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM IDE)

# IDE-specific settings
if(MSVC)
    # Visual Studio specific settings
    set(CMAKE_PROJECT_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateVSProject.cmake")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    set(CMAKE_VS_GLOBALS "EnableImportAndExportData=true")
endif()

# === Example 9: Integration Testing ===
cmake_minimum_required(VERSION 3.20)
project(NLM Integration Testing)

# Integration testing setup
option(BUILD_INTEGRATION_TESTS "Enable integration tests" ON)
option(BUILD_PERFORMANCE_TESTS "Enable performance tests" ON)

if(BUILD_INTEGRATION_TESTS)
    add_subdirectory(tests)
endif()

# === Example 10: Documentation Generation ===
cmake_minimum_required(VERSION 3.20)
project(NLM Documentation)

# Documentation configuration
option(BUILD_DOCS "Enable documentation generation" ON)

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
    endif()
endif()

# === Example 11: Advanced Build Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Advanced)

# Advanced configuration
include(GNUInstallDirs)

# Custom installation directories
set(CMAKE_INSTALL_BINDIR "${CMAKE_INSTALL_PREFIX}/bin")
set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/include")

# Component-based installation
include(CPackComponent)

set(CPACK_COMPONENTS_ALL
    Runtime
    Development
    Documentation
)

# === Example 12: Custom Build Script ===
cmake_minimum_required(VERSION 3.20)
project(NLM Custom Build)

# Custom build target
add_custom_target(custom_build
    COMMAND ${CMAKE_COMMAND} -E echo "Running custom build..."
    COMMAND ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_SOURCES}
    COMMENT "Building with custom compiler options"
)

# === Example 13: Testing Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Testing)

# Comprehensive testing setup
option(ENABLE_UNIT_TESTS "Enable unit tests" ON)
option(ENABLE_INTEGRATION_TESTS "Enable integration tests" ON)
option(ENABLE_PERFORMANCE_TESTS "Enable performance tests" OFF)

if(ENABLE_UNIT_TESTS)
    enable_testing()
    add_test(NAME unit_tests COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)
endif()

# === Example 14: Export Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Export)

# Export configuration for find_package
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

install(EXPORT NLMTargets
    FILE NLMTargets.cmake
    NAMESPACE NLM::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

# === Example 15: Target Properties Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Target Properties)

# Target properties configuration
add_library(nlm_core STATIC src/core/*.cpp)

# Set target properties
target_compile_features(nlm_core PUBLIC cxx_std_20)
target_compile_options(nlm_core PRIVATE -Wall -Wextra -Wpedantic)
target_compile_definitions(nlm_core PUBLIC NLM_PLATFORM_UNIX)

# Set target properties for different configurations
set_target_properties(nlm_core PROPERTIES
    DEBUG_POSTFIX "_d"
    RELEASE_POSTFIX ""
    POSITION_INDEPENDENT_CODE ON
)

# === Example 16: Conditional Build ===
cmake_minimum_required(VERSION 3.20)
project(NLM Conditional)

# Conditional build based on platform and options
if(NLM_PLATFORM_WINDOWS)
    add_definitions(-D_WINDOWS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
elseif(NLM_PLATFORM_MACOS)
    add_definitions(-DAPPLE)
elseif(NLM_PLATFORM_UNIX)
    add_definitions(-DUNIX)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# === Example 17: Build Types and Configurations ===
cmake_minimum_required(VERSION 3.20)
project(NLM Build Types)

# Multiple build configurations
foreach(build_type Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${build_type}" build_type_upper)
    set(CMAKE_BUILD_TYPE "${build_type}" CACHE STRING "Build type ${build_type_upper}" FORCE)
    
    # Add target for each build type
    add_custom_target(${build_type}_build
        COMMAND ${CMAKE_COMMAND} -E echo "Building type: ${build_type}"
        COMMENT "Building ${build_type} configuration"
    )
endforeach()

# === Example 18: Package Dependency Management ===
cmake_minimum_required(VERSION 3.20)
project(NLM Package Dependencies)

# Package dependency management
find_package(PkgConfig REQUIRED)
pkg_check_modules(THIRD_PARTY_LIB REQUIRED libthirdparty)

# Include directories
include_directories(${THIRD_PARTY_LIB_INCLUDE_DIRS})

# Link libraries
link_libraries(${THIRD_PARTY_LIB_LIBRARIES})

# === Example 19: Cross-Compiler Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Cross Compiler)

# Cross-compiler configuration
set(CMAKE_C_COMPILER ${CROSS_COMPILER}_gcc)
set(CMAKE_CXX_COMPILER ${CROSS_COMPILER}_g++)
set(CMAKE_AR ${CROSS_COMPILER}_ar)

# Cross-compiler flags
string(APPEND CMAKE_C_FLAGS " -mcpu=cortex-m4")
string(APPEND CMAKE_CXX_FLAGS " -mcpu=cortex-m4")

# === Example 20: Install Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Install)

# Installation configuration
include(GNUInstallDirs)

# Install headers
install(DIRECTORY src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# Install libraries
install(TARGETS nlm_core nlm_agent nlm_world
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install executables
install(TARGETS nlm
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# === Example 21: Test Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Test Config)

# Comprehensive test configuration
option(ENABLE_CTEST "Enable CTest" ON)
option(ENABLE_GMOCK "Enable Google Mock" OFF)
option(ENABLE_CATCH2 "Enable Catch2" OFF)

if(ENABLE_GMOCK)
    find_package(GMock REQUIRED)
endif()

if(ENABLE_CATCH2)
    # Download and configure Catch2
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v2.13.8
    )
    FetchContent_MakeAvailable(Catch2)
endif()

# === Example 22: Installation Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Install Config)

# Installation configuration with components
include(GNUInstallDirs)
include(CPackComponent)

# Set installation directories
set(CMAKE_INSTALL_BINDIR bin)
set(CMAKE_INSTALL_LIBDIR lib)
set(CMAKE_INSTALL_INCLUDEDIR include)

# Component definitions
set(CPACK_COMPONENTS_ALL
    Runtime
    Development
    Documentation
    PythonBindings
    JavaBindings
    NodeBindings
)

# === Example 23: Runtime Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Runtime)

# Runtime configuration
option(ENABLE_DYNAMIC_LINKING "Enable dynamic linking" ON)
option(ENABLE_THREADING "Enable threading support" ON)
option(ENABLE_OPENMP "Enable OpenMP" OFF)

if(ENABLE_OPENMP)
    find_package(OpenMP REQUIRED)
    if(OpenMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    endif()
endif()

# === Example 24: Security Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Security)

# Security configuration
option(ENABLE_ASLR "Enable Address Space Layout Randomization" ON)
option(ENABLE_STACK_PROTECTOR "Enable stack protector" ON)
option(ENABLE_FORTIFY_SOURCE "Enable compile-time bounds checking" ON)

if(ENABLE_ASLR)
    target_link_options(nlm_core PRIVATE -Wl,--disable-new-dtags)
endif()

if(ENABLE_STACK_PROTECTOR)
    target_compile_options(nlm_core PRIVATE -fstack-protector-strong)
endif()

if(ENABLE_FORTIFY_SOURCE)
    target_compile_options(nlm_core PRIVATE -D_FORTIFY_SOURCE=3)
endif()

# === Example 25: Performance Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Performance)

# Performance configuration
option(ENABLE_PGO "Enable Profile-Guided Optimization" OFF)
option(ENABLE_LTO "Enable Link-Time Optimization" OFF)
option(ENABLE_OMIT_FRAME_POINTERS "Omit frame pointers for performance" OFF)

if(ENABLE_OMIT_FRAME_POINTERS)
    target_compile_options(nlm_core PRIVATE -foptimized-omitted-frame-pointer)
endif()

# === Example 26: Platform-Specific Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Platform Specific)

# Platform-specific configuration
if(WIN32)
    # Windows-specific configuration
    add_definitions(-D_WIN32_WINNT=0x0601)
    target_link_libraries(nlm_core kernel32 user32 gdi32)
elseif(APPLE)
    # macOS-specific configuration
    find_library(COCOA_LIBRARY Cocoa)
    if(COCOA_LIBRARY)
        link_libraries(${COCOA_LIBRARY})
    endif()
    target_link_libraries(nlm_core "${CMAKE_FRAMEWORKS_PATH}/Foundation.framework")
elseif(UNIX)
    # Unix-specific configuration
    target_link_libraries(nlm_core pthread dl)
    target_compile_options(nlm_core PRIVATE -pthread)
endif()

# === Example 27: Feature Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Features)

# Feature configuration
option(ENABLE_CUDA "Enable CUDA support" OFF)
option(ENABLE_OPENCL "Enable OpenCL support" OFF)
option(ENABLE_MPI "Enable MPI support" OFF)

if(ENABLE_CUDA)
    enable_language(CUDA)
    find_package(CUDA REQUIRED)
endif()

if(ENABLE_OPENCL)
    find_package(OpenCL REQUIRED)
endif()

if(ENABLE_MPI)
    find_package(MPI REQUIRED)
endif()

# === Example 28: Installation Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Install)

# Installation configuration
include(GNUInstallDirs)

# Installation paths
install(TARGETS nlm_core nlm_agent nlm_world
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/nlm_config.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm
)

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
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

# === Example 29: Testing Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Testing)

# Comprehensive testing configuration
option(ENABLE_UNIT_TESTS "Enable unit tests" ON)
option(ENABLE_INTEGRATION_TESTS "Enable integration tests" ON)
option(ENABLE_PERFORMANCE_TESTS "Enable performance tests" OFF)
option(ENABLE_COVERAGE_TESTS "Enable coverage tests" OFF)

if(ENABLE_UNIT_TESTS)
    enable_testing()
    add_test(NAME unit_tests COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)
endif()

if(ENABLE_COVERAGE_TESTS)
    add_custom_target(coverage
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/scripts/coverage.cmake
        COMMENT "Generate coverage report"
    )
endif()

# === Example 30: Debug Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Debug)

# Debug configuration
option(DEBUG_SYMBOLS "Generate debug symbols" ON)
option(DEBUG_INFORMATION "Enable debug information" ON)
option(DEBUG_RUNTIME "Enable debug runtime" ON)

if(DEBUG_SYMBOLS)
    target_compile_options(nlm_core PRIVATE -g)
endif()

if(DEBUG_INFORMATION)
    target_compile_options(nlm_core PRIVATE -ggdb3)
endif()

if(DEBUG_RUNTIME)
    # Configure debug runtime
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")
    endif()
endif()

# === Example 31: Release Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Release)

# Release configuration
option(OPTIMIZATION "Enable optimization" ON)
option(DEAD_CODE_ELIMINATION "Enable dead code elimination" ON)
option(CONSTANT_FOLDING "Enable constant folding" ON)

if(OPTIMIZATION)
    target_compile_options(nlm_core PRIVATE -O3)
endif()

if(DEAD_CODE_ELIMINATION)
    target_compile_options(nlm_core PRIVATE -fdead-code-elimination)
endif()

if(CONSTANT_FOLDING)
    target_compile_options(nlm_core PRIVATE -fconstant-folding)
endif()

# === Example 32: Documentation Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Documentation)

# Documentation configuration
option(BUILD_DOCS "Build documentation" ON)
option(GENERATE_API_DOCS "Generate API documentation" ON)
option(GENERATE_USER_DOCS "Generate user documentation" OFF)
option(GENERATE_DEV_DOCS "Generate developer documentation" ON)

if(BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    
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
        COMMENT "Generating documentation"
    )
endif()

# === Example 33: Installation Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Installation)

# Installation configuration
include(GNUInstallDirs)

# Installation options
set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation prefix")
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# Install components
install_component(nlm_core Runtime)
install_component(nlm_agent Runtime)
install_component(nlm_world Runtime)

# Install headers
install(DIRECTORY src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
    PATTERN "*/tests/*" EXCLUDE
)

# === Example 34: Package Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Package)

# Package configuration
include(CPack)

# Package options
set(CPACK_PACKAGE_NAME "NLM")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION "Neuromorphic Learning Machine library")
set(CPACK_PACKAGE_CONTACT "contact@nlm.org")
set(CPACK_PACKAGE_VENDOR "NLM Project")

# Package components
set(CPACK_COMPONENTS_ALL
    Runtime
    Development
    Documentation
)

# Package generators
set(CPACK_GENERATOR "TGZ;DEB;RPM")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "NLM Team")
set(CPACK_RPM_PACKAGE_RELEASE "1")

# === Example 35: IDE Integration ===
cmake_minimum_required(VERSION 3.20)
project(NLM IDE)

# IDE integration
option(GENERATE_VISUAL_STUDIO_PROJECTS "Generate Visual Studio projects" OFF)
option(GENERATE_XCODE_PROJECTS "Generate Xcode projects" OFF)
option(GENERATE_CLION_PROJECTS "Generate CLion projects" OFF)

if(GENERATE_VISUAL_STUDIO_PROJECTS)
    set(CMAKE_PROJECT_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateVSProject.cmake")
endif()

# === Example 36: Cross-Platform Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Cross Platform)

# Cross-platform configuration
option(BUILD_FOR_WINDOWS "Build for Windows" OFF)
option(BUILD_FOR_MACOS "Build for macOS" OFF)
option(BUILD_FOR_LINUX "Build for Linux" OFF)

if(BUILD_FOR_WINDOWS)
    add_definitions(-D_WINDOWS)
    set(CMAKE_SYSTEM_NAME Windows)
elseif(BUILD_FOR_MACOS)
    add_definitions(-DAPPLE)
    set(CMAKE_SYSTEM_NAME Darwin)
elseif(BUILD_FOR_LINUX)
    add_definitions(-DUNIX)
    set(CMAKE_SYSTEM_NAME Linux)
endif()

# === Example 37: Feature Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Features)

# Feature configuration
option(ENABLE_NEURON_FEATURES "Enable neuron features" ON)
option(ENABLE_SYNAPSE_FEATURES "Enable synapse features" ON)
option(ENABLE_PLASTICITY_FEATURES "Enable plasticity features" ON)
option(ENABLE_MEMORY_FEATURES "Enable memory features" ON)
option(ENABLE_COGNITION_FEATURES "Enable cognition features" ON)

if(ENABLE_NEURON_FEATURES)
    target_compile_definitions(nlm_core PUBLIC NLM_NEURON_FEATURES)
endif()

if(ENABLE_SYNAPSE_FEATURES)
    target_compile_definitions(nlm_core PUBLIC NLM_SYNAPSE_FEATURES)
endif()

if(ENABLE_PLASTICITY_FEATURES)
    target_compile_definitions(nlm_core PUBLIC NLM_PLASTICITY_FEATURES)
endif()

if(ENABLE_MEMORY_FEATURES)
    target_compile_definitions(nlm_core PUBLIC NLM_MEMORY_FEATURES)
endif()

if(ENABLE_COGNITION_FEATURES)
    target_compile_definitions(nlm_core PUBLIC NLM_COGNITION_FEATURES)
endif()

# === Example 38: Advanced Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Advanced)

# Advanced configuration
option(ENABLE_EXPORT_ALL_SYMBOLS "Export all symbols" OFF)
option(ENABLE_PRECOMPILED_HEADERS "Enable precompiled headers" ON)
option(ENABLE_PCH "Enable PCH" ON)

if(ENABLE_EXPORT_ALL_SYMBOLS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if(ENABLE_PRECOMPILED_HEADERS AND ENABLE_PCH)
    # Configure precompiled headers
    target_precompile_headers(nlm_core PRIVATE "src/core/Config/Config.hpp")
endif()

# === Example 39: Build Customization ===
cmake_minimum_required(VERSION 3.20)
project(NLM Customization)

# Build customization
option(CUSTOM_COMPILER_FLAGS "Enable custom compiler flags" OFF)
option(CUSTOM_LINKER_FLAGS "Enable custom linker flags" OFF)
option(CUSTOM_DEFINITIONS "Enable custom definitions" OFF)

if(CUSTOM_COMPILER_FLAGS)
    target_compile_options(nlm_core PRIVATE -march=native -mtune=native)
endif()

if(CUSTOM_LINKER_FLAGS)
    target_link_options(nlm_core PRIVATE -Wl,--gc-sections)
endif()

if(CUSTOM_DEFINITIONS)
    target_compile_definitions(nlm_core PUBLIC NLM_CUSTOM_CONFIG)
endif()

# === Example 40: Security Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Security)

# Security configuration
option(ENABLE_ASLR "Enable ASLR" ON)
option(ENABLE_STACK_PROTECTOR "Enable stack protector" ON)
option(ENABLE_FORTIFY_SOURCE "Enable fortify source" ON)
option(ENABLE_CANARY_VALUES "Enable canary values" ON)

if(ENABLE_ASLR)
    target_link_options(nlm_core PRIVATE -Wl,--disable-new-dtags)
endif()

if(ENABLE_STACK_PROTECTOR)
    target_compile_options(nlm_core PRIVATE -fstack-protector-strong)
endif()

if(ENABLE_FORTIFY_SOURCE)
    target_compile_options(nlm_core PRIVATE -D_FORTIFY_SOURCE=3)
endif()

if(ENABLE_CANARY_VALUES)
    target_compile_options(nlm_core PRIVATE -fstack-protector-strong)
endif()

# === Example 41: Performance Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Performance)

# Performance configuration
option(ENABLE_PGO "Enable Profile-Guided Optimization" OFF)
option(ENABLE_LTO "Enable Link-Time Optimization" OFF)
option(ENABLE_OMIT_FRAME_POINTERS "Omit frame pointers" OFF)
option(ENABLE_INLINE_FUNCTIONS "Enable function inlining" ON)

if(ENABLE_OMIT_FRAME_POINTERS)
    target_compile_options(nlm_core PRIVATE -foptimized-omitted-frame-pointer)
endif()

if(ENABLE_INLINE_FUNCTIONS)
    target_compile_options(nlm_core PRIVATE -finline-functions)
endif()

# === Example 42: Installation Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Installation)

# Installation configuration
include(GNUInstallDirs)

# Installation paths
set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation prefix")
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# Installation components
install(TARGETS nlm_core nlm_agent nlm_world
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install configuration files
install(EXPORT NLMTargets
    FILE NLMTargets.cmake
    NAMESPACE NLM::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

# === Example 43: Package Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Package)

# Package configuration
include(CPack)

# Package options
set(CPACK_PACKAGE_NAME "NLM")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION "Neuromorphic Learning Machine library")
set(CPACK_PACKAGE_CONTACT "contact@nlm.org")
set(CPACK_PACKAGE_VENDOR "NLM Project")

# Package components
set(CPACK_COMPONENTS_ALL
    Runtime
    Development
    Documentation
)

# === Example 44: IDE Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM IDE)

# IDE configuration
option(GENERATE_VISUAL_STUDIO "Generate Visual Studio projects" OFF)
option(GENERATE_XCODE "Generate Xcode projects" OFF)
option(GENERATE_CLION "Generate CLion projects" OFF)

if(GENERATE_VISUAL_STUDIO)
    set(CMAKE_PROJECT_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateVSProject.cmake")
endif()

# === Example 45: Cross-Platform Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Cross Platform)

# Cross-platform configuration
option(BUILD_FOR_WINDOWS "Build for Windows" OFF)
option(BUILD_FOR_MACOS "Build for macOS" OFF)
option(BUILD_FOR_LINUX "Build for Linux" OFF)

if(BUILD_FOR_WINDOWS)
    add_definitions(-D_WINDOWS)
    set(CMAKE_SYSTEM_NAME Windows)
elseif(BUILD_FOR_MACOS)
    add_definitions(-DAPPLE)
    set(CMAKE_SYSTEM_NAME Darwin)
elseif(BUILD_FOR_LINUX)
    add_definitions(-DUNIX)
    set(CMAKE_SYSTEM_NAME Linux)
endif()

# === Example 46: Feature Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Features)

# Feature configuration
option(ENABLE_NEURON "Enable neuron features" ON)
option(ENABLE_SYNAPSE "Enable synapse features" ON)
option(ENABLE_PLASTICITY "Enable plasticity features" ON)
option(ENABLE_MEMORY "Enable memory features" ON)
option(ENABLE_COGNITION "Enable cognition features" ON)

if(ENABLE_NEURON)
    target_compile_definitions(nlm_core PUBLIC NLM_NEURON)
endif()

if(ENABLE_SYNAPSE)
    target_compile_definitions(nlm_core PUBLIC NLM_SYNAPSE)
endif()

if(ENABLE_PLASTICITY)
    target_compile_definitions(nlm_core PUBLIC NLM_PLASTICITY)
endif()

if(ENABLE_MEMORY)
    target_compile_definitions(nlm_core PUBLIC NLM_MEMORY)
endif()

if(ENABLE_COGNITION)
    target_compile_definitions(nlm_core PUBLIC NLM_COGNITION)
endif()

# === Example 47: Advanced Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Advanced)

# Advanced configuration
option(EXPORT_ALL_SYMBOLS "Export all symbols" OFF)
option(PRECOMPILED_HEADERS "Enable precompiled headers" ON)
option(PCH "Enable PCH" ON)

if(EXPORT_ALL_SYMBOLS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if(PRECOMPILED_HEADERS AND PCH)
    target_precompile_headers(nlm_core PRIVATE "src/core/Config/Config.hpp")
endif()

# === Example 48: Build Customization ===
cmake_minimum_required(VERSION 3.20)
project(NLM Customization)

# Build customization
option(CUSTOM_COMPILER_FLAGS "Enable custom compiler flags" OFF)
option(CUSTOM_LINKER_FLAGS "Enable custom linker flags" OFF)
option(CUSTOM_DEFINITIONS "Enable custom definitions" OFF)

if(CUSTOM_COMPILER_FLAGS)
    target_compile_options(nlm_core PRIVATE -march=native -mtune=native)
endif()

if(CUSTOM_LINKER_FLAGS)
    target_link_options(nlm_core PRIVATE -Wl,--gc-sections)
endif()

if(CUSTOM_DEFINITIONS)
    target_compile_definitions(nlm_core PUBLIC NLM_CUSTOM_CONFIG)
endif()

# === Example 49: Security Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Security)

# Security configuration
option(ASLR "Enable ASLR" ON)
option(STACK_PROTECTOR "Enable stack protector" ON)
option(FORTIFY_SOURCE "Enable fortify source" ON)
option(CANARY_VALUES "Enable canary values" ON)

if(ASLR)
    target_link_options(nlm_core PRIVATE -Wl,--disable-new-dtags)
endif()

if(STACK_PROTECTOR)
    target_compile_options(nlm_core PRIVATE -fstack-protector-strong)
endif()

if(FORTIFY_SOURCE)
    target_compile_options(nlm_core PRIVATE -D_FORTIFY_SOURCE=3)
endif()

if(CANARY_VALUES)
    target_compile_options(nlm_core PRIVATE -fstack-protector-strong)
endif()

# === Example 50: Performance Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM Performance)

# Performance configuration
option(PGO "Enable Profile-Guided Optimization" OFF)
option(LTO "Enable Link-Time Optimization" OFF)
option(OMIT_FRAME_POINTERS "Omit frame pointers" OFF)
option(INLINE_FUNCTIONS "Enable function inlining" ON)

if(OMIT_FRAME_POINTERS)
    target_compile_options(nlm_core PRIVATE -foptimized-omitted-frame-pointer)
endif()

if(INLINE_FUNCTIONS)
    target_compile_options(nlm_core PRIVATE -finline-functions)
endif()
