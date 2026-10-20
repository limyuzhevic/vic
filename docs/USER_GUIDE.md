# NLM (Neural Learning Machine) - User Guide

## Overview

This user guide provides comprehensive documentation for getting started with the NLM (Neural Learning Machine) framework. Whether you're a researcher, developer, or student, this guide will help you understand the key concepts and start using NLM effectively.

## Prerequisites

### System Requirements

- **Operating System**: Linux, macOS, or Windows
- **Compiler**: GCC 9+, Clang 12+, or MSVC 2019+
- **Build Tools**: CMake 3.16+
- **Memory**: 2GB RAM minimum (8GB+ recommended for complex simulations)
- **Disk Space**: 500MB free space

### Software Dependencies

#### Core Dependencies

```bash
# Ubuntu/Debian
 sudo apt-get update
sudo apt-get install -y g++ cmake build-essential git

# macOS
brew install cmake gcc

# Windows
# Use WSL2 or Visual Studio with C++ support
```

#### Python Dependencies (for Python bindings)

```bash
# Install Python package dependencies
pip install pybind11 scikit-build-core numpy pytest

# For development
pip install -e .
```

## Quick Start

### Step 1: Installation

1. **Clone the repository**

```bash
git clone https://github.com/yourusername/NLM.git
cd NLM
```

2. **Build the project**

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

3. **Build Python bindings**

```bash
cd ..
pip install -e .
```

### Step 2: Create Your First Brain

**C++ Example**:

```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 1000);
    config->set("brain.synapse_density", 0.1);
    
    // Create brain
    Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Run simulation
    std::cout << "Starting simulation with " << config->get("brain.neuron_count") << " neurons" << std::endl;
    for (int step = 0; step < 100; ++step) {
        brain.step(step);
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": " 
                      << brain.getTotalSpikeCount() << " spikes" << std::endl;
        }
    }
    
    std::cout << "Simulation complete!" << std::endl;
    return 0;
}
```

**Python Example**:

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print("Simulation complete!")
```

### Step 3: Explore Advanced Features

1. **Agent-Based Control**

```cpp
#include "AgentBrain.hpp"
#include "SimpleWorld.hpp"

int main() {
    // Create brain and agent
    auto config = std::make_shared<Config>();
    Brain brain(config);
    AgentBrain agentBrain(brain);
    
    // Create world
    SimpleWorld world;
    world.configure(width=100, height=100, visionWidth=8, visionHeight=8);
    
    // Initialize agent
    agentBrain.initialize(world);
    agentBrain.enableRewardModulation(true);
    agentBrain.enableCuriosity(true);
    
    // Run interactive simulation
    world.reset();
    while (!world.shouldTerminate()) {
        world.update(0.1);
        
        // Process environment
        auto percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        
        // Brain computation
        static int step = 0;
        brain.step(step++);
        
        // Generate and apply action
        auto action = agentBrain.decodeMotorCommand();
        world.applyMotorCommand(action.get(), step * 0.1);
    }
    
    return 0;
}
```

## Core Concepts

### Neural Computation

NLM implements spiking neural networks using Leaky Integrate-and-Fire (LIF) neurons with:

- **Event-driven dynamics**: Only process neurons that spike
- **Synaptic delays**: Realistic transmission delays
- **Multiple plasticity rules**: STDP, Hebbian, and structural plasticity
- **Neuromodulation**: Dopamine, curiosity, novelty for adaptive behavior

### Memory Systems

NLM includes multiple memory systems that work together:

1. **Working Memory**: Persistent neural activity for temporary storage
2. **Episodic Memory**: Experience encoding with replay mechanisms
3. **Associative Memory**: Pattern completion for memory recall
4. **Concept Formation**: Pattern discovery without explicit labels

### Development

The brain develops through stages that modulate plasticity:

1. **Initial**: High plasticity, rapid learning
2. **Critical Period**: Sensitive window for certain types of learning
3. **Maturation**: Decreased plasticity, fine-tuning
4. **Adult**: Stable performance with limited further learning

## Key Components

### Brain Systems

| System | Description | Key Functions |
|--------|-------------|---------------|
| **Core** | Basic neural computation | Neuron dynamics, spike processing |
| **Memory** | Storage and retrieval | Working + episodic + associative memory |
| **Prediction** | Temporal sequence learning | Action consequence prediction |
| **Cognition** | Higher-level processing | Attention, planning, self-model |
| **Neuromodulation** | Learning signals | Reward, curiosity, novelty |
| **Development** | Maturation process | Stage-dependent plasticity |
| **Plasticity** | Synaptic changes | STDP, Hebbian, structural changes |

### Configuration Categories

| Category | Description | Example Parameters |
|----------|-------------|-------------------|
| **brain** | Neural network parameters | neuron_count, synapse_density |
| **plasticity** | Learning rule parameters | stdp_learning_rate, pruning_rate |
| **neuromod** | Neuromodulation parameters | dopamine_scale, curiosity_threshold |
| **development** | Developmental parameters | critical_period_duration |
| **world** | World/environment parameters | width, height, vision_range |

## Working with Configuration

### Setting Parameters

```cpp
Config config;

// Basic configuration
config.set("brain.neuron_count", 2000);
config.set("brain.synapse_density", 0.15);
config.set("plasticity.stdp_learning_rate", 0.001);
config.set("neuromod.dopamine_scale", 1.0);
config.set("neuromod.curiosity_threshold", 0.5);
```

### Configuration Files

NLM supports JSON configuration files:

```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "tau_mem": 20.0
    },
    "plasticity": {
        "stdp_learning_rate": 0.001,
        "stdp_tau_plus": 20.0,
        "stdp_tau_minus": 20.0,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
    },
    "neuromod": {
        "dopamine_scale": 1.0,
        "curiosity_threshold": 0.3,
        "novelty_sensitivity": 0.5
    },
    "development": {
        "critical_period_duration": 100.0,
        "development_enabled": true
    }
}
```

### Using Python Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify parameters
config.set("brain.neuron_count", 500)
config.set("plasticity.stdp_learning_rate", 0.01)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")
```

