# NLM Python API - Enhanced Documentation

## Overview

This document covers the enhanced Python API for NLM (Neural Learning Machine), including new convenience methods, improved error handling, and higher-level APIs for common use cases.

## Table of Contents

1. [Enhanced Configuration API](#enhanced-configuration-api)
2. [Higher-Level Simulation APIs](#higher-level-simulation-apis)
3. [Error Handling and Validation](#error-handling-and-validation)
4. [Migration Guide](#migration-guide)
5. [Advanced Usage Examples](#advanced-usage-examples)
6. [Performance Tips](#performance-tips)
7. [Troubleshooting](#troubleshooting)

## Enhanced Configuration API

### 1.1 Factory Methods

The enhanced Python API provides several factory methods for creating configurations and components:

```python
import pynlm

# Create default configuration
config = pynlm.Config.createDefault()

# Create configuration from file
config = pynlm.Config.fromFile("my_config.json")

# Create configuration from Python dictionary
config = pynlm.Config.fromDict({
    "neuron.count": 500,
    "world.width": 16,
    "world.height": 16,
    "plasticity.stdp_enabled": True,
    "neuromodulation.curiosity_enabled": True
})
```

### 1.2 Convenience Getters and Setters

```python
# Get common values with defaults
neuron_count = config.getNeuronCount(1000)
world_width, world_height = config.getWorldSize()
timestep = config.getSimulationTimestep(0.001)
max_steps = config.getMaxSteps(10000)

# Set values with Pythonic interface
config.set("neuron.count", 1000)
config.set("world.width", 32)
config.set("plasticity.stdp_enabled", True)
config.set("neuromodulation.curiosity_enabled", False)

# Convert configuration to dictionary for JSON serialization
config_dict = config.toDict()
```

### 1.4 Configuration Validation

```python
# Validate configuration
is_valid, errors = pynlm.Config.validate(config)
if not is_valid:
    print("Configuration errors:", errors)
    # Fix configuration issues...
    config.set("neuron.count", 500)  # Set valid value
    is_valid, errors = pynlm.Config.validate(config)

# Merge configurations (other config takes precedence)
base_config = pynlm.Config.createDefault()
override_config = pynlm.Config.fromDict({"world.width": 32})
base_config.merge(override_config)
```

### 1.5 Configuration Cloning and Copying

```python
# Create a copy of existing configuration
config_copy = config.clone()

# Create configuration from existing one
config2 = pynlm.Config.fromDict(config.toDict())
```

## Higher-Level Simulation APIs

### 2.1 Complete Simulation Setup

```python
import pynlm

# Create a complete, ready-to-use simulation (RECOMMENDED!)
brain, world, agent = pynlm.createReadySimulation()

# Alternatively, create individual components
config = pynlm.Config.createDefault()
brain = pynlm.Config.createBrain(config)
world = pynlm.Config.createSimpleWorldWithConfig(config)
agent = pynlm.Config.createAgentBrainWithWorld(brain, world)
```

### 2.2 Brain Creation Helpers

```python
# Create brain from default configuration
brain = pynlm.createBrainFromDefaults()

# Create brain from file
brain = pynlm.Config.createBrainFromFile("my_config.json")

# Create agent with world
agent = pynlm.Config.createAgentBrainWithWorld(brain, world)

# Simple agent initialization
agent.initialize(world)
```

### 2.3 World Configuration

```python
# Create simple world
world = pynlm.Config.createSimpleWorld()

# Configure world with specific dimensions
world.configure(32, 32, 16, 16)  # width, height, vision_width, vision_height

# Create world from configuration
world = pynlm.Config.createSimpleWorldWithConfig(config)
```

## Error Handling and Validation

### 3.1 Improved Error Messages

```python
try:
    config = pynlm.Config.fromFile("nonexistent.json")
except Exception as e:
    print(f"Error: {e}")

# More specific error handling
try:
    brain.initialize()
except pynlm.RuntimeError as e:
    print(f"Brain initialization failed: {e}")
```

### 3.2 Input Validation

```python
# Configuration file validation
config = pynlm.Config.fromFile("valid_config.json")
is_valid, errors = pynlm.Config.validate(config)

# Value validation
config.set("neuron.count", 0)  # Invalid (must be > 0)
is_valid, errors = pynlm.Config.validate(config)
```

### 3.3 Safe Operations

```python
# Safe file operations
try:
    config.saveToFile("backup.json")
    print("Configuration saved successfully")
except Exception as e:
    print(f"Failed to save configuration: {e}")
```

## Migration Guide

### 4.1 From Old API to New API

**Old API (Version 0.1.0):**

```python
import pynlm

# Old way - verbose and error-prone
config = pynlm.Config()
config.loadFromFile("config.json")
config.set("neuron.count", 500)

brain = pynlm.Brain(config)
brain.initialize()

world = pynlm.SimpleWorld()
world.configure(16, 16, 8, 8)

agent = pynlm.AgentBrain(brain)
agent.initialize(world)
```

**New API (Version 0.2.0+):**

```python
import pynlm

# New way - concise and robust
config = pynlm.Config.fromFile("config.json")
config.set("neuron.count", 500)

brain = pynlm.Config.createBrain(config)
world = pynlm.Config.createSimpleWorldWithConfig(config)
agent = pynlm.Config.createAgentBrainWithWorld(brain, world)
```

### 4.2 Feature Matrix

| Feature | Old API | New API | Notes |
|---------|---------|---------|-------|
| Config Creation | `Config()` + `loadFromFile()` | `Config.fromFile()` | One line |
| Config Defaults | Manual setting | `Config.createDefault()` | Sensible defaults |
| Brain Creation | `Brain(config)` + `initialize()` | `Config.createBrain(config)` | Auto-initializes |
| World Creation | `SimpleWorld()` + `configure()` | `Config.createSimpleWorldWithConfig(config)` | Configured from config |
| Agent Creation | `AgentBrain(brain)` + `initialize(world)` | `Config.createAgentBrainWithWorld(brain, world)` | Auto-initializes |

## Advanced Usage Examples

### 5.1 Learning Simulation

```python
import pynlm
import time

# Create ready-to-use simulation
simulation = pynlm.createReadySimulation()
brain, world, agent = simulation

# Configure agent for learning
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation for many steps
for step in range(10000):
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process input
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action and execute
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Provide reward feedback
    reward = 0.0  # Calculate based on outcome
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print progress
    if step % 1000 == 0:
        print(f"Step {step}: Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"Reward: {reward:.3f}, Dopamine: {agent.getNeuromodulationLevel():.3f}")
```

### 5.2 Experiment Management

```python
import pynlm
import json

# Create experimental configuration
config = pynlm.Config.createDefault()
config.set("neuron.count", 200)  # Smaller brain for faster experiments
config.set("simulation.max_steps", 5000)

# Create experimental setup
brain, world, agent = pynlm.createSimulationSetup(config)

# Enable specific experimental features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run experiment
results = []
for step in range(config.getOr<int64_t>("simulation.max_steps", 5000)):
    # Run experiment step
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Collect metrics
    metrics = {
        "step": step,
        "firing_rate": brain.getAverageFiringRate(),
        "dopamine": agent.getNeuromodulationLevel(),
        "curiosity": agent.getCuriosityLevel(),
        "novelty": agent.getNoveltyLevel()
    }
    results.append(metrics)

# Save results
with open("experiment_results.json", "w") as f:
    json.dump(results, f, indent=2)

# Calculate statistics
avg_firing = sum(r["firing_rate"] for r in results) / len(results)
max_firing = max(r["firing_rate"] for r in results)
```

### 5.3 Configuration Management

```python
import pynlm

# Configuration template system
templates = {
    "small": {
        "neuron.count": 100,
        "world.width": 8,
        "world.height": 8,
        "simulation.max_steps": 1000
    },
    "medium": {
        "neuron.count": 500,
        "world.width": 16,
        "world.height": 16,
        "simulation.max_steps": 10000
    },
    "large": {
        "neuron.count": 2000,
        "world.width": 32,
        "world.height": 32,
        "simulation.max_steps": 50000
    }
}

# Create configuration from template
def createConfigFromTemplate(name):
    config = pynlm.Config.createDefault()
    for key, value in templates[name].items():
        config.set(key, value)
    return config

# Use different configurations
config_small = createConfigFromTemplate("small")
config_medium = createConfigFromTemplate("medium")
config_large = createConfigFromTemplate("large")
```

## Performance Tips

### 6.1 Optimize Configuration Loading

```python
# Load configuration once, reuse it
config = pynlm.Config.fromFile("config.json")

# Create multiple brains with same configuration
brain1 = pynlm.Config.createBrain(config)
brain2 = pynlm.Config.createBrain(config)  # Efficient - shares config
```

### 6.2 Batch Processing

```python
# Create simulation setup
brain, world, agent = pynlm.createReadySimulation()

# Process multiple steps efficiently
for step in range(1000):
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### 6.3 Memory Management

```python
import pynlm

# Clean up references to free memory
brain = None
world = None
agent = None

# Force garbage collection
import gc
gc.collect()
```

## Troubleshooting

### 7.1 Common Issues and Solutions

**Issue 1: "Failed to load configuration from file"**

**Solution:**
```python
try:
    config = pynlm.Config.fromFile("config.json")
except Exception as e:
    print(f"Configuration error: {e}")
    # Check if file exists
    import os
    if not os.path.exists("config.json"):
        print("File not found. Creating default configuration...")
        config = pynlm.Config.createDefault()
```

**Issue 2: "Invalid configuration value"**

**Solution:**
```python
# Validate before setting
if not pynlm.Config.validate(config):
    print("Configuration has errors. Fixing...")
    config.set("neuron.count", 500)  # Set valid value
    config.set("world.width", 16)     # Set valid value
```

**Issue 3: "Brain initialization failed"**

**Solution:**
```python
brain = pynlm.Config.createBrain(config)
try:
    brain.initialize()
except Exception as e:
    print(f"Brain initialization failed: {e}")
    # Check configuration
    print("Current configuration:", config.summary())
```

### 7.2 Error Codes

| Error Code | Description | Solution |
|------------|-------------|----------|
| 1001 | Configuration file not found | Check file path, create default config |
| 1002 | Invalid neuron count | Set value between 1 and 1000000 |
| 1003 | Invalid world dimensions | Set width/height between 1 and 1000 |
| 1004 | Invalid simulation timestep | Set timestep between 0.0001 and 1.0 |
| 1005 | Brain initialization failed | Check configuration, increase resources |

## Conclusion

The enhanced NLM Python API provides a more intuitive, robust, and user-friendly interface for neural simulation. The new features include:

- **Factory methods** for common use cases
- **Improved error handling** with descriptive messages
- **Configuration validation** to prevent runtime errors
- **Higher-level APIs** that abstract away boilerplate code
- **Comprehensive documentation** with examples and migration guides

These improvements make NLM more accessible to researchers, students, and developers while maintaining the power and flexibility of the underlying C++ implementation.

For additional support, please visit the NLM documentation or contact the development team.
