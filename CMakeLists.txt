cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# ==============================================================================
# MODERN CMAKE CONFIGURATION - Best Practices
# ==============================================================================

# Enable position independent code (needed for shared libraries)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ==============================================================================
# PLATFORM-SPECIFIC CONFIGURATIONS
# ==============================================================================

# Compiler-specific optimizations and warnings
if(MSVC)
    add_compile_options(/W4 /WX)
    add_compile_options(/permissive- /bigobj)
else()
    # GNU/Clang compiler options
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror=return-type
        -Werror=uninitialized
        -Werror=shadow
        -Werror=non-virtual-dtor
        -Werror=overloaded-virtual
        -Werror=missing-include-dirs
        -Werror=cast-align
        -Werror=strict-aliasing
        -Werror=zero-as-null-pointer-constant
    )
    
    # Optimization flags based on build type
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -march=native -ffast-math")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -ggdb3 -fno-omit-frame-pointer -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -ggdb3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")
    
    # Enable additional performance optimizations
    add_compile_options(-fomit-frame-pointer -falign-functions=16 -falign-loops=16)
endif()
endif()

# Debug vs Release specific definitions
target_compile_definitions(nlm_core PRIVATE $<$<CONFIG:Debug>:DEBUG>)

# ==============================================================================
# SOURCE FILE DISCOVERY - Automatic and Maintainable
# ==============================================================================

# Function to discover source files recursively
glob_all_sources()
glob_all_headers()
glob_all_configs()

# Get current source directory
get_filename_component(NLM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

# Function definitions for source discovery
function(glob_all_sources)
    file(GLOB_RECURSE SOURCES "${NLM_SOURCE_DIR}/src/*.cpp")
    file(GLOB_RECURSE HEADERS "${NLM_SOURCE_DIR}/src/**/*.hpp")
    file(GLOB_RECURSE CONFIGS "${NLM_SOURCE_DIR}/configs/*.cfg")
    
    # Set for external visibility
    set(SOURCES ${SOURCES} PARENT_SCOPE)
    set(HEADERS ${HEADERS} PARENT_SCOPE)
    set(CONFIGS ${CONFIGS} PARENT_SCOPE)
    
    # Group source files by component for better IDE organization
    separate_component_sources()
endfunction()

function(separate_component_sources)
    foreach(src ${SOURCES})
        get_filename_component(dir ${src} DIRECTORY)
        get_filename_component(comp ${dir} NAME)
        string(LENGTH "${NLM_SOURCE_DIR}/src/" prefix_len)
        string(SUBSTRING "${dir}" ${prefix_len} -1 comp)
        list(APPEND components "${comp}")
    endforeach()
    
    list(REMOVE_DUPLICATES components)
    foreach(comp ${components})
        file(GLOB component_sources "${NLM_SOURCE_DIR}/src/${comp}/*.cpp")
        file(GLOB component_headers "${NLM_SOURCE_DIR}/src/${comp}/*.hpp")
        set(${comp}_SOURCES ${component_sources} PARENT_SCOPE)
        set(${comp}_HEADERS ${component_headers} PARENT_SCOPE)
    endforeach()
endfunction()

# ==============================================================================
# LIBRARY BUILD TARGETS - Modular and Well-Structured
# ==============================================================================

# Core neural computation library
add_library(nlm_core STATIC ${SOURCES})
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Python bindings support
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${Python_INCLUDE_DIRS}>
)

# Compiler optimizations for core library
if(NOT MSVC)
    target_compile_options(nlm_core PRIVATE -march=native -flto)
endif()

# Agent system library
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system library
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# ==============================================================================
# EXPERIMENT MODULES - Phased Development
# ==============================================================================

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)
target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)
target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# ==============================================================================
# EXECUTABLE TARGETS - Demo Applications
# ==============================================================================

# Main simulation executable
add_executable(nlm src/main.cpp)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo src/experiments/Phase3Demo.cpp)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo src/experiments/Phase4Demo.cpp)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# ==============================================================================
# PYTHON BINDINGS CONFIGURATION
# ==============================================================================

