# Enhanced NLM CMake Configuration
# This CMake file provides enhanced build options for the NLM project
# with improved checkpointing and learning capabilities

cmake_minimum_required(VERSION 3.16)
project(NLM-Enhanced VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configuration options
option(NLM_BUILD_EXPERIMENTS "Build experiment demos" ON)
option(NLM_BUILD_CHECKPOINT_DEMO "Build enhanced checkpoint demo" ON)
option(NLM_BUILD_TESTING "Build test suite" ON)
option(NLM_ENABLE_PROFILING "Enable performance profiling" OFF)
option(NLM_ENABLE_SANITIZERS "Enable memory sanitizers" OFF)

# Project directories
set(NLM_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(NLM_EXPERIMENTS_DIR "${NLM_SRC_DIR}/experiments")
set(NLM_PERFORMANCE_DIR "${NLM_SRC_DIR}/performance")
set(NLM_TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")

# Core library
add_library(nlm_core STATIC
    ${NLM_SRC_DIR}/core/Config/Config.cpp
    ${NLM_SRC_DIR}/core/Random/Random.cpp
    ${NLM_SRC_DIR}/core/Logger/Logger.cpp
    ${NLM_SRC_DIR}/core/SimulationClock/SimulationClock.cpp
    ${NLM_SRC_DIR}/core/Types/Types.cpp
)

# Enhanced checkpoint system
add_library(nlm_checkpoint Enhanced
    ${NLM_PERFORMANCE_DIR}/CheckpointSystem.cpp
)

# Brain implementation
add_library(nlm_brain STATIC
    ${NLM_SRC_DIR}/brain/Brain.cpp
    ${NLM_SRC_DIR}/brain/Brain.cpp
)

# Experiment components
if(NLM_BUILD_EXPERIMENTS)
    add_library(nlm_experiments STATIC
        ${NLM_EXPERIMENTS_DIR}/Experiment.cpp
        ${NLM_EXPERIMENTS_DIR}/ExperimentRunner.cpp
        ${NLM_EXPERIMENTS_DIR}/Metrics.cpp
        ${NLM_EXPERIMENTS_DIR}/AblationSystem.cpp
        ${NLM_EXPERIMENTS_DIR}/ScalingBenchmark.cpp
    )
    
    # Enhanced checkpoint demo
    if(NLM_BUILD_CHECKPOINT_DEMO)
        add_executable(nlm_enhanced_checkpoint_demo
            ${NLM_EXPERIMENTS_DIR}/EnhancedCheckpointDemo.cpp
        )
        target_link_libraries(nlm_enhanced_checkpoint_demo
            nlm_core
            nlm_checkpoint
            nlm_brain
            nlm_experiments
        )
        target_compile_definitions(nlm_enhanced_checkpoint_demo PRIVATE "NLM_BUILD_CHECKPOINT_DEMO")
    endif()
endif()

# Phase 3 demo (original)
add_executable(nlm_phase3_demo
    ${NLM_EXPERIMENTS_DIR}/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo
    nlm_core
    nlm_brain
)

# Phase 6 demo (original)
add_executable(nlm_phase6_demo
    ${NLM_EXPERIMENTS_DIR}/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo
    nlm_core
    nlm_brain
    nlm_experiments
)

# Test executables
if(NLM_BUILD_TESTING)
    enable_testing()
    
    # Core tests
    add_executable(test_core_utilities
        ${NLM_TESTS_DIR}/test_types.cpp
        ${NLM_TESTS_DIR}/test_config.cpp
        ${NLM_TESTS_DIR}/test_logger.cpp
    )
    target_link_libraries(test_core_utilities nlm_core)
    add_test(NAME core_utilities_test COMMAND test_core_utilities)
    
    # Synapse tests
    add_executable(test_synapse)
    target_sources(test_synapse PRIVATE
        ${NLM_TESTS_DIR}/test_synapse.cpp
    )
    target_link_libraries(test_synapse nlm_core nlm_brain)
    add_test(NAME synapse_test COMMAND test_synapse)
    
    # Neuron tests
    add_executable(test_neuron)
    target_sources(test_neuron PRIVATE
        ${NLM_TESTS_DIR}/test_neuron.cpp
    )
    target_link_libraries(test_neuron nlm_core nlm_brain)
    add_test(NAME neuron_test COMMAND test_neuron)
    
    # Enhanced checkpoint tests
    add_executable(test_checkpoint_integration
        ${NLM_TESTS_DIR}/test_checkpoint_integration.cpp
        ${NLM_TESTS_DIR}/test_checkpoint_performance.cpp
    )
    target_link_libraries(test_checkpoint_integration
        nlm_core
        nlm_checkpoint
        nlm_brain
        nlm_experiments
    )
    add_test(NAME checkpoint_integration_test COMMAND test_checkpoint_integration)
    
    # Reward system tests
    add_executable(test_reward_system
        ${NLM_TESTS_DIR}/test_reward_system.cpp
    )
    target_link_libraries(test_reward_system
        nlm_core
        nlm_brain
        nlm_experiments
    )
    add_test(NAME reward_system_test COMMAND test_reward_system)
    
    # Learning dynamics tests
    add_executable(test_learning_dynamics
        ${NLM_TESTS_DIR}/test_learning_dynamics.cpp
    )
    target_link_libraries(test_learning_dynamics
        nlm_core
        nlm_brain
        nlm_experiments
    )
    add_test(NAME learning_dynamics_test COMMAND test_learning_dynamics)
endif()

# Installation
install(TARGETS nlm_core nlm_brain
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

if(NLM_BUILD_EXPERIMENTS)
    install(TARGETS nlm_phase3_demo nlm_phase6_demo
        RUNTIME DESTINATION bin
    )
endif()

if(NLM_BUILD_CHECKPOINT_DEMO)
    install(TARGETS nlm_enhanced_checkpoint_demo
        RUNTIME DESTINATION bin
    )
endif()

# Configuration files
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/default.cfg
    DESTINATION etc/nlm
)

# Documentation
install(DIRECTORY docs/
    DESTINATION share/doc/nlm
    FILES_MATCHING PATTERN "*.md"
)

# Print build summary
function(print_build_summary)
    message(STATUS "=== NLM Enhanced Build Summary ===")
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
    message(STATUS "Experiments enabled: ${NLM_BUILD_EXPERIMENTS}")
    message(STATUS "Checkpoint demo enabled: ${NLM_BUILD_CHECKPOINT_DEMO}")
    message(STATUS "Testing enabled: ${NLM_BUILD_TESTING}")
    message(STATUS "Profiling enabled: ${NLM_ENABLE_PROFILING}")
    message(STATUS "Sanitizers enabled: ${NLM_ENABLE_SANITIZERS}")
    message(STATUS "")
    message(STATUS "Targets:")
    if(NLM_BUILD_EXPERIMENTS)
        message(STATUS "  - nlm_phase3_demo (Phase 3 world interaction)")
        message(STATUS "  - nlm_phase6_demo (Phase 6 integration test)")
    endif()
    if(NLM_BUILD_CHECKPOINT_DEMO)
        message(STATUS "  - nlm_enhanced_checkpoint_demo (Enhanced checkpoint system)")
    endif()
    if(NLM_BUILD_TESTING)
        message(STATUS "  - test_core_utilities (Core functionality tests)")
        message(STATUS "  - test_synapse (Synapse functionality tests)")
        message(STATUS "  - test_neuron (Neuron functionality tests)")
        message(STATUS "  - test_checkpoint_integration (Checkpoint tests)")
        message(STATUS "  - test_reward_system (Reward system tests)")
        message(STATUS "  - test_learning_dynamics (Learning dynamics tests)")
    endif()
    message(STATUS "")
    message(STATUS "=== End Build Summary ===")
endfunction()

# Call build summary
print_build_summary()

# Compiler flags
if(NLM_ENABLE_PROFILING)
    target_compile_options(nlm_core PRIVATE -O2 -g -march=native -fomit-frame-pointer)
    target_compile_options(nlm_brain PRIVATE -O2 -g -march=native -fomit-frame-pointer)
endif()

if(NLM_ENABLE_SANITIZERS)
    target_compile_options(nlm_core PRIVATE -fsanitize=address,undefined)
    target_compile_options(nlm_brain PRIVATE -fsanitize=address,undefined)
    target_link_options(nlm_core PRIVATE -fsanitize=address,undefined)
    target_link_options(nlm_brain PRIVATE -fsanitize=address,undefined)
endif()

# Installation prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation prefix" FORCE)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Clean build option
if(CMAKE_GENERATOR_IS_MAKEFILE)
    add_custom_target(clean
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/bin
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/lib
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/test_results
        COMMENT "Cleaning build artifacts"
    )
endif()

# Help target
add_custom_target(help
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/help.cmake
    COMMENT "Showing available build targets and options"
)

# Development convenience targets
add_custom_target(dev-setup
    COMMAND ${CMAKE_COMMAND} -E echo "=== Development Setup Complete ==="
    COMMAND ${CMAKE_COMMAND} -E echo "To build all targets: make -j4"
    COMMAND ${CMAKE_COMMAND} -E echo "To run all tests: make test"
    COMMAND ${CMAKE_COMMAND} -E echo "To run enhanced checkpoint demo: ./bin/nlm_enhanced_checkpoint_demo"
    COMMAND ${CMAKE_COMMAND} -E echo "To run Phase - 3 demo: ./bin/nlm_phase3_demo"
    COMMAND ${CMAKE_COMMAND} -E echo "To run Phase 6 demo: ./bin/nlm_phase6_demo"
    COMMENT "Showing development setup information"
)