#pragma once

# NLM Feature Configuration and Options
# This file provides comprehensive feature options for NLM customization

# ===== BASIC FEATURE FLAGS =====

# Core functionality
option(NLM_FEATURE_CORE "Enable core neural computation" ON)
option(NLM_FEATURE_NEURAL_COMPUTATION "Enable real spiking neural computation" ON)
option(NLM_FEATURE_EVENT_DRIVEN "Enable event-driven spike propagation" ON)

# Memory systems
option(NLM_FEATURE_WORKING_MEMORY "Enable working memory system" ON)
option(NLM_FEATURE_EPISODIC_MEMORY "Enable episodic memory system" ON)
option(NLM_FEATURE_ASSOCIATIVE_MEMORY "Enable associative memory system" ON)

# Neuromodulation
option(NLM_FEATURE_DOPAMINE "Enable dopamine neuromodulation" ON)
option(NLM_FEATURE_CURiosity "Enable curiosity-driven learning" ON)
option(NLM_FEATURE_NOVELTY "Enable novelty detection" ON)

# Plasticity
option(NLM_FEATURE_STDP "Enable Spike-Timing Dependent Plasticity" ON)
option(NLM_FEATURE_HEBBIAN "Enable Hebbian learning" ON)
option(NLM_FEATURE_STRUCTURAL_PLASTICITY "Enable structural plasticity" ON)

# Development
option(NLM_FEATURE_DEVELOPMENT "Enable developmental stages" ON)
option(NLM_FEATURE_MATURATION "Enable neural maturation" ON)
option(NLM_FEATURE_PRUNING "Enable synaptic pruning" ON)

# Cognition
option(NLM_FEATURE_PLANNING "Enable action planning" ON)
option(NLM_FEATURE_CONCEPT_FORMATION "Enable concept formation" ON)
option(NLM_FEATURE_ATTENTION "Enable attentional selection" ON)

# Prediction and control
option(NLM_FEATURE_PREDICTION "Enable prediction system" ON)
option(NLM_FEATURE_PREDICTION_ERROR "Enable prediction error" ON)

# Performance and optimization
option(NLM_FEATURE_PERFORMANCE_OPTIMIZED "Enable performance optimizations" ON)
option(NLM_FEATURE_EVENT_QUEUE "Enable event queue system" ON)
option(NLM_FEATURE_MEMORY_POOLS "Enable memory pools" ON)

# Storage and persistence
option(NLM_FEATURE_CHECKPOINTING "Enable checkpoint save/load" ON)
option(NLM_FEATURE_REPLAY "Enable memory replay" ON)
option(NLM_FEATURE_SERIALIZATION "Enable state serialization" ON)

# Visualization and monitoring
option(NLM_FEATURE_VISUALIZATION "Enable visualization interfaces" OFF)
option(NLM_FEATURE_PROFILING "Enable performance profiling" OFF)

# Advanced features
option(NLM_FEATURE_SCRIPTING "Enable scripting support" OFF)
option(NLM_FEATURE_WEB_INTERFACE "Enable web interface" OFF)
option(NLM_FEATURE_PYTHON_BINDINGS "Enable Python bindings" ON)

# ===== FEATURE DEPENDENCIES =====

# Set up feature dependencies
if(NLM_FEATURE_EVENT_DRIVEN)
    set(NLM_FEATURE_NEURAL_COMPUTATION ON CACHE STRING "" FORCE)
    set(NLM_FEATURE_SPiking ON CACHE STRING "" FORCE)
endif()

if(NLM_FEATURE_STRUCTURAL_PLASTICITY)
    set(NLM_FEATURE_DEVELOPMENT ON CACHE STRING "" FORCE)
endif()

if(NLM_FEATURE_REPLAY)
    set(NLM_FEATURE_EPISODIC_MEMORY ON CACHE STRING "" FORCE)
endif()

if(NLM_FEATURE_PLANNING OR NLM_FEATURE_CONCEPT_FORMATION OR NLM_FEATURE_ATTENTION)
    set(NLM_FEATURE_COGNITION ON CACHE STRING "" FORCE)