# Find Python and pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

# Python module build
add_library(pynlm MODULE python/bindings.cpp)
target_link_libraries(pynlm PUBLIC pybind11::module nlm_core)

# Set module properties
set_target_properties(pynlm PROPERTIES
    PREFIX ""
    OUTPUT_NAME "pynlm"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
)

# Install Python module
dest(deploy_pynlm)

function(deploy_pynlm)
    install(TARGETS pynlm
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages
    )
endfunction()

# ==============================================================================
# TESTING INFRASTRUCTURE
# ==============================================================================

# Test framework implementation
add_library(nlm_test_framework STATIC
    src/core/tests/TestFramework.hpp
    src/core/tests/TestRunner.cpp
)

target_include_directories(nlm_test_framework PUBLIC src/core/tests)
target_link_libraries(nlm_test_framework PUBLIC nlm_core nlm_agent nlm_world)

# Test executable using new test framework
add_executable(nlm_test_framework_run tests/test_main.cpp)
target_link_libraries(nlm_test_framework_run PUBLIC nlm_test_framework nlm_core nlm_agent nlm_world)

# Legacy test executables (keep for backward compatibility)
add_executable(nlm_test tests/test_main.cpp)
target_link_libraries(nlm_test PUBLIC nlm_core)

# Add test directories to build
add_subdirectory(tests_legacy)

# Add test directories to build
add_subdirectory(tests_legacy)

# Legacy tests directory structure
add_custom_target(run_all_tests
    COMMAND ./nlm_test
    COMMAND ./nlm_test_framework_run
    COMMENT "Running all legacy and framework tests"
    VERBATIM
)

# ==============================================================================
# INSTALLATION CONFIGURATION
# ==============================================================================

# Install libraries
install(TARGETS nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install executables
install(TARGETS nlm nlm_phase3_demo nlm_phase4_demo
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY src/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

# Install configuration files
install(DIRECTORY configs/
    DESTINATION etc/nlm
    FILES_MATCHING PATTERN "*.cfg"
)

# ==============================================================================
# CUSTOM BUILD TARGETS AND UTILITIES
# ==============================================================================

# Performance optimization target
add_custom_target(perf_optimize
    COMMAND ${CMAKE_COMMAND} -E echo "Applying performance optimizations..."
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/PerfOptimize.cmake
    COMMENT "Running performance optimizations"
    VERBATIM
)

# Add a target for performance analysis
add_custom_target(perf_analyze
    COMMAND ${CMAKE_COMMAND} -E echo "Running performance analysis..."
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/scripts/perf_analyzer.py
    DEPENDS nlm
    COMMENT "Running performance analysis"
    VERBATIM
)

# Add a target for microbenchmarks
add_custom_target(microbench
    COMMAND ${CMAKE_COMMAND} -E echo "Running microbenchmarks..."
    COMMAND ./nlm --benchmark-mode
    DEPENDS nlm
    COMMENT "Running microbenchmarks"
    VERBATIM
)

# ==============================================================================
# BUILD VERIFICATION AND TESTING
# ==============================================================================

# Build verification target
add_custom_target(check_build
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/VerifyBuild.cmake
    COMMENT "Verifying build integrity"
    VERBATIM
)

# Documentation generation target
add_custom_target(docs
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/docs
    COMMAND doxygen Doxyfile
    COMMENT "Generating documentation"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
)

# Configuration validation target
add_custom_target(validate_config
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/scripts/validate_config.py
    DEPENDS pynlm
    COMMENT "Validating configuration"
    VERBATIM
)

# ==============================================================================
# BUILD STATISTICS AND REPORTING
# ==============================================================================

# Generate build report
add_custom_target(build_report
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBuildReport.cmake
    COMMENT "Generating build report"
    VERBATIM
)

message(STATUS "NLM Build Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Source files: ${SOURCES}")
message(STATUS "  Build directory: ${CMAKE_BINARY_DIR}")
