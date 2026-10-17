# NLM (Neural Learning Machine) - Enhanced README Documentation

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

### Core Principles

NLM is based on brain-inspired computing with the following key principles:

1. **Biological Fidelity**: Implements computational neuroscience principles while maintaining practical performance
2. **Developmental Learning**: Brains start simple and mature over time
3. **Closed-Loop Control**: Brains interact with environments and learn from consequences
4. **Emergent Cognition**: Complex cognitive abilities arise from neural dynamics
5. **Memory Integration**: Multiple memory systems work with neural processing

### Key Components

| Component | Function | Biological Inspiration |
|-----------|----------|----------------------|
| **Neurons** | Information processing units | Leaky Integrate-and-Fire (LIF) neurons |
| **Synapses** | Signal transmission between neurons | Event-driven spike propagation |
| **Membrane Dynamics** | Subthreshold integration | Realistic membrane potentials |
| **Plasticity** | Synaptic weight modification | STDP, Hebbian learning, structural changes |
| **Neuromodulators** | Global system state modulation | Dopamine, curiosity, novelty signals |
| **Memory Systems** | Information storage and retrieval | Working, episodic, associative memory |
| **Prediction** | Expectation formation | Bayesian prediction errors |
| **Development** | Plasticity rate modulation over time | Critical periods, maturation stages |

## Project Overview

### Current Phase: Phase 6 - Final Integration

Phase 6 represents the culmination of years of research, integrating all major brain systems into a coherent artificial neural architecture. This phase focuses on ensuring all subsystems work together as a unified cognitive system.

#### Integration Achievements

**Memory Systems Integration**
- Working memory with persistent activity and competitive dynamics
- Episodic memory with experience encoding and replay mechanisms
- Associative memory using Hebbian pattern completion
- Multi-modal memory integration with neural processing loops

**Neuromodulation Network**
- Dopamine system modulating neural excitability and synaptic plasticity
- Curiosity system driving exploratory behavior
- Novelty detection integrated with sensory processing
- Coordinated action across all plasticity mechanisms

**Prediction System**
- Forward models predicting sensory consequences
- Prediction error signals driving learning updates
- Confidence tracking and uncertainty quantification
- Integration with memory consolidation

**Cognition Architecture**
- Neural planner evaluating action sequences
- Concept formation from experience patterns
- Attention mechanisms with competitive selection
- Self-model generation and maintenance

**Developmental Framework**
- Stage-dependent plasticity rates
- Structural reorganization with age
- Experience-dependent maturation
- Aging and plasticity decline modeling

#### Persistence Features

- Checkpoint save/load system for brain states
- Complete state serialization with compression
- Resume capability from saved simulations
- Metadata tracking for experimental reproducibility

#### Replay and Consolidation

- Episodic memory replay during rest periods
- Memory strengthening through consolidation
- Sleep cycle integration with neural dynamics
- Stress-dependent prioritization

## Installation and Setup

### Prerequisites

- **Operating System**: Linux, macOS, or Windows with WSL
- **Compiler**: C++17 compatible compiler (GCC 9+, Clang 7+, MSVC 2019+)
- **Build Tools**: CMake 3.16+, Make or Ninja
- **Python**: 3.8+ (for Python bindings and examples)

### Build Instructions

#### Standard Build

```bash
# Create build directory
mkdir -p build
cd build

# Configure with optimized settings
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_CXX_STANDARD=17 \
         -DCMAKE_CXX_STANDARD_REQUIRED=ON

# Build with parallel compilation
make -j$(nproc)

# Run integration tests
ctest --output-on-failure
```

#### Development Build

```bash
# Debug build with symbols and assertions
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_BUILD_COMPILER=g++

# Install system-wide (optional)
make install
```

#### Python Bindings Build

```bash
# Install Python dependencies
pip install pybind11 scikit-build-core numpy pytest

# Build Python module
pip install -e .

# Or build standalone
python -m build --wheel
```

### Building and Running Examples

#### Complete Demo

