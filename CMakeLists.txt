# CMake configuration for NLM project
# Enhanced CMake configuration for improved build process

cmake_minimum_required(VERSION 3.15)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# Set default C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configuration options
option(BUILD_PYTHON_BINDINGS "Build Python bindings via scikit-build-core" ON)
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_DEMOS "Build demo executables" ON)
option(BUILD_EXAMPLES "Build example applications" OFF)
option(ENABLE_PERFORMANCE_OPTIMIZATIONS "Enable performance optimizations" ON)
option(ENABLE_VALIDATION "Enable validation and error checking" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Performance optimization settings
if (ENABLE_PERFORMANCE_OPTIMIZATIONS)
    add_compile_options(-O3 -march=native -flto)
    add_link_options(-flto)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# Memory pool and event queue settings
add_compile_definitions(NLM_MEMORY_POOL NLM_EVENT_QUEUE NLM_SPARSE_CONNECTIVITY)

# Build different components
include(FetchContent)

# Core library
add_subdirectory(src/core)

# Brain module (main integration point)
add_subdirectory(src/brain)

# Dynamics
add_subdirectory(src/dynamics)

# Plasticity
add_subdirectory(src/plasticity)

# Development
add_subdirectory(src/development)

# Neuromodulation
add_subdirectory(src/neuromodulation)

# Memory systems
add_subdirectory(src/memory)

# Prediction system
add_subdirectory(src/prediction)

# Cognition systems
add_subdirectory(src/cognition)

# Sensory system
add_subdirectory(src/sensory)

# Motor system
add_subdirectory(src/motor)

# Environment
add_subdirectory(src/environment)

# Experiments
add_subdirectory(src/experiments)

# Tests
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(src/tests)
endif()

# Demos
if (BUILD_DEMOS)
    add_subdirectory(src/demos)
endif()

# Python bindings
if (BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# Installation
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY lib/ DESTINATION lib)
install(DIRECTORY bin/ DESTINATION bin)

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Generate compile_commands.json for tooling
if (CMAKE_EXPORT_COMPILE_COMMANDS)
    file(WRITE ${CMAKE_BINARY_DIR}/compile_commands.json "{}")
endif()

# Print build summary
message(STATUS "========================================")
message(STATUS "NLM Brain Simulator Build Configuration")
message(STATUS "========================================")
message(STATUS "Project version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build tests: ${BUILD_TESTS}")
message(STATUS "Build demos: ${BUILD_DEMOS}")
message(STATUS "Build Python bindings: ${BUILD_PYTHON_BINDINGS}")
message(STATUS "Performance optimizations: ${ENABLE_PERFORMANCE_OPTIMIZATIONS}")
message(STATUS "Validation enabled: ${ENABLE_VALIDATION}")
message(STATUS "Build directory: ${CMAKE_BINARY_DIR}")
message(STATUS "Runtime output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "Library output: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message(STATUS "========================================")

# Function to validate build configuration
function(validate_build_configuration)
    message(STATUS "Validating build configuration...")
    
    # Check for required source files
    set(REQUIRED_CORE_FILES
        src/core/Config/Config.hpp
        src/core/Random/Random.hpp
        src/core/Logger/Logger.hpp
        src/core/SimulationClock/SimulationClock.hpp
    )
    
    foreach(file ${REQUIRED_CORE_FILES})
        if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${file})
            message(FATAL_ERROR "Required source file not found: ${file}")
        endif()
    endforeach()
    
    # Check for Brain integration files
    set(REQUIRED_BRAIN_FILES
        src/brain/Brain.hpp
        src/brain/Brain.cpp
    )
    
    foreach(file ${REQUIRED_BRAIN_FILES})
        if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${file})
            message(FATAL_ERROR "Required brain file not found: ${file}")
        endif()
    endforeach()
    
    # Check for memory system files
    set(REQUIRED_MEMORY_FILES
        src/memory/NeuralWorkingMemory.hpp
        src/memory/NeuralEpisodicMemory.hpp
        src/memory/NeuralAssociativeMemory.hpp
    )
    
    foreach(file ${REQUIRED_MEMORY_FILES})
        if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${file})
            message(FATAL_ERROR "Required memory file not found: ${file}")
        endif()
    endforeach()
    
    message(STATUS "Build configuration validation passed")
endfunction()

# Call validation function
validate_build_configuration()
