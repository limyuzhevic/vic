# NLM Python Bindings Documentation

## Overview

The NLM (Neural Learning Machine) Python bindings provide access to the full neural simulation framework through an intuitive, Pythonic interface. This document covers the enhanced API with comprehensive error handling, validation, and Pythonic features.

## Installation

```bash
pip install pynlm
```

## Quick Start

### Basic Usage

```python
import pynlm as nlm

# Create a basic neural simulation
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)

# Configure the world
world.configure(width=80, height=60, vision_width=16, vision_height=16)
world.setAgentStart(40.0, 30.0)

# Initialize the brain and agent
brain.initialize()
agent.initialize(world)

print("NLM simulation environment initialized successfully!")
print(f"Agent features enabled: {agent.getFeatureCount()}")
```

### Advanced Usage

```python
import pynlm as nlm
import math

# Create test environment
config, world, brain, agent = nlm.createTestEnvironment()

# Set up agent with all features enabled
agent.enableAllFeatures()

# Create sensory percept
sensor_data = nlm.SensoryPercept()

# Set up vision data (16x16 grid)
vision_data = [0.5] * (16 * 16)
sensor_data.setVision(vision_data)
sensor_data.setTimestamp(0.0)

# Process sensory input
agent.processSensoryInput(sensor_data)

# Get agent status
status = agent.getProcessingStatus()
print(f"Neuromodulation level: {status['neuromodulation_level']:.3f}")
print(f"Curiosity level: {status['curiosity_level']:.3f}")
print(f"Prediction error: {status['prediction_error']:.3f}")

# Get brain activity statistics
stats = brain.getActivityStats()
print(f"Active neurons: {stats['active_neurons']}")
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Total spikes: {stats['total_spikes']}")
```

## Core Classes

### Config

The `Config` class manages neural simulation parameters. The enhanced version includes:

- **Pythonic property access**: `config.get('key')`, `config.get_int('key')`, etc.
- **Type-safe setters**: Automatic type conversion
- **Dictionary serialization**: `config.to_dict()` and `config.from_dict()`
- **Enhanced validation**: Input validation and error handling

```python
import pynlm as nlm

# Create and configure config
config = nlm.createDefaultConfig()
config.set('neuron_count', 1000)
config.set('learning_rate', 0.01)
config.set('simulation_speed', 1.0)

# Access values with type safety
neuron_count = config.get_int('neuron_count')
learning_rate = config.get_float('learning_rate')
simulation_speed = config.get_float('simulation_speed')

# Convert to Python dictionary
settings = config.to_dict()
print(f"Settings: {settings}")

# Load from dictionary
new_config = nlm.createDefaultConfig()
new_config.from_dict(settings)
```

### Brain

The `Brain` class represents the central neural network. New features include:

- **Activity statistics**: `brain.getActivityStats()` returns comprehensive stats
- **Region management**: `brain.addRegion()`, `brain.getRegion()`, etc.
- **Enhanced error handling**: Validation of all inputs
- **Utility methods**: `brain.clearRegionActivity()`, `brain.getCurrentTime()`

```python
import pynlm as nlm

# Create brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Get comprehensive activity statistics
stats = brain.getActivityStats()
print("Brain Activity Statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")

# Add neural regions
brain.addRegion("sensor_region")
brain.addRegion("motor_region")

# Get region information
region_ids = brain.getRegionIds()
print(f"Regions: {region_ids}")
```

### AgentBrain

The `AgentBrain` class connects the brain to the world and handles sensor-motor integration:

- **Processing modes**: `strict`, `fast`, `curious`
- **Feature control**: Individual feature enable/disable
- **Enhanced validation**: Input validation and error handling
- **Status monitoring**: `agent.getProcessingStatus()`

```python
import pynlm as nlm

# Create agent brain
agent = nlm.createAgentBrain(brain)

# Set processing mode
agent.setProcessingMode("strict")  # Enable strict validation

# Or set to curious mode for exploration
agent.setProcessingMode("curious")

# Enable all features for maximum capability
agent.enableAllFeatures()

# Get comprehensive status
status = agent.getProcessingStatus()
print(f"Neuromodulation: {status['neuromodulation_level']:.3f}")
print(f"Curiosity: {status['curiosity_level']:.3f}")
print(f"Features enabled: {agent.getFeatureCount()}")
```

### SensoryPercept

The `SensoryPercept` class represents agent perceptions. Enhancements include:

- **Pixel-level access**: `sensor_data.getVisionPixel(x, y)`, `sensor_data.setVisionPixel(x, y, value)`
- **Validation**: Bounds checking and input validation
- **Convenience methods**: `getTouchCount()`, `getProprioceptionCount()`, etc.
- **Data normalization**: Chainable methods for data processing