After building, run the Phase 6 integration demo:

```bash
# Phase 6 integration test
./nlm_phase6_demo

# C++ API examples
./basic_brain_example
./world_interaction_example
./memory_integration_example
./neuromodulation_example
./development_example
```

#### Python Examples

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

## Usage Examples

### 1. Basic Brain Simulation

This example demonstrates creating and running a simple neural brain:

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    // Create default configuration
    nlm::Config config;
    config.set("brain.neuron_count", 1000);
    config.set("brain.synapse_density", 0.1);
    
    // Initialize brain
    nlm::Brain brain(config);
    brain.initialize();
    
    std::cout << "Brain initialized with " << brain.getTotalNeuronCount() 
              << " neurons and " << brain.getTotalSynapseCount() 
              << " synapses" << std::endl;
    
    // Run simulation
    for (int step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Log statistics periodically
        if (step % 10 == 0) {
            std::cout << "Step " << step << ": " 
                      << brain.getFiringNeuronCount() << " firing neurons, "
                      << brain.getAverageFiringRate() << " Hz average rate" << std::endl;
        }
    }
    
    return 0;
}
```

### 2. World Interaction

This example shows how a brain interacts with an environment:

```cpp
#include "agent/AgentBrain.hpp"
#include "environment/Environment.hpp"
#include <iostream>

int main() {
    // Setup components
    nlm::Config config;
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::Environment world;
    world.configure(20, 20, 8, 8);
    world.reset();
    world.setAgentStart(10.0, 10.0);
    
    nlm::AgentBrain agent(brain);
    agent.initialize(world);
    
    // Enable learning subsystems
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    
    std::cout << "Starting agent simulation..." << std::endl;
    
    // Run simulation loop
    for (int step = 0; step < 500; ++step) {
        // Update world
        world.update(0.1);
        
        // Get sensory input
        auto percept = world.getSensoryPercept();
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Run brain step
        brain.step(step);
        
        // Get motor command
        auto action = agent.decodeMotorCommand();
        
        // Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime());
        
        // Apply reward modulation
        double reward = 0.0;
        if (world.getSensoryPercept().getInternal()) {
            reward = world.getSensoryPercept().getInternal()[0];
        }
        agent.applyRewardModulation(reward, 0.0);
        
        // Update development
        agent.updateDevelopment(0.1);
        
        // Log progress
        if (step % 50 == 0) {
            std::cout << "Step " << step << ": "
                      << "Firing: " << brain.getFiringNeuronCount() << ", "
                      << "Curiosity: " << agent.getCuriosityLevel() << ", "
                      << "Novelty: " << agent.getNoveltyLevel() << ", "
                      << "Dev Stage: " << brain.getDevelopmentalStage() << std::endl;
        }
    }
    
    return 0;
}
```

### 3. Memory Integration

This example demonstrates how memory systems integrate with neural processing:

```cpp
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <vector>

