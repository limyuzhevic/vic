# NLM Python Bindings: Quick Start Guide

This guide provides comprehensive examples and best practices for using the NLM Python bindings. It includes practical demonstrations of the enhanced features and troubleshooting for common issues.

## Table of Contents

1. [Quick Examples](#quick-examples)
2. [Configuration Management](#configuration-management)
3. [Brain Setup and Initialization](#brain-setup-and-initialization)
4. [Simulation Control](#simulation-control)
5. [Advanced Features](#advanced-features)
6. [Troubleshooting](#troubleshooting)
7. [Performance Tips](#performance-tips)
8. [Migration Guide](#migration-guide)

---

## Quick Examples

### 1. Basic Brain Setup

```python
import pynlm as nlm

# Create a brain with Phase 2 configuration
config = nlm.createPhase2Config()
brain = nlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Regions: {brain.getRegionCount()}")
```

### 2. Simple Learning Episode

```python
import pynlm as nlm
import numpy as np

# Create a learning environment
brain, agent_brain = nlm.createBrainWithWorld(nlm.createPhase6Config())

# Create sensory input (vision)
vision = nlm.Vision(20, 20, 3)
vision.setData(np.random.rand(20 * 20 * 3).astype(np.float32))

# Run a simple learning episode
episode = nlm.SimulationEpisode(brain, 100)
episode.run()

# Get brain statistics
stats = nlm.getBrainStats(brain)
print(f"Learning completed: {stats['spike_count']} spikes, "
      f"{stats['average_firing_rate']:.2f} Hz average firing rate")
```

### 3. Batch Operations for Performance

```python
import pynlm as nlm
import time

# Create brain and agent
brain, agent_brain = nlm.createBrainWithWorld(nlm.createPhase2Config())

# Inject current into many neurons at once (more efficient)
neuron_injections = []
for i in range(50):
    neuron_injections.append((nlm.INVALID_NEURON_ID, 30.0))

start_time = time.time()
brain = nlm.injectCurrentBatch(brain, neuron_injections)
print(f"Batch injection completed in {time.time() - start_time:.4f} seconds")
```

### 4. Configuration Management

```python
import pynlm as nlm

# Create configuration from dictionary
config_dict = {
    "neuron_count": 2000,
    "region_count": 5,
    "connection_probability": 0.25,
    "stdp_ltp_weight": 0.03,
    "stdp_ltd_weight": 0.02
}

config = nlm.createConfigFromDict(config_dict)
brain = nlm.createBrain(config)
brain.initialize()

# Validate configuration
is_valid, warnings = nlm.validateConfig(config)
if not is_valid:
    for warning in warnings:
        print(f"Warning: {warning}")
```

---

## Configuration Management

### Factory Functions

The enhanced bindings provide several convenience functions for creating common configurations:

```python
# Phase 2: Basic neural computation
config = nlm.createPhase2Config()

# Phase 6: Full integration with all systems
config = nlm.createPhase6Config()

# Default empty configuration (use your own settings)
config = nlm.createDefaultConfig()
```

### Configuration Validation

```python
# Validate your configuration before using it
is_valid, warnings = nlm.validateConfig(config)

if is_valid:
    print("Configuration is valid!")
else:
    print("Configuration issues found:")
    for warning in warnings:
        print(f"  - {warning}")
```

### Setting Configuration Values

```python
config = nlm.createDefaultConfig()

# String values
config.set("world_width", "100")
config.set("world_height", "100")

# Integer values  
config.set("neuron_count", 1000)
config.set("region_count", 3)

# Floating-point values
config.set("connection_probability", 0.15)
config.set("stdp_ltp_weight", 0.02)

# Boolean values
config.set("enable_plasticity", True)
config.set("enable_development", False)
```

---

## Brain Setup and Initialization

### Basic Brain Creation

```python
import pynlm as nlm

# Method 1: Manual setup
config = nlm.createPhase2Config()
brain = nlm.createBrain(config)
brain.initialize()

# Method 2: Using helper function (recommended)
brain = nlm.enhanced_createBrain(nlm.createPhase6Config())
```

### World Integration

```python
# Create brain with integrated world and agent
brain, agent_brain = nlm.createBrainWithWorld(
    nlm.createPhase6Config(),
    worldWidth=150,
    worldHeight=100
)

# Configure the world
agent_brain.initialize()
agent_brain.getBrain().logStatus()
```

### Sensory Input Setup

```python
import pynlm as nlm
import numpy as np

# Create different types of sensory input
vision = nlm.Vision(32, 32, 3)  # RGB image
vision.setData(np.random.rand(32 * 32 * 3).astype(np.float32))

audio = nlm.Audio(44100, 1024)  # Audio samples
sine_wave = np.sin(2 * np.pi * 440 * np.arange(1024) / 44100)
audio.setData(sine_wave.astype(np.float32))
audio.setSampleRate(44100)

internal = nlm.InternalSignals() internal.addSignal(0.5)
```

### Motor Action Creation

```python
import pynlm as nlm

# Create actions using different constructors
action1 = nlm.Action(nlm.ActionType.MoveForward)
action2 = nlm.Action(nlm.ActionType.Custom, [0.5, 0.3, 0.8])
action3 = nlm.Action(nlm.ActionType.Look, [0.2, 0.9])

# Clone actions
cloned_action = action1.clone()

# Get action information
print(f"Action: {action1.getName()}")
print(f"Parameters: {action1.getParameters()}")
```

---

## Simulation Control

### Simple Simulation

```python
import pynlm as nlm

brain, agent_brain = nlm.createBrainWithWorld()

# Step through simulation one step at a time
for step in range(10):
    brain.step(step, step * 0.001)
    
    # Optional: run custom logic every few steps
    if step % 5 == 0:
        stats = nlm.getBrainStats(brain)
        print(f"Step {step}: {stats['spike_count']} spikes")
```

### Batch Simulation

```python
import pynlm as nlm
import time

# Run a simulation with batch operations
brain, agent_brain = nlm.createBrainWithWorld()

# Create batch current injections
batch_injections = []
for i in range(20):
    batch_injections.append((nlm.INVALID_NEURON_ID, 25.0))

# Inject all at once (efficient)
brain = nlm.injectCurrentBatch(brain, batch_injections)

# Run simulation
brain = nlm.safeStep(brain, 100, maxSteps=1000)

# Get performance metrics
metrics = nlm.getPerformanceMetrics(brain)
print(f"Simulation metrics: {metrics}")
```

### Episode Management

```python
import pynlm as nlm

# Create a managed simulation episode
brain = nlm.createBrain(nlm.createPhase6Config())
episode = nlm.SimulationEpisode(brain, 500)

# Run the episode
if episode.run():
    print(f"Episode completed in {episode.getStep()} steps")
    
    # Get statistics
    stats = nlm.getBrainStats(brain)
    print(f"Total spikes: {stats['spike_count']}")
else:
    print("Episode failed to run")
```

---

## Advanced Features

### Configuration from Dictionary

```python
import pynlm as nlm

# Create configuration from Python dict (more Pythonic than JSON)
config_dict = {
    "neural": {
        "neuron_count": 2000,
        "region_count": 4,
        "connection_probability": 0.2
    },
    "plasticity": {
        "stdp_ltp_weight": 0.025,
        "stdp_ltd_weight": 0.018
    },
    "development": {
        "enable_structural_plasticity": True,
        "development_stage": "Adult"
    }
}

config = nlm.createConfigFromDict(config_dict)

# Access nested values
global_neurons = config.getOr("neural.neuron_count", 500)
plasticity_enabled = config.getOr("development.enable_structural_plasticity", False)
```

### Brain Comparison and Analysis

```python
import pynlm as nlm

# Create two brains for comparison
brain1 = nlm.createBrain(nlm.createPhase2Config())
brain2 = nlm.createBrain(nlm.createPhase6Config())

# Compare key metrics
comparison = nlm.compareBrains(brain1, brain2)
for metric, (value1, value2) in comparison.items():
    print(f"{metric}: Brain1={value1}, Brain2={value2}")
```

### Error Handling and Validation

```python
import pynlm as nlm
import traceback

try:
    # This will fail gracefully with helpful error message
    invalid_brain = None
    invalid_brain.step(1, 0.001)  # Will raise RuntimeError
    
except nlm.RuntimeError as e:
    print(f"Caught expected error: {e}")
    
except Exception as e:
    print(f"Unexpected error: {e}")
    traceback.print_exc()

# Configuration validation
try:
    config = nlm.createDefaultConfig()
    config.set("neuron_count", -1)  # Invalid value
    
    # This should raise InvalidArgument
    is_valid, warnings = nlm.validateConfig(config)
    
except nlm.InvalidArgument as e:
    print(f"Invalid argument: {e}")
```

---

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: "Brain is null" errors

```python
# Problem: Calling methods on a null brain
# Solution: Always validate before use

brain = None
try:
    if brain is None:
        raise nlm.RuntimeError("Brain cannot be null")
    brain.step(1, 0.001)
except nlm.RuntimeError as e:
    print(f"Error: {e}")
    brain = nlm.createBrain(nlm.createPhase2Config())  # Fix
```

#### Issue 2: Configuration validation fails

```python
# Problem: Configuration has invalid values
# Solution: Use createConfigFromDict with proper validation

config_dict = {"neuron_count": -100}  # Invalid
try:
    config = nlm.createConfigFromDict(config_dict)
    
    # Check validation
    is_valid, warnings = nlm.validateConfig(config)
    if not is_valid:
        raise nlm.InvalidArgument(f"Invalid config: {warnings}")
        
except nlm.InvalidArgument as e:
    print(f"Config error: {e}")
    # Fix: Use valid values
    config_dict = {"neuron_count": 1000}
    config = nlm.createConfigFromDict(config_dict)
```

#### Issue 3: Simulation doesn't advance

```python
# Problem: Simulation appears stuck
# Solution: Check brain state and enable plasticity

brain = nlm.createBrain(nlm.createPhase6Config())
brain.initialize()

# Check if plasticity is enabled
if not brain.getConfig().getOr("stdp_ltp_weight", 0.0) > 0:
    print("Warning: STDP LTP weight is zero - learning may not occur")

# Verify simulation is progressing
total_spikes_before = brain.getTotalSpikeCount()
brain.step(10, 0.01)
total_spikes_after = brain.getTotalSpikeCount()

if total_spikes_after == total_spikes_before:
    print("No spikes generated - check if neurons are receiving input")
else:
    print(f"Simulation progressing: {total_spikes_after - total_spikes_before} new spikes")
```

---

## Performance Tips

### 1. Use Batch Operations

```python
import pynlm as nlm
import time

brain, agent_brain = nlm.createBrainWithWorld()

# Single large batch vs multiple small injections
single_batch_start = time.time()
batch_injections = [(i, 10.0) for i in range(1000)]
brain = nlm.injectCurrentBatch(brain, batch_injections)
print(f"Single batch: {time.time() - single_batch_start:.4f} seconds")

# Compare with individual injections
individual_start = time.time()
for i, current in batch_injections:
    brain.injectCurrent(i, current)
print(f"Individual injections: {time.time() - individual_start:.4f} seconds")
```

### 2. Pre-compile and Cache

```python
import pynlm as nlm

# For repeated simulations, pre-compile common configurations
common_config = nlm.createPhase6Config()

# Cache frequently used brains
brain_cache = {}

def getBrainFromCache(config_key):
    if config_key not in brain_cache:
        brain = nlm.createBrain(common_config)
        brain.initialize()
        brain_cache[config_key] = brain
    return brain_cache[config_key]

# Use cached brains
brain1 = getBrainFromCache("default")
brain2 = getBrainFromCache("default")
print(f"Same brain object: {brain1 is brain2}")
```

### 3. Memory Management

```python
import pynlm as nlm
import gc

# Explicit cleanup
brain = nlm.createBrain(nlm.createPhase2Config())

# Clear references when done
del brain
gc.collect()  # Force garbage collection
```

---

## Migration Guide

### From Old to New API

#### Configuration Creation

```python
# OLD way
config = nlm.Config()
config.loadFromArgs([])
config.set("neuron_count", 500)

# NEW way (recommended)
config = nlm.createPhase2Config()
# or
config = nlm.createDefaultConfig()
config.set("neuron_count", 500)
```

#### Brain Creation

```python
# OLD way
brain = nlm.Brain(config)
brain.initialize()

# NEW way (recommended)
brain = nlm.createBrain(config)
brain.initialize()

# Even better - one-liner
brain = nlm.enhanced_createBrain(nlm.createPhase6Config())
```

#### Error Handling

```python
# OLD way - basic error handling
try:
    brain.step(1, 0.001)
except std::runtime_error as e:
    print("Error:", e.what())

# NEW way - Pythonic error handling
try:
    brain.step(1, 0.001)
except nlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except nlm.InvalidArgument as e:
    print(f"Invalid argument: {e}")
```

### Migration Checklist

- [ ] Replace `nlm.Config()` with `nlm.createDefaultConfig()` or factory functions
- [ ] Replace manual brain creation with `nlm.createBrain()`
- [ ] Add validation using `nlm.validateConfig()`
- [ ] Use batch operations with `nlm.injectCurrentBatch()`
- [ ] Implement proper error handling with Python-specific exceptions
- [ ] Use context managers for simulation episodes
- [ ] Add performance monitoring with `nlm.getPerformanceMetrics()`

---

## Advanced Usage Examples

### Creating Complex Learning Environments

```python
import pynlm as nlm
import numpy as np

def create_learning_environment(learning_type="reinforcement"):
    """Create a brain tailored for specific learning scenarios"""
    
    if learning_type == "reinforcement":
        config = nlm.createPhase6Config()
        config.set("enable_reward_modulation", True)
        config.set("enable_curiosity", True)
        config.set("enable_novelty", True)
        
    elif learning_type == "pattern":
        config = nlm.createPhase2Config()
        config.set("neuron_count", 5000)
        config.set("connection_probability", 0.3)
        
    elif learning_type == "continuous":
        config = nlm.createPhase6Config()
        config.set("simulation_timestep", 0.0001)  # Faster simulation
        config.set("enable_structural_plasticity", True)
    
    # Create brain with enhanced setup
    brain, agent_brain = nlm.createBrainWithWorld(config)
    
    # Configure world and sensory inputs
    world = agent_brain.getBrain().getWorld()
    world.configure(100, 100, 20, 20)
    
    return brain, agent_brain

# Use the learning environment
brain, agent_brain = create_learning_environment("reinforcement")
print(f"Created {agent_brain.getLearningType()} learning environment")
```

### Monitoring and Debugging

```python
import pynlm as nlm
import time
import matplotlib.pyplot as plt

def monitor_brain(brain, duration_steps=1000):
    """Monitor brain activity and return metrics"""
    
    spike_history = []
    firing_rate_history = []
    time_steps = []
    
    for step in range(duration_steps):
        brain.step(step, step * 0.001)
        
        spike_history.append(brain.getTotalSpikeCount())
        firing_rate_history.append(brain.getAverageFiringRate())
        time_steps.append(step * 0.001)
        
        # Log progress every 10% complete
        if step % (duration_steps // 10) == 0:
            print(f"Progress: {step}/{duration_steps} steps "
                  f"({spike_history[-1]} total spikes)")
    
    # Calculate statistics
    metrics = {
        "total_spikes": spike_history[-1],
        "max_spikes_per_step": max(spike_history) - min(spike_history),
        "average_firing_rate": sum(firing_rate_history) / len(firing_rate_history),
        "final_spike_count": spike_history[-1],
        "stability": np.std(np.diff(spike_history))  # Measure of variability
    }
    
    return metrics, time_steps, spike_history

# Monitor brain activity
brain = nlm.createBrain(nlm.createPhase6Config())
metrics, times, spikes = monitor_brain(brain, 500)

print(f"Monitoring complete: {metrics['total_spikes']} total spikes")
print(f"Average firing rate: {metrics['average_firing_rate']:.2f} Hz")
print(f"Activity stability: {metrics['stability']:.4f}")
```

### Custom Simulation Loop

```python
import pynlm as nlm
import numpy as np

def custom_simulation_loop(brain, world, max_steps=1000):
    """Custom simulation loop with goal-based control"""
    
    episode_reward = 0.0
    episode_steps = 0
    brain.reset()
    
    print("Starting custom simulation...")
    
    for step in range(max_steps):
        # 1. Sense current state
        percept = world.getSensoryPercept()
        
        # 2. Process sensory input
        world.applyMotorCommand(
            nlm.Action(nlm.ActionType.Wait), 
            step * 0.001
        )
        
        # 3. Simulate neural processing
        brain.step(step, step * 0.001)
        
        # 4. Get action from brain
        action = brain.produceAction()
        
        # 5. Apply action and get reward
        result = world.applyMotorCommand(action, step * 0.001)
        episode_reward += result.reward
        
        # 6. Apply reward to brain (learning)
        brain.applyRewardModulation(result.reward, 0.0)
        
        episode_steps += 1
        
        # 7. Check for termination conditions
        if brain.getTotalSpikeCount() > 100000:  # Too much activity
            print(f"Terminating: Excessive neural activity")
            break
            
        if episode_steps % 100 == 0:
            print(f"Step {step}: Reward={result.reward:.3f}, "
                  f"Total Reward={episode_reward:.3f}")
        
        # 8. Check if learning objective achieved
        if brain.getAverageFiringRate() > 50.0:  # Stable firing
            print(f"Learning objective achieved at step {step}")
            break
    
    return episode_reward, episode_steps

# Run custom simulation
brain, agent_brain = nlm.createBrainWithWorld(nlm.createPhase6Config())
world = agent_brain.getBrain().getWorld()

final_reward, final_steps = custom_simulation_loop(brain, world, max_steps=2000)
print(f"Simulation completed: {final_steps} steps, {final_reward:.3f} total reward")
```

---

## Conclusion

The enhanced NLM Python bindings provide a powerful, Pythonic interface to the neural learning machine framework. This guide covers:

1. **Basic usage** for getting started quickly
2. **Configuration management** with factory functions
3. **Simulation control** with batch operations and validation
4. **Advanced features** like context managers and performance monitoring
5. **Troubleshooting** for common issues
6. **Performance tips** for optimizing your simulations
7. **Migration guidance** for upgrading from previous versions
8. **Advanced examples** for complex learning scenarios

The enhanced bindings maintain full compatibility with the original NLM framework while providing significant improvements in usability, safety, and performance.

---

*This document is part of the enhanced NLM Python bindings package.*
For more information, visit the NLM project documentation or GitHub repository.