## Advanced Features

### Memory Management

NLM provides several memory systems for different purposes:

1. **Working Memory**: For maintaining active representations
2. **Episodic Memory**: For storing experience sequences
3. **Associative Memory**: For pattern completion

**Example: Working Memory**

```cpp
Brain brain(config);
auto workingMemory = brain.getWorkingMemory();

// Store a pattern
Pattern pattern = { /* neural pattern */ };
workingMemory.storePattern(pattern, 0.8f);

// Retrieve similar patterns
auto similar = workingMemory.retrieveSimilarPatterns(pattern, 0.7f);
```

### Neuromodulation

Neuromodulators control learning and exploration:

- **Dopamine**: Reward prediction error, reinforcement learning
- **Curiosity**: Exploration motivation, intrinsic reward
- **Novelty**: Detection of unexpected events
- **Prediction Error**: Discrepancy between expected and actual outcomes

**Example: Neuromodulation Setup**

```cpp
AgentBrain agentBrain(brain);
agentBrain.enableRewardModulation(true);
agentBrain.enableCuriosity(true);
agentBrain.enableNovelty(true);

// Apply reward signals
agentBrain.applyRewardModulation(reward, predicted_reward);
```

### Development

Development stages affect plasticity over time:

```cpp
void runDevelopmentalSimulation(Brain& brain, AgentBrain& agentBrain) {
    for (int step = 0; step < 1000; ++step) {
        // Update development
        agentBrain.updateDevelopment(0.1);
        
        // Check current developmental stage
        auto stage = brain.getDevelopmentalStage();
        if (stage == DevelopmentalStage.CriticalPeriod) {
            std::cout << "In critical period - high plasticity" << std::endl;
        }
        
        // Continue simulation...
    }
}
```

## World Simulation

### SimpleWorld Interface

The `SimpleWorld` class provides a basic 2D environment for testing:

**Configuration**:

```cpp
SimpleWorld world;
world.configure(width=100, height=100, visionWidth=8, visionHeight=8);
world.setMaxEnergy(100.0);
world.setTemperature(25.0);
```

**Perception**:

```cpp
// Get sensory input
SensoryPercept percept = world.getSensoryPercept();

// Access different sensory modalities
auto visual = percept.getVisual();
auto internal = percept.getInternal();
auto proprioceptive = percept.getProprioceptive();
```

**Action**:

```cpp
// Apply motor command
Action action = /* create action */;
world.applyMotorCommand(action, simulation_time);
```

## Performance Optimization

### Large-Scale Simulations

For networks with thousands of neurons:

1. **Use Optimized Brain**

```cpp
OptimizedBrain optimizedBrain(std::move(brain));
```

2. **Memory Management**

```cpp
// Clean up regularly
brain.reset();
```

3. **Parallel Processing**

```cpp
// Enable multi-threading (if supported)
// Configure CMake with -DTHREADS_PREFER_PTHREAD_FLAG=ON
```

### Profiling and Monitoring

```cpp
// Performance monitoring
auto startTime = std::chrono::high_resolution_clock::now();
brain.step(step);
auto endTime = std::chrono::high_resolution_clock::now();

auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
std::cout << "Step time: " << duration.count() << " ms" << std::endl;
```

## Troubleshooting

### Common Issues

**Problem**: Brain initialization fails
**Solution**:

```cpp
if (!brain.initialize()) {
    std::cerr << "Initialization failed. Check configuration." << std::endl;
    // Debug: Print configuration errors
    config.printErrors();
}
```

**Problem**: Memory allocation fails
**Solution**:

```cpp
// Reduce brain size
config.set("brain.neuron_count", 500);

// Check available memory
// Use `ulimit -v` on Unix systems
```

**Problem**: Simulation runs too slowly
**Solution**:

```cpp
// Optimize performance
config.set("brain.synapse_density", 0.05);  // Fewer connections
brain.enableOptimizedMode(true);  // Use optimized implementation
```

### Debug Tools

1. **Logging**: Enable detailed logging for debugging
2. **Validation**: Use `config.validate()` to check parameter ranges
3. **Checkpointing**: Use `brain.save()` and `brain.load()` for debugging

## Examples

See [docs/EXAMPLES.md](EXAMPLES.md) for detailed examples and tutorials.

## Further Reading

- [API Reference](API_REFERENCE.md) - Complete API documentation
- [Architecture Documentation](ARCHITECTURE.md) - System design and components
- [Configuration Documentation](CONFIGURATION.md) - Configuration system details
- [Python Bindings](PYTHON_BINDINGS.md) - Python API documentation
- [Testing Documentation](TESTING.md) - Testing framework and procedures
- [Examples and Tutorials](EXAMPLES.md) - Working code examples
- [Upgrading Guide](UPGRADING_GUIDE.md) - Migration from previous versions

## Contact and Support

For questions, issues, or feedback:

- **GitHub Issues**: https://github.com/yourusername/NLM/issues
- **Documentation**: https://nlm.readthedocs.io/
- **Discord**: https://discord.gg/nlm
- **Email**: support@yourcompany.com

## License

MIT License - See LICENSE file for details.

## Contributors

NLM is developed and maintained by the NLM Research Team. Contributions are welcome!