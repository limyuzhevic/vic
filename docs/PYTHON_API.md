# Enhanced NLM Python API Documentation

This document describes the enhanced Python bindings for the Neural Learning Machine (NLM) framework, providing Pythonic wrapper classes, factory functions, and utilities for advanced brain simulation and analysis.

## Overview

The enhanced Python bindings extend the basic NLM bindings with the following features:

- **Pythonic Wrapper Classes**: Enhanced configuration, brain, world, and agent brain wrappers
- **Enhanced Factory Functions**: Intuitive creation methods with Python dict configuration
- **Improved Error Handling**: Custom NLMError exception with meaningful error messages
- **Batch Processing Utilities**: Advanced batch simulation and comparative analysis
- **Configuration Management**: Enhanced configuration validation and management
- **Statistical Analysis Tools**: Comprehensive statistics collection and analysis
- **Sensory Data Handling**: Better integration with Python arrays and data structures
- **Action and WorldObject Support**: Pythonic APIs for actions and world objects

## Pythonic Wrapper Classes

### PythonConfigWrapper

The `PythonConfigWrapper` class provides an enhanced configuration interface with validation and Pythonic access patterns.

#### Constructor
```python
def __init__(self, config_dict=None, filepath=None):
    Initialize configuration wrapper
    
    Args:
        config_dict: Python dictionary with configuration values
        filepath: Path to configuration file (JSON format)
```

#### Type-Safe Getters
```python
def get_int(self, key, default_value=0):
    Get integer value with default fallback

def get_double(self, key, default_value=0.0):
    Get double/float value with default fallback

def get_bool(self, key, default_value=False):
    Get boolean value with default fallback

def get_string(self, key, default_value=""):
    Get string value with default fallback

def get_float_list(self, key):
    Get list of floats from configuration

def get_int_list(self, key):
    Get list of integers from configuration

def get_double_list(self, key):
    Get list of doubles from configuration
```

#### Type-Safe Setters
```python
def set_int(self, key, value):
    Set integer value in configuration

def set_double(self, key, value):
    Set double value in configuration

def set_bool(self, key, value):
    Set boolean value in configuration

def set_string(self, key, value):
    Set string value in configuration

def set_float_list(self, key, value):
    Set list of float values in configuration
```

#### Configuration Operations
```python
def has(self, key):
    Check if configuration key exists

def get_keys(self):
    Get all configuration keys

def remove(self, key):
    Remove configuration key

def clear(self):
    Clear all configuration entries

def update_from_dict(self, config_dict):
    Update configuration from Python dictionary

def batch_update(self, updates):
    Batch update configuration from dictionary of updates

def save_to_file(self, filepath):
    Save configuration to file

def load_from_file(self, filepath):
    Load configuration from file

def summary(self):
    Get configuration summary string

def to_dict(self):
    Convert configuration to Python dictionary

def validate(self):
    Validate configuration structure
```

#### Exception Handling
All methods raise `NLMError` exceptions with meaningful error messages when validation fails or operations cannot be completed.

### PythonBrainWrapper

The `PythonBrainWrapper` class provides enhanced brain simulation capabilities with convenience methods and statistics.

#### Constructor
```python
def __init__(self, brain):
    Wrap existing Brain object
```

#### Simulation Control
```python
def initialize_with_config(self, config_dict):
    Initialize brain with Python dictionary configuration

def simulate_step(self, step, time=0.0):
    Perform simulation step with optional timestamp

def inject_sensory(self, data, width, height, channels=3):
    Inject sensory data (vision) into brain

def get_action_vector(self):
    Get current motor action as parameter vector
```

#### Statistics Collection
```python
def get_statistics(self):
    Get comprehensive brain statistics as Python dictionary
    Returns: {
        'total_neurons': int,
        'total_synapses': int,
        'active_neurons': int,
        'firing_neurons': int,
        'average_firing_rate': float,
        'excitation_inhibition_ratio': float,
        'total_spike_count': int,
        'pending_spike_events': int
    }

def get_neuron_population_stats(self):
    Get neuron population statistics by type and region
    Returns: list of dictionaries with region, neuron type, and population details
```

#### Development and State Management
```python
def set_developmental_stage(self, stage):
    Set developmental stage by string name

def get_developmental_stage(self):
    Get current developmental stage as string

def reset_simulation(self):
    Reset brain state

def save_to_file(self, filepath):
    Save brain state to file

def load_from_file(self, filepath):
    Load brain state from file

def get_learned_preference(self):
    Get learned preference trait

def set_learned_preference(self, preference):
    Set learned preference trait
```

