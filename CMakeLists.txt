cmake_minimum_required(VERSION 3.16)

project(nlm VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_TESTS "Build test executable" ON)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" OFF)

# Source directories
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Include directories
include_directories(${SRC_DIR})

# Core library
file(GLOB_RECURSE CORE_SOURCES "${SRC_DIR}/core/*.cpp")
add_library(nlm_core ${CORE_SOURCES})

# Brain library
file(GLOB_RECURSE BRAIN_SOURCES "${SRC_DIR}/brain/*.cpp")
add_library(nlm_brain ${BRAIN_SOURCES})

# Dynamics library
file(GLOB_RECURSE DYNAMICS_SOURCES "${SRC_DIR}/dynamics/*.cpp")
add_library(nlm_dynamics ${DYNAMICS_SOURCES})

# Plasticity library
file(GLOB_RECURSE PLASTICITY_SOURCES "${SRC_DIR}/plasticity/*.cpp")
add_library(nlm_plasticity ${PLASTICITY_SOURCES})

# Memory library
file(GLOB_RECURSE MEMORY_SOURCES "${SRC_DIR}/memory/*.cpp")
add_library(nlm_memory ${MEMORY_SOURCES})

# Integration of libraries
# Dependencies are managed through header includes, so we just need to link them appropriately

# Tests
if(BUILD_TESTS)
    enable_testing()
    
    # Test executable
    add_executable(nlm_test
        tests/test_main.cpp
        tests/test_types.cpp
        tests/test_config.cpp
        tests/test_random.cpp
        tests/test_clock.cpp
        tests/test_neuron.cpp
        tests/test_synapse.cpp
        tests/test_stdp.cpp
        tests/test_brain.cpp
        tests/test_neural_dynamics.cpp
    )
    
    target_link_libraries(nlm_test PRIVATE nlm_core nlm_brain nlm_dynamics nlm_plasticity nlm_memory)
    target_compile_features(nlm_test PRIVATE cxx_std_20)
    
    add_test(NAME nlm_test COMMAND nlm_test)
endif()

# Install targets (for development)
install(TARGETS nlm_core nlm_brain nlm_dynamics nlm_plasticity nlm_memory
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

# Print configuration summary
message(STATUS "NLM Build Configuration:")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  Build Python bindings: ${BUILD_PYTHON_BINDINGS}")
message(STATUS "  Shared libraries: ${BUILD_SHARED_LIBS}")
