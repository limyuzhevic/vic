# Enhanced Python Bindings for NLM

## Overview

This document provides an overview of the enhanced Python bindings for the Neural Learning Machine (NLM) framework. The bindings have been significantly improved to make them more Pythonic, following best practices and providing better usability for Python developers.

## Key Improvements

### 1. **Context Managers**
All major classes now support context management for proper resource handling:
- `Brain`: `with brain: ...`
- `SimpleWorld`: `with world: ...`
- `Config`: `with config: ...`

### 2. **Property Access**
Replaced traditional getters/setters with Pythonic property decorators:
- `config.value` instead of `config.get('value')`
- `agent_body.x` instead of `agent_body.getX()`
- `action.type` instead of `action.getType()`

### 3. **Custom Python Exceptions**
Added specific exception types for better error handling:
- `NLMError`: Base exception for all NLM errors
- `ConfigurationError`: For configuration-related errors
- `BrainError`: For brain-related errors
- `WorldError`: For world-related errors
- `SensoryInputError`: For sensory input errors
- `ActionError`: For action-related errors

### 4. **Convenience Methods**
Added helper methods for common operations:
- `config.get_or('key', default)` for safe value retrieval
- `config.set_value('key', value)` for type-specific setting
- `action.with_type(ActionType.MoveForward)` for fluent interface
- `action.with_parameters([1.0, 2.0])` for fluent interface

### 5. **Enhanced Documentation**
Improved docstrings with type information and usage examples:
- Comprehensive parameter descriptions
- Type hints where possible
- Usage examples in docstrings

### 6. **Iterator Support**
Added iteration capabilities:
- `InternalSignals`: Can iterate over signal values
- `Config`: Can iterate over keys
- Better integration with Python iteration protocols

### 7. **Copy Operations**
Implemented proper copy semantics:
- `brain.copy()` for creating brain copies
- `action.clone()` for creating action copies
- `sensory_input.copy()` for creating sensory input copies

### 8. **String Representation**
Enhanced `__str__` and `__repr__` methods:
- More informative string representations
- Pythonic formatting
- Better debugging support

## Enhanced Classes

### Config
- Properties: `config.value`, `config.has('key')`
- Convenience methods: `config.get_or()`, `config.set_value()`
- Dictionary conversion: `config.to_dict()`, `config.from_dict()`
- Context manager support

### SensoryInput (Base Class)
- Type checking: `sensory_input.ensure_type('Vision')`
- Copy operations: `sensory_input.copy()`
- Data access: `sensory_input.get_data()` (Pythonic alias)

### Vision
- Shape detection: `vision.get_shape()` returns `(width, height, channels)`
- Color space checks: `vision.is_rgb()`, `vision.is_grayscale()`
- Pixel operations: `vision.get_pixel_count()`
- Data as array: `vision.get_data_as_array()`

### Audio
- Duration calculation: `audio.get_duration()` returns seconds
- Sample rate access: `audio.get_sample_rate()` (Pythonic alias)
- Frequency range: `audio.get_frequency_range()`

### InternalSignals
- Iteration support: Can iterate over signal values
- Length detection: `len(signals)`
- Signal management: `signals.add_signal(value)`, `signals.clear_signals()`

### Action
- Fluent interface: `action.with_type(ActionType.MoveForward).with_parameters([1.0])`
- Parameter validation: `action.is_valid()`
- Parameter access: `action.get_parameter_value(index)`
- Dictionary conversion: `action.as_dict()`

### WorldObject
- Position operations: `world_object.distance_to(x, y)`
- Range checks: `world_object.is_within_range(x, y, range)`
- Point containment: `world_object.contains_point(x, y)`
- Type safety: `world_object.type` as typed property

### AgentBody
- Movement operations: `agent_body.move(dx, dy, dt)`
- Rotation operations: `agent_body.turn(angle)`
- Velocity calculations: `agent_body.get_velocity_magnitude()`
- Position access: `agent_body.get_position()` returns `(x, y)` tuple
- Distance calculation: `agent_body.get_distance_to(other_body)`

### ActionResult
- Fluent interface: `result.with_message('Success').with_reward(10.0)`
- Success validation: `result.is_successful()`, `result.is_failure()`
- Dictionary conversion: `result.as_dict()`
- Reward management: `result.add_reward(5.0)`

