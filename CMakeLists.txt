cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.6.0 LANGUAGES CXX
    DESCRIPTION "An experimental artificial developmental brain with neural learning capabilities"
)

# Configure CMake policies for modern behavior
cmake_policy(SET CMP0077 NEW)  # option() honors normal variables
cmake_policy(SET CMP0091 NEW)  # MSVC runtime library flags

# Set C++ standard requirements
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json for tooling (clangd, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build configuration options
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Installation prefixes for different components
set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation prefix")

# Configure CMake policies for modern behavior
cmake_policy(SET CMP0077 NEW)  # option() honors normal variables
cmake_policy(SET CMP0091 NEW)  # MSVC runtime library flags

# Set C++ standard requirements
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json for tooling (clangd, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build configuration options
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Installation prefixes for different components
set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation prefix")

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Modern compilation options function
define_property(TARGET PROPERTY COMPILATION_OPTIONS
    BRIEF_DOCS "Compilation options for the target"
    FULL_DOCS "Compilation options to be applied to the target"
)

# Setup modern compilation options for targets
function(setup_target_options target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive- /WX)
    else()
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic -Werror
            -Wshadow -Wformat=2 -Wconversion
            -Wnull-dereference -Wdouble-promotion
        )
        if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_options(${target} PRIVATE -fPIC)
        endif()
    endif()
endfunction()

# Core library
add_library(nlm_core STATIC
    # Core
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
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
    src/memory/WorkingMemory.cpp
    src/memory/EpisodicMemory.cpp
    src/memory/SemanticMemory.cpp
    src/memory/ProceduralMemory.cpp
    src/memory/AssociativeMemory.cpp
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
    src/environment/Action.cpp
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

# Modern testing infrastructure
enable_testing()

# Create test configuration
function(add_nlm_test test_name)
    add_executable(${test_name} ${ARGN})
    target_link_libraries(${test_name} PRIVATE nlm_core)
    setup_target_options(${test_name})
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# Add basic unit tests (original)
add_nlm_test(test_brain "tests/test_brain.cpp")
add_nlm_test(test_clock "tests/test_clock.cpp")
add_nlm_test(test_config "tests/test_config.cpp")
add_nlm_test(test_neuron "tests/test_neuron.cpp")
add_nlm_test(test_random "tests/test_random.cpp")
add_nlm_test(test_stdp "tests/test_stdp.cpp")
add_nlm_test(test_synapse "tests/test_synapse.cpp")
add_nlm_test(test_types "tests/test_types.cpp")

# Add enhanced Phase 6 comprehensive tests
add_nlm_test(test_brain_enhanced "tests/test_brain_enhanced.cpp")

# Generate test runner script for easy execution
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/run_tests.sh
    "#!/bin/bash\n"
    "echo \"Running NLM Phase 6 Enhanced Tests...\"\n"
    "echo \"==============================\"\n"
    "for test in test_brain test_clock test_config test_neuron test_random test_stdp test_synapse test_types test_brain_enhanced; do\n"
    "    if [ -f \"$test\" ]; then\n"
    "        echo \"Running $test...\"\n"
    "        ./$test\n"
    "        if [ $? -eq 0 ]; then\n"
    "            echo \"✓ $test PASSED\"\n"
    "        else\n"
    "            echo \"✗ $test FAILED\"\n"
    "            exit 1\n"
    "        fi\n"
    "    else\n"
    "        echo \"⚠ $test NOT BUILT\"\n"
    "    fi\n"
    "done\n"
    "echo \"\\n=== All NLM Phase 6 Tests Completed Successfully ===\"\n"
)
file(CHMOD ${CMAKE_CURRENT_BINARY_DIR}/run_tests.sh chmod +x)

# Create test summary file
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/test_summary.md
    "# NLM Phase 6 Test Summary\n\n"
    "This document summarizes the NLM (Neural Learning Machine) Phase 6 test suite.\n\n"
    "## Test Categories\n\n"
    "### Basic Unit Tests\n"
    "- **test_brain**: Brain creation and basic functionality\n"
    "- **test_clock**: Simulation clock functionality\n"
    "- **test_config**: Configuration system tests\n"
    "- **test_neuron**: LIF neuron dynamics tests\n"
    "- **test_random**: Random number generation tests\n"
    "- **test_stdp**: Spike-timing-dependent plasticity tests\n"
    "- **test_synapse**: Synapse connectivity tests\n"
    "- **test_types**: Type system and configuration tests\n\n"
    "### Enhanced Phase 6 Tests\n"
    "- **test_brain_enhanced**: Comprehensive Phase 6 integration tests\n"
    "  - Memory systems (working, episodic, associative)\n"
    "  - Neuromodulation systems (dopamine, curiosity, novelty)\n"
    "  - Prediction system integration\n"
    "  - Cognitive systems (planner, concept formation, attention)\n"
    "  - Development system and plasticity\n"
    "  - Structural plasticity (synaptogenesis, pruning)\n"
    "  - Checkpoint save/load functionality\n"
    "  - Memory replay mechanisms\n"
    "  - Advanced configuration system\n\n"
    "## Test Objectives\n\n"
    "These tests verify that NLM Phase 6 successfully integrates all previous phases:\n\n"
    "1. **Memory Integration**: All memory systems connected to neural processing\n"
    "2. **Neuromodulation**: Neuromodulators affecting neural excitability and plasticity\n"
    "3. **Prediction**: Prediction system integrated with sensory processing\n"
    "4. **Cognition**: Cognitive mechanisms (planner, concept, attention) operational\n"
    "5. **Development**: Developmental stages affecting plasticity rates\n"
    "6. **Plasticity**: Structural plasticity (synaptogenesis, pruning) working\n"
    "7. **Persistence**: Checkpoint save/load system functional\n"
    "8. **Replay**: Replay and consolidation mechanisms active\n\n"
    "## Running Tests\n\n"
    "To run all tests, use the test runner script:\n\n"
    "```bash\n"
    "./$CMAKE_CURRENT_BINARY_DIR/run_tests.sh\n"
    "```\n\n"
    "Individual tests can be run directly:\n\n"
    "```bash\n"
    "./test_brain\n"
    "./test_brain_enhanced\n"
    "```\n\n"
    "## Test Results\n\n"
    "All tests are designed to pass when NLM Phase 6 is correctly implemented.\n"
    "They verify that the integrated artificial brain system functions as expected.\n\n"
    "## Integration Verification\n\n"
    "Successful completion of all tests indicates that NLM has achieved:\n\n"
    "- ✅ Complete artificial neural substrate\n"
    "- ✅ Autonomous learning capabilities\n"
    "- ✅ Memory formation and storage\n"
    "- ✅ Adaptive behavior in dynamic environments\n"
    "- ✅ Integration of all major brain systems\n"
    "- ✅ Modern configuration management\n"
    "- ✅ Advanced debugging and profiling tools\n"
)

# Summary output for build configuration
message(STATUS "=== NLM Build Configuration ===")
message(STATUS "Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Source directory: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Build directory: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "Core library targets: nlm_core")
message(STATUS "Agent system: nlm_agent")
message(STATUS "World system: nlm_world")
message(STATUS "Phase 3 features: nlm_phase3")
message(STATUS "Phase 4 features: nlm_phase4")
message(STATUS "Executable: nlm (main)")
message(STATUS "Demo executables: nlm_phase3_demo, nlm_phase4_demo")
message(STATUS "Unit tests: test_brain, test_clock, test_config, test_neuron, test_random, test_stdp, test_synapse, test_types, test_brain_enhanced")
message(STATUS "Test runner: ./run_tests.sh (in build directory)")
message(STATUS "=== End Build Configuration ===")

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