```python
import pynlm as nlm

# Create sensory percept
sensor_data = nlm.SensoryPercept()

# Set vision data (normalized to [0, 1])
vision_data = [i / 255.0 for i in range(16 * 16)]  # Simple gradient
sensor_data.setVision(vision_data)

# Access pixel data with bounds checking
try:
    pixel_value = sensor_data.getVisionPixel(8, 8)
    print(f"Center pixel value: {pixel_value:.3f}")
except Exception as e:
    print(f"Error accessing pixel: {e}")

# Normalize vision data
sensor_data.normalizeVision()  # Normalizes to [0, 1] range

# Get sensory counts
touch_count = sensor_data.getTouchCount()
proprioception_count = sensor_data.getProprioceptionCount()
print(f"Touch sensors: {touch_count}, Proprioception sensors: {proprioception_count}")
```

### SimpleWorld

The `SimpleWorld` class represents the agent environment:

- **Object management**: `addResource()`, `addHazard()`
- **Position validation**: `isValidPosition()`
- **Distance utilities**: `distanceTo()`, `angleTo()`
- **Speed calculation**: `getSpeed()`
- **Enhanced error handling**: Input validation

```python
import pynlm as nlm

# Create world
world = nlm.createSimpleWorld()
world.configure(width=80, height=60, vision_width=16, vision_height=16)

# Add objects
world.addResource(10.0, 20.0, 100.0)  # Resource at (10, 20) with value 100
world.addHazard(70.0, 50.0, 50.0)     # Hazard at (70, 50) with value 50
world.addWall(40.0, 0.0, 5.0)         # Wall at (40, 0) with radius 5

# Check positions
agent_x, agent_y = 25.0, 25.0
if world.isValidPosition(agent_x, agent_y):
    distance = world.distanceTo(agent_x, agent_y)
    angle = world.angleTo(agent_x, agent_y)
    print(f"Position valid. Distance to target: {distance:.1f}, Angle: {angle:.2f} rad")
```

### Action

The `Action` class represents motor commands:

- **Type safety**: `action.isMovement()`, `action.isLookAction()`, `action.isInteraction()`
- **Parameter management**: Add, get, and set parameters
- **Chainable methods**: Return `self` for method chaining
- **Convenience methods**: Check action type categories

```python
import pynlm as nlm

# Create actions
action1 = nlm.Action(nlm.ActionType.MoveForward)
action2 = nlm.Action(nlm.ActionType.Look, [0.5, 0.0])  # Look with parameters

# Check action types
print(f"Is movement: {action1.isMovement()}")
print(f"Is look action: {action2.isLookAction()}")

# Work with parameters
action1.addParameter(1.5)
action1.addParameter(2.0)
action1.addParameter(3.0)

print(f"Action has parameters: {action1.hasParameters()}")
print(f"Parameter count: {action1.getParameterCount()}")

# Access parameters
for i in range(action1.getParameterCount()):
    param = action1.getParameter(i)
    print(f"Parameter {i}: {param}")
```

### Audio and Vision Processing

Enhanced audio and vision classes with processing utilities:

```python
import pynlm as nlm

# Vision processing
vision = nlm.Vision(32, 32, 3)
data = [0.1] * (32 * 32 * 3)

# Normalize vision data (chainable)
vision.setData(data).normalize()

# Audio processing
audio = nlm.Audio(44100, 44100)  # 1 second of audio
samples = [0.5] * 44100
audio.setData(samples).amplify(2.0)  # Double volume

# Apply gain in dB
audio.applyGain(6.0)  # Increase by 6 dB
```

## Error Handling

The enhanced bindings include comprehensive error handling:

### Common Exceptions

```python
import pynlm as nlm
import pynlm

try:
    config = nlm.createDefaultConfig()
    value = config.get('nonexistent_key')  # Will raise KeyError
except nlm.KeyError as e:
    print(f"Configuration key not found: {e}")

try:
    sensor_data = nlm.SensoryPercept()
    pixel = sensor_data.getVisionPixel(100, 100)  # Will raise IndexError
except nlm.IndexError as e:
    print(f"Pixel index out of bounds: {e}")

try:
    action = nlm.Action(nlm.ActionType.MoveForward)
    action.setType(999)  # Invalid enum value
except Exception as e:
    print(f"Invalid action type: {e}")
```

### Validation Functions

```python
# Validate configuration
try:
    config.validate()  # May be available through config
    print("Configuration is valid")
except Exception as e:
    print(f"Configuration validation failed: {e}")

# Validate action result
result = nlm.ActionResult(1.0, True, "Success")
try:
    result.validate()
    print("Action result is valid")
except Exception as e:
    print(f"Action result validation failed: {e}")
```

## Performance Monitoring

The system includes built-in monitoring capabilities:

```python
import pynlm as nlm

# Get system information
system_info = nlm.getSystemInfo()
print(f"Python version: {system_info['python_version']}")
print(f"Build type: {system_info['build_type']}")

# Validate environment
env_status = nlm.validateEnvironment()
print("Environment validation:")
for component, status in env_status.items():
    print(f"  {component}: {'✓' if status else '✗'}")
```

## Serialization

Convert configurations and data between Python and internal formats:

```python
import pynlm as nlm

# Configuration serialization
config = nlm.createDefaultConfig()
config.set('test_value', 42)
config.set('test_string', 'hello')

# To dictionary
settings = config.to_dict()
print(f"Serialized: {settings}")

# From dictionary
new_config = nlm.createDefaultConfig()
new_config.from_dict(settings)
print(f"Deserialized neuron count: {new_config.get_int('neuron_count')}")
```

## Advanced Features

### Custom Processing Modes

```python
import pynlm as nlm

agent = nlm.createAgentBrain(brain)

# Set custom processing modes
agent.setProcessingMode("strict")   # Strict validation
agent.setProcessingMode("fast")    # Fast execution  
agent.setProcessingMode("curious")  # Exploration-driven

# Disable all features for minimal mode
agent.disableAllFeatures()
```

### Data Processing Pipelines

```python
import pynlm as nlm
import numpy as np

# Create processing pipeline
vision = nlm.Vision(64, 64, 1)
audio = nlm.Audio(22050, 22050)

# Apply multiple processing steps
vision.setData(np.random.rand(64 * 64))
vision.normalize()  # Chainable method

# Create comprehensive sensory input
sensor_data = nlm.SensoryPercept()
sensor_data.setVision(vision.getData())
sensor_data.setTimestamp(0.0)
```

## Best Practices

### 1. Use Type-Safe Accessors
```python
# Good: Type-safe access
neuron_count = config.get_int('neuron_count')
learning_rate = config.get_float('learning_rate')

# Bad: Unvalidated access
try:
    value = config.get('key')  # May return unexpected type
except:
    pass
```

### 2. Validate Inputs
```python
# Always validate critical inputs
if not sensor_data.getVision():
    print("Warning: No vision data available")
    continue

# Check bounds before accessing data
if x < 0 or x >= width or y < 0 or y >= height:
    raise ValueError("Position out of bounds")
```

### 3. Handle Errors Gracefully
```python
try:
    result = brain.getRegion(region_id)
    if result:
        # Process region
        pass
except Exception as e:
    print(f"Error accessing region: {e}")
    # Fallback behavior
    pass
```

### 4. Use Convenience Methods
```python
# Use convenience methods when available
sensor_count = sensor_data.getTouchCount()
proprioception_count = sensor_data.getProprioceptionCount()

# Instead of manually counting
# touch_count = len(sensor_data.getTouch())
```

### 5. Leverage Chaining
```python
# Chain methods for cleaner code
agent.enableAllFeatures().setProcessingMode("strict")

# Process data in pipeline
vision.setData(data).normalize().smooth()
```

## Migration Guide

### From Old Version

If upgrading from the previous version, note these API changes:

1. **Property Access**: Use `config.get_int('key')` instead of `config.get('key')`
2. **Error Handling**: All methods now include validation and error handling
3. **Type Safety**: Added type-safe accessors and automatic conversion
4. **Convenience Methods**: New helper methods like `getVisionPixel()`, `distanceTo()`, etc.

```python
# Old way (may have missing validation)
value = config.get('key')

# New way (type-safe with validation)
value = config.get_int('key')

# Old way (manual indexing)
vision_data = sensor_data.getVision()
pixel = vision_data[y * width + x]

# New way (convenient access)
pixel = sensor_data.getVisionPixel(x, y)
```

## Troubleshooting

### Common Issues

1. **"Configuration key not found"**
   ```python
   if config.has('key'):
       value = config.get_int('key')
   ```

2. **"Invalid input data"**
   ```python
   vision = nlm.Vision(32, 32, 3)
   if len(data) == 32 * 32 * 3:
       vision.setData(data)
   ```

3. **"Null reference"**
   ```python
   if brain:
       brain.initialize()
   ```

4. **"Index out of bounds"**
   ```python
   if x < width and y < height:
       value = sensor_data.getVisionPixel(x, y)
   ```

## Examples Directory

For additional examples and use cases, see the examples directory:

- `examples/basic_usage.py` - Basic setup and configuration
- `examples/advanced_processing.py` - Advanced sensor processing
- `examples/error_handling.py` - Comprehensive error handling
- `examples/performance_monitoring.py` - System monitoring
- `examples/serialization.py` - Data serialization

## Support

For issues and questions:
1. Check the [NLM Documentation](https://nlm.readthedocs.io)
2. Visit the [NLM GitHub Repository](https://github.com/nlm-project/nlm)
3. Check the [Issues Tracker](https://github.com/nlm-project/nlm/issues)

## License

MIT License - See LICENSE file for details.