### PythonWorldWrapper

The `PythonWorldWrapper` class provides enhanced world simulation with Pythonic interface.

#### Constructor
```python
def __init__(self, config_dict=None):
    Initialize world wrapper with optional configuration
```

#### World Configuration
```python
def configure_from_dict(self, config_dict):
    Configure world from Python dictionary
    Supported keys: width, height, vision_width, vision_height, max_energy, energy_decay_rate, random_seed
```

#### Simulation Control
```python
def reset(self):
    Reset world to initial state

def set_agent_start(self, x, y):
    Set agent starting position

def update(self, timestep):
    Update world state for given timestep

def apply_action(self, action_dict):
    Apply action from Python dictionary
    Supported action types: MoveForward, MoveBackward, MoveLeft, MoveRight, TurnLeft, TurnRight, Look, Interact, Wait
```

#### State Access
```python
def get_agent_state(self):
    Get agent state as Python dictionary
    Returns: {
        'x': float, 'y': float, 'orientation': float,
        'velocity_x': float, 'velocity_y': float,
        'angular_velocity': float, 'energy': float,
        'health': float, 'age': float, 'is_moving': bool,
        'is_turning': bool, 'last_action_time': float
    }

def get_sensory_percept(self):
    Get sensory percept as Python dictionary
    Returns vision, touch, internal, proprioception, audio data
```

### PythonAgentBrainWrapper

The `PythonAgentBrainWrapper` class provides step-by-step agent control for enhanced debugging and analysis.

#### Constructor
```python
def __init__(self, agent_brain):
    Wrap existing AgentBrain object
```

#### Step-by-Step Control
```python
def initialize_step_by_step(self, world_wrapper):
    Initialize agent brain with world wrapper

def process_sensory_input_step_by_step(self, percept_dict):
    Process sensory percept from Python dictionary

def decode_motor_command_step_by_step(self):
    Decode motor command and return as string

def apply_reward_modulation_step_by_step(self, reward, predicted_reward):
    Apply reward modulation with specified values

def update_development_step_by_step(self, timestep):
    Update development system with step-by-step control
```

#### Configuration Flags
```python
def enable_reward_modulation(self, enable):
    Enable or disable reward modulation

def enable_structural_plasticity(self, enable):
    Enable or disable structural plasticity

def enable_development(self, enable):
    Enable or disable development

def enable_curiosity(self, enable):
    Enable or disable curiosity system
```

## Enhanced Factory Functions

### createConfiguredBrain()
```python
def createConfiguredBrain(config_dict):
    Create brain with Python dictionary configuration
    
    Args:
        config_dict: Python dictionary with configuration parameters
        
    Returns:
        PythonBrainWrapper instance
```

### createConfiguredWorld()
```python
def createConfiguredWorld(config_dict):
    Create world with Python dictionary configuration
    
    Args:
        config_dict: Python dictionary with world configuration
        
    Returns:
        PythonWorldWrapper instance
```

### createSimpleAgentSetup()
```python
def createSimpleAgentSetup(brain_config_dict, world_config_dict, agent_config_dict):
    Create complete agent setup with Python dictionary configurations
    
    Args:
        brain_config_dict: Brain configuration
        world_config_dict: World configuration
        agent_config_dict: Agent configuration
        
    Returns:
        PythonAgentBrainWrapper instance
```

### createConfiguredConfig()
```python
def createConfiguredConfig(config_dict):
    Create configuration from Python dictionary
    
    Args:
        config_dict: Python dictionary with configuration values
        
    Returns:
        PythonConfigWrapper instance
```

## Improved Error Handling

### NLMError Exception
```python
class NLMError(Exception):
    Custom exception for NLM errors
    
    Args:
        message: Error message
        operation: Operation that caused the error
```

All enhanced bindings raise `NLMError` with descriptive messages when:
- Configuration validation fails
- Invalid parameters are provided
- File operations fail
- Neural network operations cannot be completed

## Batch Processing Utilities

