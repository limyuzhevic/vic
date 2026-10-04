cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configuration options
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Profile" CACHE STRING "Available build types" FORCE)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
endif()

# Configuration-specific options
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -DDEBUG)
    add_definitions(-DNLM_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Profile")
    add_compile_options(-O2 -g -pg -DNLM_PROFILE)
    add_definitions(-DNLM_PROFILE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -DNDEBUG)
    add_definitions(-DNLM_RELEASE)
endif()

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic )
endif()

# Platform-specific optimizations
if(UNIX AND NOT APPLE)
    add_compile_options(-march=native -fopenmp)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Performance optimization flags for different configurations
set(DEBUG_OPT_FLAGS "-g -O0 -DNLM_DEBUG")
set(PROFILE_OPT_FLAGS "-O2 -g -pg -DNLM_PROFILE")
set(RELEASE_OPT_FLAGS "-O3 -DNDEBUG -DNLM_RELEASE -ffast-math")

# Set default optimization flags based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEBUG_OPT_FLAGS}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Profile")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PROFILE_OPT_FLAGS}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${RELEASE_OPT_FLAGS}")
endif()

# CLI11 dependency
include(FetchContent)
FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.4.2
)
FetchContent_MakeAvailable(CLI11)

# Phase configuration options
set(PHASE_TO_BUILD "ALL" CACHE STRING "Phases to build (ALL, 2, 3, 4, 5, 6)")

# Build phase targets
add_custom_target(build_phase_2
    COMMAND ${CMAKE_COMMAND} -E echo "Building Phase 2 (Core foundation)"
    COMMENT "Building Phase 2: Core neural foundation"
)

add_custom_target(build_phase_3
    COMMAND ${CMAKE_COMMAND} -E echo "Building Phase 3 (World interaction)"
    COMMENT "Building Phase 3: Sensory-motor and reward systems"
)

add_custom_target(build_phase_4
    COMMAND ${CMAKE_COMMAND} -E echo "Building Phase 4 (Advanced cognition)"
    COMMENT "Building Phase 4: Working memory, episodic memory, planning"
)

add_custom_target(build_phase_5
    COMMAND ${CMAKE_COMMAND} -E echo "Building Phase 5 (Performance optimizations)"
    COMMENT "Building Phase 5: Performance optimizations and checkpointing"
)

add_custom_target(build_phase_6
    COMMAND ${CMAKE_COMMAND} -E echo "Building Phase 6 (Final integration)"
    COMMENT "Building Phase 6: Final integration and demonstration"
)

# Add Version directory to include path
include_directories(src/core/Version)

# Core library
add_library(nlm_core STATIC
    # Core
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    src/core/Version/Version.cpp
    # Brain
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    # Dynamics
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
    # Plasticity
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
    # Development
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
    # Neuromodulation
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
    # Memory
    src/memory/Memory.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
    # Cognition
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
    # Sensory
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
    # Prediction
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
    # Motor
    src/motor/MotorSystem.cpp
    # Environment
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    # Experiments
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    # Visualization
    src/visualization/VisualizationInterface.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core)

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

# Phase 5 experiments
add_library(nlm_phase5 STATIC
    src/experiments/Phase5Experiment.cpp
)
target_include_directories(nlm_phase5 PUBLIC src)
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 6 experiments
add_library(nlm_phase6 STATIC
    src/experiments/Phase6IntegratedExperiment.cpp
)
target_include_directories(nlm_phase6 PUBLIC src)
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5)

# Main executable
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Phase 5 demo executable
add_executable(nlm_phase5_demo
    src/experiments/Phase5Demo.cpp
)
target_link_libraries(nlm_phase5_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase5)

# Phase 6 demo executable
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6)

