# Python Bindings - Enhanced API Documentation

## Overview

This document describes the enhanced Python API for NLM (Neural Learning Machine) neural simulation framework. The enhanced bindings provide comprehensive Pythonic features while maintaining full backward compatibility with the original API.

## New Pythonic Features

### 1. Core Types with Pythonic Properties

#### NeuronId
```python
# Creation
neuron_id = pynlm.NeuronId(42)

# Pythonic properties
print(neuron_id.value)      # Access value
print(neuron_id.index())    # Get index
print(neuron_id.is_valid)   # Check if valid ID

# String representation
print(repr(neuron_id))      # <NeuronId: 42>
```

#### SynapseId, RegionId, PopulationId
Similar Pythonic properties as NeuronId.

### 2. Enumerations with Helper Methods

#### NeuronType
```python
# Enumeration values
neuron_type = pynlm.NeuronType.Excitatory

# Pythonic helper methods
print(neuron_type.is_excitatory_or_inhibitory)  # True for Excitatory/Inhibitory
print(neuron_type.is_sensory_or_motor)          # True for Sensory/Motor
```

#### SynapseType
```python
print(synapse_type.is_significant)  # True for Excitatory/Inhibitory/Modulatory
```

#### DevelopmentalStage
```python
print(stage.is_developing)  # True for Initial/CriticalPeriod/Maturation
print(stage.is_mature)      # True for Adult/Aging
```

#### ActionType
```python
print(action_type.is_movement)  # True for movement actions
print(action_type.is_visual)    # True for visual actions
```

### 3. Configuration System with Fluent Interface

#### Enhanced Config Class
```python
# Create config with defaults
config = pynlm.create_config_with_defaults()

# Fluent interface
config.set("brain.region_count", 1)
config.set("simulation.max_steps", 1000)

# Safe getters with defaults
value = config.safe_get("some.key", "default")
int_value = config.safe_get_int("int.key", 42)
bool_value = config.safe_get_bool("bool.key", True)

# Serialization
summary = config.summary()
config_str = pynlm.serialize_config(config)
```

#### Helper Functions
```python
# Load from JSON
config = pynlm.load_config_from_json("config.json")

# Factory functions
config = pynlm.create_default_config()
config = pynlm.create_config_with_defaults()
```

### 4. Context Managers for Resource Management

#### Brain Context Manager
```python
with pynlm.brain_context(brain, auto_save=True, checkpoint_path="brain.json") as ctx:
    brain = ctx.get_brain()
    # ... work with brain
    elapsed = ctx.get_elapsed_time()  # Time elapsed in context
```

#### World Context Manager
```python
with pynlm.world_context(world, reset_on_exit=True) as ctx:
    world = ctx.get_world()
    # ... work with world
```

#### Config Context Manager
```python
with pynlm.config_context(config, auto_save=True, backup_path="config_backup.json") as ctx:
    config = ctx.get_config()
    # ... work with config
```

### 5. Action Class with Pythonic Properties

#### Action Properties
```python
action = pynlm.Action()

# Pythonic property access
action.type = pynlm.ActionType.MoveForward
action.parameters = [0.5, 0.5]

print(action.type)          # pynlm.ActionType.MoveForward
print(action.parameters)    # [0.5, 0.5]

# Pythonic helper properties
print(action.is_movement)   # True for movement actions
print(action.is_visual)     # True for visual actions

# String representation
print(repr(action))         # <Action: move_forward parameters=2>
```

### 6. WorldObject Class with Pythonic Properties

#### WorldObject Properties
```python
obj = pynlm.WorldObject(1.0, 2.0, pynlm.WorldObjectType.Resource, 5.0)

# Pythonic properties
print(obj.is_resource)      # True for resource objects
print(obj.is_hazard)        # True for hazard objects
print(obj.is_passable)      # True for passable objects

# Distance calculation
distance = obj.distance_to(4.0, 6.0)
print(f"Distance: {distance}")  # 5.0
```

### 7. SensoryPercept Class with Pythonic Properties

#### SensoryPercept Properties
```python
percept = pynlm.SensoryPercept()
percept.setVision([0.1, 0.2, 0.3, 0.4])

# Pythonic properties
print(percept.has_vision)                   # True if vision data exists
print(percept.vision_shape)                 # (2, 2) for 4 elements

# Data utilities
normalized = percept.get_normalized_vision()  # [0.0, 0.33, 0.67, 1.0]
```

### 8. Data Utilities

#### Vector Operations
```python
# Normalize vector
vec = [1.0, 2.0, 3.0, 4.0, 5.0]
normalized = pynlm.normalize_vector(vec)

# Vector statistics
stats = pynlm.vector_stats(vec)
print(f"Mean: {stats['mean']}, Std: {stats['std']}")

# Convert numpy array
import numpy as np
arr = np.array([1.0, 2.0, 3.0])
vec = pynlm.to_fvec(arr)
```

### 9. Custom Exception Handling

#### NLM Exception Hierarchy
```python
try:
    # Code that might raise exceptions
    pass
except pynlm.NLMException as e:
    print(f"NLM error: {e}")
except pynlm.ConfigError as e:
    print(f"Config error: {e}")
except pynlm.BrainError as e:
    print(f"Brain error: {e}")
except pynlm.WorldError as e:
    print(f"World error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
```

### 10. Simulation Helpers

#### Simulation Runner
```python
runner = pynlm.SimulationRunner(brain, world, agent, max_steps=1000)
action_history, time_history = runner.run()
print(f"Steps completed: {runner.get_steps_completed()}")
print(f"Total reward: {runner.get_total_reward()}")
```

## API Reference

### Module-level Functions

