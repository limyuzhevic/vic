cmake_minimum_required(VERSION 3.16)

# Core component libraries with modern CMake practices

# Types subsystem - fundamental data structures
add_library(nlm_types
    core/Types/Types.cpp
)
target_include_directories(nlm_types
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)
set_target_properties(nlm_types PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    DEBUG_POSTFIX "d"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Config subsystem - system configuration
add_library(nlm_config
    core/Config/Config.cpp
)
target_include_directories(nlm_config
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_link_libraries(nlm_config PUBLIC nlm_types)

# Random subsystem - stochastic processes
add_library(nlm_random
    core/Random/Random.cpp
)
target_include_directories(nlm_random
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Logger subsystem - logging infrastructure
add_library(nlm_logger
    core/Logger/Logger.cpp
)
target_include_directories(nlm_logger
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# SimulationClock subsystem - time management
add_library(nlm_simulationclock
    core/SimulationClock/SimulationClock.cpp
)
target_include_directories(nlm_simulationclock
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Core interface library for fundamental components
add_library(nlm_core_fundamental
    INTERFACE
)
target_link_libraries(nlm_core_fundamental INTERFACE
    nlm_types
    nlm_config
    nlm_random
    nlm_logger
    nlm_simulationclock
)

# Brain subsystem - neural computation
add_library(nlm_brain
    brain/Neuron.cpp
    brain/Synapse.cpp
    brain/NeuralPopulation.cpp
    brain/NeuralRegion.cpp
    brain/Brain.cpp
)
target_include_directories(nlm_brain
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_link_libraries(nlm_brain PUBLIC nlm_core_fundamental)

# Dynamics subsystem - neural dynamics
add_library(nlm_dynamics
    dynamics/NeuralDynamics.cpp
    dynamics/SpikeSystem.cpp
)
target_include_directories(nlm_dynamics
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_link_libraries(nlm_dynamics PUBLIC nlm_brain)

# Plasticity subsystem - synaptic plasticity
add_library(nlm_plasticity
    plasticity/PlasticityRule.cpp
    plasticity/Hebbian.cpp
    plasticity/STDP.cpp
    plasticity/StructuralPlasticity.cpp
)
target_include_directories(nlm_plasticity
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Development subsystem - neural development
add_library(nlm_development
    development/DevelopmentSystem.cpp
    development/Synaptogenesis.cpp
    development/Pruning.cpp
    development/Maturation.cpp
)
target_include_directories(nlm_development
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_link_libraries(nlm_development PUBLIC nlm_plasticity nlm_brain)

# Neuromodulation subsystem - optional
if(NLM_ENABLE_NEUROMODULATION)
    add_library(nlm_neuromodulation
        neuromodulation/Neuromodulator.cpp
        neuromodulation/Reward.cpp
        neuromodulation/PredictionError.cpp
        neuromodulation/Novelty.cpp
        neuromodulation/Curiosity.cpp
        neuromodulation/Acetylcholine.cpp
        neuromodulation/Norepinephrine.cpp
        neuromodulation/Serotonin.cpp
    )
    target_include_directories(nlm_neuromodulation
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )
endif()

# Memory subsystem - optional
if(NLM_ENABLE_MEMORY_SYSTEMS)
    add_library(nlm_memory
        memory/Memory.cpp
        memory/NeuralWorkingMemory.cpp
        memory/NeuralEpisodicMemory.cpp
    )
    target_include_directories(nlm_memory
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )
endif()

# Cognition subsystem - optional
if(NLM_ENABLE_COGNITION)
    add_library(nlm_cognition
        cognition/ConceptFormation.cpp
        cognition/NeuralPlanner.cpp
    )
    target_include_directories(nlm_cognition
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )
endif()

# Sensory subsystem - perception
add_library(nlm_sensory
    sensory/SensoryInput.cpp
    sensory/Vision.cpp
    sensory/Audio.cpp
    sensory/InternalSignals.cpp
)
target_include_directories(nlm_sensory
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Prediction subsystem - optional
if(NLM_ENABLE_PREDICTION)
    add_library(nlm_prediction
        prediction/PredictionSystem.cpp
        prediction/NeuralPrediction.cpp
    )
    target_include_directories(nlm_prediction
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )
endif()

# Motor subsystem - action
add_library(nlm_motor
    motor/MotorSystem.cpp
)
target_include_directories(nlm_motor
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Environment subsystem - interaction
add_library(nlm_environment
    environment/Environment.cpp
    environment/Observation.cpp
)
target_include_directories(nlm_environment
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Experiments subsystem - research
add_library(nlm_experiments
    experiments/Experiment.cpp
    experiments/ExperimentRunner.cpp
    experiments/Metrics.cpp
)
target_include_directories(nlm_experiments
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Visualization subsystem - optional
if(NLM_ENABLE_VISUALIZATION)
    add_library(nlm_visualization
        visualization/VisualizationInterface.cpp
    )
    target_include_directories(nlm_visualization
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )
endif()

# Performance optimizations - optional
if(NLM_ENABLE_PERFORMANCE_OPTIMIZATIONS)
    # Note: Only header files exist in performance directory
    # This is a placeholder for actual performance implementations
    add_library(nlm_performance INTERFACE)
    target_include_directories(nlm_performance
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/performance>
    )
endif()

# Main executable
add_executable(nlm_main
    main.cpp
)
target_link_libraries(nlm_main PUBLIC nlm_brain nlm_core_fundamental)

# Demo executables for different phases
add_executable(nlm_phase3_demo
    experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core_fundamental nlm_brain nlm_sensory nlm_agent nlm_world nlm_phase3)

add_executable(nlm_phase4_demo
    experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core_fundamental nlm_brain nlm_sensory nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Install targets for libraries
install(TARGETS nlm_types nlm_config nlm_random nlm_logger nlm_simulationclock
    EXPORT nlmTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(TARGETS nlm_brain nlm_dynamics nlm_plasticity nlm_sensory nlm_motor nlm_environment
    EXPORT nlmTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(TARGETS nlm_core_fundamental
    EXPORT nlmTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install optional components
if(NLM_ENABLE_NEUROMODULATION)
    install(TARGETS nlm_neuromodulation
        EXPORT nlmTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
endif()

if(NLM_ENABLE_MEMORY_SYSTEMS)
    install(TARGETS nlm_memory
        EXPORT nlmTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
endif()

if(NLM_ENABLE_COGNITION)
    install(TARGETS nlm_cognition
        EXPORT nlmTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
endif()

if(NLM_ENABLE_PREDICTION)
    install(TARGETS nlm_prediction
        EXPORT nlmTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
endif()

if(NLM_ENABLE_VISUALIZATION)
    install(TARGETS nlm_visualization
        EXPORT nlmTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
endif()

if(NLM_ENABLE_PERFORMANCE_OPTIMIZATIONS)
    install(TARGETS nlm_performance
        EXPORT nlmTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
endif()

# Install main executables
install(TARGETS nlm_main
    RUNTIME DESTINATION bin
)

install(TARGETS nlm_phase3_demo
    RUNTIME DESTINATION bin
)

install(TARGETS nlm_phase4_demo
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY src/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

# Create configuration file for find_package
include(CMakePackageConfigHelpers)

# Generate and install nlmConfig.cmake
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/../nlmConfig.cmake.in"
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfig.cmake
    INSTALL_DESTINATION lib/cmake/nlm
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfigVersion.cmake
    DESTINATION lib/cmake/nlm
)

install(EXPORT nlmTargets
    FILE nlmTargets.cmake
    NAMESPACE NLM::
    DESTINATION lib/cmake/nlm
)
