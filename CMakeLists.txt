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

# Testing
enable_testing()
add_subdirectory(tests)

# Python bindings build (if pybind11 is available)
# Note: This is handled by scikit-build-core via pyproject.toml
# The Python module build is managed by the pyproject.toml configuration

# Enhanced main executable with additional capabilities
add_executable(nlm_enhanced
    src/main.cpp
    src/tools/NLMCommandLine.cpp
)
target_link_libraries(nlm_enhanced PUBLIC nlm_core)

# Enhanced Phase 3 demo with additional features
add_executable(nlm_phase3_enhanced
    src/experiments/Phase3Demo.cpp
    src/tools/NLMCommandLine.cpp
)
target_link_libraries(nlm_phase3_enhanced PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Enhanced Phase 4 demo with additional features
add_executable(nlm_phase4_enhanced
    src/experiments/Phase4Demo.cpp
    src/tools/NLMCommandLine.cpp
)
target_link_libraries(nlm_phase4_enhanced PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Enhanced Phase 6 demo with additional features
add_executable(nlm_phase6_enhanced
    src/experiments/Phase6Demo.cpp
    src/tools/NLMCommandLine.cpp
)
target_link_libraries(nlm_phase6_enhanced PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Create sample usage documentation
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/SAMPLE_USAGE.md.in
    ${CMAKE_CURRENT_BINARY_DIR}/SAMPLE_USAGE.md
    @ONLY
)

# Add option to build examples
option(BUILD_EXAMPLES "Build example files and documentation" ON)
if(BUILD_EXAMPLES)
    # Create examples directory
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/examples)
    
    # Write a comprehensive example Python script
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/examples/example_simple.py
        "#!/usr/bin/env python3
# Simple NLM Python Example

import pynlm
import time

def main():
    print(\"=== NLM Python Example ===\")
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    
    # Configure for faster demo
    config.set(\"brain.neuron_count\", 500)
    
    # Create and initialize the brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f\"Created brain with {brain.getTotalNeuronCount()} neurons\")
    
    # Run a simple simulation
    for step in range(100):
        brain.step(step)
        
        # Print progress every 25 steps
        if step % 25 == 0:
            print(f\"Step {step}: {brain.getFiringNeuronCount()} firing neurons, {brain.getTotalSpikeCount()} total spikes\")
    
    print(\"=== Simulation Complete ===\")
    print(f\"Total spikes: {brain.getTotalSpikeCount()}\")
    print(f\"Average firing rate: {brain.getAverageFiringRate():.2f}\")
    
    # Save state
    brain.save(\"demo_brain.bin\")
    print(\"Brain state saved to demo_brain.bin\")

if __name__ == \"__main__\":
    main()
"
    )
    
    # Write a comprehensive example C++ program
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/examples/example_simple.cpp
        "// Simple NLM C++ Example
// Compile with: g++ -std=c++20 example_simple.cpp -I../include -L./build -lnlm_core -o example_simple

#include \"../include/nlm/brain/Brain.hpp\"
#include \"../include/nlm/core/Config/Config.hpp\"
#include <iostream>
#include <memory>

int main() {
    std::cout << \"=== NLM C++ Example ===\" << std::endl;
    
    try {
        // Create default configuration
        auto config = std::make_shared<nlm::Config>();
        
        // Configure for simpler demo
        config->set(\"neuron_count\", 500);
        
        // Create and initialize brain
        nlm::Brain brain(config);
        if (!brain.initialize()) {
            std::cerr << \"Failed to initialize brain!\" << std::endl;
            return 1;
        }
        
        std::cout << \"Created brain with \" << brain.getTotalNeuronCount() 
                  << \" neurons\" << std::endl;
        
        // Run simulation
        for (int step = 0; step < 50; ++step) {
            brain.step(step);
            
            // Print progress every 10 steps
            if (step % 10 == 0) {
                std::cout << \"Step \" << step << ": \" 
                          << brain.getFiringNeuronCount() << \" firing neurons, \"
                          << brain.getTotalSpikeCount() << \" total spikes\" << std::endl;
            }
        }
        
        std::cout << \"=== Simulation Complete ===\" << std::endl;
        std::cout << \"Total spikes: \" << brain.getTotalSpikeCount() << std::endl;
        std::cout << \"Average firing rate: \" << brain.getAverageFiringRate() << std::endl;
        
        // Save state
        if (brain.save(\"demo_brain.bin\")) {
            std::cout << \"Brain state saved to demo_brain.bin\" << std::endl;
        } else {
            std::cerr << \"Failed to save brain state\" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << \"Error: \" << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
"
    )
    
    # Write a simple demo script
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/examples/README.txt
        "NLM Project Examples Directory
===============================

This directory contains simple example programs for using the NLM (Neural Learning Machine) framework.

C++ Examples:
- example_simple.cpp: Basic brain simulation in C++

Python Examples:
- example_simple.py: Basic brain simulation in Python using pynlm bindings

Building and Running:

C++:
1. Compile: g++ -std=c++20 example_simple.cpp -I../include -L./build -lnlm_core -o example_simple
2. Run: ./example_simple

Python:
1. Ensure Python bindings are installed: pip install .
2. Run: python3 example_simple.py

Requirements:
- C++ examples need the NLM libraries built
- Python examples need the pynlm module installed

For more examples and documentation, see:
- docs/ARCHITECTURE.md - System architecture
- docs/SCIENCE.md - Scientific background
- README.md - Project overview and build instructions
"
    )
endif()
