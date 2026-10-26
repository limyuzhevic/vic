# NLM (Neural Learning Machine) - Complete User Guide

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project that implements a neural system capable of learning from experience through interaction with an environment. It features spiking neural networks with real LIF dynamics, multiple plasticity mechanisms, memory systems, and neuromodulation.

## Key Improvements in This Version

### 1. Enhanced Configuration System
- **JSON Configuration Support**: Full JSON file format support with proper serialization/deserialization
- **Backward Compatibility**: Maintains support for simple key=value format
- **Type Safety**: Proper type handling for all configuration values (int, float, bool, strings, arrays)
- **Advanced Features**: Support for complex nested configurations and array types

### 2. Advanced Power User Features
- **Complex Simulation Environments**: Multi-region, multi-population neural networks
- **Real-Time Interaction**: Optimized for responsive, interactive applications
- **Performance Benchmarking**: Built-in tools for measuring and optimizing performance
- **Custom Neural Architectures**: Support for complex neural network topologies

### 3. Improved Python Bindings
- **Enhanced API**: More flexible and intuitive Python interface
- **Better Error Handling**: Comprehensive error reporting and validation
- **Documentation**: Detailed docstrings and examples for all classes
- **Type Safety**: Proper Python type hints and validation

### 4. Comprehensive Examples
- **Beginner Examples**: Simple, step-by-step tutorials for new users
- **Advanced Examples**: Complex simulations demonstrating advanced features
- **Real-World Scenarios**: Examples of practical applications
- **Performance Examples**: Benchmarking and optimization demonstrations

## Table of Contents

