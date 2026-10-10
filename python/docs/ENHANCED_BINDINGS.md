# NLM Python Bindings - Enhanced Documentation

This document describes the enhanced Python bindings for the NLM (Neural Learning Machine) project. The bindings have been completely restructured to provide better organization, error handling, type safety, and documentation.

## Overview

The NLM Python bindings provide a Python interface to the NLM C++ neural simulation framework. The enhanced version maintains full backward compatibility while adding significant improvements:

- **Better Organization**: Split into multiple header files for maintainability
- **Enhanced Error Handling**: Custom Python exceptions for different error types
- **Type Safety**: Runtime type checking and validation
- **Comprehensive Documentation**: Detailed docstrings with examples
- **Advanced Features**: Helper functions and performance monitoring

## Installation

The bindings are built using scikit-build. See the main README.md for build instructions.

## Basic Usage

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain with configuration
brain = pynlm.createBrain(config)

# Create a simulation world
world = pynlm.createSimpleWorld()

# Create an agent brain interface
agent = pynlm.createAgentBrain(brain)

# Your code here...
```

## Enhanced Features

### 1. Better Error Handling

The enhanced bindings provide custom Python exceptions:

```python
import pynlm

config = pynlm.Config()

try:
    config.loadFromFile("/invalid/path.json")
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")

try:
    config.loadFromFile("")
except pynlm.ValidationError as e:
    print(f"Validation error: {e}")
```

**Available Exceptions:**
- `pynlm.ConfigError`: Configuration-related errors
- `pynlm.BrainError`: Brain-related errors
- `pynlm.WorldError`: World-related errors
- `pynlm.AgentError`: Agent-related errors
- `pynlm.ValidationError`: Parameter validation errors
- `pynlm.MemoryError`: Memory allocation errors

### 2. Type Safety and Validation

The bindings now include runtime type checking and validation:

```python
# Values are validated when set
neuron_id = pynlm.NeuronId(123)  # Automatically validated

# Vectors are bounds-checked
vision_data = [0.0] * (width * height * 3)  # Must be properly sized

# Parameters are validated
brain = pynlm.createBrain(config)  # Config is validated
```

### 3. Improved Type Bindings

All types now have comprehensive bindings with better documentation:

```python
# NeuronId with improved error handling
neuron_id = pynlm.NeuronId(123)
print(f"Neuron ID: {neuron_id}")
print(f"Value: {neuron_id.value}")

# Enums with better documentation
print(f"NeuronType: {pynlm.NeuronType.Excitatory}")
print(f"ActionType: {pynlm.ActionType.MoveForward}")
print(f"ActionType: {pynlm.ActionType.Interact}")

# Invalid ID constants (for reference)
invalid_neuron_id = pynlm.INVALID_NEURON_ID
invalid_synapse_id = pynlm.INVALID_SYNAPSE_ID
```

### 4. Enhanced Config Class

```python
config = pynlm.Config()

# Set configuration values
config.set("learning_rate", 0.01)
config.set("population_size", 100)
config.set("max_steps", 1000)

# Check if key exists
if config.has("learning_rate"):
    value = config.get("learning_rate")

# Get all keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")

# Load from file (with error handling)
try:
    config.loadFromFile("config.json")
    print("Configuration loaded successfully")
except pynlm.ConfigError as e:
    print(f"Failed to load configuration: {e}")

# Get summary
summary = config.summary()
print(f"Configuration summary: {summary}")
```

### 5. Enhanced Action and WorldObject

```python
# Create actions with parameters
action = pynlm.Action(pynlm.ActionType.MoveForward)
print(f"Action: {action}")
print(f"Type: {action.getType()}")
print(f"Name: {action.getName()}")

# Actions with parameters
custom_action = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])

# Action cloning
cloned_action = custom_action.clone()

# Create world objects
obj = pynlm.WorldObject(10.0, 20.0, pynlm.WorldObjectType.Resource, value=5.0)
print(f"World object: {obj}")
print(f"Type: {obj.type}, Value: {obj.value}")
print(f"Position: ({obj.x}, {obj.y})")

# Calculate distance
distance = obj.distanceTo(5.0, 5.0)
print(f"Distance to (5,5): {distance:.2f}")
```

### 6. AgentBrain Interface

```python
# Create agent brain
agent = pynlm.createAgentBrain(brain)

# Get sensory input size
input_size = agent.getSensoryInputSize()
output_size = agent.getMotorOutputSize()

