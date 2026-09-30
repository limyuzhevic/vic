# NLM - API Reference

## Overview

This document provides the complete API reference for the NLM (Neural Learning Machine) framework, including the core Brain API, the new fluent BrainBuilder API, and utility functions for advanced usage.

## Table of Contents

1. [Core Brain API](core_api.md)
2. [BrainBuilder Fluent API](brain_builder_api.md)
3. [Factory Functions](factory_functions.md)
4. [Configuration System](configuration.md)
5. [Performance Tuning](performance.md)
6. [Examples](examples.md)

## Core Brain API

The Core Brain API provides the fundamental interface for creating and managing neural network simulations. It includes methods for brain initialization, simulation control, and system introspection.

### Key Classes

- `nlm::Brain` - Main neural network simulation controller
- `nlm::Neuron` - Individual neuron with LIF dynamics
- `nlm::Synapse` - Synaptic connection between neurons

### Key Methods

- `initialize()` - Initialize the brain with current configuration
- `step()` - Advance simulation by one timestep
- `receiveSensoryInput()` - Inject sensory input into sensory neurons
- `produceAction()` - Select and produce motor action
- `save()`/`load()` - Checkpoint management
- `logStatus()` - System status reporting

## BrainBuilder Fluent API

The BrainBuilder provides a fluent, object-oriented interface for configuring neural networks. It supports method chaining and preset configurations for common use cases.

### Key Features

- **Fluent Configuration**: Method chaining for easy setup
- **Preset Architectures**: Pre-configured neural network patterns
- **Task-Specific Constructors**: Brains optimized for specific tasks
- **Validation**: Automatic configuration validation
- **Export/Import**: Save and load configurations

### Example Usage

```cpp
// Simple brain using fluent interface
auto brain = nlm::BrainBuilder()
    .withNeuronCount(1000)
    .withRegionCount(2)
    .withConnectionProbability(0.1f)
    .createCognitiveArchitecture()
    .buildAndInitialize();

// Brain for navigation task
auto navBrain = nlm::BrainBuilder()
    .forNavigationTask()
    .withNeuronCount(2000)
    .withDopamineLevel(0.7f)
    .withCuriosityLevel(0.8f)
    .buildAndInitialize();

// Cognitive brain with advanced features
auto cognitiveBrain = nlm::BrainBuilder()
    .createCognitiveArchitecture()
    .withWorkingMemoryCapacity(500)
    .withEpisodicMemoryCapacity(2000)
    .withPlanningEnabled(true)
    .withConceptFormationEnabled(true)
    .withPredictionEnabled(true)
    .buildAndInitialize();
```

### Builder Configuration Methods

#### Basic Configuration

```cpp
withNeuronCount(size_t count)              // Set total number of neurons
withRegionCount(size_t count)              // Set number of neural regions
withConnectionProbability(float prob)      // Set synaptic connection probability
withTimestep(double timestep)              // Set simulation timestep
```

#### Memory System

```cpp
withWorkingMemoryCapacity(size_t capacity)        // Set working memory size
withEpisodicMemoryCapacity(size_t capacity)       // Set episodic memory capacity
withAssociativeMemoryEnabled(bool enabled)        // Enable/disable associative memory
```

#### Neuromodulation

```cpp
withDopamineLevel(float level)          // Set dopamine (reward) level
withCuriosityLevel(float level)         // Set curiosity level
withNoveltyLevel(float level)           // Set novelty detection sensitivity
withPredictionErrorLevel(float level)   // Set prediction error sensitivity
```

#### Cognitive Architecture

```cpp
withPlanningEnabled(bool enabled)             // Enable/disable planning
withConceptFormationEnabled(bool enabled)      // Enable/disable concept formation
withAttentionEnabled(bool enabled)             // Enable/disable attention system
withPredictionEnabled(bool enabled)            // Enable/disable prediction system
```

#### Population Configuration