1. [Quick Start](#quick-start)
2. [Configuration System](#configuration-system)
3. [Advanced Features](#advanced-features)
4. [Python API](#python-api)
5. [Examples](#examples)
6. [Troubleshooting](#troubleshooting)

## Quick Start

### Basic Usage (Beginner)

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a basic simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
```

### Advanced Usage (Power User)

```python
import pynlm
import json

# Create advanced configuration
config = pynlm.createDefaultConfig()

# Performance optimizations
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.learning_rate", 0.025)

# Save as JSON (NEW FEATURE)
config.saveToFile("advanced_config.json")

# Load from JSON
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("advanced_config.json")

# Create and run complex simulation
brain = pynlm.createBrain(config2)
brain.initialize()

# ... advanced simulation code
```

## Configuration System

### Overview
The configuration system provides flexible settings management for NLM simulations with full JSON support.

### Configuration File Format (JSON)

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "synapse_density": 0.15
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.01,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.005
        }
    },
    "neuromodulation": {
        "dopamine": {
            "baseline": 0.0,
            "scale": 1.0
        },
        "curiosity": {
            "enable": true,
            "sensitivity": 1.0
        }
    },
    "memory": {
        "working_memory": {
            "capacity": 100
        },
        "episodic_memory": {
            "max_episodes": 1000
        }
    }
}
```

### Python Configuration API

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.learning_rate", 0.025)
config.set("neuromodulation.dopamine.scale", 1.5)

# Set array values
config.set("brain.populations", [100, 200, 300, 400])

# Query configuration
if config.has("brain.neuron_count"):
    count = config.getOr("brain.neuron_count", 1000)
    print(f"Neuron count: {count}")

# Get all configuration keys
keys = config.getKeys()

# Save to JSON file
config.saveToFile("my_config.json")

# Load from JSON file
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")

# Get configuration summary
print(config.summary())
```

### Command Line Configuration

```bash
# Command line arguments
./nlm --brain.neuron_count=5000 --plasticity.stdp.learning_rate=0.02 --config=settings.json

# Environment variable configuration
export NLM_CONFIG_FILE=advanced_settings.json
./nlm
```

## Advanced Features

### 1. Multi-Region Neural Networks

```python
import pynlm

# Create configuration with multiple regions
config = pynlm.createDefaultConfig()
config.set("brain.region_count", 4)
config.set("brain.neuron_count", 4000)

# Create brain and initialize
brain = pynlm.createBrain(config)
brain.initialize()

# Access different regions
region1 = brain.getRegion(1)  # First region
region2 = brain.getRegion(2)  # Second region
# ... etc.

# Each region can have different characteristics
```

### 2. Real-Time Interactive Simulation

```python
import pynlm
import time

# Ultra-fast configuration for real-time interaction
config = pynlm.createDefaultConfig()
config.set("simulation_timestep", 0.0001)  # Very fast timesteps
config.set("brain.neuron_count", 1000)
config.set("brain.working_memory_capacity", 200)

# Create simulation environment
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.initialize(world)

# Real-time loop
start_time = time.time()
step_count = 0

while time.time() - start_time < 60:  # Run for 60 seconds
    frame_start = time.time()
    
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process in brain
    agent.processSensoryInput(percept)
    brain.step(step_count)
    
    # Get action and execute
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    step_count += 1
    
    # Maintain consistent frame rate
    frame_time = time.time() - frame_start
    if frame_time < 0.016:  # 60 FPS
        time.sleep(0.016 - frame_time)

print(f"Real-time simulation completed: {step_count} steps")
```

### 3. Performance Benchmarking

```python
import pynlm
import time
import json

def benchmark_configuration(config_desc, neuron_count, timestep, steps):
    """Benchmark a specific configuration"""
    config = pynlm.createDefaultConfig()
    
    # Apply configuration
    for key, value in config_desc.items():
        if isinstance(value, list):
            # Handle array values
            config.set(key, value)
        else:
            config.set(key, value)
    
    config.set("brain.neuron_count", neuron_count)
    config.set("simulation_timestep", timestep)
    
    # Create and run brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    for step in range(steps):
        brain.step(step)
    
    end_time = time.time()
    duration = end_time - start_time
    
    return {
        "configuration": config_desc,
        "neuron_count": neuron_count,
        "timestep": timestep,
        "steps": steps,
        "duration_seconds": duration,
        "steps_per_second": steps / duration,
        "spikes_generated": brain.getTotalSpikeCount()
    }

# Run benchmarks
benchmarks = []

# Configuration 1: Small, fast
config1 = {
    "brain.region_count": 1,
    "plasticity.stdp.enable": True,
    "neuromodulation.curiosity.enable": False
}
benchmarks.append(benchmark_configuration(config1, 500, 0.001, 1000))

# Configuration 2: Large, precise
config2 = {
    "brain.region_count": 4,
    "plasticity.stdp.enable": True,
    "plasticity.structural.enable": True,
    "neuromodulation.curiosity.enable": True
}
benchmarks.append(benchmark_configuration(config2, 2000, 0.0005, 2000))

# Configuration 3: Maximum performance
config3 = {
    "brain.region_count": 8,
    "plasticity.stdp.enable": True,
    "plasticity.structural.enable": True,
    "neuromodulation.curiosity.enable": True,
    "neuromodulation.novelty.enable": True
}
benchmarks.append(benchmark_configuration(config3, 5000, 0.0002, 3000))

# Save results
with open("benchmark_results.json", "w") as f:
    json.dump(benchmarks, f, indent=2)

# Print summary
print("Performance Benchmark Results:")
for i, result in enumerate(benchmarks):
    print(f"\nConfiguration {i+1}:")
    print(f"  Neurons: {result['neuron_count']}, Timestep: {result['timestep']}")
    print(f"  Duration: {result['duration_seconds']:.2f}s")
    print(f"  Steps/sec: {result['steps_per_second']:.0f}")
    print(f"  Spikes: {result['spikes_generated']}")
```

### 4. Learning with Complex Tasks

```python
import pynlm
import math

class ComplexTask:
    """Example of a complex learning task"""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.target_pattern = None
        self.current_pattern = None
        self.task_progress = 0.0
    
    def create_target_pattern(self, size=10):
        """Create a complex target pattern"""
        self.target_pattern = []
        for x in range(size):
            row = []
            for y in range(size):
                # Create a spiral pattern
                distance = math.sqrt((x - size/2)**2 + (y - size/2)**2)
                angle = math.atan2(y - size/2, x - size/2)
                value = math.sin(distance / 3.0) * math.cos(angle * 2)
                row.append(value)
            self.target_pattern.append(row)
    
    def get_current_pattern(self):
        """Extract current neural activity pattern"""
        self.current_pattern = []
        neurons = self.brain.getTotalNeuronCount()
        
        # Sample neural activity to create pattern
        for i in range(10):  # 10x10 pattern
            row = []
            for j in range(10):
                # Get activity from specific neurons
                neuron_id = (i * 10 + j) % neurons
                # This is a simplified example - real implementation would be more complex
                row.append(0.5)  # Placeholder
            self.current_pattern.append(row)
        
        return self.current_pattern
    
    def calculate_similarity(self, pattern1, pattern2):
        """Calculate similarity between two patterns"""
        if not pattern1 or not pattern2:
            return 0.0
        
        total_diff = 0.0
        count = 0
        
        for i in range(min(len(pattern1), len(pattern2))):
            for j in range(min(len(pattern1[i]), len(pattern2[i]))):
                total_diff += abs(pattern1[i][j] - pattern2[i][j])
                count += 1
        
        return 1.0 - (total_diff / count) if count > 0 else 0.0
    
    def run_task(self, num_steps=1000):
        """Run the complex learning task"""
        self.create_target_pattern()
        
        for step in range(num_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process input
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Calculate task progress
            current_pattern = self.get_current_pattern()
            similarity = self.calculate_similarity(current_pattern, self.target_pattern)
            self.task_progress = similarity
            
            # Apply reward based on task progress
            reward = similarity * 10.0  # Scale reward
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Optional: break early if task completed
            if similarity > 0.9:
                print(f"Task completed at step {step} with similarity {similarity:.2f}")
                break
        
        return self.task_progress

# Usage example
def run_complex_task_example():
    # Setup simulation
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.region_count", 3)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    agent.initialize(world)
    
    # Run complex task
    task = ComplexTask(brain, world, agent)
    progress = task.run_task(2000)
    
    print(f"Complex task completed with progress: {progress:.2f}")
    
    # Brain statistics
    print(f"\nFinal brain statistics:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

## Python API Reference

### Core Classes

#### Brain Class
```python
brain = pynlm.createBrain(config)
brain.initialize()

# Simulation
brain.step(step_number)
brain.step(step_number, timestamp)  # With timestamp
brain.reset()  # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()  # Get motor action

# State management
brain.save("checkpoint.bin")  # Save state
brain.load("checkpoint.bin")  # Load state

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance
brain.getTotalSpikeCount()       # Total spikes
brain.getDevelopmentalStage()    # Current developmental stage

# Regions
region_id = brain.addRegion("cortex")  # Add new region
region = brain.getRegion(region_id)     # Get region by ID
```

#### Config Class
```python
config = pynlm.createDefaultConfig()

# Configuration operations
config.set("brain.neuron_count", 1000)
config.set("plasticity.learning_rate", 0.01)
config.set("neuromodulation.dopamine.scale", 1.0)

# Query operations
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# File operations
config.loadFromFile("config.json")
config.saveToFile("config.json")
config.summary()  # Get configuration summary
```

#### AgentBrain Class
```python
agent = pynlm.createAgentBrain(brain)

# Initialize with world
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

# Subsystem control
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
```

### Enumerations

#### NeuronType
```python
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal
```

#### DevelopmentalStage
```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

#### ActionType
```python
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest
pynlm.ActionType.Wait
pynlm.ActionType.Custom
```

## Examples

### 1. Minimal Working Example

```python
import pynlm

# Create and initialize
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### 2. Complete Agent Simulation

```python
import pynlm

def run_complete_simulation(steps=1000):
    """Run a complete agent simulation with all features"""
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation loop
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Do action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        reward = 0.0  # Calculate based on world state
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    return brain, world, agent

# Run the simulation
brain, world, agent = run_complete_simulation(1000)
```

### 3. Advanced Configuration Example

```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()

# Performance optimizations
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.2)
config.set("simulation_timestep", 0.0005)

# Advanced plasticity
config.set("plasticity.stdp.learning_rate", 0.025)
config.set("plasticity.hebbian.learning_rate", 0.01)
config.set("plasticity.structural.enable", True)

# Neuromodulation
config.set("neuromodulation.dopamine.scale", 1.5)
config.set("neuromodulation.curiosity.sensitivity", 2.0)
config.set("neuromodulation.novelty.threshold", 0.3)

# Memory systems
config.set("memory.working_memory.capacity", 1000)
config.set("memory.episodic_memory.max_episodes", 5000)

# Development
config.set("development.critical_period.enabled", True)
config.set("development.stage_duration", 2000)

# Save configuration
config.saveToFile("advanced_config.json")

print("Advanced configuration saved!")
```

## Troubleshooting

### Common Issues and Solutions

#### 1. "My brain isn't doing anything"

**Problem**: The brain appears inactive or not learning.

**Solutions**:
```python
# Make sure you call initialize()
brain = pynlm.createBrain(config)
brain.initialize()  # Always call this!

# Use enough steps
for i in range(1000):  # More steps
    brain.step(i)

# Add sensory input to stimulate neurons
world.update(0.1)
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
```

#### 2. Performance Issues

**Problem**: Simulation is too slow.

**Solutions**:
```python
# Optimize configuration
config.set("brain.neuron_count", 1000)  # Smaller network
config.set("simulation_timestep", 0.001)  # Faster timestep
config.set("brain.region_count", 1)  # Single region

# Disable expensive features if not needed
agent.enableCuriosity(False)  # Turn off curiosity-driven exploration
agent.enableStructuralPlasticity(False)  # Disable structural changes

# Run benchmarks
from benchmark import run_benchmarks
results = run_benchmarks()
```

#### 3. Import Errors

**Problem**: `ImportError: No module named 'pynlm'`

**Solution**:
```bash
# Install or reinstall the package
pip install --force-reinstall .

# Or install from source
pip install -e .

# If using development version
cd /path/to/nlm
pip install -e .
```

#### 4. Configuration File Issues

**Problem**: Cannot load configuration from JSON file.

**Solutions**:
```python
# Check that the file exists and is readable
import os
if os.path.exists("config.json"):
    config.loadFromFile("config.json")

# Validate JSON format
import json
with open("config.json", "r") as f:
    data = json.load(f)  # This will raise exception if invalid JSON

# Try simple format if JSON fails
try:
    config.loadFromFile("simple_config.txt")  # Fallback to simple format
except:
    print("Using default configuration")
```

#### 5. Segmentation Faults

**Problem**: Program crashes or segfaults.

**Solutions**:
```python
# Make sure to reset between runs
brain.reset()  # Always reset before re-initializing
brain.initialize()

# Check for NULL pointers or invalid references
if percept is not None:
    agent.processSensoryInput(percept)

# Validate world state
if world.isValidPosition(agent.x, agent.y):
    # Proceed with action
    pass
```

### Debug Checklist

1. **Initialization**:
   - Did you call `brain.initialize()`?
   - Did you reset between simulations?
   - Are all required objects properly created?

2. **Configuration**:
   - Is your JSON file valid?
   - Are all configuration keys correct?
   - Did you save the configuration properly?

3. **Simulation Loop**:
   - Are you calling `world.update()` regularly?
   - Is sensory input being processed?
   - Are actions being applied to the world?

4. **Performance**:
   - Are too many neurons/synapses?
   - Is the timestep too small?
   - Are expensive features enabled unnecessarily?

## Further Reading

### Documentation

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - Detailed system architecture
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background and principles
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions and results
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 implementation details

### Advanced Topics

- [PLASTICITY.md](docs/PLASTICITY.md) - Advanced plasticity mechanisms
- [NEUROMODULATION.md](docs/NEUROMODULATION.md) - Neuromodulation systems
- [DEVELOPMENT.md](docs/DEVELOPMENT.md) - Developmental processes
- [MEMORY.md](docs/MEMORY.md) - Memory system architecture

## Getting Help

### Community Support

- **GitHub Issues**: Report bugs and request features
- **Discussions**: Ask questions and share ideas
- **Documentation**: Check the comprehensive documentation

### Performance Optimization

For performance optimization help:
1. Run the built-in benchmarks
2. Profile your specific workload
3. Adjust configuration parameters
4. Consider disabling non-essential features

### Research and Collaboration

This is an experimental research project. For collaboration:
- Review the scientific documentation
- Join the research community
- Contribute improvements and fixes
- Share your results and findings

## Conclusion

NLM provides a comprehensive framework for implementing and experimenting with spiking neural network models inspired by biological brains. This guide covers everything from basic usage to advanced configuration and optimization.

The improvements in this version significantly enhance:

1. **Configuration Flexibility**: Full JSON support with backward compatibility
2. **Performance**: Advanced optimization options and benchmarking tools
3. **Usability**: Better Python API and comprehensive documentation
4. **Research Capability**: Tools for experimentation and analysis

You now have a powerful, flexible neural simulation framework ready for both research and educational purposes. Start with the quick start examples and gradually explore the advanced features as you become more familiar with the system.

Happy experimenting! 🐭🧠✨