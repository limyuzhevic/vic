# NLM Documentation Overview

This document provides an overview of the Neural Learning Machine (NLM) documentation structure and contents.

## Overview

NLM is a spiking neural network simulator designed for implementing and testing various neural models, dynamics, and learning algorithms. The documentation is organized to provide clear, comprehensive information about the public APIs, making it easy for developers to understand and use the library.

## Documentation Structure

### Core Modules
- **Config Module**: Configuration management system for application parameters and settings
- **Random Module**: Random number generation utilities for reproducible simulations
- **Logger Module**: Logging infrastructure with multiple output destinations and pluggable logger implementations
- **SimulationClock Module**: Time management for neural simulations with real-time support

### Brain Components
- **Brain Class**: Central coordinator managing neural populations, dynamics, and integrated systems
- **NeuralPopulation**: Groups of neurons with shared properties and behaviors
- **NeuralRegion**: Functional subdivisions containing multiple populations and local connectivity
- **Neuron**: Single spiking neurons implementing integrate-and-fire dynamics
- **Synapse**: Connections between neurons with delays, weights, and plasticity mechanisms

### Neural Dynamics
- **NeuralDynamics**: Abstract base class for neural dynamics models
- **IntegrateAndFireDynamics**: Simple but efficient dynamics model for large-scale simulations
- **HodgkinHuxleyDynamics**: Detailed biophysical model of neuron electrical properties
- **FitzHughNagumoDynamics**: Simplified excitable system model

### Plasticity Mechanisms
- **PlasticityRule**: Abstract base class for synaptic plasticity rules
- **STDP**: Spike-timing-dependent plasticity
- **Hebbian**: Hebbian learning rules
- **StructuralPlasticity**: Rules for creating and modifying synaptic connections

### Development and Neuromodulation
- **DevelopmentSystem**: Manages developmental processes and maturation
- **Maturation**: Neuron and circuit maturation processes
- **Pruning**: Synaptic elimination and refinement
- **Synaptogenesis**: Synaptic formation and growth

### Neuromodulation
- **Dopamine**: Reward-based learning and reinforcement
- **Curiosity**: Exploration motivation and intrinsic reward
- **Novelty**: Novelty detection and attention
- **PredictionError**: Error signals for predictive coding
- **Reward**: Reward signals and reinforcement learning

### Memory Systems
- **WorkingMemory**: Short-term active storage for current processing
- **EpisodicMemory**: Storage of specific events and experiences
- **SemanticMemory**: General knowledge and facts
- **ProceduralMemory**: Learned skills and habits
- **AssociativeMemory**: Relationships between representations

### Prediction and Cognition
- **PredictionSystem**: Predictive coding and error computation
- **NeuralPrediction**: Neural models for prediction and inference
- **ConceptFormation**: Pattern discovery and categorization
- **NeuralPlanner**: Action planning and decision making

### Sensory and Motor Systems
- **SensoryInput**: Interface for environmental input processing
- **Vision**: Visual system implementation
- **Audio**: Auditory system implementation
- **InternalSignals**: Internal signal processing
- **Action**: Motor command generation and output
- **MotorSystem**: Integration of motor control systems

### API Reference
This section provides detailed documentation for all public classes, methods, and functions in the NLM library.

## Usage Examples

### Basic Neural Network Simulation
```cpp
#include "Brain.hpp"
#include "Config.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("network.timestep", 0.001);
    config->set("network.neuron_count", 1000);
    
    // Create brain
    Brain brain(config);
    
    // Initialize and run simulation
    if (brain.initialize()) {
        for (int step = 0; step < 1000; ++step) {
            brain.step(step);
        }
    }
    
    return 0;
}
```

### Working with Neural Populations
```cpp
#include "brain/Brain.hpp"
#include "core/Types/Types.hpp"

void configureNeurons(Brain& brain) {
    // Access brain configuration
    auto config = brain.getConfig();
    
    // Get all regions
    const auto& regions = brain.getRegions();
    
    // Configure each population
    for (const auto& region : regions) {
        const auto& populations = region->getPopulations();
        
        for (const auto& population : populations) {
            // Set neuron type based on configuration
            if (population->getId() % 2 == 0) {
                population->setNeuronType(NeuronType::Excitatory);
            } else {
                population->setNeuronType(NeuronType::Inhibitory);
            }
            
            // Initialize with random parameters
            population->initializeRandom(*brain.getRandomGenerator());
        }
    }
}
```

### Logging and Diagnostics
```cpp
#include "core/Logger/Logger.hpp"

void logSimulationStatus(const Brain& brain) {
    brain.logStatus();
    
    // Log specific information
    NLM_LOG_INFO("Simulation completed: " << brain.getTotalNeuronCount() << " neurons");
    NLM_LOG_DEBUG("Firing rate: " << brain.getAverageFiringRate() << " Hz");
    
    // Log at different levels
    NLM_LOG_WARNING("High excitation/inhibition ratio: " << brain.getExcitationInhibitionRatio());
    NLM_LOG_ERROR("Simulation error: " << "some error message");
}
```

### Memory System Usage
```cpp
#include "memory/Memory.hpp"
#include "brain/Neuron.hpp"

void demonstrateMemoryUsage() {
    // Working memory
    WorkingMemory workingMem;
    workingMem.store(1, 0.5f);
    workingMem.store(2, 0.8f);
    
    float value = workingMem.retrieve(1);
    
    // Episodic memory
    EpisodicMemory episodicMem;
    EpisodicMemoryItem item;
    item.timestamp = 100;
    item.neurons = {1, 2, 3};
    item.values = {0.9f, 0.3f, 0.7f};
    item.metadata = "sensory_input";
    
    episodicMem.storeEpisode(item);
    
    // Semantic memory
    SemanticMemory semanticMem;
    semanticMem.storeFact("neuron_type_1", "excitatory");
    semanticMem.storeFact("threshold", "-55.0");
}
```

### Dynamics and Plasticity Configuration
```cpp
#include "dynamics/NeuralDynamics.hpp"
#include "plasticity/STDP.hpp"

void configureDynamicsAndPlasticity() {
    // Configure dynamics
    IntegrateAndFireDynamics iaf;
    iaf.setMembraneTimeConstant(20.0f);
    
    // Configure plasticity
    STDP stdp;
    stdp.setLTPWeight(0.1f);
    stdp.setLTDWeight(0.1f);
    stdp.setTimeConstant(20.0f);
    
    // Configure synaptic properties
    Synapse synapse(1, 2, 0.5f);
    synapse.setDelay(2);
    synapse.setType(SynapseType::Excitatory);
}
```

## References

### Related Documentation
- **Phase 6 Architecture Documentation**: Detailed architecture and implementation details
- **HOW_TO_USE.md**: Usage instructions and examples
- **README.md**: Project overview and setup instructions

### External Resources
- **Doxygen Documentation**: Technical reference for the documentation generation tool
- **GitHub Repository**: Source code and contribution guidelines
- **Documentation Index**: Complete API reference and examples

## Acknowledgements

This documentation was generated automatically using Doxygen and represents the public API of the NLM library. For the most up-to-date information, refer to the source code headers and the generated Doxygen output.