#### Factory Functions
- `create_default_config()`: Create default configuration
- `create_brain(config)`: Create brain with configuration
- `create_simple_world()`: Create world
- `create_agent_brain(brain)`: Create agent brain interface

#### Helper Functions
- `create_config_with_defaults()`: Create config with sensible defaults
- `load_config_from_json(filepath)`: Load config from JSON file
- `create_world()`: Create world helper
- `configure_world(world, width, height, vision_width, vision_height)`: Configure world
- `add_resource_to_world(world, x, y, value)`: Add resource to world
- `add_hazard_to_world(world, x, y, value)`: Add hazard to world
- `add_wall_to_world(world, x, y, radius)`: Add wall to world
- `normalize_vector(vector)`: Normalize vector to [0,1] range
- `vector_stats(vector)`: Calculate vector statistics
- `to_fvec(array)`: Convert numpy array to vector
- `serialize_config(config)`: Serialize config to string
- `debug_percept(percept)`: Debug percept string

### Context Manager Classes

#### BrainContext
- `__init__(brain, auto_save=False, checkpoint_path="")`
- `get_brain()`: Get brain
- `get_elapsed_time()`: Get elapsed time
- `set_auto_save()`: Set auto-save flag

#### WorldContext
- `__init__(world, reset_on_exit=True)`
- `get_world()`: Get world
- `set_reset_on_exit()`: Set reset flag

#### ConfigContext
- `__init__(config, auto_save=False, backup_path="")`
- `get_config()`: Get config
- `set_auto_save()`: Set auto-save flag

### Simulation Classes

#### SimulationRunner
- `__init__(brain, world, agent, max_steps=1000)`
- `run()`: Run simulation
- `get_steps_completed()`: Get completed steps
- `get_total_reward()`: Get total reward

## Migration Guide

### From Old API to New API

#### 1. Factory Functions
**Old:**
```python
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
```

**New:**
```python
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
world = pynlm.create_simple_world()
agent = pynlm.create_agent_brain(brain)
```

#### 2. Configuration Access
**Old:**
```python
value = config.get("key")
```

**New:**
```python
value = config.safe_get("key", "default")
```

#### 3. Error Handling
**Old:**
```python
except Exception as e:
    print(f"Error: {e}")
```

**New:**
```python
except pynlm.ConfigError as e:
    print(f"Config error: {e}")
except pynlm.BrainError as e:
    print(f"Brain error: {e}")
```

## Examples

### Example 1: Basic Simulation
```python
import pynlm

# Create simulation components
config = pynlm.create_config_with_defaults()
brain = pynlm.create_brain(config)
world = pynlm.create_world()
world.configure(20.0, 20.0, 8, 8)
world.setAgentStart(10.0, 10.0)
agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Run simulation
with pynlm.brain_context(brain) as ctx:
    for step in range(100):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, float(step))
        world.update(0.1)

print(f"Simulation completed! Final position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
```

### Example 2: Using Pythonic Properties
```python
import pynlm

# Create objects with Pythonic properties
neuron_id = pynlm.NeuronId(42)
print(f"Neuron is valid: {neuron_id.is_valid}")

action = pynlm.Action(pynlm.ActionType.MoveForward, [0.5, 0.5])
print(f"Action is movement: {action.is_movement}")
print(f"Action is visual: {action.is_visual}")

world_obj = pynlm.WorldObject(1.0, 2.0, pynlm.WorldObjectType.Resource, 5.0)
print(f"World object is resource: {world_obj.is_resource}")
print(f"World object is passable: {world_obj.is_passable}")
```

### Example 3: Context Manager for Resource Management
```python
import pynlm

config = pynlm.create_config_with_defaults()
brain = pynlm.create_brain(config)

# Use context manager for automatic resource management
with pynlm.brain_context(brain, auto_save=True, checkpoint_path="brain_checkpoint.json"):
    # Simulate something
    for step in range(50):
        brain.step(step)
    
    # Context manager automatically saves checkpoint

print("Simulation completed with checkpoint saved")
```

## Backward Compatibility

### What Changed
1. **Function Names**: CamelCase → snake_case (e.g., `createDefaultConfig` → `create_default_config`)
2. **New Pythonic Features**: Added properties, helpers, context managers
3. **Enhanced Error Handling**: Custom exception hierarchy
4. **Data Utilities**: Vector operations, normalization, statistics

### What Remains Unchanged
1. **Core Types**: All original types and methods preserved
2. **Original Factory Functions**: Kept for backward compatibility
3. **Global Constants**: `INVALID_NEURON_ID`, etc. still available
4. **Basic API**: Core functionality preserved with enhanced features

### Migration Path
- **Immediate**: Existing code continues to work unchanged
- **Gradual**: Adopt new Pythonic features as needed
- **Future**: Eventually transition to new API for enhanced functionality

## Testing

Run the test suite:
```bash
pytest python/tests/test_bindings_comprehensive.py -v
```

Individual test modules:
- `test_bindings_comprehensive.py`: Comprehensive test suite
- `demo.py`: Quick demonstration script

## Building

The enhanced bindings are built automatically when installing the package:
```bash
pip install .
```

Or build manually:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Performance Considerations

1. **Context Managers**: Minimal overhead, only adds try/finally blocks
2. **Pythonic Properties**: Generated by pybind11, no runtime cost
3. **Helper Functions**: Minimal overhead, focused utilities
4. **Backward Compatibility**: Original API paths preserved

## Future Enhancements

1. **Advanced Simulation Helpers**: More sophisticated simulation utilities
2. **Performance Monitoring**: Integration with profiling tools
3. **Advanced Data Visualization**: Built-in plotting capabilities
4. **Machine Learning Integration**: Integration with ML libraries

## License

MIT License. See LICENSE file for details.

## Authors

Enhanced Python bindings by Kilo team. Original NLM authors preserved.
