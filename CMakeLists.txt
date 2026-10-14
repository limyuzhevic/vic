cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Project options
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_BENCHMARKS "Build benchmarks" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(ENABLE_JSON_CONFIG "Enable JSON configuration system" ON)
option(ENABLE_PERFORMANCE "Enable performance optimizations" ON)
option(ENABLE_VALIDATION "Enable configuration validation" ON)
option(ENABLE_JSON_SCHEMA "Enable JSON schema validation" ON)

# Find nlohmann_json
find_package(nlohmann_json 3.10.5 REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Configuration files
set(CONFIG_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/default.json
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/nlm_config_schema.json
)

# Source files
file(GLOB_RECURSE SRC_FILES
    "src/**/*.cpp"
    "src/**/*.c"
)

# Exclude test files from main build
list(FILTER SRC_FILES EXCLUDE REGEX ".*/tests/.*")

# Include directories for source files
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Main executable
add_executable(nlm ${SRC_FILES} ${CONFIG_FILES})
target_link_libraries(nlm PRIVATE nlohmann_json::nlohmann_json)

# Link with required libraries
if(ENABLE_PERFORMANCE)
    target_compile_definitions(nlm PRIVATE NLM_PERFORMANCE_OPTIMIZATIONS)
endif()

if(ENABLE_VALIDATION)
    target_compile_definitions(nlm PRIVATE NLM_VALIDATION_ENABLED)
endif()

if(ENABLE_JSON_SCHEMA)
    target_compile_definitions(nlm PRIVATE NLM_JSON_SCHEMA_ENABLED)
endif()

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Installation
install(TARGETS nlm DESTINATION bin)
install(DIRECTORY configs/ DESTINATION config)

# Python bindings (if enabled)
if(BUILD_PYTHON_BINDINGS)
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
    
    # Find scikit-build
    find_package(scikit-build-core REQUIRED)
    
    # Configure Python package
    set(PYTHON_MODULE_NAME "nlm")
    set(PYTHON_MODULE_VERSION "${PROJECT_VERSION}")
    
    # Create Python bindings
    # This would be configured separately via pyproject.toml
endif()

# Testing
if(BUILD_TESTS)
    enable_testing()
    
    # Add tests directory
    add_subdirectory(tests)
endif()

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Benchmarks
if(BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

# Documentation
if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        
        add_custom_target(doc
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
        )
    endif()
endif()