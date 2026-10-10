// Config.cmake for NLM Build System
// Generated automatically by build configuration system

cmake_minimum_required(VERSION 3.15)
project(NLM LANGUAGES CXX)

# NLM Configuration Options
set(NLM_VERSION "2.0" CACHE STRING "NLM version number")
set(NLM_BUILD_TYPE "Release" CACHE STRING "Build type (Debug/Release/RelWithDebInfo)")

# Neural Network Parameters
set(NEURON_COUNT 1000 CACHE STRING "Number of neurons in the network")
set(REGION_COUNT 1 CACHE STRING "Number of neural regions")
set(CONNECTION_PROBABILITY 0.1 CACHE STRING "Probability of random connections between neurons")
set(SIMULATION_TIMESTEP 0.001 CACHE STRING "Simulation timestep in seconds")

# Neuron Dynamics Parameters
set(NEURON_RESTING_POTENTIAL -70.0 CACHE STRING "Resting membrane potential (mV)")
set(NEURON_THRESHOLD -55.0 CACHE STRING "Firing threshold (mV)")
set(NEURON_RESET_POTENTIAL -70.0 CACHE STRING "Post-spike reset potential (mV)")
set(NEURON_LEAK_CONDUCTANCE 10.0 CACHE STRING "Membrane leak conductance (nS)")

# Plasticity Parameters
set(STDP_LTP_WEIGHT 0.02 CACHE STRING "STDP long-term potentiation weight")
set(STDP_LTD_WEIGHT 0.015 CACHE STRING "STDP long-term depression weight")
set(STDP_TAU 20.0 CACHE STRING "STDP time constant (ms)")

# Structural Plasticity Parameters
set(SYNAPTOGENESIS_RATE 0.0001 CACHE STRING "Rate of new synapse formation")
set(PRUNING_RATE 0.00001 CACHE STRING "Rate of synapse elimination")

# Development Parameters
set(DEVELOPMENT_CRITICAL_PERIOD_START 60.0 CACHE STRING "Start of critical period (simulation steps)")
set(DEVELOPMENT_CRITICAL_PERIOD_END 300.0 CACHE STRING "End of critical period (simulation steps)")
set(DEVELOPMENT_MATURATION_END 900.0 CACHE STRING "End of maturation period (simulation steps)")

# Neuromodulation Parameters
set(DOPAMINE_MAX_LEVEL 1.0 CACHE STRING "Maximum dopamine level")
set(CURIOSITY_WEIGHT_NOVELTY 2.0 CACHE STRING "Novelty contribution to curiosity")
set(CURIOSITY_WEIGHT_PREDICTION_ERROR 0.5 CACHE STRING "Prediction error contribution to curiosity")

# Memory System Parameters
set(WORKING_MEMORY_CAPACITY 100 CACHE STRING "Working memory capacity (neurons)")
set(EPISODIC_MEMORY_MAX_EPISODES 1000 CACHE STRING "Maximum episodic memory episodes")
set(ASSOCIATIVE_MEMORY_PATTERN_THRESHOLD 0.7 CACHE STRING "Pattern matching threshold for associative memory")

# Sensory Processing Parameters
set(SENSORY_VISION_WIDTH 16 CACHE STRING "Vision sensory width")
set(SENSORY_VISION_HEIGHT 16 CACHE STRING "Vision sensory height")
set(SENSORY_TOUCH_DIMENSION 8 CACHE STRING "Touch sensory dimension")
set(SENSORY_INTERNAL_DIMENSION 4 CACHE STRING "Internal signals dimension")
set(SENSORY_PROPRIOCEPTION_DIMENSION 6 CACHE STRING "Proprioception dimension")

# Motor System Parameters
set(MOTOR_ACTION_SPACE 8 CACHE STRING "Size of motor action space")
set(MOTOR_EXPLORATION_RATE 0.3 CACHE STRING "Base exploration rate (0.0-1.0)")

# Simulation Parameters
set(MAX_SIMULATION_STEPS 10000 CACHE STRING "Maximum simulation steps")
set(SIMULATION_OUTPUT_INTERVAL 100 CACHE STRING "Output interval (steps)")
set(RECORD_EVERY_STEP false CACHE STRING "Record all steps for replay")

# Checkpoint System
set(CHECKPOINT_DIRECTORY "./checkpoints" CACHE STRING "Checkpoint directory")
set(CHECKPOINT_MAX_FILES 10 CACHE STRING "Maximum checkpoint files to keep")
set(CHECKPOINT_COMPRESSION_LEVEL "balanced" CACHE STRING "Checkpoint compression level")

