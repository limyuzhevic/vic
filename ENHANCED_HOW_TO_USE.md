# NLM (Neural Learning Machine) - Enhanced Documentation

## Overview

This document provides enhanced documentation for the NLM Python API, including all new features and improvements.

## New Enhanced Features

The NLM project has been significantly enhanced with improved Python bindings, advanced configuration options, and better user experience.

## Part 4: Advanced Python API Features

### Enhanced Configuration System

The enhanced Python bindings provide:

#### Advanced Configuration Creation

```python
import pynlm

# Create configuration with automatic validation and error handling
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)  # Automatically validates and initializes

# Load configuration from file with error handling
config = pynlm.createConfigFromFile("config.json")

# Validate configuration and get detailed error messages
if not config.validate():
    errors = config.getErrorMessages()
    for error in errors:
        print(f"Configuration error: {error}")
```

#### Configuration Management

```python
# Set configuration values with type checking
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.random_seed", 42)

# Get configuration values with error handling
try:
    neuron_count = config.get("brain.neuron_count")
except ValueError as e:
    print(f"Error getting config value: {e}")

# Use convenience methods for common operations
if config.has("brain.neuron_count"):
    val = config.getOr("brain.neuron_count", 1000)
```

### Enhanced Brain Creation

```python
# Method 1: Create brain with default configuration
brain = pynlm.createBrainWithDefaults()

# Method 2: Create brain from file
brain = pynlm.createBrain("my_config.json")

# Method 3: Create brain with error handling
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
```

### World Configuration

```python
# Create custom world with precise control
world = pynlm.createCustomWorld(
    width=30,
    height=30,
    visionWidth=12,
    visionHeight=12
)

# Set agent starting position
world.setAgentStart(15.0, 15.0)
```

### Advanced Agent Features

```python
# Create agent with world integration
agent = pynlm.createAgentBrainWithWorld(brain, world)

# Enable all subsystems at once (recommended for full functionality)
agent.enableAllSubsystems()

# Or enable subsystems individually
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

### Simulation Control

```python
# Run complete simulation with statistics
result = agent.simulateEpisode(maxSteps=1000)

print(f"Total reward: {result['total_reward']}")
print(f"Final curiosity: {result['final_curiosity']}")
print(f"Final novelty: {result['final_novelty']}")

# Run development simulation
agent.runDevelopmentSimulation(world, steps=500)
```

### State Management

```python
# Checkpoint management
brain.save("checkpoint1.bin")
brain.load("checkpoint2.bin")
brain.reset()
```

### Statistics and Debugging

```python
# Get comprehensive brain statistics
stats = brain.getBrainStats()

# Use simulation utilities
pynlm.runQuickIntegrationTest()
version = pynlm.getVersion()
```

## Part 5: Error Handling and Validation

The enhanced Python bindings include comprehensive error handling:

```python
import pynlm

try:
    # This will throw exceptions on errors
    config = pynlm.createConfigFromFile("nonexistent.json")
except ValueError as e:
    print(f"Configuration error: {e}")

try:
    brain = pynlm.createBrain(config)
except RuntimeError as e:
    print(f"Brain creation failed: {e}")
```

## Part 6: Performance and Best Practices

### Recommended Usage Patterns

```python
# Pattern 1: Basic simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Pattern 2: Complete agent simulation
brain = pynlm.createBrainWithDefaults()
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrainWithWorld(brain, world)
agent.enableAllSubsystems()

# Pattern 3: Configuration-driven simulation
config = pynlm.createConfigFromFile("my_config.json")
brain = pynlm.createBrain(config)
```

### Performance Tips

- Use `createBrainWithDefaults()` for quick testing
- Enable all subsystems at once with `enableAllSubsystems()`
- Use configuration files for different simulation scenarios
- Monitor statistics with `getBrainStats()`

## Part 7: Advanced Features for Power Users

### Subprocess and Script Integration

```python
import subprocess
import sys

# Use NLM from command line
subprocess.run([
    sys.executable, "-c",
    """
import pynlm
brain = pynlm.createBrainWithDefaults()
for i in range(100):
    brain.step(i)
print(f"Final stats: {brain.getTotalNeuronCount()} neurons")
    """
])
```

### Configuration Export

```python
# Export configuration to file
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.saveToFile("my_config.json")
```

## Part 8: Troubleshooting

### Common Issues and Solutions

**Issue 1: Configuration validation errors**
```python
# Solution: Validate configuration before use
if not config.validate():
    errors = config.getErrorMessages()
    print("Configuration errors:", errors)
```

**Issue 2: Brain initialization failure**
```python
# Solution: Use error handling
config = pynlm.createConfigFromFile("bad_config.json")
try:
    brain = pynlm.createBrain(config)
except RuntimeError as e:
    print(f"Brain initialization failed: {e}")
```

**Issue 3: Simulation crashes**
```python
# Solution: Use proper initialization sequence
brain = pynlm.createBrain(config)
brain.initialize()

# Or use enhanced method that handles initialization
brain = pynlm.createBrain(config)  # Automatically initializes
```

## Getting Started Examples

### Example 1: Basic Simulation

```python
import pynlm

# Simple simulation
brain = pynlm.createBrainWithDefaults()

for step in range(100):
    brain.step(step)
    if step % 25 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print("Basic simulation complete!")
```

### Example 2: Complete Agent Simulation

```python
import pynlm

# Complete agent simulation
brain = pynlm.createBrainWithDefaults()
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrainWithWorld(brain, world)
agent.enableAllSubsystems()

# Run simulation
result = agent.simulateEpisode(maxSteps=500)

print(f"Simulation complete!")
print(f"Total reward: {result['total_reward']}")
print(f"Final curiosity: {result['final_curiosity']}")
```

### Example 3: Configuration-Driven Simulation

```python
import pynlm
import json

# Create custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)

# Save configuration
config.saveToFile("custom_config.json")

# Load configuration
config = pynlm.createConfigFromFile("custom_config.json")
brain = pynlm.createBrain(config)

print(f"Loaded brain with {brain.getTotalNeuronCount()} neurons")
```

### Example 4: Development and Learning

```python
import pynlm

# Development-focused simulation
brain = pynlm.createBrainWithDefaults()
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrainWithWorld(brain, world)
agent.enableAllSubsystems()

# Run development simulation
agent.runDevelopmentSimulation(world, steps=300)

print("Development simulation complete!")
print(f"Final developmental stage: {agent.getDevelopmentalStage()}")
```

## Summary

The enhanced NLM Python bindings provide:

1. **Improved error handling** with comprehensive validation and exceptions
2. **Convenience methods** for common operations
3. **Advanced configuration** with file and argument loading
4. **Better integration** between brain, world, and agent components
5. **Enhanced statistics** and debugging capabilities
6. **Flexible configuration** for different use cases
7. **Development-focused features** for learning simulations
8. **Comprehensive documentation** with examples and troubleshooting

These improvements make NLM easier to use for beginners while providing advanced features for experienced users.
