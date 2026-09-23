# Python Bindings Documentation Guide

This document provides comprehensive documentation for the improved NLM Python bindings, including best practices, examples, and API usage patterns.

## Overview

The NLM (Neural Learning Machine) Python bindings provide a high-level interface to the C++ neural simulation framework. The bindings have been significantly improved with:

- **Enhanced Error Handling**: Custom exceptions and clear error messages
- **Memory Safety**: Proper reference counting and ownership management
- **Comprehensive API**: Complete coverage of NLM functionality
- **Pythonic Design**: Follows Python conventions and patterns
- **Extensive Documentation**: Detailed docstrings with examples

## Quick Start

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run for 100 simulation steps
for i in range(100):
    brain.step(i)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
```

## Core Classes

### Config Class

The `Config` class manages configuration for NLM simulations.

**Key Features:**
- Type-safe configuration with multiple data types
- Persistent storage and loading
- Validation and safety checks
- Runtime and persistent configuration sources

**Common Operations:**

```python
# Create configuration
config = pynlm.createDefaultConfig()

# Set values with type safety
config.set("neuron_count", 100)
config.set("region_count", 2)
config.set("connection_probability", 0.1)
config.set("learning_rate", 0.01)
config.set("plasticity_enabled", True)

# Get values with type safety
neuron_count = config.get("neuron_count").value  # Returns optional, use .value if exists
learning_rate = config.getOr("learning_rate", 0.001)  # Returns default if not found

# Check configuration
if config.has("neuron_count"):
    print(f"Neuron count: {config.get('neuron_count').value}")

# Save and load
config.saveToFile("my_config.cfg")
config.loadFromFile("other_config.cfg")

# Configuration validation
issues = pynlm.validateConfiguration(config)
if issues:
    print(f"Configuration issues: {issues}")
```

### Brain Class

The `Brain` class represents the central neural simulation system.

**Key Features:**
- Complete neural network simulation
- Multiple memory systems (working, episodic, associative)
- Prediction and cognition systems
- Neuromodulation and development
- Action production and sensory processing

**Basic Operations:**

```python
# Create and initialize brain
brain = pynlm.createBrain(config)
pynlm.initializeBrain(brain)  # Convenience function

# Simulation loop
for step in range(1000):
    brain.step(step)

# Get comprehensive statistics
stats = pynlm.getBrainStats(brain)
print(f"Neurons: {stats[0]}, Synapses: {stats[1]}, Active: {stats[2]}")

# Save and load
brain.save("brain_checkpoint.nlm")
brain.load("previous_brain.nlm")

# Get detailed status
pynlm.printBrainSummary(brain)
```

**Advanced Usage:**

```python
# Access internal systems (use with caution)
spike_system = brain.getSpikeSystem()
stdp = brain.getSTDP()
working_memory = brain.getWorkingMemory()
prediction_system = brain.getPredictionSystem()

# Regional management
region_id = brain.addRegion("SensoryCortex")
region = brain.getRegion(region_id)

# System controls
brain.updatePlasticity()
brain.develop()
brain.applyNeuromodulation(dopamine_signal)
```

### AgentBrain Class

The `AgentBrain` class connects the brain to the environment for agent-based simulation.

**Key Features:**
- Sensory processing and motor decoding
- Reward-based learning and neuromodulation
- Curiosity-driven exploration
- Development and adaptation

**Common Operations:**

```python
# Create agent
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)

# Processing loop
for step in range(100):
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Get neuromodulation levels
print(f"Neuromodulation: {agent.getNeuromodulationLevel():.2f}")
print(f"Curiosity: {agent.getCuriosityLevel():.2f}")
print(f"Novelty: {agent.getNoveltyLevel():.2f}")
print(f"Prediction Error: {agent.getPredictionError():.2f}")

# Enable features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
```

### SimpleWorld Class

The `SimpleWorld` class represents the simulated environment.

**Key Features:**
- 2D spatial simulation
- Sensory perception system
- Action execution and results
- Object management and collision detection

**Common Operations:**

```python
# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=100, height=100, visionWidth=16, visionHeight=16)
world.setAgentStart(50, 50)
world.setRandomSeed(42)

# Reset and run
world.reset()