int main() {
    // Create brain with memory integration
    nlm::Config config;
    config.set("brain.neuron_count", 500);
    config.set("memory.working.capacity", 100);
    config.set("memory.episodic.capacity", 1000);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Simulate experience generation and memory
    std::vector<double> experiencePattern(50, 0.0);
    
    std::cout << "Simulating experience generation and memory storage..." << std::endl;
    
    for (int episode = 0; episode < 50; ++episode) {
        // Generate random experience pattern
        for (int i = 0; i < 50; ++i) {
            experiencePattern[i] = (std::rand() % 100) / 100.0f;
        }
        
        // Encode in working memory
        brain.storeWorkingMemory(experiencePattern);
        
        // Update episodic memory
        brain.recordEpisodicMemory(experiencePattern, episode, 0.5);
        
        // Run brain step
        brain.step(episode);
        
        // Retrieve from working memory
        auto workingMemory = brain.retrieveWorkingMemory(experiencePattern, 0.8);
        
        // Replay episodic memory
        auto replayResult = brain.replayEpisodicMemory(episode, 0.3);
        
        // Print memory statistics
        if (episode % 10 == 0) {
            std::cout << "Episode " << episode << ": "
                      << "Working memory usage: " << brain.getWorkingMemoryUsage() << "/" 
                      << brain.getWorkingMemoryCapacity() << ", "
                      << "Episodic episodes: " << brain.getEpisodicMemoryCount() << std::endl;
        }
    }
    
    return 0;
}
```

### 4. Neuromodulation Control

This example shows neuromodulation effects on learning:

```cpp
#include "neuromodulation/Neuromodulator.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    // Create brain with neuromodulation
    nlm::Config config;
    config.set("brain.neuron_count", 800);
    config.set("neuromod.dopamine.scale", 1.0);
    config.set("neuromod.curiosity.enable", true);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    std::cout << "Neuromodulation simulation started..." << std::endl;
    
    // Simulation variables
    double noveltyLevel = 0.0;
    double dopamineLevel = 0.0;
    double learningRate = 0.001;
    
    for (int step = 0; step < 300; ++step) {
        // Generate novelty (random input changes)
        double novelty = (std::rand() % 100) / 1000.0f; // 0-0.1
        noveltyLevel = noveltyLevel * 0.9 + novelty * 0.1;
        
        // Apply novelty to neuromodulation
        brain.applyNovelty(noveltyLevel);
        
        // Get neuromodulation state
        dopamineLevel = brain.getDopamineLevel();
        
        // Run brain step (neuromodulators affect plasticity)
        brain.step(step);
        
        // Apply curiosity-driven exploration
        double curiosity = brain.getCuriosity();
        if (curiosity > 0.7 && (std::rand() % 20 == 0)) {
            std::cout << "Step " << step << ": Curiosity-driven exploration! (" 
                      << "Novelty: " << noveltyLevel << ", "
                      << "Dopamine: " << dopamineLevel << ", "
                      << "Curiosity: " << curiosity << ")" << std::endl;
        }
        
        // Log neuromodulation effects
        if (step % 50 == 0) {
            std::cout << "Step " << step << ": "
                      << "Novelty: " << noveltyLevel << ", "
                      << "Dopamine: " << dopamineLevel << ", "
                      << "Learning rate: " << learningRate << std::endl;
        }
        
        // Simulate learning from rewards
        double reward = (std::rand() % 100) / 100.0f;
        brain.applyReward(reward, 0.0);
        
        // Update neuromodulators
        brain.updateNeuromodulators(0.1);
    }
    
    return 0;
}
```

### 5. Development Stages

This example demonstrates brain development over time:

```cpp
#include "development/DevelopmentalSystem.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <vector>

int main() {
    // Create brain with developmental parameters
    nlm::Config config;
    config.set("brain.neuron_count", 600);
    config.set("development.initial_plasticity", 1.0);
    config.set("development.maturation_rate", 0.001);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    std::cout << "Brain development simulation started..." << std::endl;
    
    // Track development metrics
    std::vector<double> plasticityRates;
    std::vector<double> neuronExcitabilities;
    std::vector<int> developmentalStages;
    
    for (int step = 0; step < 200; ++step) {
        // Get current developmental state
        auto devState = brain.getDevelopmentalState();
        
        // Record metrics
        plasticityRates.push_back(devState.plasticityRate);
        neuronExcitabilities.push_back(devState.neuronExcitability);
        developmentalStages.push_back(static_cast<int>(devState.developmentStage));
        
        // Advance development
        brain.updateDevelopment(0.1);
        
        // Run brain step (development affects plasticity)
        brain.step(step);
        
        // Log developmental progress
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": "
                      << "Development Stage: " << brain.getDevelopmentalStage() << ", "
                      << "Plasticity: " << plasticityRates.back() << ", "
                      << "Excitability: " << neuronExcitabilities.back() << std::endl;
        }
        
        // Simulate learning with age-dependent plasticity
        double reward = (std::rand() % 100) / 100.0f;
        
        // Younger brains learn faster
        double ageFactor = brain.getDevelopmentalStage() / static_cast<double>(nlm::DevelopmentalStage::Adult);
        brain.applyReward(reward * (1.0 + ageFactor * 0.5), 0.0);
    }
    
    // Print development summary
    std::cout << "\nDevelopment Summary:" << std::endl;
    std::cout << "Final developmental stage: " << brain.getDevelopmentalStage() << std::endl;
    std::cout << "Average plasticity rate: " << std::accumulate(plasticityRates.begin(), plasticityRates.end(), 0.0) / plasticityRates.size() << std::endl;
    
    return 0;
}
```

## Configuration System

### Configuration Parameters

The NLM configuration system uses a hierarchical key-value structure. Configuration can be specified via:

1. **Default configuration** - Built-in defaults
2. **Configuration file** - JSON or custom format
3. **Runtime parameters** - Via C++ API
4. **Python bindings** - Via Python API

#### Core Configuration Categories

**Brain Configuration**

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Connectivity probability |
| `brain.connection_probability` | float | 0.05 | Probability of synapse formation |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Weight initialization standard deviation |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

**Plasticity Configuration**

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP potentiation time constant |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP depression time constant |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.0001 | Hebbian learning rate |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.growth_rate` | float | 0.001 | Neural growth rate |

