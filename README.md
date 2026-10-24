# NLM User Guide: Comprehensive Tutorial and Examples

## Overview

NLM (Neural Learning Machine) is an advanced computational brain simulation framework that implements a complete artificial neural system. This guide provides comprehensive documentation for using NLM at all skill levels, from beginners to advanced users.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Core Concepts](#core-concepts)
3. [Getting Started with NLM](#getting-started-with-nlm)
4. [Basic Operations](#basic-operations)
5. [Advanced Features](#advanced-features)
6. [Performance Optimization](#performance-optimization)
7. [Troubleshooting](#troubleshooting)
8. [API Reference](#api-reference)
9. [Migration Guide](#migration-guide)
10. [Contributing](#contributing)

## Quick Start

### What is NLM?

NLM simulates a brain-like system with:

- **Neurons**: Computational units that process information
- **Synapses**: Connections between neurons that can strengthen or weaken
- **Memory**: Working memory, episodic memory, and associative memory
- **Learning**: STDP, Hebbian learning, and structural plasticity
- **Motivation**: Dopamine, curiosity, and novelty detection
- **Cognition**: Planning, concept formation, and attention
- **Development**: Stage-based maturation and adaptation

### Your First Brain

```python
import pynlm

# Create a simple brain
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)

# Initialize the brain
brain.initialize()

# Run some computations
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### World Integration

```python
import pynlm

# Create a complete agent-world system
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

world = pynlm.create_simple_world()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process in brain
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Execute action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    agent.updateDevelopment(0.1)
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

## Core Concepts

### 1. Brain Architecture

NLM follows a closed-loop architecture:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

### 2. Key Components

- **Neurons**: Leaky Integrate-and-Fire (LIF) units
- **Synapses**: Dynamic connections with plasticity
- **Memory Systems**: Working, episodic, and associative memory
- **Neuromodulation**: Dopamine, curiosity, and novelty detection
- **Cognition**: Planning, concept formation, and attention
- **Development**: Stage-based maturation

### 3. Neuron Types

- **Sensory**: Receive external input
- **Motor**: Produce actions
- **Internal**: Process and integrate information
- **Excitatory**: Promote firing
- **Inhibitory**: Suppress firing
- **Modulatory**: Regulate other neurons' excitability

### 4. Plasticity Mechanisms

- **STDP**: Spike-Timing Dependent Plasticity
- **Hebbian**: Concurrent activation strengthens connections
- **Structural**: Synaptogenesis and pruning

## Getting Started with NLM

### Installation

```bash
# Install NLM via pip
pip install pynlm

# For development, install from source
pip install -e .

# Verify installation
import pynlm
print(pynlm.__version__)
```

### Basic Configuration

```python
import pynlm

# Create default configuration
config = pynlm.create_default_config()

# Modify configuration
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", true)
config.set("neuromod.curiosity.enable", true)

# Create brain with custom configuration
brain = pynlm.create_brain(config)
```

### Brain Lifecycle

```python
# 1. Create brain
brain = pynlm.create_brain(config)

# 2. Initialize
brain.initialize()

# 3. Run simulation
for step in range(1000):
    brain.step(step)

# 4. Use the brain
stats = brain.get_performance_stats()

# 5. Save/load state
brain.save("checkpoint.bin")
# ... later ...
brain.load("checkpoint.bin")

# 6. Reset
brain.reset()
brain.initialize()
```

## Basic Operations

### 1. Sensory Processing

```python
import pynlm

# Create brain and agent
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

agent = pynlm.create_agent_brain(brain)

# Create world
world = pynlm.create_simple_world()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()

# Initialize agent with world
agent.initialize(world)

# Process sensory input
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### 2. Memory and Learning

```python
import pynlm

# Create brain with memory subsystems
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Enable memory systems
agent = pynlm.create_agent_brain(brain)
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_structural_plasticity(True)

# Run learning simulation
for step in range(1000):
    # Generate some sensory input
    vision = pynlm.Vision(8, 8)
    # ... fill vision data ...
    
    # Process input
    percept = pynlm.SensoryPercept()
    percept.setVision(vision)
    
    agent.process_sensory_input(percept)
    brain.step(step)
    
    # Apply learning
    reward = 0.1  # Example reward
    agent.apply_reward_modulation(reward, 0.0)
    
    # Update development
    agent.update_development(0.1)
    
    # Check memory
    if step % 100 == 0:
        working_memory = brain.get_working_memory()
        if working_memory:
            print(f"Working memory traces: {working_memory.get_active_traces()}")
```

### 3. World Simulation

```python
import pynlm

# Create and configure world
world = pynlm.create_simple_world()
world.configure(
    width=40,
    height=30,
    visionWidth=16,
    visionHeight=12
)

world.reset()
world.set_agent_start(20.0, 15.0)

# Set up obstacles and resources
# Add walls
for x in range(5, 35):
    world.add_object(pynlm.WorldObject(x, 5, pynlm.WorldObjectType.Wall, radius=0.5))
    world.add_object(pynlm.WorldObject(x, 25, pynlm.WorldObjectType.Wall, radius=0.5))

# Add resources
for x in [10, 15, 25, 30]:
    world.add_object(pynlm.WorldObject(x, 15, pynlm.WorldObjectType.Resource, value=1.0))

# Add hazards
world.add_object(pynlm.WorldObject(20, 15, pynlm.WorldObjectType.Hazard, value=1.5))

# Run world simulation
for step in range(500):
    world.update(0.1)
    
    # Get agent state
    body = world.get_agent_body()
    percept = world.get_sensory_percept()
    
    print(f"Step {step}: Agent at ({body.x:.1f}, {body.y:.1f}), "
          f"energy: {body.energy:.1f}, health: {body.health:.1f}")
```

### 4. Advanced Brain Configuration

```python
import pynlm

# Create configuration with custom parameters
config = pynlm.create_default_config()

# Brain configuration
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.2)
config.set("brain.v_thresh", -40.0)  # Lower threshold
config.set("brain.v_rest", -65.0)   # Resting potential

# Plasticity configuration
config.set("plasticity.stdp.enable", true)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", true)
config.set("plasticity.structural.enable", true)

# Neuromodulation
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", true)
config.set("neuromod.novelty.enable", true)

# Development
config.set("development.stage", "critical_period")
config.set("development.plasticity_rate", 2.0)

# Create and initialize brain
brain = pynlm.create_brain(config)
brain.initialize()

print(brain.get_config_summary())
```

## Advanced Features

### 1. Custom Patterns and Learning

```python
import pynlm
import math

# Create brain
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Custom input patterns
patterns = {
    'sine': lambda t: math.sin(t * 2) * 10,
    'square': lambda t: 20 if (int(t * 5) % 2) else -5,
    'random': lambda t: (hash(t) % 100 - 50) / 10,
    'ramp': lambda t: t * 5 if t < 2 else 10
}

# Run with custom pattern
pattern_name = 'sine'
for step in range(1000):
    t = step * 0.01
    input_value = patterns[pattern_name](t)
    
    # Inject into sensory neurons
    for i in range(20):
        brain.inject_current_to_neurons(pynlm.NeuronType.Sensory, input_value)
    
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}, Input: {input_value:.2f}, "
              f"Spikes: {brain.get_total_spike_count()}")
```

### 2. Performance Monitoring

```python
import pynlm
import time

# Enable performance tracking
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Start timing
start_time = time.time()

# Run intensive simulation
for step in range(10000):
    brain.step(step)
    
    # Periodic performance reporting
    if step % 1000 == 0:
        elapsed = time.time() - start_time
        stats = brain.get_performance_stats()
        print(f"Step {step}: {elapsed:.2f}s elapsed, "
              f"{brain.get_firing_neuron_count()} neurons firing")

# Get final performance report
elapsed = time.time() - start_time
print(f"Total simulation time: {elapsed:.2f}s")
print(f"Performance stats: {stats}")
```

### 3. Checkpoint and Persistence

```python
import pynlm
import os

# Create brain
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Create checkpoint directory
checkpoint_dir = "nlm_checkpoints"
os.makedirs(checkpoint_dir, exist_ok=True)

# Run some simulation
for step in range(500):
    brain.step(step)
    
    # Save checkpoint periodically
    if step % 100 == 0:
        checkpoint_file = f"{checkpoint_dir}/checkpoint_step_{step}.bin"
        if brain.save(checkpoint_file):
            print(f"Checkpoint saved: {checkpoint_file}")

# Resume from latest checkpoint
latest_checkpoint = f"{checkpoint_dir}/checkpoint_step_{500}.bin"
if os.path.exists(latest_checkpoint):
    print("Resuming from checkpoint...")
    brain.load(latest_checkpoint)
    
    # Continue simulation
    for step in range(500, 1000):
        brain.step(step)
```

### 4. Visualization and Debugging

```python
import pynlm
import matplotlib.pyplot as plt

# Create visualization
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Collect statistics
spike_counts = []
firing_rates = []
neuron_counts = []

for step in range(1000):
    brain.step(step)
    
    spike_counts.append(brain.get_total_spike_count())
    firing_rates.append(brain.get_firing_neuron_count())
    neuron_counts.append(brain.get_total_neuron_count())

# Plot results
fig, axes = plt.subplots(3, 1, figsize=(12, 10))

axes[0].plot(spike_counts)
axes[0].set_title("Total Spike Count Over Time")
axes[0].set_ylabel("Spike Count")

axes[1].plot(firing_rates)
axes[1].set_title("Firing Neurons Over Time")
axes[1].set_ylabel("Firing Neurons")

axes[2].plot(neuron_counts, label='Neurons')
axes[2].set_title("Neuron Count Over Time")
axes[2].set_ylabel("Neuron Count")
axes[2].set_xlabel("Step")

plt.tight_layout()
plt.savefig("nlm_simulation_results.png")
plt.show()
```

### 5. Multi-Agent Systems

```python
import pynlm

# Create multiple brains for multi-agent simulation
brains = []
agents = []
worlds = []

for i in range(3):
    # Create brain
    brain = pynlm.create_brain(pynlm.create_default_config())
    brain.initialize()
    brains.append(brain)
    
    # Create agent
    agent = pynlm.create_agent_brain(brain)
    agents.append(agent)
    
    # Create world
    world = pynlm.create_simple_world()
    world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
    world.reset()
    world.set_agent_start(10 + i * 15, 25)
    worlds.append(world)

# Run multi-agent simulation
for step in range(500):
    # Update all worlds
    for i, world in enumerate(worlds):
        world.update(0.1)
        
        # Get percept
        percept = world.get_sensory_percept()
        
        # Process in each brain
        for j, agent in enumerate(agents):
            if j != i:  # All agents can sense others
                agent.process_sensory_input(percept)
        
        # Each brain thinks
        brains[i].step(step)
        
        # Each agent acts
        action = agents[i].decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        print(f"Step {step}, Agent {i}: "
              f"pos=({world.get_agent_body().x:.1f},{world.get_agent_body().y:.1f}), "
              f"energy={world.get_agent_body().energy:.1f}")
```

## Performance Optimization

### 1. Performance Tips

- **Use batch operations**: Process multiple steps in loops
- **Optimize configuration**: Set only necessary parameters
- **Monitor performance**: Use built-in performance tracking
- **Profile your code**: Use external profiling tools
- **Adjust simulation parameters**: Optimize timestep and neuron count

### 2. Configuration Optimization

```python
# Optimized configuration for different use cases

# Fast testing configuration
config_fast = pynlm.create_default_config()
config_fast.set("brain.neuron_count", 100)
config_fast.set("brain.synapse_density", 0.05)
config_fast.set("plasticity.stdp.enable", false)
config_fast.set("neuromod.curiosity.enable", false)

# Production configuration
config_prod = pynlm.create_default_config()
config_prod.set("brain.neuron_count", 5000)
config_prod.set("brain.synapse_density", 0.15)
config_prod.set("plasticity.stdp.enable", true)
config_prod.set("plasticity.hebbian.enable", true)
config_prod.set("neuromod.curiosity.enable", true)
config_prod.set("neuromod.novelty.enable", true)

# Research configuration
config_research = pynlm.create_default_config()
config_research.set("brain.neuron_count", 20000)
config_research.set("brain.synapse_density", 0.25)
config_research.set("development.stage", "adult")
config_research.set("plasticity.structural.enable", true)
```

### 3. Benchmarking

```python
import pynlm
import time

# Benchmark different configurations
configs = [
    {"name": "Small", "neurons": 100, "steps": 100},
    {"name": "Medium", "neurons": 1000, "steps": 1000},
    {"name": "Large", "neurons": 5000, "steps": 5000},
    {"name": "XL", "neurons": 20000, "steps": 2000}
]

results = []

for config_spec in configs:
    print(f"\nBenchmarking {config_spec['name']} configuration...")
    
    # Create configuration
    config = pynlm.create_default_config()
    config.set("brain.neuron_count", config_spec['neurons'])
    
    # Create brain
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Run benchmark
    start_time = time.time()
    
    for step in range(config_spec['steps']):
        brain.step(step)
    
    elapsed = time.time() - start_time
    
    # Calculate metrics
    spikes_per_second = brain.get_total_spike_count() / elapsed
    neurons_per_second = config_spec['neurons'] / elapsed
    steps_per_second = config_spec['steps'] / elapsed
    
    result = {
        'name': config_spec['name'],
        'neurons': config_spec['neurons'],
        'steps': config_spec['steps'],
        'elapsed_time': elapsed,
        'spikes_per_second': spikes_per_second,
        'neurons_per_second': neurons_per_second,
        'steps_per_second': steps_per_second
    }
    
    results.append(result)
    
    print(f"  Time: {elapsed:.2f}s")
    print(f"  Spikes/sec: {spikes_per_second:.0f}")
    print(f"  Neurons/sec: {neurons_per_second:.0f}")
    print(f"  Steps/sec: {steps_per_second:.0f}")

# Print summary
print("\n=== Benchmark Summary ===")
for result in results:
    print(f"{result['name']} ({result['neurons']} neurons): "
          f"{result['steps_per_second']:.0f} steps/sec, "
          f"{result['spikes_per_second']:.0f} spikes/sec")
```

## Troubleshooting

### 1. Common Issues and Solutions

#### Issue: Brain initialization fails

**Problem**: Brain initialization fails with error message.

**Solutions**:
1. Check configuration parameters
2. Verify required configuration keys
3. Ensure file paths are correct
4. Check system resources

```python
# Debug configuration
config = pynlm.create_default_config()
print(config.get_keys())

# Check if configuration is valid
if not config.has("brain.neuron_count"):
    print("Missing required key: brain.neuron_count")
```

#### Issue: Simulation becomes unstable

**Problem**: Network becomes unstable with exploding spike activity.

**Solutions**:
1. Reduce neuron count
2. Increase synaptic decay
3. Adjust neuron thresholds
4. Reduce input current

```python
# More stable configuration
config = pynlm.create_default_config()
config.set("brain.neuron_count", 500)  # Reduce neuron count
config.set("brain.v_thresh", -50.0)   # Increase threshold
config.set("plasticity.stdp.learning_rate", 0.0001)  # Reduce learning rate
```

#### Issue: Memory usage too high

**Problem**: Simulation uses excessive memory.

**Solutions**:
1. Reduce neuron count
2. Disable unused subsystems
3. Use more efficient configuration
4. Monitor memory usage

```python
# Memory-efficient configuration
config = pynlm.create_default_config()
config.set("brain.neuron_count", 200)
config.set("plasticity.structural.enable", false)
config.set("neuromod.curiosity.enable", false)
```

### 2. Debugging Tips

#### 1. Enable detailed logging

```python
import pynlm

# Enable detailed logging
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Add custom logging
import logging
logging.basicConfig(level=logging.DEBUG)

# Run simulation with logging
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: Firing neurons = {brain.get_firing_neuron_count()}")
```

#### 2. Use performance monitoring

```python
import pynlm

# Enable performance tracking
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Run simulation and get performance stats
for step in range(1000):
    brain.step(step)

stats = brain.get_performance_stats()
print("Performance stats:", stats)

# Generate detailed report
brain.generate_performance_report()
```

#### 3. Check system resources

```python
import psutil
import pynlm

# Check system resources
process = psutil.Process()
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")
print(f"CPU cores: {psutil.cpu_count()}")

# Check available memory
memory = psutil.virtual_memory()
print(f"Available memory: {memory.available / 1024 / 1024 / 1024:.2f} GB")

# Create brain with resource monitoring
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()
```

## API Reference

### Core Classes

#### Brain Class

```python
# Creation and initialization
brain = pynlm.create_brain(config)
brain.initialize()

# Simulation
brain.step(step)                    # Step with default time
brain.step(step, time)              # Step with specific time

# State management
brain.reset()                       # Reset to initial state
brain.save(filepath)               # Save to file
brain.load(filepath)               # Load from file

# Statistics
brain.get_total_neuron_count()     # Total neurons
brain.get_total_synapse_count()     # Total synapses
brain.get_firing_neuron_count()     # Currently firing
brain.get_total_spike_count()       # Total spikes
brain.get_average_firing_rate()     # Average firing rate
brain.get_excitation_inhibition_ratio()  # E/I balance

# Memory access
brain.get_working_memory()          # Access working memory
brain.get_episodic_memory()         # Access episodic memory
brain.get_associative_memory()      # Access associative memory

# Neuromodulation
brain.get_dopamine()               # Access dopamine system
brain.get_curiosity()               # Access curiosity system
brain.get_novelty()                 # Access novelty system
brain.get_prediction_error()        # Access prediction error

# Development
brain.get_developmental_stage()     # Get current stage
brain.set_developmental_stage(stage)# Set developmental stage
```

#### Config Class

```python
# Creation and manipulation
config = pynlm.create_default_config()
config.set("key", "value")
config.set("number", 42)
config.set("float", 3.14)
config.set("boolean", True)

# Access and queries
config.has("key")                    # Check if key exists
config.get("key")                   # Get value (optional)
config.get_or("key", "default")     # Get with default
config.get_keys()                   # Get all keys
config.clear()                       # Clear all
config.summary()                     # Get summary

# File operations
config.load_from_file("config.json")
config.save_to_file("config.json")

# Environment variables
config.load_from_environment()
```

#### AgentBrain Class

```python
# Creation and initialization
agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Sensory processing
agent.process_sensory_input(percept)

# Motor control
action = agent.decode_motor_command()

# Learning
agent.apply_reward_modulation(reward, predicted_reward)
agent.update_development(timestep)

# Subsystem control
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)

# Statistics
agent.get_curiosity_level()
agent.get_novelty_level()
agent.get_prediction_error()
agent.get_neuromodulation_level()
```

#### SimpleWorld Class

```python
# Creation and configuration
world = pynlm.create_simple_world()
world.configure(width, height, visionWidth, visionHeight)

# Control
world.reset()
world.update(timestep)
world.set_agent_start(x, y)

# Actions
world.apply_motor_command(command, current_time)

# State access
percept = world.get_sensory_percept()
body = world.get_agent_body()

# Object management
world.add_object(object)
world.remove_object(x, y)
world.is_valid_position(x, y)

# Properties
world.get_width()
world.get_height()
world.get_simulation_time()
world.get_max_energy()
world.set_max_energy(energy)
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

#### ActionType

```python
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.MoveLeft
pynlm.ActionType.MoveRight
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.LookUp
pynlm.ActionType.LookDown
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest
pynlm.ActionType.Wait
pynlm.ActionType.Custom
```

#### DevelopmentalStage

```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

### Utility Functions

```python
# Factory functions
pynlm.create_default_config()
pynlm.create_brain(config)
pynlm.create_simple_world()
pynlm.create_agent_brain(brain)

# Constants
pynlm.__version__
pynlm.__doc__
```

## Migration Guide

### From Previous NLM Versions

This guide helps migrate from earlier NLM versions to the current Phase 6 implementation.

#### 1. API Changes

**Changes**:
- Enhanced Python bindings with Pythonic APIs
- Custom exception classes added
- New configuration system with JSON/YAML support
- Improved performance monitoring
- Comprehensive test framework

**Migration**:
```python
# Old way
from nlm import Config, Brain

# New way (Pythonic)
import pynlm

config = pynlm.create_default_config()
brain = pynlm.create_brain(config)

# Old way
brain.getStats()

# New way
brain.get_performance_stats()
```

#### 2. Configuration Changes

**Changes**:
- New JSON/YAML configuration support
- Environment variable support
- Schema validation
- Configuration merging and diffing

**Migration**:
```python
# Old way
config = Config()
config.set("key", "value")

# New way (supports multiple formats)
config = pynlm.create_default_config()
config.set("key", "value")
config.set("json_key", json_value)  # Auto-converted
```

#### 3. Error Handling Changes

**Changes**:
- Custom exception classes
- Better error messages
- Exception chaining

**Migration**:
```python
# Old way
try:
    brain.initialize()
except std::runtime_error& e:
    std::cerr << "Error: " << e.what() << std::endl;

# New way (Pythonic)
try:
    brain = pynlm.create_brain(config)
    brain.initialize()
except pynlm.NLMException as e:
    print(f"Error: {e}")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
```

#### 4. Performance Monitoring Changes

**Changes**:
- Built-in performance tracking
- Detailed performance reports
- Operation-specific timing

**Migration**:
```python
# Old way (manual timing)
auto start = std::chrono::high_resolution_clock::now();
for (int i = 0; i < 1000; ++i) {
    brain.step(i);
}
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration<double>(end - start).count();

# New way (automatic)
brain = pynlm.create_brain(config)
brain.initialize()
for (int i = 0; i < 1000; ++i) {
    brain.step(i);
}
print(f"Performance: {brain.get_performance_stats()}")
```

### 1. Upgrading from Phase 2/3/4/5

If you're upgrading from earlier NLM phases:

1. **API Compatibility**: The current API maintains compatibility with Phase 6
2. **Enhanced Features**: You get all Phase 6 features automatically
3. **Performance**: Significant performance improvements
4. **New Capabilities**: Advanced memory systems, neuromodulation, and cognition

#### Migration Checklist

- [ ] Update imports: `from nlm import *` → `import pynlm`
- [ ] Update factory calls: `Config()` → `pynlm.create_default_config()`
- [ ] Update brain creation: `Brain(config)` → `pynlm.create_brain(config)`
- [ ] Update error handling to use Pythonic exceptions
- [ ] Enable new features: memory, neuromodulation, development
- [ ] Test with existing code
- [ ] Optimize performance

## Contributing

### 1. Contributing Code

#### Guidelines

- Follow the existing code style
- Add comprehensive tests
- Write clear documentation
- Use meaningful variable names
- Add type hints for Python bindings
- Include error handling and edge cases

#### Pull Request Process

1. Fork the repository
2. Create feature branch
3. Implement changes
4. Add tests
5. Update documentation
6. Submit pull request
7. Review and iterate

### 2. Documentation

#### Writing Documentation

- Use Markdown format
- Include code examples
- Document API changes
- Add migration guides
- Provide troubleshooting sections

#### Documentation Structure

- **Getting Started**: Quick start guide
- **API Reference**: Complete API documentation
- **Examples**: Working code examples
- **Tutorials**: Step-by-step guides
- **Troubleshooting**: Common issues and solutions

### 3. Testing

#### Writing Tests

- Use the enhanced test framework
- Test edge cases
- Test error conditions
- Test performance
- Test compatibility

#### Test Categories

1. **Unit Tests**: Test individual components
2. **Integration Tests**: Test system integration
3. **Performance Tests**: Test performance characteristics
4. **Regression Tests**: Test for regressions
5. **Stress Tests**: Test under extreme conditions

### 4. Performance Optimization

#### Optimization Guidelines

- Profile your code
- Use efficient data structures
- Minimize unnecessary allocations
- Use appropriate algorithms
- Configure for your use case

#### Optimization Checklist

- [ ] Profile simulation performance
- [ ] Optimize configuration
- [ ] Reduce memory usage
- [ ] Improve I/O operations
- [ ] Optimize algorithms
- [ ] Use appropriate data structures

### 5. Code Quality

#### Coding Standards

- Use consistent naming conventions
- Follow PEP 8 for Python code
- Use meaningful variable names
- Add comments for complex code
- Use proper error handling

#### Code Review

- Check for potential bugs
- Ensure code quality
- Verify performance
- Test compatibility
- Review documentation

## Conclusion

NLM (Neural Learning Machine) Phase 6 provides a complete artificial brain simulation system with:

- **Real Spiking Neurons**: LIF neurons with event-driven dynamics
- **Multiple Plasticity Mechanisms**: STDP, Hebbian, and structural plasticity
- **Integrated Memory Systems**: Working, episodic, and associative memory
- **Advanced Neuromodulation**: Dopamine, curiosity, and novelty detection
- **Cognition Systems**: Planning, attention, and concept formation
- **Development**: Stage-based maturation and adaptation
- **World Simulation**: Environment and agent interaction
- **Performance Monitoring**: Built-in performance tracking
- **Enhanced Configuration**: JSON/YAML support and validation

This comprehensive guide provides everything you need to use NLM effectively, from basic setup to advanced applications. Use this guide as your primary resource for working with NLM, and don't hesitate to explore and experiment with the various features.

### Happy Brain Building! 🚀

*Created by the NLM Development Team*
*Phase 6: Final Integration*
*Version 6.0.0*