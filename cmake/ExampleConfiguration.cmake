# Example: Modern CMake Configuration for NLM
# This file demonstrates how to use the improved CMake configuration

# === Basic Configuration ===
cmake_minimum_required(VERSION 3.20)
project(NLM-Example VERSION 0.1.0 LANGUAGES CXX)

# === Modern Build Options ===
option(BUILD_DOCS "Build documentation" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_DEMOS "Build demo executables" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(NLM_ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(NLM_ENABLE_SANITIZERS "Enable sanitizers" OFF)
option(NLM_ENABLE_PROFILING "Enable profiling" OFF)

# === Platform Detection ===
if(WIN32)
    set(NLM_PLATFORM_WINDOWS 1)
    message(STATUS "Configuring for Windows")
elseif(APPLE)
    set(NLM_PLATFORM_MACOS 1)
    message(STATUS "Configuring for macOS")
elseif(UNIX)
    set(NLM_PLATFORM_UNIX 1)
    message(STATUS "Configuring for Unix/Linux")
endif()

# === Build Type ===
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# === Modern Target-Based Architecture ===

# Core library using modern CMake
add_library(nlm_core
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    # ... other core files
)

# Apply modern CMake settings
setup_compile_options(nlm_core)
setup_platform(nlm_core)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Agent system library
add_library(nlm_agent src/agent/SensoryPercept.cpp src/agent/AgentBrain.cpp)
setup_compile_options(nlm_agent)
setup_platform(nlm_agent)
target_include_directories(nlm_agent PUBLIC src)
add_nlm_dependency(nlm_agent nlm_core)

# === Executable Examples ===

# Main executable
add_executable(nlm_example src/main.cpp)
setup_compile_options(nlm_example)
setup_platform(nlm_example)
add_nlm_dependency(nlm_example nlm_core)

# Demo executable (conditional)
if(BUILD_DEMOS)
    add_executable(nlm_demo_example src/experiments/Phase3Demo.cpp)
    setup_compile_options(nlm_demo_example)
    setup_platform(nlm_demo_example)
    add_nlm_dependency(nlm_demo_example nlm_core nlm_agent nlm_world nlm_phase3)
endif()

# === Testing Examples ===

if(BUILD_TESTS)
    enable_testing()
    
    # Add test
    add_test(NAME nlm_example_test
        COMMAND nlm_example
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    
    # Add custom test target
    add_custom_target(run_all_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS nlm_example
        COMMENT "Running all tests..."
    )
endif()

# === Documentation Examples ===

if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        configure_file(cmake/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
        
        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            COMMENT "Generating documentation"
            VERBATIM
        )
    endif()
endif()

# === Installation Examples ===

install(TARGETS nlm_core nlm_agent nlm_example
    EXPORT NLMTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY src/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# === Summary ===

message(STATUS "NLM Example Configuration Complete")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Platform: ${NLM_PLATFORM_WINDOWS}_${NLM_PLATFORM_MACOS}_${NLM_PLATFORM_UNIX}")
message(STATUS "Build docs: ${BUILD_DOCS}")
message(STATUS "Build tests: ${BUILD_TESTS}")
message(STATUS "Build demos: ${BUILD_DEMOS}")

# === Generated Configuration ===

# Generate and install CMake configuration files
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NLMDemoConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMDemoConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/NLMDemoConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMDemoConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

install(EXPORT NLMTargets
    FILE NLMDemoTargets.cmake
    NAMESPACE NLM::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

# === Usage Example ===

# This configuration can be used in various ways:
# 1. Standard build:
#    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
#    cmake --build build
#
# 2. Debug build with tests:
#    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DBUILD_DOCS=ON
#    cmake --build build --target run_all_tests
#
# 3. Installation:
#    cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/usr/local
#    cmake --build build --target install
#
# 4. Documentation:
#    cmake -S . -B build -DBUILD_DOCS=ON
#    cmake --build build --target docs
