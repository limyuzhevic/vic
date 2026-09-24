cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Python bindings support
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

# Set Python module output directory
set(PYTHON_MODULE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/python")

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

# Python bindings
# Find pybind11
find_package(pybind11 REQUIRED)

# Python bindings for NLM
pybind11_add_module(pynlm python/bindings.cpp)
target_include_directories(pynlm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${PYTHON_INCLUDE_DIRS}
)
target_link_libraries(pynlm PRIVATE nlm_core)
set_target_properties(pynlm PROPERTIES
    OUTPUT_NAME "pynlm"
    PREFIX ""
    SUFFIX ".so"
)

# Copy Python module to build directory
add_custom_command(TARGET pynlm POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PYTHON_MODULE_OUTPUT_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:pynlm> ${PYTHON_MODULE_OUTPUT_DIR}/
    COMMENT "Copying Python module to build directory"
)

# Testing
enable_testing()
add_subdirectory(tests)

# Documentation generation
find_program(DOT doxygen PATHS /usr/bin /usr/local/bin REQUIRED)

# Configure documentation
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile
    @ONLY
)

# Add custom targets for documentation
add_custom_target(docs
    COMMAND ${DOT} docs/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating documentation"
    VERBATIM
)

# Add custom targets for Python module installation
add_custom_target(python_module
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PYTHON_MODULE_OUTPUT_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:pynlm> ${PYTHON_MODULE_OUTPUT_DIR}/
    COMMENT "Creating Python module distribution"
    DEPENDS pynlm
)

# Install Python module
install(FILES ${PYTHON_MODULE_OUTPUT_DIR}/pynlm.so
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/python3.8/site-packages
    OPTIONAL
)

# Install configuration files
install(DIRECTORY configs/
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/nlm
    FILES_MATCHING PATTERN "*.cfg"
)

# Install documentation
install(DIRECTORY docs/
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/nlm/doc
    FILES_MATCHING PATTERN "*.md"
)

# Install convenience module documentation
install(FILES python/NLM_CONVENIENCE.md
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/nlm/doc
)

# Install improvements summary
install(FILES IMPROVEMENTS_SUMMARY.md
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/nlm/doc
)

# Add test targets for Python bindings
enable_testing()

# Find Python for testing
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Add Python test target
add_test(NAME python_bindings_test
    COMMAND ${Python3_EXECUTABLE} -c "
import sys
sys.path.insert(0, '${PYTHON_MODULE_OUTPUT_DIR}')
try:
    import pynlm
    print('Python bindings test PASSED')
    sys.exit(0)
except Exception as e:
    print(f'Python bindings test FAILED: {e}')
    sys.exit(1)
"
)

# Set test properties

# Add custom target for running all tests
add_custom_target(runtests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    COMMENT "Running all tests"
    VERBATIM
)

# Summary
message(STATUS "NLM Build Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Python bindings: ${Python3_EXECUTABLE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Core library: nlm_core")
message(STATUS "  Agent library: nlm_agent")
message(STATUS "  World library: nlm_world")
message(STATUS "  Python module: pynlm")
message(STATUS "  Documentation target: docs")
message(STATUS "  Python module target: python_module")
message(STATUS "  Test target: runtests")

# Create a summary file with build information
file(WRITE ${CMAKE_BINARY_DIR}/BUILD_INFO.txt
    "NLM Build Summary:\n"
    "==================\n\n"
    "Build type: ${CMAKE_BUILD_TYPE}\n"
    "C++ standard: ${CMAKE_CXX_STANDARD}\n"
    "Python interpreter: ${Python3_EXECUTABLE}\n"
    "\n"
    "Targets:\n"
    "  - nlm (main executable)\n"
    "  - nlm_core (core library)\n"
    "  - nlm_agent (agent library)\n"
    "  - nlm_world (world library)\n"
    "  - nlm_phase3 (phase 3 experiments)\n"
    "  - nlm_phase4 (phase 4 experiments)\n"
    "  - nlm_phase3_demo (phase 3 demo)\n"
    "  - nlm_phase4_demo (phase 4 demo)\n"
    "  - pynlm (Python bindings)\n"
    "\n"
    "Install targets:\n"
    "  - bin/nlm\n"
    "  - lib/libnlm_core.a\n"
    "  - lib/libnlm_agent.a\n"
    "  - lib/libnlm_world.a\n"
    "  - lib/libnlm_phase3.a\n"
    "  - lib/libnlm_phase4.a\n"
    "  - python/pynlm.so\n"
)

# Add a custom target to show build information
add_custom_target(show_build_info
    COMMAND echo "Build information written to ${CMAKE_BINARY_DIR}/BUILD_INFO.txt"
    COMMAND cat ${CMAKE_BINARY_DIR}/BUILD_INFO.txt
    COMMENT "Showing build information"
    VERBATIM
)