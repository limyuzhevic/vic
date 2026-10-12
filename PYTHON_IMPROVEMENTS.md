# NLM Python Bindings Improvements

## Overview

This document provides comprehensive documentation for the improvements made to the NLM (Neural Learning Machine) Python bindings. The enhancements include modern Python API design, enhanced error handling, improved configuration management, and comprehensive documentation.

## Key Improvements

### 1. Enhanced Python API Design

#### Context Managers for Resource Management

The enhanced bindings introduce context managers for automatic resource cleanup:

```python
import pynlm as nlm

# Context manager for brain simulation
brain = nlm.create_brain(config)
try:
    brain.initialize()
    # Run simulation
    brain.step(0)
    brain.step(1)
finally:
    brain.reset()

# Context manager with automatic checkpointing
brain = nlm.create_brain(config)
with brain.make_context(save_on_exit=True, checkpoint_path="brain_checkpoint.bin"):
    # Run simulation with automatic checkpointing
    brain.initialize()
    # ... simulation code
    # Checkpoint automatically saved on exit
```

#### Enhanced Error Handling

Custom exception classes for better error handling:

```python
import pynlm as nlm

try:
    config = nlm.create_config()
    config.set("invalid_key", "invalid_value")
except nlm.NLMConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.NLMValidationError as e:
    print(f"Validation error: {e}")
except nlm.NLMSerializationError as e:
    print(f"Serialization error: {e}")
```

### 2. Enhanced Configuration System

#### Preset Configurations

```python
import pynlm as nlm

# Create configuration from preset
config = nlm.create_config_from_preset("learning")
print(f"Neuron count: {config.get('neuron_count', 0)}")
print(f"Learning rate: {config.get('learning_rate', 0.0)}")

# Available presets
presets = [
    "minimal",    # Minimal configuration (100 neurons)
    "small",      # Small configuration (500 neurons)
    "medium",     # Medium configuration (1000 neurons)
    "large",      # Large configuration (5000 neurons)
    "learning",   # Learning-focused configuration
    "development" # Development-focused configuration
]
```

#### Configuration Validation

```python
config = nlm.create_config()
config.set("neuron_count", 1000)
config.set("learning_rate", 0.01)
config.set("connection_probability", 0.1)

# Validate configuration
if config.validate():
    print("✓ Configuration is valid!")
else:
    print("✗ Configuration validation failed")
    # You can access detailed error messages through the config system
```

#### Environment Variable Support

```python
import os

# Set environment variables
os.environ['NLM_NEURON_COUNT'] = '2000'
os.environ['NLM_LEARNING_RATE'] = '0.05'

# Load configuration with environment variable expansion
config = nlm.create_config()
config.load_from_args(['--neuron_count=${NLM_NEURON_COUNT}',
                      '--learning_rate=${NLM_LEARNING_RATE}'])
```

#### File Format Support

The enhanced configuration system supports multiple file formats:

```python
# Save in different formats
config = nlm.create_config()
config.save_to_file("config.json")    # JSON format
config.save_to_file("config.yaml")    # YAML format
config.save_to_file("config.cfg")     # Simple key=value format

# Load from different formats
config1 = nlm.create_config()
config1.load_from_file("config.json")

config2 = nlm.create_config()
config2.load_from_file("config.yaml")
```

### 3. Enhanced Brain Interface

#### Context Manager Support

```python
import pynlm as nlm

config = nlm.create_config()
brain = nlm.create_brain(config)

# Use context manager for automatic cleanup
with brain.make_context(save_on_exit=True, checkpoint_path="brain.bin"):
    brain.initialize()
    
    # Run simulation steps
    for step in range(100):
        brain.step(step)
    
    # Context manager automatically saves checkpoint and resets
    print("Simulation completed and cleaned up!")
```

#### High-Level APIs

```python
import pynlm as nlm

# Create agent for simulation
agent = nlm.create_agent_brain(brain)
world = nlm.create_simple_world()

# Run training episode
result = nlm.run_training_episode(
    brain=brain,
    world=world,
    agent=agent,
    max_steps=1000,
    save_checkpoints=True,
    checkpoint_prefix="training_"
)

print(f"Training completed!")
print(f"Total reward: {result['total_reward']}")
print(f"Average firing rate: {result['avg_firing_rate']}")
print(f"Episodes stored: {result['episodes_stored']}")
```

#### Action Creation and Management

