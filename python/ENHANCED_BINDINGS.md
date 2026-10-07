# Enhanced Python Bindings for NLM

This document provides comprehensive documentation for the enhanced Python bindings for the NLM (Neural Learning Machine) framework.

## Overview

The enhanced Python bindings provide a Pythonic interface to the NLM C++ neural simulation framework, featuring:

- **Better error handling** with custom Python exceptions
- **Convenience methods** for common operations
- **Utility functions** for creating and managing NLM components
- **Type checking and validation** for robust development
- **Advanced features** for experienced users
- **Comprehensive test coverage** for reliable usage

## Quick Start

### Basic Usage

```python
import pynlm

# Create a default configuration with typical settings
config = pynlm.createDefaultConfig()

# Create a brain with the configuration
brain = pynlm.createBrain(config)

# Create a simple world with specified dimensions
world = pynlm.createSimpleWorldConfigured(100, 100, 16, 16)

# Create and initialize an agent brain
agent = pynlm.createAgentBrainWithWorld(brain, world)
```

### Advanced Usage

```python
# Create a configuration with specific capacity
config = pynlm.Brain.createDefaultConfigWithCapacity(1000)

# Run a simulation episode
reward = agent.runEpisode(world, sensory_inputs)

# Apply reward modulation
agent.applyRewardModulation(reward, predicted_reward)

# Get brain statistics
total_neurons = brain.getTotalNeuronCount()
excitation_inhibition_ratio = brain.getExcitationInhibitionRatio()
```

## Enhanced Classes

### Config Class

The `Config` class now has enhanced methods for better configuration management:

#### New Methods:

1. **`validate()`** - Validates configuration and returns True if valid
2. **`getType(key)`** - Gets the type of a configuration value
3. **`clone()`** - Creates a copy of the configuration
4. **`get(key, create_if_missing=False)`** - Enhanced get method
5. **`getOr(key, default)`** - Multiple overloads for different types
6. **`set(key, value)`** - Multiple overloads for different types

#### Example:

```python
# Create and configure config
config = pynlm.Config()
config.set("simulation.steps", 100)
config.set("brain.memory.enabled", True)
config.set("brain.plasticity.enabled", True)

# Validate config
if config.validate():
    print("Configuration is valid")

# Clone config
config2 = config.clone()

# Get with default
steps = config.getOr("simulation.steps", 50)
```

### Vision Class

The `Vision` class has enhanced visualization capabilities:

#### New Methods:

1. **`setDataNormalized(data, min_val, max_val)`** - Sets data with automatic normalization
2. **`getDataAsArray()`** - Gets data as Python list
3. **`getPixel(x, y, c)`** - Gets specific pixel value with bounds checking
4. **`getArrayDimensions()`** - Gets dimensions as tuple
5. **`resize(new_width, new_height)`** - Placeholder for resizing

#### Example:

```python
# Create vision with dimensions
vision = pynlm.Vision(10, 10, 3)

# Set data with normalization
vision.setDataNormalized([0.5, 0.7, 0.3, 0.9], 0.0, 1.0)

# Access pixel values
pixel_value = vision.getPixel(0, 0, 0)

# Get dimensions as tuple
width, height, channels = vision.getArrayDimensions()
```

### Brain Class

The `Brain` class has enhanced factory methods and functionality:

#### New Methods:

1. **`createDefaultConfig()`** - Creates config with typical settings
2. **`createDefaultConfigWithCapacity(neuron_capacity)`** - Creates config with specified capacity
3. **`createBrainFromConfigFile(filepath)`** - Creates brain from file
4. **`stepAdvanced(current_step, current_time, input_data)`** - Advanced step with input
5. **`getRegionOrCreate(name)`** - Get region or create if doesn't exist

#### Example:

```python
# Create brain with typical settings
brain = pynlm.Brain.createDefaultConfig()

# Create brain with specific capacity
brain = pynlm.Brain.createDefaultConfigWithCapacity(1000)

# Create brain from file
brain = pynlm.Brain.createBrainFromConfigFile("config.json")

# Advanced step with input
brain.stepAdvanced(step, time, input_data)
```

### AgentBrain Class

The `AgentBrain` class has enhanced methods for agent-based simulations:

#### New Methods:

1. **`runEpisode(world, sensory_inputs, max_reward)`** - Run an episode and return reward
2. **`simulateEpisode(world, steps)`** - Simulate episode with random input

#### Example:

```python
# Run episode with given sensory inputs
reward = agent.runEpisode(world, sensory_inputs, max_reward=100.0)

# Simulate episode with random input
reward = agent.simulateEpisode(world, 50)
```

## Utility Functions

The enhanced bindings include several utility functions:

### Factory Functions:

1. **`createSensoryPercept()`** - Creates empty sensory percept
2. **`createVision(width, height, channels)`** - Creates vision with dimensions
3. **`createAudio(sample_rate, num_samples)`** - Creates audio with parameters
4. **`createAction(type, parameters)`** - Creates action with optional parameters

### Query Functions:

1. **`getNeuronTypes()`** - Returns list of neuron types
2. **`getSynapseTypes()`** - Returns list of synapse types
3. **`getActionTypes()`** - Returns list of action types
4. **`actionTypeToString(type)`** - Converts ActionType enum to string

### Advanced Functions:

1. **`generateRandomConfig(gen)`** - Generates random configuration
2. **`createDefaultConfig()`** - Creates default configuration
3. **`createSimpleWorldConfigured(width, height, vision_width, vision_height)`** - Creates configured world

## Error Handling

The enhanced bindings provide better error handling with custom Python exceptions:

### Exception Types:

1. **`NLMError`** - Base exception for all NLM errors
2. **`ConfigError`** - Configuration-specific errors
3. **`BrainError`** - Brain-specific errors
4. **`AgentError`** - Agent-specific errors
5. **`VisionError`** - Vision-specific errors

### Example:

```python
import pynlm

try:
    config = pynlm.Config()
    config.set("invalid.key", "value")
    
    # This may raise ConfigError
    if not config.validate():
        raise pynlm.ConfigError("Invalid configuration")
        
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
except pynlm.NLMError as e:
    print(f"NLM error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Testing

The enhanced bindings include comprehensive test coverage:

### Running Tests

```bash
# Run comprehensive test suite
python python/test_pynlm.py

# Run quick test
python python/test_quick.py
```

### Test Categories:

1. **Basic Imports** - Tests all imports and basic functionality
2. **Config Class** - Tests enhanced Config methods
3. **Brain Class** - Tests brain creation and configuration
4. **Vision Class** - Tests enhanced Vision methods
5. **SensoryPercept Class** - Tests sensory perception
6. **Action Class** - Tests action creation and management
7. **SimpleWorld Class** - Tests world simulation
8. **AgentBrain Class** - Tests agent brain functionality
9. **Utility Functions** - Tests utility functions
10. **Error Handling** - Tests error handling
11. **Advanced Features** - Tests advanced simulation features

## Performance Considerations

### Memory Management:

- Use `clone()` method to create copies of objects
- Dispose of unused objects to free memory
- Monitor memory usage during long simulations

### Simulation Performance:

- Use `createDefaultConfigWithCapacity()` for better memory allocation
- Batch sensory inputs when possible
- Use `runEpisode()` for efficient episode management
- Monitor brain statistics for performance tuning

### Best Practices:

1. **Create objects once** and reuse them when possible
2. **Use appropriate capacities** for your use case
3. **Validate configurations** before simulation
4. **Monitor performance** during simulation
5. **Use utility functions** for common operations

## Advanced Features for Experienced Users

### Custom Configuration:

```python
# Create custom configuration
config = pynlm.Config()
config.set("simulation.dt", 0.005)
config.set("brain.memory.enabled", True)
config.set("brain.development.enabled", True)
config.set("brain.curiosity.enabled", True)
```

### Custom Brain Initialization:

```python
# Initialize brain with custom configuration
brain = pynlm.Brain(config)
brain.initialize()

# Add custom regions
region_id = brain.addRegion("custom_region")
```

### Custom Agent Behaviors:

```python
# Custom reward modulation
agent.applyRewardModulation(reward, predicted_reward)

# Custom development updates
agent.updateDevelopment(timestep)

# Custom curiosity
agent.setCuriosityEnabled(True)
```

## Migration Guide

### From Basic to Enhanced Bindings:

1. **Configuration** - Use `createDefaultConfig()` instead of manual creation
2. **Brain Creation** - Use `createBrain()` instead of manual constructor
3. **Error Handling** - Use custom exceptions instead of generic ones
4. **Utility Functions** - Use `createVision()`, `createAction()`, etc.
5. **Testing** - Use enhanced test suite instead of manual testing

## Troubleshooting

### Common Issues and Solutions:

1. **Import Errors**:
   ```
   Error: cannot import name 'VisionError'
   ```
   **Solution**: Ensure the bindings are properly built and reinstalled.

2. **Memory Issues**:
   ```
   Error: memory exhausted during simulation
   ```
   **Solution**: Use smaller configurations or increase capacity appropriately.

3. **Type Errors**:
   ```
   Error: argument type mismatch for 'setData'
   ```
   **Solution**: Ensure data types match expected types (float for Vision, etc.).

4. **Performance Issues**:
   ```
   Slow simulation performance
   ```
   **Solution**: Check configuration, use appropriate capacities, optimize input data.

## License and Attribution

This enhanced Python bindings implementation is part of the NLM (Neural Learning Machine) project.
The enhanced bindings provide improved usability and reliability for Python users of the NLM framework.

## Acknowledgements

This implementation builds upon the original NLM framework and enhances its Python bindings
with better error handling, convenience methods, and comprehensive testing.

---

*For more detailed information, please refer to the original NLM documentation and the C++ framework documentation.*

## Quick Reference

### Essential Functions:

```python
# Core creation
pynlm.createDefaultConfig()
pynlm.createBrain(config)
pynlm.createSimpleWorld()
pynlm.createAgentBrain(brain)

# Enhanced vision
pynlm.createVision(width, height, channels)
vision.setDataNormalized(data, min, max)

# Enhanced actions
pynlm.createAction(ActionType.MoveForward)
pynlm.createActionWithParams(ActionType.Custom, params)

# Query functions
pynlm.getNeuronTypes()
pynlm.getSynapseTypes()
pynlm.getActionTypes()
```

### Exception Types:

```python
pynlm.NLMError
pynlm.ConfigError
pynlm.BrainError
pynlm.AgentError
pynlm.VisionError
```

This enhanced Python bindings implementation provides a robust, user-friendly interface to the NLM framework,
catering to both beginners and experienced users with comprehensive features and extensive testing.