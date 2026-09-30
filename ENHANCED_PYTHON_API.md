# NLM (Neural Learning Machine) - Enhanced Python API Documentation

## Overview

This document describes the enhanced Python bindings for NLM (Neural Learning Machine), version 0.2.0. The bindings have been completely refactored to provide a Pythonic API that abstracts away C++ complexity while maintaining full access to the simulation capabilities.

## Key Improvements

### 1. Modular Binding Organization
- **Split from 425-line monolithic file** to 8 specialized binding files
- Each file focuses on a specific domain (Config, Sensory, Motor, World, Brain, Agent, etc.)
- Easier to maintain, extend, and understand

### 2. Pythonic API Design
- Context managers for resource management
- Fluent interface for method chaining
- Type-safe configuration access
- NumPy-like array interfaces for sensory data
- Dictionary serialization support

### 3. Enhanced Developer Experience
- Full type hints with pybind11
- Version information and compatibility checking
- Better error handling with custom exceptions
- Comprehensive documentation generation
- Quick start guides and examples

## Installation and Setup

### Prerequisites
```bash
# Install dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install from source
cd /path/to/nlm
pip install -e .

# Or build in development mode
pip install -e .[dev]
```

### Basic Verification
```python
import pynlm

# Check version
print(f"NLM Python bindings version: {pynlm.__version__}")

# Get version info
info = pynlm.get_version_info()
print(f"Description: {info['description']}")

# Quick start guide
pynlm.quick_start()
```

## Core API Reference

### 1. Configuration (EnhancedConfig)

The enhanced configuration provides type-safe access and fluent interface:

```python
from pynlm import EnhancedConfig

# Create configuration
config = EnhancedConfig()

# Set values with fluent interface
config.set("brain.neuron_count", 1000) \
    .set("brain.synapse_density", 0.1) \
    .set("random_seed", 42) \
    .set("simulation_timestep", 0.001)

# Type-safe getters with defaults
neuron_count = config.get("brain.neuron_count", 500)
connection_prob = config.get("brain.connection_probability", 0.05)
default_weight = config.get("brain.initial_weight_mean", 0.5)

# Check if key exists
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.get('brain.neuron_count')}")

# Get all keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")

# Context manager for temporary configs
temp_config = config.withKey("brain.neuron_count", 2000).copy()
```

### 2. Brain Simulation with Context Manager

```python
from pynlm import EnhancedConfig, BrainContext

# Create and initialize brain with context manager
config = EnhancedConfig()
with BrainContext(config) as brain_context:
    brain = brain_context.getBrain()
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    for step in range(100):
        brain_context.step(step, step * 0.001)
        if step % 10 == 0:
            status = brain_context.getStatus()
            print(f"Step {step}: {status['firing']} neurons firing")
```

### 3. World Simulation

```python
from pynlm import WorldContext, EnhancedAgentBrain

# Create world with context manager
with WorldContext() as world_context:
    world = world_context.getWorld()
    
    # Configure world
    world_context.configure(20, 20, 8, 8)
    world_context.setAgentStart(10.0, 10.0)
    
    # Create brain and agent
    config = EnhancedConfig()
    brain = EnhancedConfig().set("brain.neuron_count", 500).getCoreConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = EnhancedAgentBrain(brain)
    
    # Run episode with fluent interface
    agent.initialize(world)
    agent.enableAll()
    
    # Simulate using fluent interface
    for step in range(100):
        agent.stepWithInput(world, 0.1) \
             .applyOutput(world)
    
    print(f"Episode completed. Curiosity level: {agent.getCuriosityLevel():.3f}")
```

### 4. Enhanced Agent with Fluent API

```python
from pynlm import EnhancedAgentBrain

# Create agent with fluent interface
config = EnhancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = EnhancedAgentBrain(brain)

# Configure using fluent methods
agent.enableAll()  # Enable all subsystems at once

# Run simulation with fluent interface
world = pynlm.createSimpleWorld()
world.configure(15, 15, 8, 8)
world.reset()

agent.initialize(world)

for step in range(100):
    # Fluent interface makes simulation clean and readable
    agent.stepWithInput(world, 0.1) \
         .applyOutput(world)
    
    # Print progress
    if step % 20 == 0:
        stats = agent.getStats()
        print(f"Step {step}: {stats['firing']} neurons firing, "
              f"curiosity: {stats['curiosity']:.3f}")
```

### 5. Vision and Sensory Processing (EnhancedVision)

