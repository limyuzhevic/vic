cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler optimizations
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-O3 -march=native -flto -fvisibility=hidden -fPIC)
endif()

# Project include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Config
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Logger
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Random
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/SimulationClock
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Types
    ${CMAKE_CURRENT_SOURCE_DIR}/src/brain
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dynamics
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plasticity
    ${CMAKE_CURRENT_SOURCE_DIR}/src/memory
    ${CMAKE_CURRENT_SOURCE_DIR}/src/prediction
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cognition
    ${CMAKE_CURRENT_SOURCE_DIR}/src/neuromodulation
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensory
    ${CURRENT_SOURCE_DIR}/src/motor
    ${CMAKE_CURRENT_SOURCE_DIR}/src/environment
    ${CMAKE_CURRENT_SOURCE_DIR}/src/experiments
    ${CMAKE_CURRENT_SOURCE_DIR}/src/performance
    ${CMAKE_CURRENT_SOURCE_DIR}/src/development
    ${CMAKE_CURRENT_SOURCE_DIR}/src/visualization
    ${CMAKE_CURRENT_SOURCE_DIR}/src/agent
)

# Library targets
add_library(nlm_core STATIC
    src/core/Config/Config.cpp
    src/core/Logger/Logger.cpp
    src/core/Random/Random.cpp
    src/core/SimulationClock/SimulationClock.cpp
)

add_library(nlm_brain STATIC
    src/brain/Brain.cpp
    src/brain/Neuron.cpp
    src/brain/NeuralRegion.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/Synapse.cpp
)

add_library(nlm_plasticity STATIC
    src/plasticity/STDP.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/StructuralPlasticity.cpp
)

add_library(nlm_memory STATIC
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
    src/memory/NeuralAssociativeMemory.cpp
)

add_library(nlm_prediction STATIC
    src/prediction/PredictionSystem.cpp
)

add_library(nlm_cognition STATIC
    src/cognition/NeuralPlanner.cpp
    src/cognition/ConceptFormation.cpp
    src/cognition/AttentionalSelection.cpp
)

add_library(nlm_neuromodulation STATIC
    src/neuromodulation/Dopamine.cpp
    src/neuromodulation/Curiosity.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/PredictionError.cpp
)

add_library(nlm_dynamics STATIC
    src/dynamics/SpikeSystem.cpp
)

add_library(nlm_development STATIC
    src/development/DevelopmentSystem.cpp
)

# Main executable
add_executable(nlm_phase2_main src/main.cpp)
target_link_libraries(nlm_phase2_main
    nlm_core
    nlm_brain
    nlm_plasticity
    nlm_memory
    nlm_prediction
    nlm_cognition
    nlm_neuromodulation
    nlm_dynamics
    nlm_development
)

# Python bindings (optional)
find_package(Python3 COMPONENTS Interpreter Development)
if(Python3_Interpreter_FOUND AND Python3_Development_FOUND)
    find_package(pybind11 REQUIRED)
    
    pybind11_add_module(pynlm bindings.cpp)
    target_link_libraries(pynlm PRIVATE
        nlm_core
        nlm_brain
        nlm_plasticity
        nlm_memory
        nlm_prediction
        nlm_cognition
        nlm_neuromodulation
        nlm_dynamics
        nlm_development
        Python3::Python
    )
endif()

# Installation targets
install(TARGETS nlm_core nlm_brain nlm_plasticity nlm_memory nlm_prediction
        nlm_cognition nlm_neuromodulation nlm_dynamics nlm_development
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(TARGETS nlm_phase2_main
        RUNTIME DESTINATION bin)

# Configuration files
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/default.cfg
    DESTINATION etc/nlm
)

# Documentation
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/ARCHITECTURE.md
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/SCIENCE.md
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/ROADMAP.md
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/EXPERIMENTS.md
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/LIMITATIONS.md
    DESTINATION share/doc/nlm
)

# Examples
glob_example_files(GLOB EXAMPLE_FILES "examples/*.cpp")
foreach(example ${EXAMPLE_FILES})
    get_filename_component(example_name ${example} NAME_WE)
    add_executable(${example_name} ${example})
    target_link_libraries(${example_name} nlm_core nlm_brain)
    install(TARGETS ${example_name} RUNTIME DESTINATION bin)
endforeach()

# Testing
enable_testing()
add_subdirectory(tests)

# Package configuration
include(GNUInstallDirs)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nlm
)

# Create package for find_package support
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/nlm-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nlm
)

# Summary
message(STATUS "NLM Build Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Core components: ${nlm_core} ${nlm_brain} ${nlm_plasticity} ${nlm_memory} ${nlm_prediction} ${nlm_cognition} ${nlm_neuromodulation} ${nlm_dynamics} ${nlm_development}")
message(STATUS "  Main executable: nlm_phase2_main")
message(STATUS "  Python bindings: ${Python3_Interpreter_FOUND}")
