cmake_minimum_required(VERSION 3.16)
project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Neural Learning Machine - Brain Simulation Framework"
    HOMEPAGE_URL "https://github.com/nlm-project/nlm"
)

# Set C++ standard and compiler features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Project-wide compile options
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Build configurations
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Platform-specific settings
if(WIN32)
    add_definitions(-D_WIN32 -D_WINDOWS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
elseif(APPLE)
    add_definitions(-D_DARWIN)
elseif(UNIX)
    add_definitions(-D_LINUX)
endif()

# Source directory structure
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(NLM_INCLUDE_DIR "${NLM_SOURCE_DIR}")
set(NLM_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/configs")
set(NLM_DOCS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/docs")

# Package configuration
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Compiler feature detection
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++20" COMPILER_SUPPORTS_CXX20)
if(NOT COMPILER_SUPPORTS_CXX20)
    message(FATAL_ERROR "Compiler does not support C++20")
endif()

# Memory model detection
include(CheckCXXSourceCompiles)
check_cxx_source_compiles("
#include <thread>
int main() {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return 0;
}
" COMPILER_HAS_ATOMIC)
if(COMPILER_HAS_ATOMIC)
    add_definitions(-DHAVE_ATOMIC)
endif()

# Core library: nlm_core
add_library(nlm_core STATIC
    # Core utilities
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
    
    # Brain components
    ${NLM_SOURCE_DIR}/brain/Neuron.cpp
    ${NLM_SOURCE_DIR}/brain/Synapse.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp
    ${NLM_SOURCE_DIR}/brain/Brain.cpp
    
    # Neural dynamics
    ${NLM_SOURCE_DIR}/dynamics/NeuralDynamics.cpp
    ${NLM_SOURCE_DIR}/dynamics/SpikeSystem.cpp
    
    # Plasticity rules
    ${NLM_SOURCE_DIR}/plasticity/PlasticityRule.cpp
    ${NLM_SOURCE_DIR}/plasticity/Hebbian.cpp
    ${NLM_SOURCE_DIR}/plasticity/STDP.cpp
    ${NLM_SOURCE_DIR}/plasticity/StructuralPlasticity.cpp
    
    # Development system
    ${NLM_SOURCE_DIR}/development/DevelopmentSystem.cpp
    ${NLM_SOURCE_DIR}/development/Synaptogenesis.cpp
    ${NLM_SOURCE_DIR}/development/Pruning.cpp
    ${NLM_SOURCE_DIR}/development/Maturation.cpp
    
    # Neuromodulation
    ${NLM_SOURCE_DIR}/neuromodulation/Neuromodulator.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Reward.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/PredictionError.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Novelty.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Curiosity.cpp
    
    # Memory systems
    ${NLM_SOURCE_DIR}/memory/WorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/EpisodicMemory.cpp
    ${NLM_SOURCE_DIR}/memory/SemanticMemory.cpp
    ${NLM_SOURCE_DIR}/memory/ProceduralMemory.cpp
    ${NLM_SOURCE_DIR}/memory/AssociativeMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralWorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralEpisodicMemory.cpp
    
    # Cognition
    ${NLM_SOURCE_DIR}/cognition/ConceptFormation.cpp
    ${NLM_SOURCE_DIR}/cognition/NeuralPlanner.cpp
    
    # Sensory processing
    ${NLM_SOURCE_DIR}/sensory/SensoryInput.cpp
    ${NLM_SOURCE_DIR}/sensory/Vision.cpp
    ${NLM_SOURCE_DIR}/sensory/Audio.cpp
    ${NLM_SOURCE_DIR}/sensory/InternalSignals.cpp
    
    # Prediction systems
    ${NLM_SOURCE_DIR}/prediction/PredictionSystem.cpp
    ${NLM_SOURCE_DIR}/prediction/NeuralPrediction.cpp
    
    # Motor system
    ${NLM_SOURCE_DIR}/motor/MotorSystem.cpp
    
    # Environment interface
    ${NLM_SOURCE_DIR}/environment/Environment.cpp
    ${NLM_SOURCE_DIR}/environment/Observation.cpp
    ${NLM_SOURCE_DIR}/environment/Action.cpp
    
    # Experiments
    ${NLM_SOURCE_DIR}/experiments/Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/ExperimentRunner.cpp
    ${NLM_SOURCE_DIR}/experiments/Metrics.cpp
    ${NLM_SOURCE_DIR}/experiments/Phase3Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/Phase4Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/Phase6IntegratedExperiment.cpp
    
    # Visualization
    ${NLM_SOURCE_DIR}/visualization/VisualizationInterface.cpp
)

# Core library include directories
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Core library compile definitions
target_compile_definitions(nlm_core PUBLIC
    NLM_CORE
)

# Agent system
add_library(nlm_agent STATIC
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBody.hpp
)

target_include_directories(nlm_agent PUBLIC ${NLM_INCLUDE_DIR})
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system
add_library(nlm_world STATIC
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
    ${NLM_SOURCE_DIR}/world/Observation.hpp
)

target_include_directories(nlm_world PUBLIC ${NLM_INCLUDE_DIR})
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    ${NLM_SOURCE_DIR}/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC ${NLM_INCLUDE_DIR})
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    ${NLM_SOURCE_DIR}/experiments/Phase4Experiment.cpp
)

target_include_directories(nlm_phase4 PUBLIC ${NLM_INCLUDE_DIR})
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 6 experiments
add_library(nlm_phase6 STATIC
    ${NLM_SOURCE_DIR}/experiments/Phase6Demo.cpp
)

target_include_directories(nlm_phase6 PUBLIC ${NLM_INCLUDE_DIR})
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Main executable: nlm
add_executable(nlm
    ${NLM_SOURCE_DIR}/main.cpp
)

target_link_libraries(nlm PUBLIC nlm_core)
target_include_directories(nlm PUBLIC ${NLM_INCLUDE_DIR})

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    ${NLM_SOURCE_DIR}/experiments/Phase3Demo.cpp
)

target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    ${NLM_SOURCE_DIR}/experiments/Phase4Demo.cpp
)