# Simulation loop with agent
for step in range(100):
    world.update(0.1)  # Update physics
    
    # Get agent perception
    percept = world.getSensoryPercept()
    
    # Process with agent
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Agent decides action
    action = agent.decodeMotorCommand()
    
    # Execute action
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    if result.success:
        print(f"Step {step}: Action succeeded with reward {result.reward}")

# World management
world.addObject(pynlm.WorldObject(x=10, y=10, type=pynlm.WorldObjectType.Resource, value=10))
world.removeObject(10, 10)
```

### SensoryInput Classes

#### Vision

```python
# Create vision sensor
vision = pynlm.Vision(width=16, height=16, channels=1)  # Grayscale

# Set vision data (normalized 0.0-1.0)
data = [0.1, 0.5, 0.9, 0.2] * (16*16 // 4)  # Resize if needed
vision.setData(data)

print(f"Vision: {vision.getWidth()}x{vision.getHeight()} pixels")
```

#### Audio

```python
# Create audio sensor
audio = pynlm.Audio(sampleRate=44100, numSamples=1024)

# Generate simple test tone
audio_data = [0.5 * (i % 100) / 100.0 for i in range(1024)]
audio.setData(audio_data)
audio.setSampleRate(44100)

print(f"Audio: {audio.getSampleRate()} Hz, {audio.getNumSamples()} samples")
```

#### InternalSignals

```python
# Create internal signals
internal = pynlm.InternalSignals()
internal.addSignal(0.1)  # Homeostatic signal
internal.addSignal(-0.2)  # Stress signal
internal.clearSignals()
```

### Action and WorldObject Classes

```python
# Create actions
action = pynlm.Action(pynlm.MotorCommand.MoveForward)
action.setParameters([1.0, 2.0])  # Custom parameters

# Create world objects
resource = pynlm.WorldObject(x=10, y=10, type=pynlm.WorldObjectType.Resource, value=5.0)
hazard = pynlm.WorldObject(x=20, y=20, type=pynlm.WorldObjectType.Hazard, value=-2.0)
wall = pynlm.WorldObject(x=30, y=30, type=pynlm.WorldObjectType.Wall)

world.addObject(resource)
world.addObject(hazard)
world.addObject(wall)
```

## Convenience Functions

### High-Level Simulation

```python
# Run a complete simulation automatically
results = pynlm.runSimulation(brain, world, agent, steps=1000)
spikes, active_neurons, curiosity = results
print(f"Simulation completed: {spikes} spikes, {active_neurons} active neurons")
```

### Test Utilities

```python
# Create fully configured test components
test_brain = pynlm.createTestBrain()
test_world = pynlm.createTestWorld()
test_agent = pynlm.createTestAgent()

# Run a quick test simulation
for i in range(50):
    test_world.update(0.1)
    test_agent.processSensoryInput(test_world.getSensoryPercept())
    test_brain.step(i)
    action = test_agent.decodeMotorCommand()
    test_world.applyMotorCommand(action, test_world.getSimulationTime())

print(f"Test completed with {test_brain.getTotalSpikeCount()} spikes")
```

## Best Practices

### 1. Memory Management

```python
# Always use shared_ptr for Python objects
brain = pynlm.createBrain(config)  # Returns shared_ptr
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

# Objects will be automatically cleaned up
# No need for manual deletion in Python
```

### 2. Error Handling

```python
import pynlm

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    for i in range(100):
        brain.step(i)
except pynlm.NLMError as e:
    print(f"NLM Error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### 3. Configuration Validation

```python
# Always validate your configuration
config = pynlm.createDefaultConfig()
issues = pynlm.validateConfiguration(config)

if issues:
    print(f"Configuration has issues: {issues}")
    # Fix configuration
    config.set("neuron_count", 100)
    config.set("region_count", 2)
    
    # Re-validate
    issues = pynlm.validateConfiguration(config)
    if not issues:
        print("Configuration is now valid")
```

### 4. Performance Considerations

```python
# For long simulations, batch operations when possible
# Get all stats at once rather than individual calls
stats = pynlm.getBrainStats(brain)
neurons, synapses, active, firing, rate, eireq, spikes, pending = stats

# Use efficient data access
vision = world.getSensoryPercept()
vision_data = vision.getVision()  # Direct reference to avoid copy
```

### 5. Development Workflow

```python
# Development workflow pattern
def develop_brain(config=None):
    """Create and develop a brain over time"""
    if config is None:
        config = pynlm.createTestConfig()
    
    brain = pynlm.createBrain(config)
    pynlm.initializeBrain(brain)
    
    # Run development phase
    for stage in range(10):
        for step in range(100):
            brain.step(stage * 100 + step)
        
        # Observe behavior
        pynlm.printBrainSummary(brain)
    
    return brain

# Usage
developed_brain = develop_brain()
```

## API Reference

### Core Functions

| Function | Description | Returns |
|----------|-------------|---------|
| `createDefaultConfig()` | Create empty configuration | `Config` |
| `createBrain(config)` | Create brain with config | `Brain` |
| `createSimpleWorld()` | Create simple world | `SimpleWorld` |
| `createAgentBrain(brain)` | Create agent interface | `AgentBrain` |

### Statistics Functions

| Function | Description | Parameters |
|----------|-------------|------------|
| `getBrainStats(brain)` | Get comprehensive brain stats | `brain` |
| `printBrainSummary(brain)` | Print brain status | `brain` |
| `validateConfiguration(config)` | Validate configuration | `config` |

### Simulation Functions

| Function | Description | Parameters |
|----------|-------------|------------|
| `runSimulation(brain, world, agent, steps)` | Run complete simulation | `brain, world, agent, steps` |
| `initializeBrain(brain)` | Initialize brain | `brain` |
| `resetBrain(brain)` | Reset brain state | `brain` |

## Troubleshooting

### Common Issues

**Issue: Brain doesn't initialize**
- Solution: Check configuration validity with `validateConfiguration()`
- Solution: Ensure required keys are present

**Issue: Simulation crashes**
- Solution: Check for null pointers in convenience functions
- Solution: Use proper error handling with try/catch

**Issue: Memory usage high**
- Solution: Ensure you're not holding unnecessary references
- Solution: Use `del` to break reference cycles when done

**Issue: No output or behavior**
- Solution: Check if neurons are firing with `brain.getFiringNeuronCount()`
- Solution: Verify sensory input is connected

### Debugging Tips

```python
# Debug a simulation step by step
brain.initialize()
world.reset()
agent.initialize(world)

for step in range(10):
    print(f"\n--- Step {step} ---")
    world.update(0.1)
    
    # Check world state
    percept = world.getSensoryPercept()
    print(f"Vision size: {len(percept.getVision())}")
    
    # Process and think
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Check brain state
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    # Get action
    action = agent.decodeMotorCommand()
    print(f"Action: {action}")
    
    # Execute
    result = world.applyMotorCommand(action, world.getSimulationTime())
    print(f"Success: {result.success}, Reward: {result.reward}")
```

## Migration Guide

### From Basic to Advanced API

**Basic Pattern:**
```python
# Basic usage (original)
brain = pynlm.createBrain(config)
brain.initialize()
for i in range(100):
    brain.step(i)
```

**Advanced Pattern:**
```python
# Advanced usage (improved)
from pynlm import createDefaultConfig, createBrain, initializeBrain, getBrainStats

# Create with validation
config = createDefaultConfig()
config.set("neuron_count", 100)
config.set("learning_rate", 0.01)

brain = createBrain(config)
initializeBrain(brain)  # Enhanced safety

# Run and get stats
for i in range(1000):
    brain.step(i)

stats = getBrainStats(brain)  # Comprehensive stats
neurons, synapses, active, firing, rate, eireq, spikes, pending = stats
```

## Performance Notes

- **Simulation Speed**: For best performance, use larger timestep values
- **Memory Usage**: The bindings are memory-safe but can be heavy for large brains
- **CPU Usage**: Complex brains with many neurons will be computationally intensive
- **Threading**: The NLM framework is single-threaded; for parallel simulation, use multiple instances

## License and Credits

This documentation accompanies the improved NLM Python bindings. For additional examples and advanced usage patterns, refer to the main NLM documentation and the NLM Phase 6 integration materials.

## Version Information

- **Bindings Version**: 0.1.0
- **C++ Framework**: NLM Phase 6
- **Python Version**: 3.8+
- **Last Updated**: 2026

The improved Python bindings provide a robust, user-friendly interface to the NLM neural simulation framework, making it accessible to Python developers while maintaining the full power and complexity of the C++ implementation.