### BatchSimulator Class
```python
class BatchSimulator:
    Advanced batch processing for multiple simulations
    
    def __init__(self, max_batch_size=10):
        Initialize batch simulator
        
    def add_config(self, config_dict):
        Add configuration for batch creation
        
    def add_world(self, config_dict):
        Add world configuration for batch creation
        
    def run_batch_simulations(self, sensory_inputs, timestep, steps):
        Run multiple simulations efficiently
        
        Args:
            sensory_inputs: List of sensory input data
            timestep: Simulation timestep
            steps: Number of simulation steps
            
        Returns:
            List of simulation results
            
    def compare_brains(self):
        Compare brains across configurations
        
    def clear(self):
        Clear all stored simulations
        
    def get_batch_size(self):
        Get current batch size
        
    def set_batch_size(self, size):
        Set batch size
```

### Convenience Functions
```python
def run_batch_simulations(config_dicts, sensory_inputs, timestep, steps):
    Run multiple simulations efficiently
    
def compare_brains():
    Compare brains across configurations
```

## Enhanced Configuration Management

### Configuration Validation
```python
class config:
    Configuration management utilities
    
    @staticmethod
    def validate_config(config_dict):
        Validate configuration dictionary structure
        
    @staticmethod
    def get_supported_types():
        Get supported configuration value types
        
    @staticmethod
    def merge_config(config1, config2):
        Merge two configurations
        
    @staticmethod
    def deep_copy_config(config_dict):
        Deep copy configuration
        
    @staticmethod
    def compare_config(config1, config2):
        Compare two configurations
```

## Statistical Analysis Tools

### statistics Module
```python
class statistics:
    Statistical analysis utilities
    
    @staticmethod
    def calculate_statistics(data):
        Calculate statistics from simulation data
        
    @staticmethod
    def calculate_neuron_population_stats(population_stats_list):
        Calculate neuron population statistics
        
    @staticmethod
    def compare_datasets(datasets):
        Compare multiple datasets
```

## Enhanced Sensory Data Handling

### sensory Module
```python
class sensory:
    Enhanced sensory data handling utilities
    
    @staticmethod
    def convert_to_vision(py_array):
        Convert Python array to Vision object
        
    @staticmethod
    def convert_to_audio(py_array, sample_rate=44100):
        Convert Python array to Audio object
        
    @staticmethod
    def extract_sensory_data(world):
        Extract sensory data from world
```

## Advanced Action and WorldObject Support

### objects Module
```python
class objects:
    Advanced action and world object utilities
    
    @staticmethod
    def create_action_from_dict(action_dict):
        Create action from Python dictionary
        
    @staticmethod
    def create_world_object_from_dict(obj_dict):
        Create world object from Python dictionary
        
    @staticmethod
   �ี่จะจัดระเบียฤและสไตล์ action_to_dict(action):
        Convert action to Python dictionary
        
    @staticmethod
    def world_object_to_dict(obj):
        Convert world object to Python dictionary
```

## Backward Compatibility

The enhanced bindings maintain full backward compatibility with the existing API:

### Existing Functions (Unchanged)
```python
# All original functions work unchanged
createDefaultConfig()
createBrain(config)
createSimpleWorld()
createAgentBrain(brain)

# All original classes are accessible
Config, Brain, SimpleWorld, AgentBrain
```

### Usage Examples

#### Basic Usage (Unchanged)
```python
# Original API still works
from pynlm import (
    Config, Brain, SimpleWorld, AgentBrain,
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain
)

# Create configuration
config = createDefaultConfig()
config.set('neuron_count', 100)

# Create brain
brain = createBrain(config)

# Create world
world = createSimpleWorld()
world.configure(100, 100, 16, 16)

# Create agent brain
agent_brain = createAgentBrain(brain)
agent_brain.initialize(world)
```

#### Enhanced Usage (New)
```python
# New enhanced API
from pynlm_enhanced import (
    PythonConfigWrapper, PythonBrainWrapper, PythonWorldWrapper, PythonAgentBrainWrapper,
    createConfiguredBrain, createConfiguredWorld, createSimpleAgentSetup,
    run_batch_simulations, compare_brains
)

# Create brain with Python dict
brain_config = {
    'neuron_count': 100,
    'region_count': 2,
    'connection_probability': 0.1
}

brain = createConfiguredBrain(brain_config)
stats = brain.get_statistics()
print(f"Total neurons: {stats['total_neurons']}")

# Create world with Python dict
world_config = {
    'width': 100.0,
    'height': 100.0,
    'vision_width': 16,
    'vision_height': 16,
    'max_energy': 100.0,
    'energy_decay_rate': 0.01,
    'random_seed': 12345
}

world = createConfiguredWorld(world_config)
world.reset()

# Create complete agent setup
agent_brain = createSimpleAgentSetup(
    brain_config, world_config, {}
)

# Batch processing
configs = [brain_config, brain_config]  # Could have different configs
sensory_inputs = [vision_data1, vision_data2]
results = run_batch_simulations(configs, sensory_inputs, timestep=0.1, steps=100)
```