**Memory Configuration**

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working.capacity` | int | 100 | Working memory capacity |
| `memory.working.trace_length` | int | 10 | Trace length for working memory |
| `memory.episodic.capacity` | int | 1000 | Episodic memory capacity |
| `memory.associative.pattern_count` | int | 100 | Number of associative patterns |
| `memory.consolidation.interval` | float | 100.0 | Consolidation interval (ms) |

**Neuromodulation Configuration**

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.dopamine.baseline` | float | 0.1 | Baseline dopamine level |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.sensitivity` | float | 1.0 | Curiosity sensitivity |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.threshold` | float | 0.05 | Novelty detection threshold |

**Development Configuration**

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.initial_plasticity` | float | 1.0 | Initial plasticity rate |
| `development.maturation_rate` | float | 0.001 | Maturation rate |
| `development.critical_period` | float | 50.0 | Critical period duration |
| `development.plasticity_decay` | float | 0.9995 | Plasticity decay rate |

**Prediction Configuration**

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `prediction.model_type` | string | "bayesian" | Prediction model type |
| `prediction.horizon` | int | 10 | Prediction horizon |
| `prediction.confidence_threshold` | float | 0.5 | Confidence threshold |
| `prediction.error_tolerance` | float | 0.1 | Error tolerance |

### Configuration File Format

Configuration files use JSON format:

```json
{
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.15,
        "v_thresh": -55.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.002
        }
    },
    "memory": {
        "working": {
            "capacity": 200
        }
    }
}
```

### Runtime Configuration

#### C++ API

```cpp
#include "core/Config/Config.hpp"

// Create configuration
nlm::Config config;

// Set values
config.set("brain.neuron_count", 1000);
config.set("plasticity.stdp.enable", true);

// Get values
int neuronCount = config.get<int>("brain.neuron_count");
bool stdpEnabled = config.get<bool>("plasticity.stdp.enable");

// Check existence
if (config.has("brain.neuron_count")) {
    // Configuration parameter exists
}

// Load from file
config.loadFromFile("config.json");

// Save to file
config.saveToFile("my_config.json");
```

#### Python API

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Set values
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)

# Get values
neuron_count = config.get("brain.neuron_count")
stdp_enabled = config.get("plasticity.stdp.enable")

# Check existence
if config.has("brain.neuron_count"):
    # Configuration parameter exists

# Load from file
config.loadFromFile("config.json")

# Save to file
config.saveToFile("my_config.json")
```

## Performance Considerations

### Optimization Strategies

1. **Memory Pooling**
   - Pre-allocate memory pools for neurons and synapses
   - Reduce dynamic memory allocation overhead

2. **Event-Driven Processing**
   - Use event queues for spike propagation
   - Minimize unnecessary computations