target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Phase 6 demo executable
add_executable(nlm_phase6_demo
    ${NLM_SOURCE_DIR}/experiments/Phase6Demo.cpp
)

target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase6)

# Python bindings
if(Python_Interpreter_FOUND)
    # Find Python components
    find_package(Python 3.8 REQUIRED COMPONENTS Interpreter Development)
    
    # Find pybind11
    find_package(pybind11 REQUIRED)
    
    # Create Python module
    pybind11_add_module(pynlm
        python/wrapper.cpp
        python/bind_core_types.cpp
        python/bind_brain.cpp
        python/bind_environment.cpp
        python/bind_agent.cpp
        python/bind_experiments.cpp
    )
    
    target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
    target_include_directories(pynlm PRIVATE ${Python_INCLUDE_DIRS})
    
    # Install Python module
    install(TARGETS pymodule
        LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES}
        RUNTIME DESTINATION bin
    )
endif()

# Testing
enable_testing()

# Add test subdirectory
add_subdirectory(tests)

# Documentation generation
if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        
        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
    endif()
endif()

# Installation targets
include(GNUInstallDirs)

# Install core library
install(TARGETS nlm_core
    EXPORT NLMLTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install agent library
install(TARGETS nlm_agent
    EXPORT NLMLTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install world library
install(TARGETS nlm_world
    EXPORT NLMLTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install experiment libraries
install(TARGETS nlm_phase3 nlm_phase4 nlm_phase6
    EXPORT NLMLTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install executables
install(TARGETS nlm nlm_phase3_demo nlm_phase4_demo nlm_phase6_demo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    BUNDLE DESTINATION ${CMAKE_INSTALL_BINDIR}  # macOS
)

# Install headers
install(DIRECTORY ${NLM_INCLUDE_DIR}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN ".*" EXCLUDE
)

# Install configuration files
install(DIRECTORY ${NLM_CONFIG_DIR}/
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/nlm
    FILES_MATCHING PATTERN "*.cfg"
    PATTERN ".*" EXCLUDE
)

# Install documentation
install(DIRECTORY ${NLM_DOCS_DIR}/
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
    FILES_MATCHING PATTERN "*.md"
    PATTERN ".*" EXCLUDE
)

# Install Python bindings
if(Python_Interpreter_FOUND)
    install(FILES ${NLM_SOURCE_DIR}/python/__init__.py
        DESTINATION ${PYTHON_SITE_PACKAGES}/pynlm
    )
    
    install(FILES ${NLM_SOURCE_DIR}/python/wrapper.cpp
              ${NLM_SOURCE_DIR}/python/bind_core_types.cpp
              ${NLM_SOURCE_DIR}/python/bind_brain.cpp
              ${NLM_SOURCE_DIR}/python/bind_environment.cpp
              ${NLM_SOURCE_DIR}/python/bind_agent.cpp
              ${NLM_SOURCE_DIR}/python/bind_experiments.cpp
        DESTINATION ${PYTHON_SITE_PACKAGES}/pynlm/bindings
    )
endif()

# Generate and install package configuration
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

# Export targets
install(EXPORT NLMLTargets
    FILE NLMLTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

# Build options
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build example executables" ON)
option(BUILD_PYTHON "Build Python bindings" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(BUILD_VISUALIZATION "Build visualization support" ON)
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(ENABLE_UBSAN "Enable Undefined Behavior Sanitizer" OFF)
option(ENABLE_THREAD_SANITIZER "Enable Thread Sanitizer" OFF)

# Compiler flags based on build options
if(ENABLE_COVERAGE)
    add_compile_options(--coverage)
    add_link_options(--coverage)
endif()

if(ENABLE_ASAN)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif()

if(ENABLE_UBSAN)
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)
endif()

if(ENABLE_THREAD_SANITIZER)
    add_compile_options(-fsanitize=thread)
    add_link_options(-fsanitize=thread)
endif()

# Create build information file
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/build_info.txt
    "NLM Build Information\n"
    "=======================\n"
    "Project: NLM ${PROJECT_VERSION}\n"
    "Build Type: ${CMAKE_BUILD_TYPE}\n"
    "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}\n"
    "Build Directory: ${CMAKE_CURRENT_BINARY_DIR}\n"
    "Source Directory: ${CMAKE_CURRENT_SOURCE_DIR}\n"
    "Generated: ${CMAKE_SYSTEM_PROCESSOR}\n"
    "\n"
    "Target Libraries:\n"
    "  - nlm_core (core neural computation)\n"
    "  - nlm_agent (agent interface)\n"
    "  - nlm_world (world simulation)\n"
    "  - nlm_phase3 (Phase 3 experiments)\n"
    "  - nlm_phase4 (Phase 4 experiments)\n"
    "  - nlm_phase6 (Phase 6 experiments)\n"
    "\n"
    "Executables:\n"
    "  - nlm (main simulation)\n"
    "  - nlm_phase3_demo (Phase 3 demo)\n"
    "  - nlm_phase4_demo (Phase 4 demo)\n"
    "  - nlm_phase6_demo (Phase 6 demo)\n"
)

# Add custom targets
add_custom_target(build_info
    COMMAND ${CMAKE_COMMAND} -E echo "Build information written to: ${CMAKE_CURRENT_BINARY_DIR}/build_info.txt"
    COMMENT "Generating build information file"
)

# Add install target for build info
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/build_info.txt
    DESTINATION ${CMAKE_INSTALL_DATAROOT}/nlm/build_info.txt
)

# Create symlink for easier access to build directory
add_custom_target(symlink_build
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/build
    COMMENT "Creating build symlink"
)

message(STATUS "NLM Build Configuration:")
message(STATUS "  Project Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Python Bindings: ${BUILD_PYTHON}")
message(STATUS "  Tests: ${BUILD_TESTS}")
message(STATUS "  Examples: ${BUILD_EXAMPLES}")
message(STATUS "  Visualization: ${BUILD_VISUALIZATION}")