endif()

if(NLM_FEATURE_PREDICTION)
    set(NLM_FEATURE_PREDICTION_ERROR ON CACHE STRING "" FORCE)
endif()

if(NLM_FEATURE_PYTHON_BINDINGS)
    # Python bindings depend on core and agent systems
    set(NLM_FEATURE_COGNITION ON CACHE STRING "" FORCE)
endif()

# ===== FEATURE SUMMARY =====

# Generate feature summary for users
function(generate_feature_summary)
    set(FEATURE_SUMMARY "NLM Feature Configuration:\n")
    set(FEATURE_COUNT 0)
    
    # Core features
    if(NLM_FEATURE_CORE)
        string(APPEND FEATURE_SUMMARY "  [✓] Core Neural Computation\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_NEURAL_COMPUTATION)
        string(APPEND FEATURE_SUMMARY "  [✓] Real Spiking Neural Computation\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_EVENT_DRIVEN)
        string(APPEND FEATURE_SUMMARY "  [✓] Event-Driven Processing\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    # Memory features
    if(NLM_FEATURE_WORKING_MEMORY)
        string(APPEND FEATURE_SUMMARY "  [✓] Working Memory\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_EPISODIC_MEMORY)
        string(APPEND FEATURE_SUMMARY "  [✓] Episodic Memory\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_ASSOCIATIVE_MEMORY)
        string(APPEND FEATURE_SUMMARY "  [✓] Associative Memory\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    # Neuromodulation features
    if(NLM_FEATURE_DOPAMINE)
        string(APPEND FEATURE_SUMMARY "  [✓] Dopamine Neuromodulation\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_CURiosity)
        string(APPEND FEATURE_SUMMARY "  [✓] Curiosity-Driven Learning\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_NOVELTY)
        string(APPEND FEATURE_SUMMARY "  [✓] Novelty Detection\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    # Plasticity features
    if(NLM_FEATURE_STDP)
        string(APPEND FEATURE_SUMMARY "  [✓] Spike-Timing Dependent Plasticity\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_HEBBIAN)
        string(APPEND FEATURE_SUMMARY "  [✓] Hebbian Learning\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_STRUCTURAL_PLASTICITY)
        string(APPEND FEATURE_SUMMARY "  [✓] Structural Plasticity\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    # Development features
    if(NLM_FEATURE_DEVELOPMENT)
        string(APPEND FEATURE_SUMMARY "  [✓] Developmental Stages\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_MATURATION)
        string(APPEND FEATURE_SUMMARY "  [✓] Neural Maturation\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_PRUNING)
        string(APPEND FEATURE_SUMMARY "  [✓] Synaptic Pruning\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    # Cognition features
    if(NLM_FEATURE_PLANNING)
        string(APPEND FEATURE_SUMMARY "  [✓] Action Planning\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_CONCEPT_FORMATION)
        string(APPEND FEATURE_SUMMARY "  [✓] Concept Formation\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_ATTENTION)
        string(APPEND FEATURE_SUMMARY "  [✓] Attentional Selection\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    # Prediction features
    if(NLM_FEATURE_PREDICTION)
        string(APPEND FEATURE_SUMMARY "  [✓] Prediction System\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_PREDICTION_ERROR)
        string(APPEND FEATURE_SUMMARY "  [✓] Prediction Error\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    # Performance features
    if(NLM_FEATURE_PERFORMANCE_OPTIMIZED)
        string(APPEND FEATURE_SUMMARY "  [✓] Performance Optimizations\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_EVENT_QUEUE)
        string(APPEND FEATURE_SUMMARY "  [✓] Event Queue System\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_MEMORY_POOLS)
        string(APPEND FEATURE_SUMMARY "  [✓] Memory Pools\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    # Storage features
    if(NLM_FEATURE_CHECKPOINTING)
        string(APPEND FEATURE_SUMMARY "  [✓] Checkpointing\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_REPLAY)
        string(APPEND FEATURE_SUMMARY "  [✓] Memory Replay\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_SERIALIZATION)
        string(APPEND FEATURE_SUMMARY "  [✓] State Serialization\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    # Advanced features
    if(NLM_FEATURE_VISUALIZATION)
        string(APPEND FEATURE_SUMMARY "  [✓] Visualization Interfaces\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_PROFILING)
        string(APPEND FEATURE_SUMMARY "  [✓] Performance Profiling\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_SCRIPTING)
        string(APPEND FEATURE_SUMMARY "  [✓] Scripting Support\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_WEB_INTERFACE)
        string(APPEND FEATURE_SUMMARY "  [✓] Web Interface\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    if(NLM_FEATURE_PYTHON_BINDINGS)
        string(APPEND FEATURE_SUMMARY "  [✓] Python Bindings\n")
        math(EXPR FEATURE_COUNT "${FEATURE_COUNT} + 1")
    endif()
    
    # Print feature summary
    message(STATUS "NLM Feature Configuration:")
    message(STATUS "${FEATURE_SUMMARY}")
    message(STATUS "  Total Features Enabled: ${FEATURE_COUNT}")
    
    # Create cache variables for feature detection
    set(NLM_FEATURE_COUNT ${FEATURE_COUNT} PARENT_SCOPE)
    set(NLM_FEATURE_SUMMARY "${FEATURE_SUMMARY}" PARENT_SCOPE)
endfunction()

# Generate the feature summary
generate_feature_summary()

# Feature-specific compile definitions
if(NLM_FEATURE_CORE)
    add_compile_definitions(NLM_FEATURE_CORE=1)
endif()

if(NLM_FEATURE_NEURAL_COMPUTATION)
    add_compile_definitions(NLM_FEATURE_NEURAL_COMPUTATION=1)
endif()

if(NLM_FEATURE_EVENT_DRIVEN)
    add_compile_definitions(NLM_FEATURE_EVENT_DRIVEN=1)
endif()

if(NLM_FEATURE_ADVANCED_MEMORY)
    add_compile_definitions(NLM_FEATURE_ADVANCED_MEMORY=1)
endif()

if(NLM_FEATURE_NEUROMODULATION)
    add_compile_definitions(NLM_FEATURE_NEUROMODULATION=1)
endif()

if(NLM_FEATURE_PREDICTION)
    add_compile_definitions(NLM_FEATURE_PREDICTION=1)
endif()

if(NLM_FEATURE_COGNITION)
    add_compile_definitions(NLM_FEATURE_COGNITION=1)
endif()

if(NLM_FEATURE_DEVELOPMENT)
    add_compile_definitions(NLM_FEATURE_DEVELOPMENT=1)
endif()

if(NLM_FEATURE_STRUCTURAL_PLASTICITY)
    add_compile_definitions(NLM_FEATURE_STRUCTURAL_PLASTICITY=1)
endif()

if(NLM_FEATURE_PERFORMANCE_OPTIMIZED)
    add_compile_definitions(NLM_FEATURE_PERFORMANCE_OPTIMIZED=1)
endif()

if(NLM_FEATURE_CHECKPOINTING)
    add_compile_definitions(NLM_FEATURE_CHECKPOINTING=1)
endif()

if(NLM_FEATURE_REPLAY)
    add_compile_definitions(NLM_FEATURE_REPLAY=1)
endif()

if(NLM_FEATURE_PYTHON_BINDINGS)
    add_compile_definitions(NLM_FEATURE_PYTHON_BINDINGS=1)
endif()

# Feature-based library selection
if(NLM_FEATURE_PERFORMANCE_OPTIMIZED)
    set(NLM_PHASE5_SOURCES
        src/performance/OptimizedBrain.cpp
        src/performance/MemoryPool.cpp
        src/performance/EventQueue.cpp
    )
endif()

# Configure source groups based on features
if(NLM_FEATURE_CORE OR NLM_FEATURE_NEURAL_COMPUTATION)
    set(NLM_PHASE2_SOURCES
        src/brain/Brain.cpp
        src/brain/Neuron.cpp
        src/dynamics/SpikeSystem.cpp
        src/plasticity/STDP.cpp
        src/plasticity/Hebbian.cpp
    )
endif()

if(NLM_FEATURE_WORKING_MEMORY OR NLM_FEATURE_EPISODIC_MEMORY OR NLM_FEATURE_ASSOCIATIVE_MEMORY)
    set(NLM_MEMORY_SOURCES
        src/memory/WorkingMemory.cpp
        src/memory/EpisodicMemory.cpp
        src/memory/NeuralWorkingMemory.cpp
        src/memory/NeuralEpisodicMemory.cpp
    )
endif()

if(NLM_FEATURE_DOPAMINE OR NLM_FEATURE_CURiosity OR NLM_FEATURE_NOVELTY)
    set(NLM_NEUROMODULATION_SOURCES
        src/neuromodulation/Neuromodulator.cpp
        src/neuromodulation/Reward.cpp
        src/neuromodulation/Curiosity.cpp
        src/neuromodulation/Novelty.cpp
        src/neuromodulation/PredictionError.cpp
    )
endif()

if(NLM_FEATURE_DEVELOPMENT OR NLM_FEATURE_MATURATION OR NLM_FEATURE_PRUNING)
    set(NLM_DEVELOPMENT_SOURCES
        src/development/DevelopmentSystem.cpp
        src/development/Synaptogenesis.cpp
        src/development/Pruning.cpp
        src/development/Maturation.cpp
    )
endif()

if(NLM_FEATURE_PLANNING OR NLM_FEATURE_CONCEPT_FORMATION OR NLM_FEATURE_ATTENTION)
    set(NLM_COGNITION_SOURCES
        src/cognition/ConceptFormation.cpp
        src/cognition/NeuralPlanner.cpp
    )
endif()

if(NLM_FEATURE_PREDICTION OR NLM_FEATURE_PREDICTION_ERROR)
    set(NLM_PREDICTION_SOURCES
        src/prediction/PredictionSystem.cpp
        src/prediction/NeuralPrediction.cpp
    )
endif()

# Create feature-specific targets
if(NLM_PHASE2_SOURCES)
    add_library(nlm_phase2_features STATIC ${NLM_PHASE2_SOURCES})
    target_include_directories(nlm_phase2_features PUBLIC src)
    target_compile_definitions(nlm_phase2_features PRIVATE NLM_PHASE2_ENABLED)
endif()

if(NLM_MEMORY_SOURCES)
    add_library(nlm_memory_features STATIC ${NLM_MEMORY_SOURCES})
    target_include_directories(nlm_memory_features PUBLIC src)
    target_compile_definitions(nlm_memory_features PRIVATE NLM_MEMORY_FEATURES_ENABLED)
endif()

if(NLM_NEUROMODULATION_SOURCES)
    add_library(nlm_neuromodulation_features STATIC ${NLM_NEUROMODULATION_SOURCES})
    target_include_directories(nlm_neuromodulation_features PUBLIC src)
    target_compile_definitions(nlm_neuromodulation_features PRIVATE NLM_NEUROMODULATION_FEATURES_ENABLED)
endif()

if(NLM_DEVELOPMENT_SOURCES)
    add_library(nlm_development_features STATIC ${NLM_DEVELOPMENT_SOURCES})
    target_include_directories(nlm_development_features PUBLIC src)
    target_compile_definitions(nlm_development_features PRIVATE NLM_DEVELOPMENT_FEATURES_ENABLED)
endif()

if(NLM_COGNITION_SOURCES)
    add_library(nlm_cognition_features STATIC ${NLM_COGNITION_SOURCES})
    target_include_directories(nlm_cognition_features PUBLIC src)
    target_compile_definitions(nlm_cognition_features PRIVATE NLM_COGNITION_FEATURES_ENABLED)
endif()

if(NLM_PREDICTION_SOURCES)
    add_library(nlm_prediction_features STATIC ${NLM_PREDICTION_SOURCES})
    target_include_directories(nlm_prediction_features PUBLIC src)
    target_compile_definitions(nlm_prediction_features PRIVATE NLM_PREDICTION_FEATURES_ENABLED)
endif()

if(NLM_FEATURE_PERFORMANCE_OPTIMIZED)
    add_library(nlm_performance_features STATIC ${NLM_PHASE5_SOURCES})
    target_include_directories(nlm_performance_features PUBLIC src)
    target_compile_definitions(nlm_performance_features PRIVATE NLM_PERFORMANCE_FEATURES_ENABLED)
endif()

# Feature-based target for main executable
add_executable(nlm src/main.cpp)

# Link feature libraries based on enabled features
if(TARGET nlm_phase2_features)
    target_link_libraries(nlm PRIVATE nlm_phase2_features)
endif()

if(TARGET nlm_memory_features)
    target_link_libraries(nlm PRIVATE nlm_memory_features)
endif()


if(TARGET nlm_neuromodulation_features)
    target_link_libraries(nlm PRIVATE nlm_neuromodulation_features)
endif()

if(TARGET nlm_development_features)
    target_link_libraries(nlm PRIVATE nlm_development_features)
endif()

if(TARGET nlm_cognition_features)
    target_link_libraries(nlm PRIVATE nlm_cognition_features)
endif()

if(TARGET nlm_prediction_features)
    target_link_libraries(nlm PRIVATE nlm_prediction_features)
endif()

if(TARGET nlm_performance_features)
    target_link_libraries(nlm PRIVATE nlm_performance_features)
endif()

# Add feature detection macros for use in source code
configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/NLMFeatureConfig.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLMFeatureConfig.h
    @ONLY
)

# Create imported targets for feature libraries
if(TARGET nlm_phase2_features)
    add_library(NLM::phase2 ALIAS nlm_phase2_features)
endif()

if(TARGET nlm_memory_features)
    add_library(NLM::memory ALIAS nlm_memory_features)
endif()

if(TARGET nlm_neuromodulation_features)
    add_library(NLM::neuromodulation ALIAS nlm_neuromodulation_features)
endif()

if(TARGET nlm_development_features)
    add_library(NLM::development ALIAS nlm_development_features)
endif()

if(TARGET nlm_cognition_features)
    add_library(NLM::cognition ALIAS nlm_cognition_features)
endif()

if(TARGET nlm_prediction_features)
    add_library(NLM::prediction ALIAS nlm_prediction_features)
endif()

if(TARGET nlm_performance_features)
    add_library(NLM::performance ALIAS nlm_performance_features)
endif()

# Export feature configuration
export(TARGETS nlm_phase2_features nlm_memory_features nlm_neuromodulation_features
            nlm_development_features nlm_cognition_features nlm_prediction_features
            nlm_performance_features
       FILE "${CMAKE_CURRENT_BINARY_DIR}/NLMFeatureTargets.cmake")

# Generate feature information file
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/NLMFeatures.txt
    "NLM Feature Configuration\n"
    "======================\n\n"
    "Core Features:\n"
    "  Neural Computation: ${NLM_FEATURE_NEURAL_COMPUTATION}\n"
    "  Event-Driven: ${NLM_FEATURE_EVENT_DRIVEN}\n"
    "  Advanced Memory: ${NLM_FEATURE_ADVANCED_MEMORY}\n"
    "  Neuromodulation: ${NLM_FEATURE_NEUROMODULATION}\n"
    "  Prediction: ${NLM_FEATURE_PREDICTION}\n"
    "  Cognition: ${NLM_FEATURE_COGNITION}\n"
    "  Development: ${NLM_FEATURE_DEVELOPMENT}\n"
    "  Structural Plasticity: ${NLM_FEATURE_STRUCTURAL_PLASTICITY}\n"
    "  Performance Optimized: ${NLM_FEATURE_PERFORMANCE_OPTIMIZED}\n"
    "  Checkpointing: ${NLM_FEATURE_CHECKPOINTING}\n"
    "  Replay: ${NLM_FEATURE_REPLAY}\n"
    "  Python Bindings: ${NLM_FEATURE_PYTHON_BINDINGS}\n\n"
    "Total Features Enabled: ${NLM_FEATURE_COUNT}\n"
)

# Install feature configuration
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLMFeatureConfig.h
    DESTINATION include/nlm
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLMFeatures.txt
    DESTINATION share/nlm
)