3. **SIMD Vectorization**
   - Utilize SIMD instructions for parallel processing
   - Vectorize neuron dynamics calculations

4. **Multi-threading**
   - Parallel processing of independent neuron populations
   - Shared memory for efficient communication

5. **Cache Optimization**
   - Optimize memory access patterns
   - Use appropriate data structures

### Profiling Tools

The project includes profiling capabilities:

```bash
# Run with performance profiling
./nlm --profile --output profile.json

# Profile specific components
./nlm --profile-brain --profile-memory
```

## Error Handling and Logging

### Error Reporting

NLM implements comprehensive error handling with the following features:

1. **Exception Safety**
   - All operations are exception-safe
   - Graceful degradation for non-critical errors

2. **Error Categories**
   - Configuration errors
   - Neural computation errors
   - Memory errors
   - I/O errors
   - Resource errors

3. **Logging Levels**
   - DEBUG: Detailed diagnostic information
   - INFO: General operational information
   - WARNING: Potential issues
   - ERROR: Actual errors
   - CRITICAL: Severe errors

### Error Example

```cpp
#include "core/ErrorHandling/ErrorHandlingSystem.hpp"

int main() {
    // Initialize error handling
    if (!initializeErrorHandling()) {
        std::cerr << "Failed to initialize error handling!" << std::endl;
        return 1;
    }
    
    try {
        // Code that may throw exceptions
        nlm::Brain brain(config);
        brain.initialize();
        
        // Simulate error
        if (someErrorCondition) {
            reportError("Invalid configuration", ErrorCode::ConfigValidationFailed);
        }
        
    } catch (const std::exception& e) {
        // Handle exceptions
        reportException(e);
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}
```

## Python Bindings

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
    
print(f"Simulation complete: {brain.getFiringNeuronCount()} firing neurons")
```

### Advanced Usage

```python
import pynlm
import numpy as np

# Custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp.enable", True)

# Create agent with world interaction
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent.initialize(world)

# Run episode
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Troubleshooting

### Common Issues

1. **Build Failures**
   - Ensure CMake version >= 3.16
   - Check compiler supports C++17
   - Verify all dependencies are installed

2. **Runtime Errors**
   - Always call `initialize()` before using brain
   - Check configuration parameters for validity
   - Verify memory usage is within limits

3. **Performance Issues**
   - Increase available memory for large networks
   - Monitor CPU usage and optimize configurations
   - Consider using Release build for production

4. **Python Binding Issues**
   - Ensure Python environment is correctly set up
   - Check that all dependencies are installed
   - Verify that the module can be imported

### Getting Help

- **Documentation**: Refer to `HOW_TO_USE.md` for detailed API documentation
- **Examples**: Check example projects in the `examples/` directory
- **Issues**: Report bugs on GitHub with reproduction steps
- **Community**: Join the NLM discussion forum for support

## Contributing

### Development Guidelines

1. **Code Style**
   - Follow existing code patterns
   - Use consistent naming conventions
   - Add comments for complex algorithms

2. **Testing**
   - Write unit tests for new functionality
   - Run existing test suite before committing
   - Test on multiple platforms

3. **Documentation**
   - Update documentation for new features
   - Add examples for complex APIs
   - Document configuration parameters

4. **Performance**
   - Profile new code for performance
   - Optimize for memory usage
   - Ensure backward compatibility

### License

MIT License. See `LICENSE` file for details.

## References

- [NLM Documentation Wiki](https://github.com/yourusername/nlm/wiki)
- [Papers on Computational Neuroscience](https://arxiv.org/search/
- [Python Bindings API Reference](https://yourusername.github.io/nlm/python/)

## Contact

For questions, issues, or contributions:
- GitHub Issues: https://github.com/yourusername/nlm/issues
- Email: research@youruniversity.edu
- Discord: https://discord.gg/nlm

---

*This project is a research initiative investigating brain-inspired artificial intelligence systems. The goals are to understand how neural architectures can develop complex behaviors and to advance computational neuroscience.*