```python
import pynlm as nlm

# Create different types of actions
action1 = nlm.Action(nlm.ActionType.MoveForward, [0.5, 0.3, 0.2])
action2 = nlm.Action(nlm.ActionType.Interact, [1.0])
action3 = nlm.Action(nlm.ActionType.Rest)

# Modify action parameters
action1.add_parameter(0.8)
action1.set_type(nlm.ActionType.TurnLeft)

# Convert to dictionary
action_dict = action1.to_dict()
print(f"Action as dict: {action_dict}")

# Clone action
action_copy = action1.clone()
```

### 4. Enhanced Sensory Processing

#### Vision Processing

```python
import pynlm as nlm
import numpy as np

# Create vision input
vision = nlm.Vision(width=16, height=16, channels=3)

# Create from numpy array
image_data = np.random.rand(16, 16, 3).astype(np.float32)
vision.set_array(image_data)

# Get as numpy array
array = vision.get_array()

# Access vision properties
print(f"Width: {vision.get_width()}")
print(f"Height: {vision.get_height()}")
print(f"Channels: {vision.get_channels()}")
```

#### Audio Processing

```python
import pynlm as nlm

# Create audio input
audio = nlm.Audio(sample_rate=44100, num_samples=44100 * 2)  # 2 seconds

# Set audio data
audio_data = [0.1, 0.2, 0.3, 0.4, 0.5] * 8820  # Repeat to fill buffer
audio.set_data(audio_data)

# Get audio properties
print(f"Sample rate: {audio.get_sample_rate()}")
print(f"Duration: {audio.get_duration():.2f} seconds")
```

### 5. Visualization and Analysis

#### Neural Activity Visualization

```python
import pynlm as nlm

# Plot neural activity (requires matplotlib)
try:
    nlm.plot_neural_activity(brain, "neural_activity.png")
    print("✓ Neural activity plot saved to 'neural_activity.png'")
except ImportError:
    print("Note: Visualization requires matplotlib. Install with: pip install matplotlib")
```

#### Platform Information

```python
import pynlm as nlm

# Get platform information
info = nlm.get_platform_info()
print(f"Platform info: {info}")

# Get version information
version_info = nlm.get_version_info()
print(f"Version: {version_info}")

# Get system information
system_info = nlm.get_system_info()
print(f"System: {system_info}")
```

## API Reference

### Core Classes

#### Config
- `create_config()`: Create a new configuration
- `create_config_from_preset(preset_name)`: Create from preset
- `validate_config(config)`: Validate configuration
- `apply_preset(config, preset_name)`: Apply preset to existing config

#### Brain
- `create_brain(config)`: Create a brain instance
- `make_context(brain, save_on_exit, checkpoint_path)`: Create context manager

#### AgentBrain
- `create_agent_brain(brain)`: Create agent brain interface
- `initialize(world)`: Initialize with world

#### SimpleWorld
- `create_simple_world()`: Create a simple world

#### Action
- `Action(type, parameters=None)`: Create action
- `add_parameter(value)`: Add parameter
- `get_parameter(index)`: Get parameter
- `to_dict()`: Convert to dictionary
- `clone()`: Clone action

#### Vision
- `Vision(width, height, channels=3)`: Create vision input
- `set_array(array)`: Set from numpy array
- `get_array()`: Get as numpy array

#### Audio
- `Audio(sample_rate, num_samples)`: Create audio input
- `set_data(data)`: Set audio data
- `get_duration()`: Get duration in seconds

### Enumerations

#### NeuronType
- `Excitatory`, `Inhibitory`, `Modulatory`, `Sensory`, `Motor`, `Internal`

#### ActionType
- `MoveForward`, `MoveBackward`, `MoveLeft`, `MoveRight`, `TurnLeft`, `TurnRight`, `Look`, `LookUp`, `LookDown`, `Interact`, `Eat`, `Drink`, `Rest`, `Wait`, `Custom`

#### ConfigSource
- `Default`, `File`, `CommandLine`, `Runtime`

### Constants

#### Configuration Presets
- `PRESET_MINIMAL`, `PRESET_SMALL`, `PRESET_MEDIUM`, `PRESET_LARGE`, `PRESET_LEARNING`, `PRESET_DEVELOPMENT`

### Exception Classes

#### NLMConfigurationError
- Thrown for configuration-related errors

#### NLMValidationError
- Thrown for validation errors

#### NLMSerializationError
- Thrown for serialization errors

#### NLMRuntimeError
- Base exception class for runtime errors