```cpp
withSensoryNeuronCount(size_t count)           // Set sensory neuron count
withInternalNeuronCount(size_t count)          // Set internal neuron count
withMotorNeuronCount(size_t count)             // Set motor neuron count
withExcitatoryNeuronRatio(float ratio)        // Set excitatory neuron ratio
withInhibitoryNeuronRatio(float ratio)        // Set inhibitory neuron ratio
```

#### Plasticity Configuration

```cpp
withSTDPWeight(float ltp, float ltd, float tau)      // Configure STDP parameters
withHebbianLearningEnabled(bool enabled)            // Enable/disable Hebbian learning
withStructuralPlasticityEnabled(bool enabled)        // Enable/disable structural plasticity
withSynaptogenesisRate(float rate)                   // Set synaptic growth rate
withPruningRate(float rate)                           // Set synaptic pruning rate
```

#### Presets and Factories

```cpp
createControlSystem()              // Simple control system
createSensorySystem()              // Sensory processing system
createCognitiveArchitecture()       // Advanced cognitive architecture
createMemoryDominant()             // Memory-focused system
createMotorControl()               // Motor control system
createDevelopmentalBrain()         // Developing brain system
createSocialLearning()             // Social learning system

forNavigationTask()                // Brain for navigation tasks
forObjectRecognition()            // Brain for object recognition
forSequentialLearning()            // Brain for sequential learning
forDecisionMaking()                // Brain for decision making
forSocialInteraction()             // Brain for social interaction
```

### Builder Configuration Objects

The BrainBuilder provides several configuration objects for fine-grained control:

#### NeuronPopulationConfigurator

```cpp
auto brain = nlm::BrainBuilder()
    .setPopulationConfigurator()
        .sensory(1000)
        .internal(2000)
        .motor(500)
        .byTypeRatios(0.8f, 0.2f)
    .buildAndInitialize();
```

#### MemorySystemConfigurator

```cpp
auto brain = nlm::BrainBuilder()
    .setMemoryConfigurator()
        .workingMemory(500)
        .episodicMemory(2000)
        .associativeMemory(0.5f, 10000)
        .enableEpisodicMemory(true)
        .enableAssociativeMemory(true)
    .buildAndInitialize();
```

#### PlasticitySystemConfigurator

```cpp
auto brain = nlm::BrainBuilder()
    .setPlasticityConfigurator()
        .stdp(0.01f, 0.012f, 20.0f, 0.001f)
        .hebbian(0.01f, 1.0f)
        .structuralPlasticity(0.0001f, 0.00001f, 0.001f)
        .enableSTDP(true)
        .enableHebbian(true)
        .enableStructural(true)
    .buildAndInitialize();
```

### Factory Functions

The BrainFactory provides convenience functions for creating common brain configurations:

```cpp
// Simple brain with default configuration
auto simpleBrain = nlm::BrainFactory::createSimpleBrain(1000);

// Cognitive brain with advanced features
auto cognitiveBrain = nlm::BrainFactory::createCognitiveBrain(5000);

// Memory-focused brain
auto memoryBrain = nlm::BrainFactory::createMemoryBrain(2000);

// Developmental brain
auto developmentalBrain = nlm::BrainFactory::createDevelopmentalBrain(3000);

// Motor control brain
auto motorControlBrain = nlm::BrainFactory::createMotorControlBrain(1500);

// Social learning brain
auto socialLearningBrain = nlm::BrainFactory::createSocialLearningBrain(4000);

// Task-specific brains
auto navBrain = nlm::BrainFactory::createBrainForTask(nlm::TaskType::Navigation, 2000);
auto objRecogBrain = nlm::BrainFactory::createBrainForTask(nlm::TaskType::ObjectRecognition, 3000);
auto seqLearningBrain = nlm::BrainFactory::createBrainForTask(nlm::TaskType::SequentialLearning, 2500);
```

### Configuration Export/Import

Brains can be saved and loaded using JSON configuration:

```cpp
// Export configuration to JSON
bool success = brainBuilder.exportToJSON("brain_config.json");

// Import configuration from JSON
bool success = brainBuilder.importFromJSON("brain_config.json");
auto brain = brainBuilder.buildAndInitialize();

// Configuration template
{
    "neuronCount": 2000,
    "regionCount": 2,
    "connectionProbability": 0.1,
    "timestep": 0.001,
    "dopamineLevel": 0.5,
    "curiosityLevel": 0.5,
    "planningEnabled": true,
    "conceptFormationEnabled": true,
    "attentionEnabled": true,
    "predictionEnabled": true,
    "workingMemoryCapacity": 500,
    "episodicMemoryCapacity": 2000,
    "synapticPlasticity": {
        "stdp": {
            "ltpWeight": 0.01,
            "ltdWeight": 0.012,
            "tau": 20.0
        },
        "hebbian": {
            "learningRate": 0.01,
            "weightMax": 1.0
        }
    }
}
```

## Configuration System

The NLM framework includes a comprehensive configuration system for managing brain parameters and validation.

### Configuration Schema

The configuration system supports:

- **Parameter validation** with type checking
- **Range constraints** for numeric parameters
- **Choice constraints** for enum-like parameters
- **Dependency constraints** between parameters
- **Custom validators** for complex validation rules

### Configuration Validation

```cpp
// Create validation schema
nlm::ValidationSchema schema;
schema.addParameter("neuronCount", "Number of neurons", 1000u, true);
schema.addParameter("timestep", "Simulation timestep", 0.001, true);
schema.addRangeConstraint("neuronCount", 1, 1000000);
schema.addRangeConstraint("timestep", 0.0001, 0.1);

// Validate configuration
nlm::ValidationResult result = schema.validate(parameters);
if (!result.valid) {
    std::cout << "Validation failed: " << result.getSummary() << std::endl;
}
```

## Performance Tuning

The NLM framework provides several performance optimization options:

### Performance Modes

```cpp
enum class PerformanceMode {
    Default,        // Balanced performance
    Speed,          // Maximum speed
    Accuracy,       // Maximum accuracy
    Balanced,       // Balanced performance
    MemoryOptimized, // Memory-efficient
    CPUOptimized,   // CPU-specific optimizations
    GPUOptimized    // GPU-accelerated (if available)
};
```

### SIMD and Parallel Processing

```cpp
auto brain = nlm::BrainBuilder()
    .withPerformanceMode(nlm::PerformanceMode::Speed)
    .withSIMDAcceleration(true)
    .withParallelProcessing(true)
    .withMemoryPoolEnabled(true)
    .buildAndInitialize();
```

## Examples

This section provides practical examples of using the NLM framework for various applications.

### Example 1: Simple Control System

```cpp
#include <nlm/BrainBuilder.hpp>
#include <iostream>

int main() {
    try {
        // Create a simple control system
        auto brain = nlm::BrainBuilder()
            .createControlSystem()
            .withNeuronCount(500)
            .withRegionCount(1)
            .buildAndInitialize();
        
        std::cout << "Created simple control system with " 
                  << brain->getTotalNeuronCount() << " neurons" << std::endl;
        
        // Run simulation
        for (nlm::SimulationStep step = 0; step < 1000; ++step) {
            brain->step(step);
        }
        
        std::cout << "Simulation completed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

### Example 2: Cognitive Navigation Agent

```cpp
#include <nlm/BrainBuilder.hpp>
#include <iostream>