# Performance Optimization
set(ENABLE_SSE true CACHE STRING "Enable SSE optimizations")
set(ENABLE_OPENMP true CACHE STRING "Enable OpenMP parallelization")
set(ENABLE_NUMBA true CACHE STRING "Enable Numba JIT acceleration")
set(ENABLE_SIMD true CACHE STRING "Enable SIMD vectorization")

# Logging Configuration
set(LOG_LEVEL "info" CACHE STRING "Default log level (trace, debug, info, warn, error, critical)")
set(LOG_FILE_ENABLED true CACHE STRING "Enable file logging")
set(LOG_CONSOLE_ENABLED true CACHE STRING "Enable console logging")
set(LOG_ROTATION_SIZE_MB 100 CACHE STRING "Log file rotation size (MB)")

# Development and Testing
set(ENABLE_UNIT_TESTS true CACHE STRING "Enable unit tests")
set(ENABLE_INTEGRATION_TESTS true CACHE STRING "Enable integration tests")
set(ENABLE_PERFORMANCE_TESTS true CACHE STRING "Enable performance tests")
set(GENERATE_COVERAGE_REPORT true CACHE STRING "Generate code coverage report")

# Output and Visualization
set(ENABLE_VISUALIZATION true CACHE STRING "Enable visualization output")
set(VISUALIZATION_FORMAT "png" CACHE STRING "Visualization output format")
set(VISUALIZATION_WIDTH 1920 CACHE STRING "Visualization window width")
set(VISUALIZATION_HEIGHT 1080 CACHE STRING "Visualization window height")

# Python Bindings
set(ENABLE_PYTHON_BINDINGS true CACHE STRING "Enable Python bindings")
set(PYTHON_MODULE_NAME "pynlm" CACHE STRING "Python module name")
set(PYTHON_PACKAGE_VERSION "2.0.0" CACHE STRING "Python package version")

# External Dependencies
set(REQUIRED_BOOST_VERSION 1.74 CACHE STRING "Required Boost version")
set(REQUIRED_PYBIND11_VERSION 2.10 CACHE STRING "Required pybind11 version")

# Advanced Features
iset(ENABLE_PREDICTION_SYSTEM true CACHE STRING "Enable prediction system")
set(ENABLE_ATTENTIONAL_SYSTEM true CACHE STRING "Enable attentional selection system")
set(ENABLE_CONCEPT_FORMATION true CACHE STRING "Enable concept formation system")
set(ENABLE_SELF_MONITORING true CACHE STRING "Enable self-monitoring capabilities")

# System Resources
set(MAX_MEMORY_USAGE_MB 2048 CACHE STRING "Maximum memory usage (MB)")
set(MAX_CPU_THREADS 8 CACHE STRING "Maximum CPU threads to use")
set(ENABLE_GPU_ACCELERATION false CACHE STRING "Enable GPU acceleration (if available)")

# Experimental Features
set(ENABLE_EXPERIMENTAL_FEATURES false CACHE STRING "Enable experimental features")
set(EXPERIMENTAL_FEATURE_NOVELTY_CURIOUSITY true CACHE STRING "Enable novelty-based curiosity")
set(EXPERIMENTAL_FEATURE_DEVELOPMENT_PHASES true CACHE STRING "Enable multi-stage development")
set(EXPERIMENTAL_FEATURE_REPLAY_CONSOLIDATION true CACHE STRING "Enable memory replay consolidation")

# Debugging and Development Tools
set(DEBUG_MODE false CACHE STRING "Enable debug mode")
set(ENABLE_VALGRIND true CACHE STRING "Enable Valgrind memory debugging")
set(ENABLE_ASAN true CACHE STRING "Enable AddressSanitizer")
set(ENABLE_TSAN true CACHE STRING "Enable ThreadSanitizer")

# Build Configuration
include(CTest)
include(AddFileDependencies)

# Installation
install(TARGETS nlm DESTINATION bin)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake DESTINATION lib/cmake/nlm)
install(DIRECTORY docs/ DESTINATION docs)
install(DIRECTORY examples/ DESTINATION examples)

# Package generation (for distribution)
include(CPack)
set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_NAME "nlm")
set(CPACK_PACKAGE_VERSION "${NLM_VERSION}")
set(CPACK_PACKAGE_CONTACT "research@neural-learning.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Neural Learning Machine - NLM")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "NLM Research Team")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-all-dev, cmake, git")

message(STATUS "NLM Build Configuration Complete")
message(STATUS "Version: ${NLM_VERSION}")
message(STATUS "Build type: ${NLM_BUILD_TYPE}")
message(STATUS "Neurons: ${NEURON_COUNT}")
message(STATUS "Region count: ${REGION_COUNT}")