## Examples

### Basic Usage

```python
import pynlm as nlm

# Create a simple configuration
config = nlm.create_config_from_preset("small")

# Create brain and run simulation
brain = nlm.create_brain(config)
brain.initialize()

# Run a few simulation steps
for step in range(10):
    brain.step(step)

# Produce an action
action = brain.produce_action()
print(f"Produced action: {action}")

# Reset the brain
brain.reset()
```

### Working with Actions

```python
import pynlm as nlm

# Create different actions
action1 = nlm.Action(nlm.ActionType.MoveForward, [0.5, 0.3])
action2 = nlm.Action(nlm.ActionType.TurnLeft, [45.0])
action3 = nlm.Action(nlm.ActionType.Rest)

# Modify action
action1.add_parameter(0.8)
action1.set_type(nlm.ActionType.Custom)

# Get action properties
print(f"Type: {action1.get_type()}")
print(f"Parameters: {action1.get_parameters()}")
print(f"Name: {action1.get_name()}")

# Convert to dictionary
action_dict = action1.to_dict()
print(f"As dictionary: {action_dict}")

# Clone action
action_copy = action1.clone()
```

### Configuration Management

```python
import pynlm as nlm

# Create configuration
config = nlm.create_config()

# Set configuration values
config.set("neuron_count", 1000)
config.set("learning_rate", 0.01)
config.set("connection_probability", 0.1)

# Apply preset
config.apply_preset("learning")

# Validate configuration
if config.validate():
    print("✓ Configuration is valid!")
else:
    print("✗ Configuration validation failed")

# Get configuration keys
keys = config.get_keys()
print(f"Configuration keys: {keys}")

# Save configuration
config.save_to_file("my_config.json")
```

### Sensory Processing

```python
import pynlm as nlm
import numpy as np

# Create vision input
vision = nlm.Vision(width=32, height=32, channels=3)

# Create from numpy array
image_data = np.random.rand(32, 32, 3).astype(np.float32) * 255.0
vision.set_array(image_data)

# Access vision properties
print(f"Vision width: {vision.get_width()}")
print(f"Vision height: {vision.get_height()}")
print(f"Vision channels: {vision.get_channels()}")

# Get vision data as numpy array
array = vision.get_array()
print(f"Array shape: {array.shape}")
```

## Installation

### Prerequisites

```bash
# Install NLM Python bindings
pip install pynlm

# For advanced features, install additional dependencies
pip install matplotlib numpy pyyaml scipy
```

### Building from Source

```bash
# Clone the NLM repository
cd /path/to/nlm

# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install python/
```

## Troubleshooting

### Common Issues

1. **Import Error**
   ```python
   # Error: cannot import name 'pynlm'
   
   # Solution: Install the package
   pip install pynlm
   
   # Or build from source
   cd /path/to/nlm
   pip install python/
   ```

2. **Missing Dependencies**
   ```python
   # Error: cannot import name 'matplotlib'
   
   # Solution: Install visualization dependencies
   pip install matplotlib numpy
   ```

3. **Configuration Errors**
   ```python
   # Error: Configuration validation failed
   
   # Solution: Check configuration values
   config = nlm.create_config()
   config.set("neuron_count", 0)  # Invalid
   
   # Fix: Use valid values
   config.set("neuron_count", 1000)  # Valid
   ```

### Debugging Tips

1. **Enable Logging**
   ```python
   import nlm.core.Logger as logger
   logger.Logger.get_global().setLevel(logger.LogLevel.Debug)
   ```

2. **Check Configuration**
   ```python
   config = nlm.create_config()
   print(config.summary())
   ```

3. **Validate Configuration**
   ```python
   if not config.validate():
       print("Configuration is invalid!")
       # Fix configuration issues
   ```

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgments

We would like to acknowledge the contributions of:

- The NLM development team
- Pybind11 contributors
- Scientific community for neural simulation research
- Users who have provided feedback and suggestions

## Contact Information

For questions, support, or feature requests, please visit:

- GitHub: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Issue Tracker: https://github.com/nlm-project/nlm/issues

## Build Information

- **Framework**: C++ neural simulation engine
- **Bindings**: Pybind11 Python bindings
- **Build System**: CMake and scikit-build
- **Python Support**: 3.8+
- **License**: MIT

---

*This documentation is generated automatically from the NLM Python bindings source code. For the most up-to-date information, please refer to the source code and README files.*
