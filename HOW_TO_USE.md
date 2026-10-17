# NLM (Neural Learning Machine) - Comprehensive Usage Guide

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document provides comprehensive guidance on using NLM, from basic setup to advanced usage patterns.

## Table of Contents

1. [Installation and Setup](#installation-and-setup)
2. [Building the C++ Project](#building-the-c-project)
3. [Installing the Python Library](#installing-the-python-library)
4. [Using the Python API](#using-the-python-api)
5. [Core Classes and Methods](#core-classes-and-methods)
6. [Configuration Options](#configuration-options)
7. [Example Scripts](#example-scripts)
8. [Advanced Usage](#advanced-usage)
9. [Performance Optimization](#performance-optimization)
10. [Troubleshooting](#troubleshooting)
11. [Best Practices](#best-practices)

## Installation and Setup

### Prerequisites

- **Operating System**: Linux, macOS, or Windows with WSL
- **Compiler**: C++17 compatible compiler (GCC 9+, Clang 7+, MSVC 2019+)
- **Build Tools**: CMake 3.16+, Make or Ninja
- **Python**: 3.8+ (for Python bindings and examples)

### Hardware Requirements

- **Minimum**: 2GB RAM, 1 CPU core
- **Recommended**: 8GB+ RAM, 4+ CPU cores for simulations
- **Large Networks**: 16GB+ RAM for >10,000 neurons

## Building the C++ Project

### Standard Build

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_CXX_STANDARD=17 \
         -DCMAKE_CXX_STANDARD_REQUIRED=ON \
         -DBUILD_PYTHON_BINDINGS=ON

# Build all targets with parallel compilation
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Build with OpenMP support (for parallel processing)
cmake .. -DOpenMP_CXX_FLAGS="-fopenmp"

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install

# Build with coverage (for testing)
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |
| `libpynlm.so` | Shared Library | Python bindings (Linux) |

## Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core numpy pytest

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verifying Installation

```python
import pynlm

# Check version
print(f"NLM Python bindings version: {pynlm.__version__}")

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Check brain properties
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print("Installation successful!")
```

## Using the Python API

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running a Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Creating a Simple World

```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Run world update
world.update(timestep=0.1)
```

### Complete Agent Example

```python
def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent

# Run the simulation
brain, world, agent = run_agent_simulation(1000)
```

### Key Classes and Methods

#### Brain Class

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class

```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### AgentBrain Class

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

#### SimpleWorld Class

```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

### Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
# ... and more

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Example Scripts

### Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

### Complete Agent Example

```python
import pynlm

def run_complete_agent_example():
    """Example showing how to use all major NLM components."""
    
    # Configuration
    config = pynlm.createDefaultConfig()
    
    # Brain setup
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Agent setup
    agent = pynlm.createAgentBrain(brain)
    
    # World setup
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        agent.processSensoryInput(world.getSensoryPercept())
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Do action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Optional: print progress
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    return True

run_complete_agent_example()
```

## Advanced Usage

### Custom Configuration

```python
import pynlm
import json

# Create custom configuration
custom_config = {
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.15,
        "v_thresh": -55.0,
        "v_rest": -65.0
    },
    "plasticity": {
        "stdp": {
            "enable": True,
            "learning_rate": 0.002
        },
        "hebbian": {
            "enable": True,
            "learning_rate": 0.0005
        }
    }
}

# Create config object
config = pynlm.createConfig(custom_config)

# Verify configuration
print(f"Neuron count: {config.get('brain.neuron_count')}")
print(f"STDP enabled: {config.get('plasticity.stdp.enable')}")
```

### Multi-threaded Simulation

```python
import pynlm
import threading
import time

def run_simulation_step(brain, step):
    """Run a single simulation step in a thread."""
    brain.step(step)
    return brain.getFiringNeuronCount()

def parallel_simulation(num_steps=100, num_threads=4):
    """Run simulation using multiple threads."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    threads = []
    results = []
    
    for step in range(num_steps):
        # Create and start thread
        thread = threading.Thread(
            target=lambda s=step: results.append(run_simulation_step(brain, s))
        )
        threads.append(thread)
        thread.start()
        
        # Limit concurrent threads
        if len(threads) >= num_threads:
            for t in threads:
                t.join()
            threads = []
    
    # Wait for remaining threads
    for t in threads:
        t.join()
    
    print(f"Completed {num_steps} simulation steps using {num_threads} threads")
    return brain

# Run parallel simulation
brain = parallel_simulation(50, 4)
```

### Checkpoint Management

```python
import pynlm
import os

def save_simulation(brain, path="checkpoint.npy"):
    """Save brain state to file."""
    brain.save(path)
    print(f"Saved brain state to {path}")

def load_simulation(path="checkpoint.npy"):
    """Load brain state from file."""
    if not os.path.exists(path):
        print(f"Checkpoint file {path} does not exist!")
        return None
    
    # Create new brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Load state
    brain.load(path)
    print(f"Loaded brain state from {path}")
    return brain

# Usage example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some steps
for i in range(100):
    brain.step(i)

# Save checkpoint
save_simulation(brain, "my_checkpoint.npy")

# Later, load the checkpoint
loaded_brain = load_simulation("my_checkpoint.npy")
if loaded_brain:
    print(f"Loaded brain has {loaded_brain.getTotalNeuronCount()} neurons")
```

## Performance Optimization

### Key Performance Considerations

1. **Memory Management**
   - Use memory pools for large neuron populations
   - Pre-allocate memory for frequent operations
   - Monitor memory usage during long simulations

2. **CPU Optimization**
   - Use parallel processing for independent neuron groups
   - Vectorize operations using SIMD instructions
   - Optimize data layouts for cache efficiency

3. **Simulation Parameters**
   - Adjust neuron count based on available memory
   - Use appropriate time steps for your application
   - Balance between simulation accuracy and performance

### Performance Monitoring

```python
import pynlm
import time
import psutil
import os

def monitor_performance(brain, num_steps=1000):
    """Monitor performance of brain simulation."""
    process = psutil.Process(os.getpid())
    
    # Initial memory usage
    initial_memory = process.memory_info().rss / 1024 / 1024  # MB
    
    start_time = time.time()
    
    for step in range(num_steps):
        brain.step(step)
        
        # Monitor every 100 steps
        if step % 100 == 0:
            current_memory = process.memory_info().rss / 1024 / 1024
            elapsed = time.time() - start_time
            
            print(f"Step {step}:")
            print(f"  Time: {elapsed:.2f}s")
            print(f"  Memory: {current_memory - initial_memory:.2f}MB")
            print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  Firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    total_time = time.time() - start_time
    print(f"\nSimulation complete!")
    print(f"Total time: {total_time:.2f}s")
    print(f"Average step time: {total_time/num_steps:.4f}s")

# Run performance monitoring
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
brain = pynlm.createBrain(config)
brain.initialize()

monitor_performance(brain, 500)
```

## Troubleshooting

### Common Issues and Solutions

#### Build Issues

**Problem**: CMake cannot find Python
```bash
# Solution:
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Problem**: Compilation errors
```bash
# Solutions:
# 1. Ensure C++20 compiler support
cmake .. -DCMAKE_CXX_STANDARD=17
# 2. Check CMake version >= 3.16
cmake --version
# 3. Verify all source files are present
ls -la src/
```

#### Runtime Issues

**Problem**: ImportError: No module named 'pynlm'
```bash
# Solution:
pip install --force-reinstall .
```

**Problem**: Segmentation faults
```python
# Solution:
# 1. Check that initialize() is called before step()
# 2. Ensure reset() is called before re-running simulation
# 3. Check for invalid configuration parameters

brain = pynlm.createBrain(config)
brain.initialize()  # Required!

for step in range(100):
    brain.step(step)
```

**Problem**: Memory errors
```python
# Solution:
# 1. Monitor memory usage during long simulations
# 2. Use smaller brain configurations for testing
# 3. Check available system memory

# For large simulations, use larger configuration
config.set("brain.neuron_count", 5000)  # Adjust based on available memory
```

### Getting Help

#### Online Resources

- **Documentation**: https://github.com/yourusername/nlm/wiki
- **API Reference**: https://yourusername.github.io/nlm/python/
- **Discussions**: https://github.com/yourusername/nlm/discussions
- **Issues**: https://github.com/yourusername/nlm/issues

#### Local Help

```bash
# View help for C++ program
./nlm --help

# View Python help
import pynlm
help(pynlm)
help(pynlm.createBrain)
```

## Best Practices

### Configuration Best Practices

1. **Always validate configuration** before running simulation:
```python
config = pynlm.createDefaultConfig()
# Set custom values
config.set("brain.neuron_count", 1000)

# Validate
if config.has("brain.neuron_count"):
    print(f"Valid neuron count: {config.get('brain.neuron_count')}")
```

2. **Use appropriate default configurations** for different use cases:
```python
# For research: large networks with detailed recording
research_config = pynlm.createDefaultConfig()
research_config.set("brain.neuron_count", 10000)

# For development: smaller, faster simulations
dev_config = pynlm.createDefaultConfig()
dev_config.set("brain.neuron_count", 100)

# For production: optimized for speed
prod_config = pynlm.createDefaultConfig()
prod_config.set("brain.neuron_count", 500)
prod_config.set("plasticity.stdp.enable", True)
```

### Simulation Best Practices

1. **Always initialize before using**:
```python
brain = pynlm.createBrain(config)
brain.initialize()  # NEVER skip this!
```

2. **Properly manage simulation state**:
```python
# Save important states
brain.save("start_state.bin")

# Run simulation
for step in range(1000):
    brain.step(step)

# Save final state
brain.save("final_state.bin")
```

3. **Monitor simulation statistics**:
```python
for step in range(1000):
    brain.step(step)
    
    # Log statistics every 100 steps
    if step % 100 == 0:
        stats = {
            'step': step,
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'development_stage': brain.getDevelopmentalStage()
        }
        print(stats)
```

4. **Enable appropriate subsystems**:
```python
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)     # For learning from rewards
agent.enableCuriosity(True)            # For exploration behavior
agent.enableDevelopment(True)          # For developmental changes
agent.enableStructuralPlasticity(True) # For neural growth
```

### File Management Best Practices

1. **Use meaningful filenames**:
```python
# Good
brain.save("experiment_1_1000_steps.npy")
brain.save("baseline_simulation.npy")
brain.save("curiosity_enabled.npy")

# Avoid
brain.save("test.npy")
brain.save("data.npy")
brain.save("output.npy")
```

2. **Document simulation parameters**:
```python
import json

# Save simulation metadata
metadata = {
    "neuron_count": config.get("brain.neuron_count"),
    "plasticity_enabled": {
        "stdp": config.get("plasticity.stdp.enable"),
        "hebbian": config.get("plasticity.hebbian.enable")
    },
    "learning_enabled": {
        "reward_modulation": agent.getNeuromodulationLevel() > 0,
        "curiosity": agent.getCuriosityLevel() > 0
    },
    "start_time": datetime.now().isoformat()
}

with open("simulation_metadata.json", 'w') as f:
    json.dump(metadata, f, indent=2)
```

## Advanced Features

### Custom Brain Creation

```python
import pynlm
import numpy as np

# Create brain with custom architecture
def create_custom_brain(neuron_counts, connection_patterns):
    """Create a brain with custom architecture."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", sum(neuron_counts))
    
    # Additional custom configuration based on connection patterns
    # This would depend on your specific requirements
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain

# Example usage
custom_brain = create_custom_brain(
    neuron_counts=[100, 200, 50],
    connection_patterns="fully_connected"
)
```

### Multi-environment Simulation

```python
import pynlm

def simulate_multiple_environments(env_configs, brain_config):
    """Run simulation in multiple environments."""
    results = {}
    
    for env_name, env_config in env_configs.items():
        # Create environment
        world = pynlm.createSimpleWorld()
        world.configure(**env_config)
        world.reset()
        
        # Create brain
        brain = pynlm.createBrain(brain_config)
        brain.initialize()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        results[env_name] = {
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'development_stage': brain.getDevelopmentalStage()
        }
    
    return results

# Example usage
env_configs = {
    'simple': {'width': 10, 'height': 10, 'visionWidth': 8, 'visionHeight': 8},
    'complex': {'width': 20, 'height': 20, 'visionWidth': 16, 'visionHeight': 16}
}

results = simulate_multiple_environments(env_configs, pynlm.createDefaultConfig())
```

### Research Mode

```python
import pynlm
import pandas as pd

def research_simulation(num_steps=5000, recording_interval=10):
    """Run research simulation with detailed recording."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Recording data structures
    recordings = {
        'steps': [],
        'firing_counts': [],
        'firing_rates': [],
        'development_stages': [],
        'curiosity_levels': [],
        'novelty_levels': []
    }
    
    for step in range(num_steps):
        # Run simulation step
        brain.step(step)
        
        # Record data periodically
        if step % recording_interval == 0:
            recordings['steps'].append(step)
            recordings['firing_counts'].append(brain.getFiringNeuronCount())
            recordings['firing_rates'].append(brain.getAverageFiringRate())
            recordings['development_stages'].append(brain.getDevelopmentalStage())
            
            # Get agent stats if available
            # (This would depend on your specific setup)
    
    # Convert to DataFrame for analysis
    df = pd.DataFrame(recordings)
    
    # Save results
    df.to_csv('research_data.csv', index=False)
    
    return df

# Run research simulation
data = research_simulation(1000)
print(f"Recorded {len(data)} data points")
```

## Conclusion

NLM provides a comprehensive framework for brain-inspired neural network simulations. This guide covers the essential aspects of using NLM effectively, from basic setup to advanced usage patterns.

Key takeaways:

1. **Always initialize components** before use
2. **Monitor simulation statistics** for debugging and optimization
3. **Use appropriate configurations** for your specific use case
4. **Save and load states** for reproducible experiments
5. **Follow best practices** for maintainable and efficient code

For more detailed information, refer to the other documentation files in the repository, particularly:

- `ARCHITECTURE.md` - System architecture overview
- `SCIENCE.md` - Scientific background
- `EXPERIMENTS.md` - Experiment descriptions
- `docs/` - Additional technical documentation

---

*This documentation is continuously updated. For the latest information, check the repository's wiki and release notes.*

**Last updated**: 2024-01-17
**Version**: 1.0.0

For issues, contributions, or feedback: https://github.com/yourusername/nlm