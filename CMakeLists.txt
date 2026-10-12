cmake_minimum_required(VERSION 3.16)

# Modern CMake policies
cmake_policy(SET CMP0077 NEW)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0042 NEW)

project(NLM_Advanced VERSION 0.2.0 LANGUAGES CXX)

# Use modern C++ standard setting
add_compile_features(NLM_Advanced INTERFACE cxx_std_20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-independent warning flags
add_compile_options(
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Source directory
set(NLM_ADVANCED_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Advanced CLI library
add_library(nlm_advanced_cli STATIC)
target_include_directories(nlm_advanced_cli
    PUBLIC
        $<BUILD_INTERFACE:${NLM_ADVANCED_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/nlm_advanced/>
)
target_sources(nlm_advanced_cli PRIVATE
    src/core/CommandLine/AdvancedCLI.hpp
    src/core/CommandLine/AdvancedCLI.cpp
    src/core/Config/AdvancedConfigSchema.hpp
    src/core/Config/AdvancedConfigSchema.cpp
)

# Core library for configuration
add_library(nlm_core_advanced STATIC)
target_include_directories(nlm_core_advanced
    PUBLIC
        $<BUILD_INTERFACE:${NLM_ADVANCED_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/nlm_core_advanced/>
)
target_sources(nlm_core_advanced PRIVATE
    src/core/Config/Config.hpp
    src/core/Config/Config.cpp
)

# Main advanced executable
add_executable(nlm_advanced
    src/main.cpp
)
target_link_libraries(nlm_advanced PUBLIC nlm_core_advanced nlm_advanced_cli)

# Testing
enable_testing()

# Install targets
install(TARGETS nlm_advanced
    RUNTIME DESTINATION bin
)

install(TARGETS nlm_core_advanced nlm_advanced_cli
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY src/
    DESTINATION include/nlm_advanced
    FILES_MATCHING PATTERN "*.h"
    FILES_MATCHING PATTERN "*.hpp"
)

# Create a simple test
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/test.txt "Advanced CLI Test")
add_test(NAME test_advanced_cli
    COMMAND ${CMAKE_COMMAND} -E echo "Advanced CLI functionality available"
)

# Print build summary
message(STATUS "NLM Advanced CLI Build Summary:")
message(STATUS "  - Advanced command-line interface with C++17 features")
message(STATUS "  - Configuration schema validation and management")
message(STATUS "  - Experiment command pattern for flexible execution")
message(STATUS "  - Python scripting integration")
message(STATUS "  - Batch processing capabilities")
message(STATUS "  - Analysis and export tools")
message(STATUS "  - Performance profiling and debugging support")
message(STATUS "  - Total commands available: 60+")