int main() {
    try {
        // Create a cognitive navigation agent
        auto navAgent = nlm::BrainFactory::createBrainForTask(
            nlm::TaskType::Navigation, 
            2000
        );
        
        // Configure neuromodulation for exploration
        navAgent->getCuriosity()->setLevel(0.8f);
        navAgent->getDopamine()->setLevel(0.6f);
        
        std::cout << "Created navigation agent" << std::endl;
        
        // Simulate interaction with environment
        for (nlm::SimulationStep step = 0; step < 5000; ++step) {
            // Simulate sensory input (e.g., visual data)
            nlm::SensoryInput visualInput;
            // ... process visual input ...
            navAgent->receiveSensoryInput(visualInput);
            
            // Execute action
            auto action = navAgent->produceAction();
            
            // Apply action to environment
            // ... apply action ...
            
            // Advance simulation
            navAgent->step(step);
            
            if (step % 1000 == 0) {
                std::cout << "Step " << step << ": " 
                          << navAgent->getFiringNeuronCount() << " firing neurons" << std::endl;
            }
        }
        
        std::cout << "Navigation simulation completed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

### Example 3: Memory-Dominated Learning System

```cpp
#include <nlm/BrainBuilder.hpp>
#include <iostream>

int main() {
    try {
        // Create memory-dominated learning system
        auto memorySystem = nlm::BrainBuilder()
            .createMemoryDominant()
            .withNeuronCount(3000)
            .withWorkingMemoryCapacity(500)
            .withEpisodicMemoryCapacity(5000)
            .withAssociativeMemoryEnabled(true)
            .buildAndInitialize();
        
        std::cout << "Created memory-dominated system" << std::endl;
        
        // Simulate learning experience
        for (nlm::SimulationStep step = 0; step < 10000; ++step) {
            // Store experience in episodic memory
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.reward = memorySystem->getDopamine()->getLevel();
            
            // Add neurons to episode based on activity
            auto neurons = memorySystem->getWorkingMemory()->getMemoryNeurons();
            for (auto neuronId : neurons) {
                episode.activeNeurons.push_back(neuronId);
            }
            
            memorySystem->getEpisodicMemory()->storeEpisode(episode);
            
            // Replay important memories periodically
            if (step % 1000 == 0) {
                auto episodesToReplay = memorySystem->getEpisodicMemory()->getEpisodesForReplay(5);
                for (const auto* replayEpisode : episodesToReplay) {
                    memorySystem->getEpisodicMemory()->replayEpisode(replayEpisode);
                }
                std::cout << "Replayed " << episodesToReplay.size() << " episodes" << std::endl;
            }
            
            // Advance simulation
            memorySystem->step(step);
            
            if (step % 2000 == 0) {
                auto stats = memorySystem->getWorkingMemory()->getStatistics();
                std::cout << "Step " << step << ": " 
                          << "Working memory: " << stats.activeTraces << " traces, "
                          << "Episodic memory: " << memorySystem->getEpisodicMemory()->getEpisodeCount() << " episodes" << std::endl;
            }
        }
        
        std::cout << "Memory learning simulation completed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

### Example 4: Developmental Brain

```cpp
#include <nlm/BrainBuilder.hpp>
#include <iostream>

int main() {
    try {
        // Create developmental brain
        auto developingBrain = nlm::BrainBuilder()
            .createDevelopmentalBrain()
            .withNeuronCount(4000)
            .withCriticalPeriodEnabled(true)
            .buildAndInitialize();
        
        std::cout << "Created developmental brain" << std::endl;
        
        // Simulate development over time
        for (nlm::SimulationStep step = 0; step < 20000; ++step) {
            // Apply development effects at specific stages
            if (step % 1000 == 0) {
                developingBrain->getDevelopmentSystem()->update(developingBrain.get(), *developingBrain->getRandomGenerator(), 1000.0);
                
                // Update plasticity rates based on developmental stage
                auto* structuralPlasticity = developingBrain->getStructuralPlasticity();
                if (structuralPlasticity) {
                    auto stage = developingBrain->getDevelopmentalStage();
                    float plasticityMod = 1.0f;
                    
                    switch (stage) {
                        case nlm::DevelopmentalStage::Initial:
                            plasticityMod = 1.0f;  // High plasticity
                            break;
                        case nlm::DevelopmentalStage::CriticalPeriod:
                            plasticityMod = 0.8f;
                            break;
                        case nlm::DevelopmentalStage::Maturation:
                            plasticityMod = 0.5f;
                            break;
                        case nlm::DevelopmentalStage::Adult:
                            plasticityMod = 0.2f;  // Stable
                            break;
                    }
                    
                    structuralPlasticity->setSynaptogenesisRate(0.0001f * plasticityMod);
                    structuralPlasticity->setPruningRate(0.00001f * (2.0f - plasticityMod));
                }
            }
            
            // Advance simulation
            developingBrain->step(step);
            
            if (step % 5000 == 0) {
                std::cout << "Step " << step << ": " 
                          << "Developmental stage: " << static_cast<int>(developingBrain->getDevelopmentalStage()) << ", "
                          << "Neurons: " << developingBrain->getTotalNeuronCount() << ", "
                          << "Synapses: " << developingBrain->getTotalSynapseCount() << std::endl;
            }
        }
        
        std::cout << "Developmental simulation completed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

## Error Handling

The NLM framework provides comprehensive error handling with detailed error messages and recovery options.

### Error Types

```cpp
enum class NLMErrorCategory {
    NoError = 0,
    Configuration = 1,
    Neural = 2,
    Memory = 3,
    Simulation = 4,
    Integration = 5,
    Performance = 6,
    ThreadSafety = 7,
    Resource = 8
};
```

### Exception Handling

```cpp
#include <nlm/ErrorHandling.hpp>

void simulate() {
    try {
        auto brain = nlm::BrainBuilder()
            .createCognitiveArchitecture()
            .buildAndInitialize();
        
        // Simulation code...
        
    } catch (const nlm::NLMException& e) {
        std::cerr << "NLM Error [" << static_cast<int>(e.getErrorCode()) << "]: " << e.what() << std::endl;
        std::cerr << "Context: " << e.getContext() << std::endl;
        
        // Handle specific error types
        if (e.getErrorCategory() == nlm::NLMErrorCategory::Configuration) {
            // Configuration error recovery...
        } else if (e.getErrorCategory() == nlm::NLMErrorCategory::Neural) {
            // Neural error recovery...
        }
        
        return 1; // Indicate error
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 1;
    }
}
```

## Integration with Existing Code

The NLM framework is designed to integrate seamlessly with existing C++ codebases.

### Including Headers

```cpp
// Core headers
#include <nlm/Brain.hpp>
#include <nlm/BrainBuilder.hpp>
#include <nlm/ErrorHandling.hpp>
#include <nlm/ValidationSystem.hpp>

// Specific component headers
#include <nlm/neuromodulation/Neuromodulator.hpp>
#include <nlm/memory/WorkingMemory.hpp>
#include <nlm/cognition/NeuralPlanner.hpp>
```

### Building with NLM

The NLM framework can be built using standard CMake:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

For Python bindings:

```bash
pip install .
```

## License and Attribution

The NLM framework is released under the MIT License. Please cite the original research papers when using this framework in publications.

## Future Improvements

### Planned Enhancements

1. **Advanced SIMD Optimizations**: Auto-vectorization guidance and intrinsic optimizations
2. **GPU Acceleration**: CUDA and OpenCL support for massive scale simulations
3. **Distributed Computing**: Multi-machine simulation support
4. **Advanced Visualization**: Real-time brain activity visualization
5. **Plugin Architecture**: Extensible plugin system for custom components
6. **Web Interface**: Web-based brain monitoring and control
7. **Mobile Support**: ARM and embedded system optimizations

### Performance Optimization Goals

- **SIMD**: Full NEON/SSE/AVX utilization for neural operations
- **Memory**: NUMA-aware allocation and cache optimization
- **Parallel**: Work-stealing scheduler for multi-core systems
- **Storage**: Efficient checkpoint format and compression

## Contact and Support

For technical support, please visit the NLM GitHub repository or contact the development team.

---

*This documentation is continuously updated. Please check the repository for the latest version.*