```python
from pynlm import EnhancedVision

# Create vision data
vision = EnhancedVision(8, 8, 3)
data = [0.1] * (8 * 8 * 3)  # Fill with test data
vision.setData(data)

# Access data with Pythonic properties
print(f"Vision shape: {vision.shape()}")
print(f"Width: {vision.width()}, Height: {vision.height()}, Channels: {vision.channels()}")

# Enhance data (example: normalization)
enhanced_vision = vision.normalize()

# Convert to NumPy-like array
array = vision.to_array()
print(f"Array shape: {array.shape()}")

# Dictionary serialization
vision_dict = vision.to_dict()
print(f"Vision as dict: {vision_dict}")

# Create from dictionary
restored_vision = EnhancedVision.from_dict(vision_dict)
```

### 6. Action System (EnhancedAction)

```python
from pynlm import ActionType, Action

# Create actions using factory methods
move_forward = Action.move_forward()
turn_left = Action.turn_left()
look = Action.look()
interact = Action.interact()

# Create actions with parameters
action = Action(ActionType.MoveForward, std::vector<float>{1.0f, 0.0f})

# Check action properties
print(f"Is movement? {action.is_movement()}")
print(f"Is rotation? {action.is_rotation()}")
print(f"Is interaction? {action.is_interaction()}")

# Convert to dictionary
action_dict = action.to_dict()
print(f"Action dict: {action_dict}")

# Create from dictionary
restored_action = Action.from_dict(action_dict)
```

### 7. World Objects

```python
from pynlm import WorldObject, WorldObjectType

# Create world objects
resource = WorldObject(5.0, 5.0, WorldObjectType.Resource, value=1.0, radius=0.5)
wall = WorldObject(10.0, 10.0, WorldObjectType.Wall, radius=1.0)
marker = WorldObject(15.0, 15.0, WorldObjectType.Marker)

# Access properties
print(f"Resource position: {resource.position()}")
print(f"Resource value: {resource.value}")
print(f"Resource type: {resource.type}")

# Modify properties
resource.set_position(6.0, 6.0)
resource.active = True

# Calculate distances
distance = resource.distance_to(wall)
print(f"Distance between resource and wall: {distance:.2f}")

# Check if near another object
if resource.is_near(wall, 2.0):
    print("Resource is near wall")
```

### 8. Quick Start Examples

#### Example 1: Simple Brain Simulation
```python
import pynlm

# Quick start using factory methods
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

#### Example 2: Complete Agent Simulation
```python
import pynlm

# Create complete setup using factory function
brain, world, agent, steps = pynlm.create_demo_agent(world_size=20, steps=100)

print(f"Starting demo simulation...")
print(f"World size: {world.getWidth()}x{world.getHeight()}")
print(f"Brain neurons: {brain.getTotalNeuronCount()}")

# Run the episode
agent.runEpisode(world, steps)

# Get final statistics
stats = agent.getStats()
print(f"\nSimulation complete!")
print(f"Final statistics:")
print(f"  Neurons firing: {stats['firing']}")
print(f"  Curiosity level: {stats['curiosity']:.3f}")
print(f"  Novelty level: {stats['novelty']:.3f}")
print(f"  Development stage: {stats['dev_stage']}")
```

#### Example 3: Advanced Sensor Processing
```python
import pynlm

# Create multiple sensory inputs
vision = EnhancedVision(16, 16, 3)
vision.setData([0.5] * (16 * 16 * 3))

# Normalize and enhance
vision_normalized = vision.normalize()
vision_resized = vision_resized.resize(8, 8)

# Create internal signals
internal = InternalSignals()
internal.add_multiple({0.1, 0.2, 0.3, 0.4, 0.5})

# Create percept
percept = SensoryPercept()
percept.setVision(vision.to_array())
percept.setInternal(internal.getSignals())
percept.setTimestamp(1.0)

print(f"Percept has vision: {percept.hasVision()}")
print(f"Percept vision shape: {percept.vision_shape()}")
print(f"Percept internal signals: {percept.getInternal()}")
```

## Advanced Features

### 1. Context Managers
All major resources now support context managers:

```python
from pynlm import BrainContext, WorldContext

# Brain context manager
with BrainContext(EnhancedConfig()) as brain_context:
    brain = brain_context.getBrain()
    brain.initialize()
    # Brain automatically resets when exiting context

# World context manager  
with WorldContext() as world_context:
    world = world_context.getWorld()
    # World automatically resets when exiting context
```

### 2. Fluent Interface
Method chaining makes code cleaner and more readable:

```python
config = EnhancedConfig() \
    .set("brain.neuron_count", 1000) \
    .set("brain.synapse_density", 0.1) \
    .set("random_seed", 42)

agent = EnhancedAgentBrain(brain) \
    .enableAll() \
    .withRewardModulation() \
    .withCuriosity()
