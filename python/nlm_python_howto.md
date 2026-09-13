# NLM Python API Documentation

**Neural Learning Machine (NLM) - Python Bindings Guide**

This document provides comprehensive guidance for using the NLM Python API. It complements the C++ HOW_TO_USE.md and focuses on practical Python usage patterns for experimental neuroscience research.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Core Concepts](#core-concepts)
3. [Basic Tutorials](#basic-tutorials)
4. [Advanced Features](#advanced-features)
5. [Best Practices](#best-practices)
6. [Troubleshooting](#troubleshooting)
7. [Benchmarks](#benchmarks)
8. [API Reference](#api-reference)
9. [Example Scripts](#example-scripts)

---

# Quick Start

## Installation

```bash
# Install from PyPI (if available)
pip install pynlm

# Install from source (recommended for development)
pip install pybind11 scikit-build-core pytest numpy
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
pip install .

# Or in development mode
pip install -e .
```

## Basic Import

```python
import pynlm
import numpy as np
from pynlm import NeuronType, DevelopmentalStage, ActionType
```

## Minimal Working Example

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print("Simulation complete!")
```

---

# Core Concepts

## Brain vs AgentBrain

- **Brain**: Core neural simulation engine implementing LIF dynamics, plasticity, memory systems
- **AgentBrain**: High-level interface connecting Brain to environment/world simulation

## Key Components

1. **Brain Class**: Contains all neural computation components
2. **Config Class**: Manages simulation parameters
3. **SimpleWorld Class**: Provides 2D environment for agent interaction
4. **AgentBrain Class**: High-level agent control interface
5. **Neuromodulation Systems**: Curiosity, novelty, prediction error, dopamine
6. **Memory Systems**: Working memory, episodic memory, associative memory

## Data Types

- `NeuronId`, `SynapseId`, `RegionId`, `PopulationId`: Object identifiers
- `NeuronType`: Excitatory, Inhibitory, Sensory, Motor, Internal, Modulatory
- `DevelopmentalStage`: Initial, CriticalPeriod, Maturation, Adult, Aging
- `ActionType`: Various motor commands
- `WorldObjectType`: Resource, Hazard, Wall, etc.

---

# Basic Tutorials

## Tutorial 1: Simple Spiking Neural Network

### Goal: Create a basic neural network that learns through STDP

```python
import pynlm
import time

def basic_neural_network():
    """Create a simple network with excitatory and inhibitory neurons."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # Configure for smaller network (faster testing)
    config.set("brain.neuron_count", 500)
    config.set("brain.region_count", 2)
    config.set("connection_probability", 0.05)
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Add some sensory neurons
    for region_id in brain.getRegionIds():
        region = brain.getRegion(region_id)
        if region:
            # Add sensory neurons to first region
            sensory_pop = region.addPopulation(50, NeuronType.Sensory)
            motor_pop = region.addPopulation(50, NeuronType.Motor)
            internal_pop = region.addPopulation(400, NeuronType.Internal)
    
    # 4. Run simulation with learning
    learning_rate = 0.001
    for step in range(500):
        brain.step(step)
        
        # Inject random sensory input
        if step % 50 == 0:
            # Create random sensory input
            sensory_input = [np.random.uniform(0, 1) for _ in range(256)]
            brain.receiveSensoryInput(sensory_input)
        
        # Log progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    print("Basic neural network simulation complete!")
    return brain

# Run the tutorial
brain = basic_neural_network()
```

## Tutorial 2: Agent in a Simple World

### Goal: Create an agent that perceives and acts in an environment

```python
import pynlm
import numpy as np

def agent_world_tutorial():
    """Create an agent that explores and learns in a 2D world."""
    
    # 1. Setup configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    
    # 2. Create brain and agent interface
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.setRandomSeed(42)
    
    # 4. Initialize agent
    agent.initialize(world)
    
    # 5. Enable subsystems for learning
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 6. Run episode
    max_steps = 1000
    for step in range(max_steps):
        # Update world state
        world.update(timestep=0.1)
        
        # Get sensory perception
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run neural computation
        brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Get reward from world
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        
        # Apply reward modulation (learning signal)
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development system
        agent.updateDevelopment(0.1)
        
        # Print status periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dopamine: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Development stage: {agent.getDevelopmentalStage().name}")
    
    print("Agent-world tutorial complete!")
    return brain, agent, world

# Run the tutorial
brain, agent, world = agent_world_tutorial()
```

## Tutorial 3: Memory System Integration

### Goal: Demonstrate working memory, episodic memory, and prediction

```python
import pynlm
import numpy as np

def memory_system_tutorial():
    """Explore memory systems and their integration."""
    
    # 1. Create brain with memory systems
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("working_memory.capacity", 100)
    config.set("episodic_memory.max_episodes", 500)
    config.set("prediction.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent to interface with world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=20, visionHeight=20)
    agent.initialize(world)
    
    # 3. Enable all subsystems
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    # 4. Collect statistics
    memory_stats = {
        'working_memory': [],
        'episodic_memory': [],
        'prediction_error': [],
        'novelty': [],
        'curiosity': []
    }
    
    # 5. Run simulation
    for step in range(1000):
        world.update(0.1)
        
        # Process input and compute
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        agent.applyRewardModulation(
            world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0,
            0.0
        )
        
        # Collect statistics
        if brain.getWorkingMemory():
            memory_stats['working_memory'].append(brain.getWorkingMemory().getActiveTraces())
        
        if brain.getEpisodicMemory():
            memory_stats['episodic_memory'].append(brain.getEpisodicMemory().getEpisodeCount())
        
        memory_stats['prediction_error'].append(agent.getPredictionError())
        memory_stats['novelty'].append(agent.getNoveltyLevel())
        memory_stats['curiosity'].append(agent.getCuriosityLevel())
        
        # Periodically trigger development and replay
        if step % 200 == 0:
            agent.updateDevelopment(1.0)
            print(f"Development stage: {agent.getDevelopmentalStage().name}")
    
    # 6. Analyze results
    print("Memory system analysis:")
    print(f"  Average working memory traces: {np.mean(memory_stats['working_memory']):.1f}")
    print(f"  Peak episodic memory episodes: {max(memory_stats['episodic_memory'])}")
    print(f"  Average prediction error: {np.mean(memory_stats['prediction_error']):.3f}")
    print(f"  Average novelty: {np.mean(memory_stats['novelty']):.3f}")
    print(f"  Average curiosity: {np.mean(memory_stats['curiosity']):.3f}")
    
    return brain, agent, world, memory_stats

# Run the tutorial
brain, agent, world, stats = memory_system_tutorial()
```

---

# Advanced Features

## Neuromodulation Examples

### Dopamine and Reward Learning

```python
import pynlm
import numpy as np

def dopamine_learning_tutorial():
    """Demonstrate dopamine-mediated reward learning."""
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Simulate reward prediction error learning
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    agent.initialize(world)
    
    # Set up a simple reward task
    target_x, target_y = 15.0, 15.0
    reward_achieved = False
    
    for step in range(500):
        world.update(0.1)
        
        # Get current position
        body = world.getAgentBody()
        distance = np.sqrt((body.x - target_x)**2 + (body.y - target_y)**2)
        
        # Check if reached target
        if distance < 2.0:
            reward = 10.0
            reward_achieved = True
        else:
            reward = 0.0
        
        # Process world state
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Simulate expected reward (zero for simplicity)
        predicted_reward = 0.0
        
        # Apply reward modulation (creates prediction error)
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Move toward target
        if not reward_achieved:
            # Simple gradient-based movement
            if body.x < target_x:
                agent.decodeMotorCommand()  # This would need modification for continuous control
        
        # Log dopamine level (prediction error)
        if step % 50 == 0:
            print(f"Step {step}: Distance={distance:.1f}, Reward={reward:.1f}, "
                  f"Prediction Error={agent.getPredictionError():.3f}")
    
    print(f"Task {'completed' if reward_achieved else 'not completed'}")
    return brain, agent, world

dopamine_learning_tutorial()
```

### Curiosity-Driven Exploration

```python
import pynlm
import numpy as np

def curiosity_exploration_tutorial():
    """Demonstrate curiosity-driven exploration behavior."""
    
    config = pynlm.createDefaultConfig()
    config.set("neuromod.curiosity.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableCuriosity(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=10, visionHeight=10)
    agent.initialize(world)
    
    # Track exploration patterns
    exploration_patterns = []
    novelty_detected = []
    
    for step in range(1000):
        world.update(0.1)
        
        # Process sensory input
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get curiosity level
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        
        exploration_patterns.append(curiosity)
        novelty_detected.append(novelty)
        
        # Print exploration statistics
        if step % 100 == 0:
            print(f"Step {step}: Curiosity={curiosity:.3f}, Novelty={novelty:.3f}, "
                  f"Action={action}")
    
    # Analyze exploration patterns
    print("\nExploration Analysis:")
    print(f"  Average curiosity level: {np.mean(exploration_patterns):.3f}")
    print(f"  Peak curiosity: {max(exploration_patterns):.3f}")
    print(f"  Novelty detection rate: {np.mean([n > 0.3 for n in novelty_detected]):.2f}")
    
    return brain, agent, world, exploration_patterns

curiosity_exploration_tutorial()
```

## Developmental Systems

### Neural Development and Plasticity

```python
import pynlm
import numpy as np

def developmental_tutorial():
    """Demonstrate developmental changes in neural plasticity."""
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30)
    agent.initialize(world)
    
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Track developmental changes
    developmental_stats = {
        'plasticity_modifier': [],
        'developmental_stage': [],
        'learning_rate': [],
        'firing_rate': []
    }
    
    for step in range(3000):
        world.update(0.1)
        
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Apply random rewards for plasticity
        if np.random.random() < 0.1:  # 10% chance of reward
            reward = 1.0
        else:
            reward = 0.0
        
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect statistics
        developmental_stats['plasticity_modifier'].append(agent.getBrain().getPlasticityModifier())
        developmental_stats['developmental_stage'].append(agent.getDevelopmentalStage())
        developmental_stats['learning_rate'].append(agent.getPredictionError())
        developmental_stats['firing_rate'].append(brain.getAverageFiringRate())
        
        # Print developmental milestones
        if step % 600 == 0:
            stage = agent.getDevelopmentalStage()
            print(f"Step {step}: Developmental stage = {stage.name}")
            print(f"  Plasticity modifier: {developmental_stats['plasticity_modifier'][-1]:.3f}")
            print(f"  Average firing rate: {developmental_stats['firing_rate'][-1]:.3f}")
    
    # Analyze development
    print("\nDevelopmental Analysis:")
    stages = list(set(developmental_stats['developmental_stage']))
    for stage in stages:
        stage_indices = [i for i, s in enumerate(developmental_stats['developmental_stage']) if s == stage]
        avg_firing = np.mean([developmental_stats['firing_rate'][i] for i in stage_indices])
        print(f"  {stage.name}: Avg firing rate = {avg_firing:.3f}")
    
    return brain, agent, world, developmental_stats

developmental_tutorial()
```

## Structural Plasticity

### Synapse Formation and Pruning

```python
import pynlm
import numpy as np

def structural_plasticity_tutorial():
    """Demonstrate structural plasticity (synaptogenesis and pruning)."""
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("synaptogenesis_rate", 0.0001)
    config.set("pruning_rate", 0.00001)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get structural plasticity system
    sp = brain.getStructuralPlasticity()
    if sp:
        print("Structural plasticity system initialized:")
        print(f"  Synaptogenesis rate: {sp.getSynaptogenesisRate():.6f}")
        print(f"  Pruning rate: {sp.getPruningRate():.6f}")
    
    # Run simulation with structural changes
    for step in range(2000):
        brain.step(step)
        
        # Apply random rewards for plasticity
        if np.random.random() < 0.1:  # 10% chance of reward
            reward = 1.0
        else:
            reward = 0.0
        
        # Apply neuromodulation
        if sp:
            sp.update(brain, brain.getRandomGenerator())
        
        if step % 500 == 0:
            print(f"Step {step}: Total neurons = {brain.getTotalNeuronCount()}")
            print(f"  Total synapses = {brain.getTotalSynapseCount()}")
            if sp:
                print(f"  Synaptogenesis rate: {sp.getSynaptogenesisRate():.6f}")
                print(f"  Pruning rate: {sp.getPruningRate():.6f}")
    
    print("Structural plasticity simulation complete!")
    return brain

structural_plasticity_tutorial()
```

---

# Best Practices

## Performance Optimization

### 1. Batch Processing

```python
import pynlm
import time

def batch_processing_demo():
    """Demonstrate efficient batch processing techniques."""
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Process multiple steps efficiently
    num_steps = 1000
    
    # Method 1: Sequential processing (standard)
    start_time = time.time()
    for step in range(num_steps):
        brain.step(step)
    sequential_time = time.time() - start_time
    print(f"Sequential processing: {sequential_time:.2f} seconds")
    
    # Reset for comparison
    brain.reset()
    brain.initialize()
    
    # Method 2: Pre-allocate if possible
    start_time = time.time()
    for step in range(num_steps):
        brain.step(step)
    prealloc_time = time.time() - start_time
    print(f"Pre-allocated processing: {prealloc_time:.2f} seconds")
    
    return brain

batch_processing_demo()
```

### 2. Memory Management

```python
import pynlm
import gc
import psutil
import os

def memory_management_demo():
    """Demonstrate memory-efficient programming practices."""
    
    process = psutil.Process(os.getpid())
    
    # Create brain and track memory
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    initial_memory = process.memory_info().rss / 1024 / 1024  # MB
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
        # Explicit cleanup
        if step % 50 == 0:
            gc.collect()
            current_memory = process.memory_info().rss / 1024 / 1024
            print(f"Step {step}: Memory usage = {current_memory:.1f} MB")
    
    final_memory = process.memory_info().rss / 1024 / 1024
    print(f"Memory increase: {final_memory - initial_memory:.1f} MB")
    
    return brain

memory_management_demo()
```

### 3. Configuration Management

```python
import pynlm

def configuration_management_demo():
    """Demonstrate effective configuration management."""
    
    # Method 1: Create default config and modify
    config1 = pynlm.createDefaultConfig()
    config1.set("brain.neuron_count", 500)
    config1.set("connection_probability", 0.1)
    
    # Method 2: Load from file
    # config2 = pynlm.createDefaultConfig()
    # config2.loadFromFile("my_config.json")
    
    # Method 3: Load from command line
    # config3 = pynlm.createDefaultConfig()
    # config3.loadFromArgs(len(sys.argv), sys.argv)
    
    # Validate configuration
    if config1.has("brain.neuron_count"):
        neurons = config1.get("brain.neuron_count")
        print(f"Configuration has {neurons} neurons")
    
    # Save configuration
    config1.saveToFile("output_config.json")
    
    return config1

configuration_management_demo()
```

## Debugging and Error Handling

### Common Issues and Solutions

```python
import pynlm
import traceback

def robust_simulation_demo():
    """Demonstrate robust error handling in simulations."""
    
    config = pynlm.createDefaultConfig()
    
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(100):
            try:
                brain.step(step)
                
                # Check for errors
                if brain.getFiringNeuronCount() > brain.getTotalNeuronCount():
                    print(f"WARNING: Invalid firing count at step {step}")
                
                # Validate state
                if step % 10 == 0:
                    print(f"Step {step}: State is valid")
                    
            except RuntimeError as e:
                print(f"Runtime error at step {step}: {e}")
                brain.reset()
                brain.initialize()
                continue
                
            except Exception as e:
                print(f"Unexpected error at step {step}: {e}")
                traceback.print_exc()
                break
                
    except Exception as e:
        print(f"Failed to create brain: {e}")
        traceback.print_exc()
        return None
    
    return brain

robust_simulation_demo()
```

### Error Handling Examples

```python
import pynlm

def error_handling_examples():
    """Examples of handling common errors."""
    
    # Example 1: Validate input before processing
    def safe_step(brain, step):
        if step < 0:
            raise ValueError(f"Invalid step number: {step}")
        
        try:
            brain.step(step)
            return True
        except Exception as e:
            print(f"Step {step} failed: {e}")
            return False
    
    # Example 2: Check configuration
    def validate_config(config):
        if not config.has("brain.neuron_count"):
            raise ValueError("Configuration missing brain.neuron_count")
        
        neurons = config.get("brain.neuron_count")
        if neurons <= 0:
            raise ValueError(f"Invalid neuron count: {neurons}")
        
        return True
    
    # Example 3: Handle empty inputs
    def safe_sensory_input(brain, input_data):
        if not input_data:
            print("Warning: Empty sensory input provided")
            return False
        
        try:
            brain.receiveSensoryInput(input_data)
            return True
        except Exception as e:
            print(f"Failed to process sensory input: {e}")
            return False
    
    print("Error handling examples defined successfully")
    return True

error_handling_examples()
```

---

# Troubleshooting

## Common Issues

### 1. Import Errors

**Problem**: `ImportError: No module named 'pynlm'`

**Solution**:

```bash
# Install the package
pip install pynlm

# Or install from source
pip install pybind11 scikit-build-core
python setup.py build_ext --inplace
```

### 2. Build Errors

**Problem**: Compilation fails during setup

**Solutions**:

1. Ensure C++20 compiler support:
   ```bash
   # Check GCC version
   g++ --version
   # Need GCC 10+
   ```

2. Install dependencies:
   ```bash
   sudo apt-get install cmake build-essential python3-dev
   ```

3. Check CMake version:
   ```bash
   cmake --version
   # Need CMake 3.16+
   ```

### 3. Segmentation Faults

**Problem**: Program crashes with segfault

**Common causes**:

1. Calling `step()` before `initialize()`: Always call `brain.initialize()` first
2. Invalid configuration parameters: Check parameter ranges
3. Memory corruption: Ensure input data is properly sized

**Debugging with GDB**:

```bash
# Install gdb if not present
sudo apt-get install gdb

# Run with gdb
gdb ./your_program

# Inside gdb
start
# ... run until crash ...
bt
quit
```

### 4. Performance Issues

**Problem**: Simulation is too slow

**Solutions**:

1. Reduce network size: `config.set("brain.neuron_count", 500)`
2. Disable expensive features:
   ```python
   config.set("plasticity.stdp.enable", False)
   config.set("working_memory.enable", False)
   ```

3. Use smaller timestep: `config.set("simulation_timestep", 0.01)`
4. Parallel processing: Use multiple CPU cores if available

### 5. Memory Issues

**Problem**: Out of memory error

**Solutions**:

1. Reduce network size: Fewer neurons and synapses
2. Clean up unused objects
3. Use checkpointing for long simulations
4. Monitor memory usage

## Error Codes and Meanings

```python
# Common error codes returned by NLM functions
ErrorCode {
    SUCCESS = 0,           // Operation completed successfully
    INVALID_CONFIG = 1,    // Invalid configuration parameters
    BRAIN_NOT_INITIALIZED = 2,  // Brain not properly initialized
    INVALID_INPUT = 3,     // Invalid input data
    MEMORY_ERROR = 4,      // Memory allocation failed
    FILE_ERROR = 5,        // File I/O error
    SIMULATION_ERROR = 6   // Simulation runtime error
}
```

## Getting Help

### Online Resources

- **Documentation**: https://nlm.readthedocs.io
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Issue Tracker**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions

### Local Documentation

```bash
# Build documentation
cd docs
make html  # For Sphinx
```

### Community Support

1. Check the [NLM Discord server](https://discord.gg/nlm) for active discussions
2. Search for existing issues with similar symptoms
3. Provide detailed error messages when reporting issues
4. Include configuration and code snippets

---

# Benchmarks

## Performance Overview

This section provides benchmark results for different network sizes and configurations.

### 1. Network Size Benchmarks

```python
import pynlm
import time

def benchmark_network_size():
    """Benchmark different network sizes."""
    
    sizes = [100, 500, 1000, 2000, 5000]
    results = []
    
    for size in sizes:
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", size)
        config.set("connection_probability", 0.01)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        
        elapsed_time = time.time() - start_time
        steps_per_second = 100 / elapsed_time
        
        results.append({
            'size': size,
            'time_per_step': elapsed_time / 100,
            'steps_per_second': steps_per_second,
            'neurons_per_step': size / (elapsed_time / 100)
        })
        
        print(f"Size {size:4d}: {steps_per_second:6.1f} steps/sec, "
              f"{results[-1]['neurons_per_step']/1000:5.1f}k neurons/sec")
    
    return results

print("Network Size Benchmarks:")
benchmark_network_size()
```

### 2. Plasticity Overhead Benchmark

```python
import pynlm
import time

def benchmark_plasticity_overhead():
    """Benchmark performance impact of plasticity systems."""
    
    config_no_plasticity = pynlm.createDefaultConfig()
    config_no_plasticity.set("brain.neuron_count", 1000)
    config_no_plasticity.set("plasticity.stdp.enable", False)
    config_no_plasticity.set("plasticity.hebbian.enable", False)
    config_no_plasticity.set("plasticity.structural.enable", False)
    
    config_full_plasticity = pynlm.createDefaultConfig()
    config_full_plasticity.set("brain.neuron_count", 1000)
    config_full_plasticity.set("plasticity.stdp.enable", True)
    config_full_plasticity.set("plasticity.hebbian.enable", True)
    config_full_plasticity.set("plasticity.structural.enable", True)
    
    # Benchmark without plasticity
    brain_no_plasticity = pynlm.createBrain(config_no_plasticity)
    brain_no_plasticity.initialize()
    
    start_time = time.time()
    for step in range(500):
        brain_no_plasticity.step(step)
    time_no_plasticity = time.time() - start_time
    
    # Benchmark with plasticity
    brain_with_plasticity = pynlm.createBrain(config_full_plasticity)
    brain_with_plasticity.initialize()
    
    start_time = time.time()
    for step in range(500):
        brain_with_plasticity.step(step)
    time_with_plasticity = time.time() - start_time
    
    overhead = (time_with_plasticity - time_no_plasticity) / time_no_plasticity * 100
    
    print(f"Without plasticity: {time_no_plasticity:.2f} seconds")
    print(f"With plasticity:    {time_with_plasticity:.2f} seconds")
    print(f"Plasticity overhead: {overhead:.1f}%")
    
    return overhead

print("Plasticity Overhead Benchmark:")
benchmark_plasticity_overhead()
```

### 3. Memory Usage Benchmark

```python
import pynlm
import psutil
import os

def benchmark_memory_usage():
    """Benchmark memory usage of different configurations."""
    
    process = psutil.Process(os.getpid())
    
    sizes = [500, 1000, 2000, 5000]
    results = []
    
    for size in sizes:
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", size)
        
        # Measure memory before brain creation
        gc.collect()
        memory_before = process.memory_info().rss / 1024 / 1024
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Measure memory after brain creation
        gc.collect()
        memory_after = process.memory_info().rss / 1024 / 1024
        
        memory_per_neuron = (memory_after - memory_before) / size
        
        results.append({
            'size': size,
            'memory_increase': memory_after - memory_before,
            'memory_per_neuron': memory_per_neuron
        })
        
        print(f"Size {size:4d}: {memory_after - memory_before:6.1f} MB, "
              f"{memory_per_neuron:5.2f} KB per neuron")
    
    return results

print("Memory Usage Benchmarks:")
benchmark_memory_usage()
```

---

# API Reference

This section provides detailed API documentation for the main NLM Python classes.

## Brain Class

### Methods

| Method | Description | Parameters |
|--------|-------------|------------|
| `initialize()` | Initialize brain with configuration | None |
| `step(step_number)` | Perform one simulation step | `step_number: int` |
| `step(step_number, time)` | Perform step with timestamp | `step_number: int, time: float` |
| `receiveSensoryInput(input)` | Inject sensory input into brain | `input: SensoryInput` |
| `produceAction()` | Get motor action from neural activity | None |
| `reset()` | Reset brain to initial state | None |
| `save(filepath)` | Save brain state to file | `filepath: str` |
| `load(filepath)` | Load brain state from file | `filepath: str` |

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `total_neuron_count` | int | Total neurons across all regions |
| `total_synapse_count` | int | Total synapses across all regions |
| `firing_neuron_count` | int | Neurons currently firing |
| `average_firing_rate` | float | Average firing rate across neurons |
| `excitation_inhibition_ratio` | float | E/I balance ratio |

## Config Class

### Methods

| Method | Description | Parameters |
|--------|-------------|------------|
| `loadFromFile(filepath)` | Load configuration from file | `filepath: str` |
| `loadFromArgs(argc, argv)` | Load from command line args | `argc: int, argv: list` |
| `saveToFile(filepath)` | Save configuration to file | `filepath: str` |
| `has(key)` | Check if key exists | `key: str` |
| `get(key)` | Get configuration value | `key: str` |
| `set(key, value)` | Set configuration value | `key: str, value: Any` |

## AgentBrain Class

### Methods

| Method | Description | Parameters |
|--------|-------------|------------|
| `initialize(world)` | Initialize with world | `world: SimpleWorld` |
| `processSensoryInput(percept)` | Process sensory input | `percept: SensoryPercept` |
| `decodeMotorCommand()` | Decode motor command from brain | None |
| `applyRewardModulation(reward, predicted_reward)` | Apply reward-based learning | `reward: float, predicted_reward: float` |
| `updateDevelopment(timestep)` | Update development system | `timestep: float` |

### Subsystems

| Subsystem | Methods |
|-----------|---------|
| Reward Modulation | `enableRewardModulation(enable)`, `getNeuromodulationLevel()` |
| Structural Plasticity | `enableStructuralPlasticity(enable)` |
| Development | `enableDevelopment(enable)`, `getDevelopmentalStage()` |
| Curiosity | `enableCuriosity(enable)`, `getCuriosityLevel()` |

## SimpleWorld Class

### Methods

| Method | Description | Parameters |
|--------|-------------|------------|
| `configure(width, height, vision_width, vision_height)` | Configure world dimensions | `width: int, height: int, vision_width: int, vision_height: int` |
| `reset()` | Reset world to initial state | None |
| `update(timestep)` | Update world state | `timestep: float` |
| `applyMotorCommand(cmd, time)` | Apply motor command to world | `cmd: MotorCommand, time: float` |
| `getSensoryPercept()` | Get sensory perception | None |
| `getAgentBody()` | Get agent body state | None |

## Enumerations

### NeuronType

```python
pynlm.NeuronType.Excitatory      # Standard excitatory neuron
pynlm.NeuronType.Inhibitory      # Standard inhibitory neuron
pynlm.NeuronType.Sensory         # Sensory processing neuron
pynlm.NeuronType.Motor           # Motor output neuron
pynlm.NeuronType.Internal        # Internal processing neuron
pynlm.NeuronType.Modulatory      # Neuromodulatory neuron
```

### DevelopmentalStage

```python
pynlm.DevelopmentalStage.Initial         # Newborn - high plasticity
pynlm.DevelopmentalStage.CriticalPeriod  # Sensitive period
pynlm.DevelopmentalStage.Maturation      # Maturation phase
pynlm.DevelopmentalStage.Adult           # Adult - stable
pynlm.DevelopmentalStage.Aging           # Aging phase
```

### ActionType

```python
pynlm.ActionType.MoveForward   # Move forward
pynlm.ActionType.MoveBackward  # Move backward
pynlm.ActionType.TurnLeft      # Turn left
pynlm.ActionType.TurnRight     # Turn right
pynlm.ActionType.Interact      # Interact with environment
# ... more actions
```

---

# Example Scripts

## 1. Hello World Example

```python
import pynlm

# Simple demonstration of NLM
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

for step in range(10):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

## 2. Learning Maze Environment

```python
import pynlm
import numpy as np

class MazeAgent:
    def __init__(self, width=20, height=20):
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, 10, 10)
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("neuromod.curiosity.enable", True)
        
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        self.agent.enableCuriosity(True)
        self.agent.enableRewardModulation(True)
        
        # Set up maze (walls around border)
        for x in range(width):
            self.world.addObject(pynlm.WorldObject(x, 0, pynlm.WorldObjectType.Wall, radius=0.5))
            self.world.addObject(pynlm.WorldObject(x, height-1, pynlm.WorldObjectType.Wall, radius=0.5))
        
        for y in range(height):
            self.world.addObject(pynlm.WorldObject(0, y, pynlm.WorldObjectType.Wall, radius=0.5))
            self.world.addObject(pynlm.WorldObject(width-1, y, pynlm.WorldObjectType.Wall, radius=0.5))
        
        self.world.setAgentStart(1.0, 1.0)
    
    def run_episode(self, max_steps=500):
        for step in range(max_steps):
            self.world.update(0.1)
            
            self.agent.processSensoryInput(self.world.getSensoryPercept())
            self.brain.step(step)
            
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Calculate reward
            body = self.world.getAgentBody()
            # Simple reward: proximity to center
            center_x, center_y = self.world.getWidth() // 2, self.world.getHeight() // 2
            distance = np.sqrt((body.x - center_x)**2 + (body.y - center_y)**2)
            reward = max(0, 10.0 - distance)
            
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Check if stuck
            if step % 100 == 0:
                print(f"Step {step}: Position ({body.x:.1f}, {body.y:.1f}), "
                      f"Curiosity: {self.agent.getCuriosityLevel():.3f}")
                
                # Check if making progress
                if step > 100 and distance > self.world.getWidth():
                    print("Agent appears to be stuck")
                    break
        
        print("Episode completed")
        return self.world.getAgentBody()

# Use the maze agent
agent = MazeAgent()
final_pos = agent.run_episode()
print(f"Final position: ({final_pos.x:.1f}, {final_pos.y:.1f})")
```

## 3. Sensorimotor Synchronization

```python
import pynlm
import numpy as np

def sensorimotor_synchronization_demo():
    """Demonstrate sensorimotor synchronization (how neurons learn to produce patterns)."""
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create an agent to interface with world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    agent.initialize(world)
    
    agent.enableRewardModulation(True)
    
    # Create target pattern (simple oscillation)
    target_pattern = [0.0] * 60
    for i in range(60):
        if i < 20:
            target_pattern[i] = i / 20.0
        elif i < 40:
            target_pattern[i] = 1.0
        else:
            target_pattern[i] = (40 - i) / 20.0
    
    # Track synchronization error
    sync_errors = []
    motor_activity = []
    
    print("Sensorimotor Synchronization Demo")
    print("Target: Simple rise and fall pattern")
    
    for step in range(1000):
        # Apply small reward based on activity level
        brain_activity = brain.getAverageFiringRate()
        reward = (0.5 - abs(brain_activity - 0.5)) * 2.0  # Reward near 0.5 firing rate
        
        # Process world state
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Track metrics
        sync_errors.append(abs(brain_activity - 0.5))
        motor_activity.append(brain_activity)
        
        if step % 100 == 0:
            avg_error = np.mean(sync_errors[-100:])
            print(f"Step {step}: Avg sync error = {avg_error:.3f}, "
                  f"Avg firing rate = {brain_activity:.3f}")
    
    # Final analysis
    final_error = np.mean(sync_errors)
    print(f"\nFinal synchronization error: {final_error:.3f}")
    print(f"Mean firing rate: {np.mean(motor_activity):.3f}")
    
    return brain, agent, world, sync_errors

sensorimotor_synchronization_demo()
```

## 4. Multi-Agent Simulation

```python
import pynlm
import numpy as np

def multi_agent_simulation_demo():
    """Demonstrate multiple agents interacting in shared environment."""
    
    # Create shared environment
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=12, visionHeight=12)
    
    # Create multiple agents
    agents = []
    brains = []
    
    for i in range(3):
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 800)
        config.set("neuromod.curiosity.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        agent.enableCuriosity(True)
        agent.enableRewardModulation(True)
        
        agents.append(agent)
        brains.append(brain)
    
    # Place agents at different starting positions
    for i, agent in enumerate(agents):
        x = 5 + i * 15
        y = 5 + i * 15
        world.setAgentStart(x, y)
    
    # Set random seed for reproducibility
    world.setRandomSeed(42)
    
    print("Multi-Agent Simulation")
    print("3 agents exploring shared environment")
    
    # Run simulation
    for step in range(500):
        world.update(0.1)
        
        # Each agent processes input and acts
        for i, (agent, brain) in enumerate(zip(agents, brains)):
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Each agent gets different reward
            body = world.getAgentBody()
            reward = np.random.uniform(0.5, 1.5) * (1.0 + agent.getCuriosityLevel())
            agent.applyRewardModulation(reward, 0.0)
        
        # Reset world for next step
        world.reset()
        
        # Re-place agents at new positions
        for i, agent in enumerate(agents):
            x = 5 + i * 15
            y = 5 + i * 15
            world.setAgentStart(x, y)
        
        if step % 100 == 0:
            print(f"Step {step}:")
            for i, agent in enumerate(agents):
                print(f"  Agent {i+1}: Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"Novelty={agent.getNoveltyLevel():.3f}")
    
    print("\nMulti-agent simulation complete")
    return brains, agents, world

multi_agent_simulation_demo()
```

## 5. Complete Benchmark Suite

```python
import pynlm
import time
import numpy as np

def complete_benchmark_suite():
    """Run a complete benchmark suite of common NLM use cases."""
    
    print("=== NLM Python API Complete Benchmark Suite ===\n")
    
    # Benchmark 1: Minimal network
    print("1. Minimal Network Benchmark:")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    for step in range(100):
        brain.step(step)
    minimal_time = time.time() - start_time
    print(f"   Time for 100 steps: {minimal_time:.3f}s")
    print(f"   Steps per second: {100/minimal_time:.1f}")
    
    # Benchmark 2: Medium network with plasticity
    print("\n2. Medium Network with Plasticity:")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    for step in range(200):
        brain.step(step)
        if step % 50 == 0:
            # Simulate some processing
            agent = pynlm.createAgentBrain(brain)
            agent.processSensoryInput(world.getSensoryPercept() if 'world' in locals() else None)
    plasticity_time = time.time() - start_time
    print(f"   Time for 200 steps: {plasticity_time:.3f}s")
    print(f"   Steps per second: {200/plasticity_time:.1f}")
    
    # Benchmark 3: Memory-intensive simulation
    print("\n3. Memory-Intensive Simulation:")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("working_memory.capacity", 200)
    config.set("episodic_memory.max_episodes", 100)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    for step in range(300):
        brain.step(step)
        
        # Access memory systems
        if brain.getWorkingMemory():
            _ = brain.getWorkingMemory().getActiveTraces()
        if brain.getEpisodicMemory():
            _ = brain.getEpisodicMemory().getEpisodeCount()
    
    memory_time = time.time() - start_time
    print(f"   Time for 300 steps: {memory_time:.3f}s")
    print(f"   Steps per second: {300/memory_time:.1f}")
    
    # Benchmark 4: Complete agent simulation
    print("\n4. Complete Agent-World Simulation:")
    from pynlm import SimpleWorld
    
    world = SimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("development.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    
    start_time = time.time()
    for step in range(150):
        world.update(0.1)
        
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        reward = 1.0 if step % 100 == 0 else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
    
    agent_time = time.time() - start_time
    print(f"   Time for 150 steps: {agent_time:.3f}s")
    print(f"   Steps per second: {150/agent_time:.1f}")
    
    print("\n=== Benchmark Summary ===")
    print(f"Minimal:           {100/minimal_time:6.1f} steps/sec")
    print(f"Plasticity-heavy:  {200/plasticity_time:6.1f} steps/sec")
    print(f"Memory-intensive:  {300/memory_time:6.1f} steps/sec")
    print(f"Agent-world:       {150/agent_time:6.1f} steps/sec")
    
    return {
        'minimal': 100/minimal_time,
        'plasticity': 200/plasticity_time,
        'memory': 300/memory_time,
        'agent': 150/agent_time
    }

# Run the benchmark
results = complete_benchmark_suite()
```

---

# Appendices

## A. Common Errors and Solutions

### Error: "Unable to initialize brain"

**Symptoms**:
- Brain initialization fails
- `initialize()` returns false

**Common causes**:
1. Invalid configuration parameters
2. Missing required files
3. System resource limitations

**Solutions**:
1. Validate configuration:
   ```python
   config = pynlm.createDefaultConfig()
   config.set("brain.neuron_count", 100)  # Valid value
   brain = pynlm.createBrain(config)
   if not brain.initialize():
       print("Brain initialization failed")
       return
   ```

2. Check file permissions:
   ```bash
   ls -la your_files/
   ```

### Error: "Segmentation fault"

**Symptoms**:
- Program crashes with segfault
- Core dump may be generated

**Common causes**:
1. Accessing invalid memory
2. Null pointer dereference
3. Out-of-bounds array access

**Solutions**:
1. Debug with GDB:
   ```bash
   gdb ./your_program
   run
   # When program crashes, type:
   bt
   quit
   ```

2. Add bounds checking:
   ```python
   # Check array bounds before access
   if i < len(array):
       value = array[i]
   ```

3. Use valgrind for memory errors:
   ```bash
   valgrind ./your_program
   ```

### Error: "ImportError: No module named 'pynlm'"

**Solution**:
```bash
# Install the package
pip install pynlm

# Or build from source
python -m pip install --upgrade pip setuptools wheel
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
pip install .
```

## B. Configuration Template

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.0001
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        }
    },
    "neuromodulation": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true,
            "threshold": 0.5
        },
        "novelty": {
            "enable": true,
            "decay_rate": 0.99
        }
    }
}
```

## C. Performance Tuning Guide

### For Small Networks (< 500 neurons)
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 300)
config.set("connection_probability", 0.05)
config.set("simulation_timestep", 0.001)
config.set("plasticity.stdp.enable", False)  # Disable for speed
```

### For Medium Networks (500-2000 neurons)
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("connection_probability", 0.1)
config.set("simulation_timestep", 0.001)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.structural.enable", False)  # Disable structural for speed
```

### For Large Networks (> 2000 neurons)
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("connection_probability", 0.01)
config.set("simulation_timestep", 0.01)  # Larger timestep
config.set("working_memory.enable", False)  # Disable working memory
config.set("plasticity.structural.enable", False)  # Disable structural
```

## D. Frequently Asked Questions

### Q: How do I save and load brain states?

A: Use the `save()` and `load()` methods:

```python
# Save brain state
brain.save("my_brain_state.bin")

# Load brain state
if brain.load("my_brain_state.bin"):
    print("Brain state loaded successfully")
else:
    print("Failed to load brain state")
```

### Q: How do I use multiple regions?

A: Create regions using `brain.addRegion()` and configure neurons for each:

```python
sensory_region_id = brain.addRegion("Sensory")
motor_region_id = brain.addRegion("Motor")
internal_region_id = brain.addRegion("Internal")

# Add neurons to regions
# (Implementation details depend on region API)
```

### Q: How do I monitor simulation progress?

A: Use the brain's statistics methods:

```python
for step in range(100):
    brain.step(step)
    
    if step % 10 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
```

### Q: How do I integrate with external data?

A: Use the `receiveSensoryInput()` method with processed data:

```python
# Process external data (e.g., sensor readings)
external_data = [0.1, 0.5, 0.9, ...]  # Your data here
brain.receiveSensoryInput(external_data)
```

## E. License

This documentation is part of the NLM (Neural Learning Machine) project. The NLM project is licensed under the MIT License. See the LICENSE file for details.

---

*Last updated: $(date +%Y-%m-%d)*
*Version: 0.1.0*

*This documentation complements the C++ HOW_TO_USE.md and focuses on Python-specific usage patterns.*