# Process sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Decode motor command
action = agent.decodeMotorCommand()

# Apply reward modulation
agent.applyRewardModulation(1.0, 0.8)  # reward, predicted_reward

# Get development stage
stage = agent.getDevelopmentalStage()
print(f"Developmental stage: {stage}")

# Check feature flags
if agent.isRewardModulationEnabled():
    print("Reward modulation is enabled")

if agent.isStructuralPlasticityEnabled():
    print("Structural plasticity is enabled")
```

### 7. Helper Functions

```python
# Run an episode
reward = pynlm.runEpisode(agent, world, maxSteps=1000, timestep=0.1)
print(f"Episode completed with total reward: {reward}")

# Get system information
system_info = pynlm.getSystemInfo()
print(f"NLM Version: {system_info['version']}")
print(f"C++ Standard: {system_info['cpp_standard']}")
print(f"Description: {system_info['description']}")

# Performance statistics
stats = pynlm.getPerformanceStats(brain)
print(f"Performance metrics: {stats}")

# Benchmark episodes
avg_time = pynlm.benchmarkEpisode(agent, world, numEpisodes=10, timestep=0.1)
print(f"Average episode time: {avg_time:.3f}s")
```

### 8. Performance Monitoring

The bindings include performance monitoring utilities:

```python
# Get brain performance statistics
brain = pynlm.createBrain(config)
stats = pynlm.getPerformanceStats(brain)

# Performance metrics include:
print(f"Neurons: {stats.get('neurons', 'N/A')}")
print(f"Synapses: {stats.get('synapses', 'N/A')}")
print(f"Memory usage: {stats.get('memory_mb', 'N/A')} MB")
print(f"CPU time: {stats.get('cpu_time', 'N/A')} s")
```

### 9. Type Conversions and Helpers

```python
# Get position as tuple
position = world.getAgentPosition()
print(f"Agent position: {position}")  # (x, y)

# Get orientation in degrees
orientation = world.getAgentBody().orientation
orientation_degrees = world.getAgentBody().getOrientationDegrees()
print(f"Orientation: {orientation_degrees:.1f}°")

# Check if agent is alive
is_alive = world.isAgentAlive()
print(f"Agent is alive: {is_alive}")
```

## Complete Example

```python
#!/usr/bin/env python3
"""
Complete example showing all enhanced features
"""

import pynlm