## Installation and Setup

### Installation
```bash
pip install pynlm-enhanced
```

### Build System
The enhanced bindings are included in the main build system. Update the main `pyproject.toml` to include the enhanced module:

```toml
[project.optional-dependencies]
enhanced = [
    "pynlm-enhanced"
]
```

### Python Environment
```bash
pip install pybind11 numpy pytest
```

## Testing

### Unit Tests
```bash
pytest tests/test_enhanced_bindings.py
```

### Integration Tests
```bash
pytest tests/test_integration.py
```

## Performance Considerations

1. **Batch Processing**: Use `BatchSimulator` for multiple simulations to reduce overhead
2. **Memory Management**: Wrapper classes use smart pointers for efficient memory management
3. **Validation**: Configuration validation adds minimal overhead for robust error handling
4. **Type Safety**: Type hints and validation improve debugging and reliability

## Migration Guide

### From Basic to Enhanced API

#### 1. Configuration Management
```python
# Old way
config = Config()
config.set('key', 'value')

# New way
config = PythonConfigWrapper({'key': 'value'})
config.set_string('key', 'value')
```

#### 2. Brain Simulation
```python
# Old way
brain = createBrain(config)
brain.step(step)

# New way
brain = createConfiguredBrain({'neuron_count': 100})
brain.simulateStep(step)
```

#### 3. Statistics
```python
# Old way
brain.logStatus()

# New way
stats = brain.get_statistics()
```

## Examples

### Example 1: Basic Brain Simulation
```python
from pynlm_enhanced import createConfiguredBrain, createConfiguredWorld, createSimpleAgentSetup

# Create configuration
config = {
    'neuron_count': 100,
    'region_count': 2,
    'connection_probability': 0.1
}

# Create brain
brain = createConfiguredBrain(config)
brain.simulateStep(0)

# Create world
world_config = {
    'width': 100.0,
    'height': 100.0,
    'vision_width': 16,
    'vision_height': 16
}

world = createConfiguredWorld(world_config)
world.reset()

# Create agent setup
agent = createSimpleAgentSetup(config, world_config, {})

# Run simulation
for step in range(100):
    world.update(0.1)
    brain.simulateStep(step, step * 0.1)
```

### Example 2: Batch Processing
```python
from pynlm_enhanced import BatchSimulator

# Create batch simulator
simulator = BatchSimulator(max_batch_size=5)

# Add configurations
for i in range(10):
    config = {'neuron_count': 50 + i * 10}
    simulator.addConfig(config)

# Run batch simulation
sensory_inputs = [vision_data1, vision_data2]  # Example data
results = simulator.run_batch_simulations(sensory_inputs, timestep=0.1, steps=50)

# Compare results
comparison = simulator.compare_brains()
print(f"Average neurons: {comparison['averages']['avg_total_neurons']}")
```

### Example 3: Error Handling
```python
from pynlm_enhanced import NLMError

try:
    config = PythonConfigWrapper({'invalid_key': 'not_a_list'})
    config.set_float_list('invalid_key', [1.0, 2.0])  # This will fail
except NLMError as e:
    print(f"Configuration error: {e}")
```

## Advanced Usage

### Custom Configuration Validation
```python
def validate_my_config(config_dict):
    # Custom validation logic
    required_keys = ['neuron_count', 'region_count']
    for key in required_keys:
        if key not in config_dict:
            raise ValueError(f"Missing required key: {key}")
    return True

# Use in enhanced wrapper
config = PythonConfigWrapper(my_config_dict)
```

### Statistical Analysis
```python
from pynlm_enhanced import statistics

# Collect simulation data
data_points = []
for i in range(100):
    # Run simulation and collect data
    stats = brain.get_statistics()
    data_points.append(stats['total_spike_count'])

# Calculate statistics
sim_stats = statistics.calculate_statistics({'values': data_points})
print(f"Mean: {sim_stats['mean']}, Std: {sim_stats['std_dev']}")
```

## Conclusion

The enhanced NLM Python bindings provide a comprehensive set of tools for neural simulation with improved Pythonic interfaces, robust error handling, and advanced features for batch processing and statistical analysis. The enhanced bindings maintain full backward compatibility while offering significant improvements in usability and functionality.