# Build-time execution capabilities
add_custom_target(run_build_time_validation
    COMMAND echo "Running build-time validation..."
    COMMAND ./nlm --validate-config
    COMMAND ./nlm --benchmark build_time
    COMMENT "Build-time validation and benchmarking"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_target(run_phase_3_validation
    COMMAND echo "Running Phase 3 validation..."
    COMMAND ./nlm_phase3_demo --run-phase3
    COMMENT "Phase 3 build-time execution"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_target(run_phase_4_validation
    COMMAND echo "Running Phase 4 validation..."
    COMMAND ./nlm_phase4_demo --run-phase4
    COMMENT "Phase 4 build-time execution"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_target(run_phase_6_integration
    COMMAND echo "Running Phase 6 integration test..."
    COMMAND ./nlm_phase6_demo --run-phase6
    COMMENT "Phase 6 build-time integration"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Resource management during build
add_custom_target(check_build_resources
    COMMAND echo "Checking build resource usage..."
    COMMAND /usr/bin/time -v cmake --build . 2>/dev/null | grep -E "Maximum resident|Elapsed"
    COMMENT "Resource usage analysis"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Installation targets with improved directory structure
install(DIRECTORY src/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "CMakeLists.txt" EXCLUDE
)

install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Installation of demo executables
install(TARGETS nlm nlm_phase3_demo nlm_phase4_demo nlm_phase5_demo nlm_phase6_demo
    RUNTIME DESTINATION bin
)

# Package creation target
add_custom_target(package_all
    COMMAND ${CMAKE_COMMAND} -E tar cf NLM-Package.tar.gz
        --format=gnu
        --mode=a
        --owner=root
        --group=root
        .
    COMMENT "Creating NLM package"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Documentation generation targets
find_program(DOXYGEN_EXECUTABLE doxygen)
if(DOXYGEN_EXECUTABLE)
    add_custom_target(generate_docs
        COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
        COMMENT "Generating API documentation"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
else()
    message(WARNING "Doxygen not found - documentation generation disabled")
endif()

add_custom_target(copy_docs
    COMMAND ${CMAKE_COMMAND} -E copy_directory docs html_docs
    COMMENT "Copying documentation to html_docs directory"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(build_all_phases
    COMMAND cmake --build . --target build_phase_2
    COMMAND cmake --build . --target build_phase_3
    COMMAND cmake --build . --target build_phase_4
    COMMAND cmake --build . --target build_phase_5
    COMMAND cmake --build . --target build_phase_6
    COMMENT "Building all phases"
)

# Integration with CI/CD
add_custom_target(ci_checks
    COMMAND echo "Running CI checks..."
    COMMAND if [ -f "${CMAKE_BINARY_DIR}/nlm" ]; then echo "Main executable exists"; else echo "Main executable missing"; exit 1; fi
    COMMAND if [ -f "${CMAKE_BINARY_DIR}/nlm_test" ]; then echo "Tests built successfully"; else echo "Tests not built"; exit 1; fi
    COMMAND ./nlm --version
    COMMENT "CI validation checks"
)

# Benchmarking target
add_custom_target(benchmark_all
    COMMAND echo "Running all benchmarks..."
    COMMAND ./nlm --benchmark full
    COMMAND ./nlm_phase3_demo --benchmark
    COMMAND ./nlm_phase4_demo --benchmark
    COMMAND ./nlm_phase6_demo --benchmark
    COMMENT "Running all benchmarks"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Continuous integration integration
enable_testing()
add_test(NAME ci_validation COMMAND ./nlm --version)
add_test(NAME unit_tests COMMAND ./nlm_test)
add_test(NAME validation_tests COMMAND ./validation_test)

# Phase-specific test targets
add_test(NAME phase_3_tests COMMAND ./nlm_phase3_demo --run-phase3)
add_test(NAME phase_4_tests COMMAND ./nlm_phase4_demo --run-phase4)
add_test(NAME phase_5_tests COMMAND ./nlm_phase5_demo --run-phase5)
add_test(NAME phase_6_tests COMMAND ./nlm_phase6_demo --run-phase6)

# Testing
enable_testing()
add_subdirectory(tests)

# Additional test targets for backward compatibility
add_custom_target(run_all_tests
    COMMAND echo "=== Running All Tests ==="
    COMMAND ./unit_core_test
    COMMAND ./unit_brain_test
    COMMAND ./unit_dynamics_test
    COMMAND ./integration_tests
    COMMAND ./performance_test
    COMMAND ./phase_3_test
    COMMAND ./phase_4_test
    COMMAND ./phase_5_test
    COMMAND ./phase_6_test
    COMMAND ./nlm_test
    COMMAND ./validation_test
    COMMENT "Running all tests"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Linting targets
find_program(CLANG_FORMAT "clang-format")
find_program(CLANG_TIDY "clang-tidy")
find_program(CPACHECK "cppcheck")

if(CLANG_FORMAT AND CLANG_TIDY AND CPACHECK)
    add_custom_target(linting_all
        COMMAND ${CMAKE_COMMAND} -P ${PROJECT_SOURCE_DIR}/cmake/linting.cmake
        COMMENT "Running all linting tools"
        VERBATIM
    )
    
    add_custom_target(linting_format
        COMMAND echo "Running code formatting check..."
        COMMAND ${CLANG_FORMAT} --version
        COMMAND for file in ${SRC_FILE_PATTERNS}; do
            ${CLANG_FORMAT} --dry-run -Werror ${file} || exit 1
        done
        COMMENT "Checking code formatting with clang-format"
        VERBATIM
    )
    
    add_custom_target(linting_tidy
        COMMAND echo "Running clang-tidy analysis..."
        COMMAND ${CLANG_TIDY} --version
        COMMAND for file in ${SRC_FILES}; do
            ${CLANG_TIDY} ${file} -- -I${PROJECT_SOURCE_DIR}/src || exit 1
        done
        COMMENT "Running clang-tidy analysis"
        VERBATIM
    )
    
    add_custom_target(linting_static
        COMMAND echo "Running cppcheck analysis..."
        COMMAND ${CPACHECK} --version
        COMMAND for file in ${SRC_FILES_STATIC}; do
            ${CPACHECK} --enable=all --inconclusive --std=c++20 ${file} || exit 1
        done
        COMMENT "Running cppcheck static analysis"
        VERBATIM
    )
    
    # Set source file patterns for linting
    file(GLOB_RECURSE SRC_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")
    file(GLOB_RECURSE SRC_FILES_STATIC "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.hpp")
    set(SRC_FILE_PATTERNS ${SRC_FILES})
else()
    message(WARNING "One or more linting tools not found: clang-format=${CLANG_FORMAT}, clang-tidy=${CLANG_TIDY}, cppcheck=${CPACHECK}")
endif()

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6 nlm_phase3_demo nlm_phase4_demo nlm_phase5_demo nlm_phase6_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)