```

### 3. Enhanced Error Handling
Better error messages and custom exceptions:

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.RuntimeError as e:
    print(f"Brain initialization failed: {e}")
except pynlm.ValueError as e:
    print(f"Invalid parameter: {e}")
except pynlm.IndexError as e:
    print(f"Index out of bounds: {e}")
```

### 4. Type Safety
Type hints and validation:

```python
# Configuration type safety
neuron_count = config.get("brain.neuron_count", 500)  # int64_t
connection_prob = config.get("brain.connection_probability", 0.05)  # double
synapse_density = config.get("brain.synapse_density", 0.1)  # double
random_seed = config.get("random_seed", 42)  # int64_t

# Vision type safety
vision = EnhancedVision(width, height, channels)
data_array = vision.to_array()  # py::array_t<float>
```

## Performance Tips

### 1. Memory Management
- Use context managers to automatically clean up resources
- Copy configurations only when necessary
- Reuse objects rather than creating new ones

### 2. Simulation Efficiency
- Use batch processing for multiple steps
- Monitor simulation statistics regularly
- Adjust simulation timestep for performance vs. accuracy

### 3. Memory Optimization
```python
# Reuse objects where possible
vision = EnhancedVision(8, 8, 3)
# Use same vision object for multiple simulation steps

# Clear unused data
internal_signals.clearSignals()
```

## Migration Guide

### From Original NLM Python API to Enhanced API

#### Old API
```python
# Original verbose API
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")
brain = pynlm.createBrain(config)
brain.initialize()

# Direct C++ style access
neuron_count = brain.getTotalNeuronCount()
spike_count = brain.getTotalSpikeCount()
```

#### New API
```python
# Enhanced API
config = EnhancedConfig() \
    .set("brain.neuron_count", 1000) \
    .set("brain.synapse_density", 0.1)

with BrainContext(config) as brain_context:
    brain = brain_context.getBrain()
    
    # Cleaner access
    stats = brain_context.getStatus()
    print(f"Neurons: {stats['neurons']}, Firing: {stats['firing']}")
```

## Troubleshooting

### Common Issues and Solutions

#### "ImportError: No module named 'pynlm'"
```bash
# Install with development dependencies
pip install -e .[dev]

# Or force reinstall
pip install --force-reinstall .
```

#### "Brain initialization failed"
```python
# Check configuration
config = EnhancedConfig()
print(config.summary())

# Try to fix common issues
config.set("brain.neuron_count", 1000)
config.set("brain.region_count", 1)
```

#### "Segmentation fault"
```python
# Always call initialize() before using brain
brain = pynlm.createBrain(config)
brain.initialize()  # Essential!

# Reset before re-running simulation
brain.reset()
```

## Further Reading

### Documentation
- `docs/ARCHITECTURE.md` - System architecture overview
- `docs/SCIENCE.md` - Scientific background and principles
- `docs/EXPERIMENTS.md` - Experiment descriptions and results

### Examples
- `examples/` - Additional example scripts
- `HOW_TO_USE.md` - Detailed usage instructions
- `easy_usage.md` - Beginner-friendly guide

### Development
- `python/bindings_new.cpp` - Main binding implementation
- `python/bindings.h` - Modular binding headers
- `CMakeLists.txt` - Build configuration

## Version Information

**Version**: 0.2.0
**Release Date**: Current
**License**: MIT
**Author**: NLM Authors
**Repository**: https://github.com/nlm-project/nlm

## Change Log

### Version 0.2.0 (Enhanced)
- Complete refactoring of Python bindings
- Modular binding organization
- Pythonic API with context managers
- Enhanced error handling and type safety
- Comprehensive documentation and examples
- Fluent interface and method chaining

### Version 0.1.0 (Original)
- Initial release with basic Python bindings
- Direct mapping of C++ API to Python
- Limited error handling
- Monolithic bindings structure

## Support

For issues, questions, or feature requests:
1. Check the documentation in `docs/`
2. Review examples in `examples/`
3. File bug reports in the repository
4. Consult `HOW_TO_USE.md` for detailed instructions

## Conclusion

The enhanced NLM Python bindings (version 0.2.0) provide a modern, Pythonic interface to the neural simulation framework. Key improvements include modular organization, context managers, fluent interfaces, type safety, and comprehensive error handling. The new API makes it easier to:

- Create and configure neural simulations
- Process sensory data with NumPy-like interfaces
- Manage simulation resources efficiently
- Build advanced agent behaviors
- Debug and monitor simulation performance

The enhanced bindings maintain full backward compatibility while providing significant improvements in developer experience and code maintainability.
