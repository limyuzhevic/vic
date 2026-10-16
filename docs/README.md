# NLM (Neural Learning Machine) - Comprehensive Documentation

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project that aims to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

### Key Features

- **Neural Systems**: Real spiking neural computation with LIF neuron dynamics
- **Memory Integration**: Working, episodic, and associative memory systems
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error systems
- **Developmental Stages**: Initial, critical period, maturation, adult, and aging stages
- **Plasticity**: STDP, Hebbian, and structural plasticity rules
- **Prediction System**: Sensorimotor predictions and prediction error computation
- **Cognition**: Neural planning, concept formation, and attentional selection
- **Advanced Agent Interface**: Connection between brain and world
- **Performance Optimizations**: SIMD vectorization, memory pools, and parallel processing

## Table of Contents

1. [Quick Start](#quick-start)
2. [Architecture Overview](#architecture-overview)
3. [Getting Started](#getting-started)
4. [Python API](#python-api)
5. [Advanced Configuration](#advanced-configuration)
6. [Building and Testing](#building-and-testing)
7. [Performance Tuning](#performance-tuning)
8. [Development Guide](#development-guide)
9. [Troubleshooting](#troubleshooting)
10. [License](#license)

## Quick Start

### Installation

The NLM framework is designed to be built with CMake. Here's how to get started:

```bash
# Clone the repository
cd /path/to/your/project

# Create build directory
mkdir build
cd build

# Configure with options
 cmake .. \
     -DCMAKE_BUILD_TYPE=Release \
     -DBUILD_PYTHON_BINDINGS=ON \
     -DBUILD_TESTS=ON \
     -DENABLE_VECTORIZATION=ON

# Build the project
make -j$(nproc)

# Run tests
ctest

# Run Phase 6 integration demo
./nlm_phase6_demo
```

### Basic Usage

```python
import pynlm

# Create configuration
config = pynlm.Config()

# Create brain
brain = pynlm.Brain(config)

# Initialize brain
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get action
action = brain.produceAction()
```

## Architecture Overview

### Core Components

```
WORLD (Environment)
  ↓
SENSORY INPUT (Vision, Audio, Internal)
  ↓
NEURAL PROCESSING (LIF Dynamics, Spikes)
  ↓
INTERNAL STATE (Working Memory, Attention)
  ↓
MEMORY / PREDICTION (All memory systems)
  ↓
MOTIVATION / NEUROMODULATION (Dopamine, Curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, Structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

### Memory Systems

- **Working Memory**: Transient active information storage
- **Episodic Memory**: Experience storage and replay
- **Associative Memory**: Pattern associations and connections

### Neuromodulation Systems

- **Dopamine**: Reward prediction error and reinforcement
- **Curiosity**: Exploration motivation and novelty detection
- **Novelty**: Sensory novelty detection
- **Prediction Error**: Discrepancy between predicted and actual outcomes

### Developmental System

Four developmental stages with changing plasticity:

1. **Initial**: High plasticity, rapid learning
2. **Critical Period**: Optimized for specific experiences
3. **Maturation**: Balanced plasticity and stability
4. **Adult**: Stable performance with limited plasticity

## Getting Started

### Phase 1: Project Setup

1. **Environment Setup**: Install dependencies (CMake, C++ compiler, Python)
2. **Configuration**: Choose build options based on your needs
3. **Compilation**: Build the core libraries and executables

### Phase 2: Neural Computation

The NLM framework implements real spiking neural computation:

- **LIF Neurons**: Integrate-and-fire neuron model
- **Event-driven Dynamics**: Spike propagation and synaptic transmission
- **Plasticity Rules**: STDP for spike timing-dependent plasticity

### Phase 3: World Interaction

Agents interact with a simulated world through:

- **Sensory Input**: Vision (grayscale images), audio, internal signals
- **Motor Output**: Actions like move forward, turn, interact
- **Reward System**: Positive and negative reinforcement

### Phase 4: Cognition and Learning

Advanced cognitive systems enable:

- **Prediction**: Forward models and outcome prediction
- **Planning**: Neural action sequence evaluation
- **Attention**: Competitive selection of information
- **Concept Formation**: Pattern discovery from experience

### Phase 5: Performance Optimizations

- **Memory Pools**: Efficient memory allocation
- **Event Queues**: Optimized event processing
- **SIMD Vectorization**: Parallel computation on CPU vectors
- **Parallel Processing**: Multi-threaded execution

### Phase 6: Final Integration

All systems are integrated into a coherent artificial brain loop that:

- Processes sensory input
- Makes predictions and actions
- Learns from experience
- Develops over time
- Achieves goal-directed behavior

## Python API

The Python bindings provide a high-level interface to the NLM system:

### Basic Brain Operations

```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain()

# Create world
world = pynlm.createSimpleWorld()

# Configure world for agent
world.configure(width=100, height=100, vision_width=16, vision_height=16)

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)

# Run simulation steps
for step in range(1000):
    # Update world
    world.update(0.1)
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain computation step
    brain.step(step)
    
    # Generate action
    action = brain.produceAction()
    
    # Apply action in world
    action_result = world.applyMotorCommand(action, world.getSimulationTime())
```

### Advanced Configuration

```python
import pynlm
import json

# Create and configure brain
config = pynlm.Config()

# Set configuration values
config.set("brain.learning_rate", 0.01)
config.set("brain.memory_capacity", 1000)
config.set("brain.noise_level", 0.1)
config.set("agent.curiosity_enabled", true)
config.set("agent.reward_modulation_enabled", true)
config.set("agent.development_enabled", true)

# Save configuration to file
config.saveToFile("config.json")

# Load configuration from file
config2 = pynlm.Config()
config2.loadFromFile("config.json")

# Verify configuration
print(f"Learning rate: {config2.get('brain.learning_rate').value}")
print(f"Memory capacity: {config2.get('brain.memory_capacity').value}")
print(f"Curiosity enabled: {config2.get('agent.curiosity_enabled').value}")
```

### Memory Management

```python
# Working with different memory types
brain = pynlm.createBrain()

# Get working memory
working_memory = brain.getWorkingMemory()

# Get episodic memory  
episodic_memory = brain.getEpisodicMemory()

# Get associative memory
associative_memory = brain.getAssociativeMemory()

# Get prediction system
prediction_system = brain.getPredictionSystem()

# Get cognitive systems
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
attention = brain.getAttention()
```

### Performance Monitoring

```python
# Enable performance logging
brain.enablePerformanceLogging(True)

# Log performance metrics
brain.logPerformanceMetric("spike_count", 12345, "neural_activity")
brain.logPerformanceMetric("memory_usage", 512.5, "memory")
brain.logPerformanceMetric("firing_rate", 8.7, "neurons")

# Get performance statistics
stats = brain.getPerformanceStats()
print(f"Total spikes: {stats['spike_count']}")
print(f"Average firing rate: {stats['firing_rate']}")
```

### Development Stages

```python
# Access and modify developmental stages
brain = pynlm.createBrain()

# Get current developmental stage
stage = brain.getDevelopmentalStage()
print(f"Current stage: {stage}")

# Set developmental stage
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)

# Developmental stage affects plasticity
# Higher plasticity in early stages = faster learning
```

## Advanced Configuration

### Configuration File Format

NLM uses JSON for configuration files:

```json
{
    "brain": {
        "learning_rate": 0.01,
        "memory_capacity": 1000,
        "noise_level": 0.1,
        "plasticity": {
            "stdp_strength": 0.01,
            "hebbian_strength": 0.005,
            "structural_rate": 0.0001
        }
    },
    "agent": {
        "curiosity_enabled": true,
        "reward_modulation_enabled": true,
        "development_enabled": true,
        "structural_plasticity_enabled": true,
        "novelty_detection": {
            "sensitivity": 0.5,
            "decay_rate": 0.99
        }
    },
    "environment": {
        "width": 100,
        "height": 100,
        "vision_width": 16,
        "vision_height": 16,
        "max_energy": 1.0,
        "energy_decay_rate": 0.01
    }
}
```

### Environment Configuration

```python
import pynlm

# Create world with custom configuration
world = pynlm.SimpleWorld()
world.configure(
    width=200,           # World width in cells
    height=150,          # World height in cells
    vision_width=32,     # Vision sensor width
    vision_height=32     # Vision sensor height
)

# Set world properties
world.setMaxEnergy(2.0)           # Maximum agent energy
world.setEnergyDecayRate(0.005)   # Energy decay per timestep
world.setRandomSeed(12345)         # Reproducible random seed

# Add objects to world
from pynlm import WorldObject, WorldObjectType

# Add resource (positive reward)
resource = WorldObject(50, 50, WorldObjectType.Resource, 10.0)
world.addObject(resource)

# Add hazard (negative reward)
hazard = WorldObject(80, 80, WorldObjectType.Hazard, -5.0)
world.addObject(hazard)

# Add wall
wall = WorldObject(10, 10, WorldObjectType.Wall, 0.0)
wall.radius = 2.0
world.addObject(wall)
```

### Agent Configuration

```python
import pynlm

# Create agent with advanced configuration
brain = pynlm.createBrain()
agent = pynlm.createAgentBrain(brain)

# Enable different agent capabilities
agent.enableRewardModulation(True)        # Reward-based learning
agent.enableStructuralPlasticity(True)   # Neural connectivity changes
agent.enableDevelopment(True)             # Developmental stages
agent.enableCuriosity(True)               # Exploration motivation

# Configure curiosity parameters
agent.setCuriosityLevel(0.5)
agent.setNoveltyLevel(0.3)
agent.setPredictionError(0.1)

# Get agent state information
print(f"Neuromodulation level: {agent.getNeuromodulationLevel()}")
print(f"Curiosity level: {agent.getCuriosityLevel()}")
print(f"Novelty level: {agent.getNoveltyLevel()}")
print(f"Prediction error: {agent.getPredictionError()}")
```

## Building and Testing

### Build Options

Use CMake options to customize the build:

```bash
# Configure with specific options
cmake .. \
    -DBUILD_PYTHON_BINDINGS=ON \
    -DBUILD_TESTS=ON \
    -DBUILD_DEMOS=ON \
    -DBUILD_BENCHMARKS=ON \
    -DENABLE_VECTORIZATION=ON \
    -DENABLE_THREADS=ON \
    -DENABLE_COVERAGE=ON

# Build specific components
make nlm_core
make pynlm
make nlm_phase6_demo

# Run specific tests
make test-nlm_core
make test-pynlm
make test-nlm_phase6_demo
```

### Testing Framework

The NLM framework includes comprehensive testing:

```bash
# Run all tests
ctest

# Run specific test suites
ctest --output-on-failure
ctest -R "config"           # Configuration tests
ctest -R "random"           # Random number tests
ctest -R "brain"           # Brain system tests
ctest -R "experiment"       # Experiment tests

# Run tests with verbose output
ctest -V

# Run tests with coverage (if enabled)
ctest --output-on-failure --coverage
```

### Build Configurations

Different build configurations for various use cases:

#### Development Mode
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_DEBUG=ON
```

#### Performance Mode
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_VECTORIZATION=ON -DENABLE_THREADS=ON
```

#### Test Mode
```bash
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_TESTS=ON -DENABLE_COVERAGE=ON
```

#### Minimal Mode
```bash
cmake .. -DBUILD_TESTS=OFF -DBUILD_PYTHON_BINDINGS=OFF -DBUILD_DEMOS=OFF
```

## Performance Tuning

### Optimizing Neural Performance

#### SIMD Vectorization
```cpp
// Enable SIMD optimizations
// Compiler flags: -mavx2 -mfma (for GCC/Clang)
// Set in CMake: option(ENABLE_VECTORIZATION "Enable SIMD vectorization" ON)
```

#### Memory Management
```cpp
// Use memory pools for better performance
// Configured via build options
```

#### Parallel Processing
```cpp
// Enable parallel event processing
// Set in CMake: option(ENABLE_PARALLEL "Enable parallel processing" ON)
```

### Performance Monitoring

```python
# Monitor neural performance in real-time
brain.enablePerformanceLogging(True)

# Log key performance metrics
brain.logPerformanceMetric("total_spikes", brain.getTotalSpikeCount())
brain.logPerformanceMetric("neuron_count", brain.getTotalNeuronCount())
brain.logPerformanceMetric("average_firing_rate", brain.getAverageFiringRate())
brain.logPerformanceMetric("memory_usage", brain.getMemoryUsage())
```

## Development Guide

### Adding New Neural Components

To add new neural components to NLM:

1. **Create new header files** in the appropriate source directory
2. **Implement the component** following existing patterns
3. **Update CMakeLists.txt** to include the new files
4. **Add tests** for the new functionality
5. **Update documentation** with examples

### Contributing to NLM

1. **Fork** the repository
2. **Create** a feature branch
3. **Make** your changes
4. **Add tests** for new functionality
5. **Update documentation** as needed
6. **Submit** a pull request

### Code Quality Guidelines

- **Follow C++20 standards**
- **Use RAII for resource management**
- **Implement proper error handling**
- **Add comprehensive documentation**
- **Include unit tests**
- **Maintain consistent coding style**

## Troubleshooting

### Common Issues and Solutions

#### Build Errors

**Issue**: Compiler doesn't support required C++ features

**Solution**: Update CMakeLists.txt to check for C++20 support:
```cmake
if(CMAKE_CXX_COMPILER_VERSION LESS 11)
    message(FATAL_ERROR "NLM requires C++17 or later")
endif()
```

#### Python Binding Errors

**Issue**: Python bindings not building

**Solution**: Check if Python3 development packages are installed:
```bash
sudo apt-get install python3-dev python3-pip
# or
brew install python3
```

#### Memory Issues

**Issue**: Running out of memory during simulation

**Solution**: Increase system memory or optimize memory usage:
```cpp
// In configuration file
{"brain": {"memory_pool_size": 2048}}
```

#### Performance Issues

**Issue**: Simulation running slowly

**Solution**: Enable performance optimizations:
```bash
cmake .. -DENABLE_VECTORIZATION=ON -DENABLE_THREADS=ON -DENABLE_PARALLEL=ON
```

### Getting Help

If you encounter issues:

1. **Check the documentation** for common problems
2. **Review the code** for potential issues
3. **Run tests** to identify failures
4. **Check build logs** for error details
5. **Consult the community** for support

## License

MIT License

Copyright (c) 2026 NLM Authors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## Acknowledgments

The NLM project builds on extensive research in computational neuroscience
and artificial intelligence. Special thanks to:

- Contributors to the neural computation research
- Developers of the C++ neural simulation framework
- Users who provide feedback and suggestions
- The open-source community for related tools and libraries

## References

1. Gerstner, W., Krojas, M., & Naud, R. (2022). Neural Dynamics.
2. Abbott, L. F., & Cauli, B. (2020). The economy of brain energy.
3. Maass, W., Bishop, T., & Wright, R. (2002). Computation by neural systems.
4. Sejnowski, T. J., & Churchland, P. S. (2009). The Computational Brain.

## Version Information

- **Version**: 0.1.0
- **Phase**: 6 - Final Integration
- **Build Date**: $(date)
- **Compiler**: $(c++ --version 2>/dev/null || echo "Unknown")
- **Platform**: ${NLM_PLATFORM}

---

*This documentation is continuously updated. Please check the repository for the latest information.*

## Quick Links

- [GitHub Repository](https://github.com/nlm-project/nlm)
- [Documentation](https://nlm.readthedocs.io)
- [API Reference](https://nlm.readthedocs.io/en/latest/api/)
- [Examples](https://github.com/nlm-project/nlm/tree/main/examples)
- [Issues](https://github.com/nlm-project/nlm/issues)
- [Discussions](https://github.com/nlm-project/nlm/discussions)

---