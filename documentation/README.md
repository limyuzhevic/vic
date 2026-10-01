# NLM Documentation and Examples

This directory contains comprehensive documentation and examples for the NLM (Neural Learning Machine) framework.

## Table of Contents

1. [Quick Start Guide](#quick-start-guide)
2. [API Reference](#api-reference)
3. [Code Examples](#code-examples)
4. [Advanced Features](#advanced-features)
5. [Performance Optimization](#performance-optimization)
6. [Troubleshooting](#troubleshooting)
7. [Migration Guide](#migration-guide)

## Quick Start Guide

### What is NLM?

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain simulation framework. It implements a complete neural system that learns from experience through:

- **Real spiking neural computation** with Leaky Integrate-and-Fire neurons
- **Event-driven dynamics** with synaptic delays and spike propagation
- **Multiple plasticity rules** (STDP, Hebbian, structural)
- **Integrated memory systems** (working, episodic, associative)
- **Neuromodulation** (reward, curiosity, novelty)
- **Cognitive functions** (prediction, planning, attention)
- **Developmental processes** (maturation, synaptogenesis, pruning)

### Installation and Setup

#### Basic Installation

```bash
# Clone the repository
https://github.com/nlm-project/nlm.git

# Create build directory and compile
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings (if needed)
pip install pynlm
```

#### Python Environment Setup

```python
# Import the library
import pynlm

# Check version
print(f"NLM version: {pynlm.__version__ if hasattr(pynlm, '__version__') else 'Latest'}")

# Create a simple brain
brain = pynlm.create_default_config()
brain = pynlm.create_brain(brain)
brain.initialize()

print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
```

### First Simulation

#### Example 1: Simple Brain Simulation

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
              f"{brain.getAverageFiringRate():.2f} Hz average rate")

print("Simulation complete!")
```

#### Example 2: Complete Agent in World

```python
import pynlm

# Setup the complete agent-world system
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Enable learning capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run the agent in the world
for step in range(200):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain processing
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Apply action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print progress every 20 steps
    if step % 20 == 0:
        stats = pynlm.get_brain_stats(brain)
        print(f"Step {step}: {stats['firing_neurons']} neurons firing, "
              f"firing rate: {stats['average_firing_rate']:.2f} Hz, "
              f"excitatory/inhibition ratio: {stats['excitatory_inhibition_ratio']:.2f}")

print("Agent simulation complete!")
```

#### Example 3: Advanced Configuration

```python
import pynlm
import json

# Create an advanced configuration using presets
config = pynlm.create_advanced_config()

# Modify specific parameters
brain = pynlm.create_brain(config)
brain.initialize()

# Enable specific features
brain.getConfig().set("enable_reward_modulation", True)
brain.getConfig().set("enable_curiosity", True)
brain.getConfig().set("enable_development", True)
brain.getConfig().set("enable_structural_plasticity", True)

# Set research parameters
brain.getConfig().set("neuron_count", 2000)
brain.getConfig().set("region_count", 3)
brain.getConfig().set("connection_probability", 0.08)

print("Advanced brain created successfully!")
print(f"Configuration: {brain.getConfig().summary()}")
```

## API Reference

### Core Classes

#### Brain
The main neural brain simulation class.

**Constructor:**
```python
brain = pynlm.create_brain(config)
```

**Methods:**
- `initialize()` - Initialize the brain with current configuration
- `step(step_number)` - Process one simulation step
- `getTotalNeuronCount()` - Get total number of neurons
- `getTotalSynapseCount()` - Get total number of synapses
- `getAverageFiringRate()` - Get average firing rate
- `produceAction()` - Produce motor action based on neural activity
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file

#### Config
Configuration management class.

**Constructor:**
```python
config = pynlm.create_default_config()
```

**Methods:**
- `loadFromFile(filepath)` - Load configuration from file
- `saveToFile(filepath)` - Save configuration to file
- `has(key)` - Check if configuration key exists
- `getOr(key, default)` - Get value with default fallback
- `set(key, value)` - Set configuration value

#### SimpleWorld
Environment simulation class.

**Constructor:**
```python
world = pynlm.createSimpleWorld()
```

**Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Configure world parameters
- `reset()` - Reset world to initial state
- `update(timestep)` - Update world state
- `getSensoryPercept()` - Get current sensory percept
- `getAgentBody()` - Get agent body state
- `applyMotorCommand(command, time)` - Apply motor command

#### AgentBrain
Bridge between brain and world.

**Constructor:**
```python
agent = pynlm.create_agent_brain(brain)
```

**Methods:**
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predicted)` - Apply reward modulation
- `updateDevelopment(timestep)` - Update development
- `enableRewardModulation(enable)` - Enable/disable reward modulation

### Configuration Keys

Common configuration keys include:

- `random_seed` - Random seed for reproducibility
- `simulation_timestep` - Time step for simulation
- `neuron_count` - Number of neurons
- `region_count` - Number of brain regions
- `connection_probability` - Probability of synaptic connections
- `stdp_ltp_weight` - STDP potentiation weight
- `stdp_ltd_weight` - STDP depression weight
- `stdp_time_constant` - STDP time constant
- `enable_reward_modulation` - Enable reward modulation
- `enable_curiosity` - Enable curiosity
- `enable_development` - Enable development
- `enable_structural_plasticity` - Enable structural plasticity

## Code Examples

### Example 1: Learning with Reward

```python
import pynlm

# Create learning scenario
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(30, 30, 12, 12)
world.reset()

agent = pynlm.create_agent_brain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

print("Starting learning simulation...")

for step in range(500):
    world.update(0.1)
    
    # Get percept and process it
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes information
    brain.step(step)
    
    # Agent selects action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Simulate reward based on position
    if step % 50 == 0:
        agent.applyRewardModulation(1.0, 0.5)  # Give reward
    
    if step % 100 == 0:
        stats = pynlm.get_brain_stats(brain)
        print(f"Step {step}: Weight change = {stats['total_weight_change']:.4f}")

print("Learning simulation completed!")
```

### Example 2: Developmental Learning

```python
import pynlm

# Create a developing brain
brain = pynlm.create_brain(pynlm.create_advanced_config())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

agent = pynlm.create_agent_brain(brain)
agent.initialize(world)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

print("Starting developmental learning...")

# Simulate development over multiple episodes
for episode in range(10):
    print(f"Episode {episode + 1}/10")
    
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Provide varying rewards to drive learning
        if step % 25 == 0:
            agent.applyRewardModulation(0.5 + episode * 0.1, 0.3)
    
    # Print developmental stage
    stage = agent.getDevelopmentalStage()
    stage_name = ["Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"][int(stage)]
    print(f"  Developmental stage: {stage_name}")

print("Developmental learning completed!")
```

### Example 3: Prediction and Planning

```python
import pynlm

# Create brain with prediction capabilities
brain = pynlm.create_brain(pynlm.create_advanced_config())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(25, 25, 10, 10)
world.reset()

agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

print("Starting prediction and planning simulation...")

for step in range(300):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes with prediction
    brain.step(step)
    
    # Agent plans action based on predictions
    action = agent.decodeMotorCommand()
    
    # Apply action
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Use prediction error for learning
    prediction_error = abs(result.reward - 0.5)  # Example prediction
    agent.applyRewardModulation(result.reward, prediction_error)
    
    if step % 50 == 0:
        stats = pynlm.get_brain_stats(brain)
        print(f"Step {step}: Prediction error = {prediction_error:.3f}, "
              f"Firing rate = {stats['average_firing_rate']:.2f} Hz")

print("Prediction simulation completed!")
```

## Advanced Features

### 1. Multiple Brain Types

```python
import pynlm

# Standard brain (recommended for most use cases)
brain1 = pynlm.create_brain(pynlm.create_default_config())

# Research brain (more neurons, advanced features)
brain2 = pynlm.create_brain(pynlm.create_advanced_config())

# Minimal brain (quick testing)
brain3 = pynlm.create_brain(pynlm.create_minimal_config())

# Custom configuration
custom_config = {
    "random_seed": 42,
    "neuron_count": 1000,
    "enable_reward_modulation": True,
    "enable_curiosity": True
}
brain4 = pynlm.create_custom_brain(custom_config)
```

### 2. Performance Monitoring

```python
import pynlm

brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Enable debug mode for detailed logging
brain.enableDebugMode(True)

# Get comprehensive statistics
stats = brain.getStatistics()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total synapses: {stats['total_synapses']}")
print(f"Active neurons: {stats['active_neurons']}")
print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
print(f"Excitatory/inhibition ratio: {stats['excitatory_inhibition_ratio']:.2f}")

# Print detailed status report
print(brain.getStatusReport())
```

### 3. Batch Processing

```python
import pynlm
import time

# Create multiple brains for batch processing
brains = []
for i in range(5):
    brain = pynlm.create_brain(pynlm.create_default_config())
    brain.initialize()
    brains.append(brain)

# Process all brains in parallel
start_time = time.time()
for i, brain in enumerate(brains):
    for step in range(50):
        brain.step(step)
    
    stats = pynlm.get_brain_stats(brain)
    print(f"Brain {i+1}: {stats['average_firing_rate']:.2f} Hz average firing rate")

end_time = time.time()
print(f"Batch processing completed in {end_time - start_time:.2f} seconds")
```

### 4. Configuration Persistence

```python
import pynlm
import json

# Create and save configuration
config = pynlm.create_advanced_config()
config.saveToFile("my_config.json")

# Load configuration from file
loaded_config = pynlm.load_config_from_json("my_config.json")

# Modify loaded configuration
loaded_config.set("neuron_count", 3000)
loaded_config.set("enable_curiosity", True)

# Create brain with loaded configuration
brain = pynlm.create_brain(loaded_config)
brain.initialize()

print("Configuration persistence test completed!")
```

## Performance Optimization

### 1. Memory Optimization

```python
import pynlm

# Create brain with optimized settings
brain = pynlm.create_brain(pynlm.create_advanced_config())

# Enable performance optimizations
brain.enableOptimizations(
    enableSIMD=True,
    enableParallel=True,
    enableCaching=True
)

print("Performance optimizations enabled!")
```

### 2. Time Step Optimization

```python
import pynlm

brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Optimize time step for different use cases
time_step = 0.001  # High precision
# or
time_step = 0.01   # Faster simulation

# Set time step
brain.setTimeStep(time_step)

print(f"Simulation time step: {time_step}")
```

### 3. Debug Mode

```python
import pynlm

brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Enable debug mode for troubleshooting
brain.enableDebugMode(True)

# Run simulation with logging
for step in range(20):
    brain.step(step)
    if step % 5 == 0:
        print(f"Step {step} completed")
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: Brain initialization fails

**Solution:**
```python
import pynlm

# Check configuration before creating brain
config = pynlm.create_default_config()
print(f"Configuration valid: {config.has('neuron_count')}")

# Create brain with error handling
try:
    brain = pynlm.create_brain(config)
    brain.initialize()
    print("Brain created successfully!")
except Exception as e:
    print(f"Error creating brain: {e}")
```

#### Issue: Simulation runs too slowly

**Solution:**
```python
import pynlm

brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Enable optimizations
brain.enableOptimizations(
    enableSIMD=True,
    enableParallel=True,
    enableCaching=True
)

# Increase time step for faster simulation
brain.setTimeStep(0.01)

# Reduce logging for performance
brain.enableDebugMode(False)
```

#### Issue: Memory usage is too high

**Solution:**
```python
import pynlm

brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Use minimal configuration for memory-constrained environments
brain = pynlm.create_minimal_brain({})
brain.initialize()

# Clear memory periodically
brain.clearMemory()
```

#### Issue: Configuration file not found

**Solution:**
```python
import pynlm
import os

config_path = "config.json"

if not os.path.exists(config_path):
    # Create default configuration
    config = pynlm.create_default_config()
    config.saveToFile(config_path)
    print(f"Created default configuration at {config_path}")

# Load configuration
try:
    config = pynlm.load_config_from_json(config_path)
    brain = pynlm.create_brain(config)
    brain.initialize()
    print("Configuration loaded successfully!")
except Exception as e:
    print(f"Error loading configuration: {e}")
```

## Migration Guide

### From Version 0.1.x to 0.2.x

#### Changes in API

1. **Enhanced Brain API**: Added new convenience methods
2. **Improved Error Handling**: Better exception messages
3. **Enhanced Configuration**: Added JSON support
4. **New Performance Features**: SIMD optimization, parallel processing

#### Migration Steps

```python
# Old API (0.1.x)
brain = pynlm.create_brain(config)
brain.initialize()
brain.step(step)
stats = brain.getStatistics()

# New API (0.2.x) - enhanced with additional features
brain = pynlm.create_advanced_brain()  # New convenience method
brain.initialize()
brain.stepWithTime(currentTime, timestep)  # New method
brain.enableOptimizations(enableSIMD=True, enableParallel=True)  # New method
stats = brain.getStatistics()  # Enhanced statistics
```

### Upgrading Python Environment

```bash
# Upgrade to latest version
pip install --upgrade pynlm

# Check installed version
pip show pynlm
```

### Migration Checklist

- [ ] Update all configuration files to use JSON format
- [ ] Replace old brain creation methods with new convenience methods
- [ ] Add error handling for new exception types
- [ ] Enable performance optimizations for better performance
- [ ] Update documentation and examples
- [ ] Test all migration scenarios thoroughly

## Conclusion

NLM provides a comprehensive neural simulation framework with:

- **Real neural computation** using spiking neuron models
- **Flexible configuration** with JSON support
- **Advanced learning capabilities** through multiple plasticity rules
- **Performance optimizations** for efficient simulation
- **Comprehensive error handling** for robust operation
- **Extensive documentation** and examples

The framework is suitable for:

- **Research**: Explore neural dynamics and learning mechanisms
- **Education**: Teach neural network concepts
- **Development**: Create intelligent agents and robotics
- **Testing**: Validate neural algorithms and models

Start with the examples above to get familiar with NLM, then explore the advanced features for your specific use case.