def main():
    print("=== NLM Python Bindings Example ===\n")
    
    # 1. Create system components
    print("1. Creating system components...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    print(f"   ✓ Config: {config}")
    print(f"   ✓ Brain: {brain}")
    print(f"   ✓ World: {world}")
    print(f"   ✓ Agent: {agent}")
    
    # 2. Configure world
    print("\n2. Configuring world...")
    world.configure(100, 100, 21, 21)
    world.setAgentStart(50, 50)
    world.setMaxEnergy(100.0)
    world.setEnergyDecayRate(0.01)
    
    print(f"   ✓ World configured: {world.getWidth()}x{world.getHeight()} cells")
    print(f"   ✓ Agent start: {world.getAgentPosition()}")
    
    # 3. Run simulation episode
    print("\n3. Running simulation episode...")
    try:
        reward = pynlm.runEpisode(agent, world, maxSteps=500, timestep=0.1)
        print(f"   ✓ Episode completed with reward: {reward:.2f}")
    except pynlm.AgentError as e:
        print(f"   ✗ Agent error: {e}")
        return
    except pynlm.WorldError as e:
        print(f"   ✗ World error: {e}")
        return
    
    # 4. Get performance statistics
    print("\n4. Getting performance statistics...")
    system_info = pynlm.getSystemInfo()
    brain_stats = pynlm.getPerformanceStats(brain)
    
    print(f"   ✓ System: {system_info['description']}")
    print(f"   ✓ Version: {system_info['version']}")
    print(f"   ✓ Brain stats: {brain_stats}")
    
    # 5. Show development stage
    print("\n5. Development stage...")
    stage = agent.getDevelopmentalStage()
    print(f"   ✓ Current developmental stage: {stage}")
    
    # 6. Show feature status
    print("\n6. Feature status...")
    print(f"   ✓ Reward modulation: {'enabled' if agent.isRewardModulationEnabled() else 'disabled'}")
    print(f"   ✓ Structural plasticity: {'enabled' if agent.isStructuralPlasticityEnabled() else 'disabled'}")
    print(f"   ✓ Development: {'enabled' if agent.isDevelopmentEnabled() else 'disabled'}")
    print(f"   ✓ Curiosity: {'enabled' if agent.isCuriosityEnabled() else 'disabled'}")
    
    # 7. Show agent properties
    print("\n7. Agent properties...")
    body = world.getAgentBody()
    print(f"   ✓ Position: ({body.x:.1f}, {body.y:.1f})")
    print(f"   ✓ Energy: {body.energy:.1f}/{body.getSpeed():.2f} speed")
    print(f"   ✓ Health: {body.health}")
    print(f"   ✓ Age: {body.age}")
    print(f"   ✓ Alive: {'yes' if body.health > 0 else 'no'}")
    
    print("\n=== Example completed successfully ===")

if __name__ == "__main__":
    main()
```

## API Reference

### Module-level functions

- `pynlm.Config()` - Create a Config instance
- `pynlm.createDefaultConfig()` - Create a default Config
- `pynlm.createBrain(config)` - Create a Brain with configuration
- `pynlm.createSimpleWorld()` - Create a SimpleWorld
- `pynlm.createAgentBrain(brain)` - Create an AgentBrain
- `pynlm.runEpisode(agent, world, maxSteps, timestep)` - Run a simulation episode
- `pynlm.getSystemInfo()` - Get system information
- `pynlm.getPerformanceStats(brain)` - Get performance statistics
- `pynlm.benchmarkEpisode(agent, world, numEpisodes, timestep)` - Benchmark episodes

### Classes

- `pynlm.Config` - Configuration management
- `pynlm.Brain` - Neural simulation brain
- `pynlm.SimpleWorld` - 2D simulation world
- `pynlm.AgentBrain` - Agent brain interface
- `pynlm.SensoryInput` - Base sensory input class
- `pynlm.Vision` - Vision sensory input
- `pynlm.Audio` - Audio sensory input
- `pynlm.InternalSignals` - Internal signals sensory input
- `pynlm.Action` - Motor action representation
- `pynlm.WorldObject` - World object representation
- `pynlm.AgentBody` - Agent body state
- `pynlm.ActionResult` - Action result from world
- `pynlm.SensoryPercept` - Sensory percept data

### Enums

- `pynlm.NeuronType` - Neuron type enumeration
- `pynlm.SynapseType` - Synapse type enumeration
- `pynlm.DevelopmentalStage` - Developmental stage enumeration
- `pynlm.FiringState` - Neuron firing state enumeration
- `pynlm.ActionType` - Action type enumeration
- `pynlm.MotorCommand` - Motor command enumeration
- `pynlm.WorldObjectType` - World object type enumeration

### ID Types

- `pynlm.NeuronId` - Neuron identifier
- `pynlm.SynapseId` - Synapse identifier
- `pynlm.RegionId` - Region identifier
- `pynlm.PopulationId` - Population identifier

### Constants

- `pynlm.INVALID_NEURON_ID` - Invalid neuron ID sentinel
- `pynlm.INVALID_SYNAPSE_ID` - Invalid synapse ID sentinel
- `pynlm.INVALID_REGION_ID` - Invalid region ID sentinel
- `pynlm.INVALID_POPULATION_ID` - Invalid population ID sentinel

## Error Handling

All functions that can fail raise appropriate exceptions:

- `pynlm.ConfigError`: Configuration-related errors
- `pynlm.BrainError`: Brain-related errors
- `pynlm.WorldError`: World-related errors
- `pynlm.AgentError`: Agent-related errors
- `pynlm.ValidationError`: Parameter validation errors
- `pynlm.MemoryError`: Memory allocation errors

## Backward Compatibility

The enhanced bindings maintain full backward compatibility with the original API. All existing Python code should continue to work without modification. The changes are internal improvements:

- Public API remains unchanged
- All existing classes and methods work as before
- New functionality is additive
- Error handling is more robust but doesn't change the API

## Building and Testing

### Building

The bindings are built using scikit-build:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Testing

Run the test suite:

```bash
# Run Python tests
python tests/test_bindings.py

# Or run all tests in the tests directory
pytest
```

## Performance Considerations

- **Runtime Type Checking**: All parameters are validated at runtime for safety
- **Memory Safety**: Automatic bounds checking for arrays and vectors
- **Error Handling**: Exceptions provide detailed error messages for debugging
- **Performance Monitoring**: Built-in utilities for profiling and benchmarking

The enhanced bindings are designed to be both safe and performant, with validation only for critical operations that affect correctness.

## License

MIT License

## Copyright

NLM Authors