### SensoryPercept
- Data accessors: `percept.get_vision_data()`, `percept.get_touch_data()`, etc.
- Fluent interface: `percept.with_vision(vision).with_touch(touch)`
- Timestamp management: `percept.update_timestamp()`
- Dictionary conversion: `percept.as_dict()`

### SimpleWorld
- Context manager support
- Dimension access: `world.get_dimensions()` returns `(width, height)`
- Alias methods: `world.reset_world()`, `world.get_agent_position()`
- Agent state: `world.get_agent_state()` returns dictionary
- Command execution: `world.execute_command(cmd, time)`

### Brain
- Region management: `brain.get_region('name')` (placeholder)
- Configuration access: `brain.get_configuration()`
- Development system: `brain.get_developmental_stage()`, `brain.set_developmental_stage()`
- System access: `brain.get_spike_system()`, `brain.get_plasticity_system()`
- Memory systems: `brain.get_working_memory()`, `brain.get_episodic_memory()`
- State checking: `brain.is_initialized()` (placeholder)

### AgentBrain
- Configuration management: `agent_brain.enable_all_features()`, `agent_brain.disable_all_features()`
- Neuromodulation: `agent_brain.get_neuromodulation_state()`, `agent_brain.set_neuromodulation_state()`
- Development: `agent_brain.update_neuromodulation(reward, predicted_reward)`
- Motor decoding: `agent_brain.decode_motor_activity()` (alias)
- Sensory processing: `agent_brain.process_percept(percept)` (alias)

## Pythonic Features

### 1. Type Safety
- Enum values can be constructed from strings: `ActionType('MoveForward')`
- Type checking methods for validation
- Better error messages for type mismatches

### 2. Fluent Interface
Many classes support method chaining for more readable code:
```python
action = Action(ActionType.MoveForward).with_parameters([1.0, 2.0])
result = agent_brain.decode_motor_command().with_type(ActionType.Interact)
```

### 3. Dictionary Integration
- Config can be converted to/from Python dictionaries
- Action and ActionResult can be converted to dictionaries
- SensoryPercept supports dictionary conversion (with placeholder for full implementation)

### 4. Iteration and Comprehension Support
- InternalSignals can be iterated over
- Better integration with Python iteration patterns

### 5. Resource Management
- Context managers for automatic resource cleanup
- Better garbage collection support

## Backward Compatibility

The enhanced bindings maintain full backward compatibility with existing code:
- All original methods continue to work
- Original method signatures preserved
- No breaking changes to public API
- Existing code will work without modification

## Usage Examples

### Basic Usage
```python
import pynlm as nlm

# Create default config
config = nlm.createDefaultConfig()

# Create brain
brain = nlm.createBrain(config)

# Create world
world = nlm.createSimpleWorld()

# Create agent brain
agent_brain = nlm.createAgentBrain(brain)

# Initialize with world
agent_brain.initialize(world)

# Process sensory input
percept = world.getSensoryPercept()
agent_brain.processSensoryInput(percept)

# Decode motor command
action = agent_brain.decodeMotorCommand()

# Apply motor command in world
result = world.applyMotorCommand(action, world.getSimulationTime())
```

### Pythonic Features
```python
# Property access
config.set_value('max_steps', 1000)
print(config.get_or('max_steps', 100))

# Fluent interface
action = Action(ActionType.MoveForward).with_parameters([1.0, 0.5])

# Context manager
with brain:
    brain.initialize()
    brain.step(1)

# Dictionary conversion
brain_config = config.to_dict()
result_dict = action.as_dict()
```

### Error Handling
```python
try:
    config.loadFromFile('config.json')
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.NLMError as e:
    print(f"NLM error: {e}")
```

## Conclusion

The enhanced Python bindings for NLM provide a much more Pythonic and user-friendly interface while maintaining full backward compatibility. The improvements include:

1. **Context managers** for proper resource management
2. **Property access** for cleaner API
3. **Custom exceptions** for better error handling
4. **Convenience methods** for common operations
5. **Enhanced documentation** with type information
6. **Iterator support** for collections
7. **Copy operations** with proper semantics
8. **String representation** for better debugging

These enhancements make the NLM framework more accessible to Python developers and provide a more intuitive interface for building neural learning applications.