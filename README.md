# NLM Python Bindings - Enhanced API Documentation

## Overview

This document describes the enhanced Python bindings for the NLM (Neural Learning Machine) framework. The bindings have been significantly improved to provide a more Pythonic, powerful, and intuitive API for users working with neural simulation frameworks.

## Key Improvements

### 1. Enhanced Property Access

The Python bindings now support property-style access to common attributes, making the API more intuitive and Pythonic:

```python
# Property access instead of method calls
brain = nlm.create_brain()
brain.total_neurons  # Instead of brain.getTotalNeuronCount()
brain.average_firing_rate  # Instead of brain.getAverageFiringRate()

world = nlm.create_simple_world()
world.width  # Instead of world.getWidth()
world.height  # Instead of world.getHeight()

# Configurable properties
world.max_energy = 2.0  # Instead of world.setMaxEnergy(2.0)
world.energy_decay_rate = 0.1  # Instead of world.setEnergyDecayRate(0.1)
```

### 2. Pythonic Context Managers

Context managers are now available for resource management and clean simulation setup/teardown:

```python
# Brain context manager
with nlm.BrainContextManager(brain) as brain_obj:
    # Brain is automatically initialized
    brain_obj.step(0)
    brain_obj.receiveSensoryInput(percept)
    # Brain is automatically reset on exit

# World context manager  
with nlm.WorldContextManager(world) as world_obj:
    # Perform simulation steps
    world_obj.update(0.01)
    # World is automatically reset on exit

# Complete scenario context manager
brain = nlm.create_brain()
world = nlm.create_simple_world()
agent_brain = nlm.create_agent_brain(brain)

with nlm.ScenarioContextManager((brain, world, agent_brain)) as (b, w, a):
    # Full scenario initialized with default settings
    w.configure(100.0, 100.0, 10, 10)
    b.initialize()
    a.initialize(*w)
    # Automatic cleanup on exit
```

### 3. Enhanced Convenience Functions

Many common operations now have dedicated convenience functions:

```python
# Neural network initialization
nlm.initialize_brain_neural_network(brain, num_regions=3, neurons_per_region=100)

# Current injection patterns
nlm.inject_current_to_pattern(brain, nlm.NeuronType.Excitatory, amplitude=5.0, duration=1.0)

# Create demo scenarios
brain, world, agent_brain = nlm.create_demo_scenario()

# Validate brain state
is_valid, errors = nlm.validate_brain_state(brain)
if not is_valid:
    print(f"Brain validation failed: {errors}")

# Get simulation statistics
stats = nlm.get_simulation_stats(brain, world)
print(f"Neurons: {stats['neuron_count']}")
print(f"Synapses: {stats['synapse_count']}")
print(f"Active neurons: {stats['active_neurons']}")

# Benchmark performance
time_ms, steps_per_sec = nlm.benchmark_simulation_step(brain, world, iterations=100)
print(f"Benchmark: {steps_per_sec:.2f} steps/sec")
```

### 4. Enhanced Error Handling and Validation

Better error handling with descriptive Python exceptions:

```python
# Type-safe property access
try:
    value = config.get_value("learning_rate")
    print(f"Learning rate: {value}")
except nlm.RuntimeError as e:
    print(f"Error: {e}")

# Validation functions with detailed error messages
is_valid, error_msg = nlm.validate_brain_state(brain)
if not is_valid:
    print("Brain state validation failed:")
    print(error_msg)
```

### 5. Enhanced Config API

The Config class now supports comprehensive Pythonic operations:

```python
# Create and manipulate configuration
config = nlm.create_default_config()

# Type-safe value setting
config.set_value("learning_rate", "0.001")
config.set_int_value("batch_size", 32)
config.set_float_value("dropout", 0.5)
config.set_bool_value("use_gpu", True)

# Type-safe value retrieval
try:
    learning_rate = config.get_value("learning_rate")
    batch_size = config.get_int_value("batch_size")
    dropout = config.get_float_value("dropout")
    use_gpu = config.get_bool_value("use_gpu")
    
    print(f"LR: {learning_rate}, Batch: {batch_size}, Dropout: {dropout}, GPU: {use_gpu}")
except nlm.RuntimeError as e:
    print(f"Configuration error: {e}")

# Update from Python dictionary
config_dict = {
    "model_type": "resnet",
    "epochs": 100,
    "learning_rate": 0.01,
    "batch_size": 64,
    "use_augmentation": True
}

config.update_from_dict(config_dict)

# Convert to Python dictionary
config_dict = config.to_dict()
print(f"Config as dict: {config_dict}")

# Get detailed summary
summary = config.get_config_summary()
print(summary)
```

### 6. Enhanced Action and Sensory APIs

Improved Action and SensoryInput classes with Pythonic interfaces:

```python
# Enhanced Action API
action = nlm.Action(nlm.ActionType.MoveForward, [0.5, 0.3, 0.8])
action.type  # Property access
print(f"Action type: {action.type}")
action.parameters  # Property access
print(f"Parameters: {action.parameters}")

# Add and get parameters with bounds checking
action.set_parameter(0, 1.0)
param = action.get_parameter(0)
print(f"First parameter: {param}")

# Enhanced SensoryPercept
percept = nlm.SensoryPercept()
percept.vision = [0.1, 0.2, 0.3, 0.4]
percept.touch = [0.0, 1.0, 0.0]
percept.internal = [0.5, 0.6]

# Convenience methods
vision_sum = percept.getSignalSum()
print(f"Total vision signal: {vision_sum}")
is_empty = percept.is_empty()
print(f"Percept empty: {is_empty}")

# Enhanced Vision and Audio
vision = nlm.Vision(84, 84, 3)
vision.setData([0.0] * (84 * 84 * 3))
print(f"Vision dimensions: {vision.width}x{vision.height}x{vision.channels}")

audio = nlm.Audio(44100, 4410)
audio.setData([0.0] * 4410)
print(f"Audio: {audio.sample_rate} Hz, {audio.num_samples} samples")
```

### 7. Comprehensive Enum Support

All enumeration types now have helper functions for better usability:

```python
# Get human-readable names
neuron_type = nlm.NeuronType.Excitatory
print(f"Neuron type: {nlm.neuron_type_name(neuron_type)}")

synapse_type = nlm.SynapseType.Excitatory
print(f"Synapse type: {nlm.synapse_type_name(synapse_type)}")

# Action type names
action_type = nlm.ActionType.MoveForward
print(f"Action type: {action_type.name}")  # Uses Python enum naming
```

## Usage Examples

### Example 1: Basic Neural Network Setup

```python
import nlm

# Create default configuration
config = nlm.create_default_config()

# Create brain and world
brain = nlm.create_brain(config)
world = nlm.create_simple_world()

# Configure world
world.configure(width=100.0, height=100.0, 
               vision_width=10, vision_height=10)

# Initialize components
brain.initialize()

# Create agent brain interface
agent_brain = nlm.create_agent_brain(brain)
agent_brain.initialize(world)

# Simulation loop
for step in range(100):
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process input through agent brain
    agent_brain.processSensoryInput(percept)
    
    # Get motor command
    motor_cmd = agent_brain.decodeMotorCommand()
    
    # Apply to world
    result = world.applyMotorCommand(motor_cmd, float(step))
    
    if result.success:
        print(f"Step {step}: Success with reward {result.reward}")
    else:
        print(f"Step {step}: Failed - {result.message}")
```

### Example 2: Advanced Neural Network with Context Managers

```python
import nlm

# Use context managers for clean resource management
brain = nlm.create_brain()
world = nlm.create_simple_world()
agent_brain = nlm.create_agent_brain(brain)

with nlm.ScenarioContextManager((brain, world, agent_brain)) as (b, w, a):
    # Setup world with default configuration
    w.configure(width=50.0, height=50.0, 
               vision_width=8, vision_height=8)
    
    # Create and initialize neural network
    nlm.initialize_brain_neural_network(b, num_regions=2, neurons_per_region=50)
    
    # Inject some activity to start the simulation
    nlm.inject_current_to_pattern(b, nlm.NeuronType.Excitatory, 
                                  amplitude=10.0, duration=0.1)
    
    # Run simulation loop
    for step in range(50):
        # Get current percept
        percept = w.getSensoryPercept()
        
        # Process through agent brain
        a.processSensoryInput(percept)
        
        # Get action
        action = b.produceAction()
        
        # Convert to motor command
        motor_cmd = a.decodeMotorCommand()
        
        # Apply to world
        result = w.applyMotorCommand(motor_cmd, float(step))
        
        # Print statistics every 10 steps
        if step % 10 == 0:
            stats = nlm.get_simulation_stats(b, w)
            print(f"Step {step}: {stats['active_neurons']} active neurons, "
                  f"{stats['firing_neurons']} firing neurons")

# Context managers ensure automatic cleanup
```

### Example 3: Configuration Management

```python
import nlm

# Create brain with custom configuration
config = nlm.create_default_config()

# Set configuration values using different types
config.set_value("model_name", "nlm_v2")
config.set_int_value("hidden_size", 256)
config.set_float_value("dropout_rate", 0.2)
config.set_bool_value("enable_batch_norm", True)

# Update from dictionary (common in Python APIs)
config_dict = {
    "model_name": "nlm_v3",
    "hidden_size": 512,
    "dropout_rate": 0.3,
    "enable_batch_norm": False,
    "learning_rate": 0.001,
    "optimizer": "adam"
}

config.update_from_dict(config_dict)

# Verify configuration
print(f"Model name: {config.get_value('model_name')}")
print(f"Hidden size: {config.get_int_value('hidden_size')}")
print(f"Dropout rate: {config.get_float_value('dropout_rate')}")
print(f"Batch norm enabled: {config.get_bool_value('enable_batch_norm')}")

# Save configuration
config.save_to_file("brain_config.txt")

# Load and verify
config2 = nlm.create_default_config()
config2.loadFromFile("brain_config.txt")
print(f"Loaded config has {config2.getKeys()} keys")
```

### Example 4: Data Analysis and Visualization Utilities

```python
import nlm
import math

# Create a brain and world
brain = nlm.create_brain()
world = nlm.create_simple_world()
world.configure(100.0, 100.0, 10, 10)
brain.initialize()

# Run simulation for a short period
for step in range(20):
    brain.step(step)
    world.update(0.01)

# Analyze simulation results
stats = nlm.get_simulation_stats(brain, world)

# Calculate derived metrics
total_activity = (stats['active_neurons'] + stats['firing_neurons']) / 2
excitation_inhibition_balance = stats['e_i_ratio']

print(f"=== Simulation Analysis ===")
print(f"Total neurons: {stats['neuron_count']}")
print(f"Total synapses: {stats['synapse_count']}")
print(f"Active neurons: {stats['active_neurons']}")
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
print(f"Excitation/Inhibition ratio: {stats['e_i_ratio']:.3f}")
print(f"Total spikes: {stats['total_spikes']}")
print(f"World dimensions: {stats['world_width']}x{stats['world_height']}")
print(f"Simulation time: {stats['simulation_time']:.2f} seconds")

# Validate the simulation state
is_valid, error_msg = nlm.validate_brain_state(brain)
if not is_valid:
    print(f"\nValidation warnings:")
    print(error_msg)
else:
    print(f"\nSimulation state: VALID")
```

## Advanced Features

### 1. Error Handling Patterns

The enhanced bindings provide comprehensive error handling:

```python
# Type-safe configuration access
try:
    value = config.get_int_value("parameter")
    print(f"Value: {value}")
except nlm.RuntimeError as e:
    print(f"Configuration error: {e}")

# Simulation validation
is_valid, errors = nlm.validate_brain_state(brain)
if not is_valid:
    print(f"Brain validation failed: {errors}")
```

### 2. Performance Benchmarking

Benchmark simulation performance:

```python
# Benchmark different configurations
brain = nlm.create_brain()
world = nlm.create_simple_world()
world.configure(50.0, 50.0, 5, 5)

# Warm up
for _ in range(10):
    brain.step(0)
    world.update(0.01)

# Benchmark
import time
start_time = time.time()
for step in range(100):
    brain.step(step)
    world.update(0.01)
end_time = time.time()

steps_per_sec = 100 / (end_time - start_time)
print(f"Performance: {steps_per_sec:.2f} steps/second")
```

### 3. Neural Network Initialization

Initialize complex neural architectures:

```python
# Initialize multi-region neural network
brain = nlm.create_brain()
num_regions = nlm.initialize_brain_neural_network(brain, 
                                                   num_regions=3,
                                                   neurons_per_region=200,
                                                   neuron_type=nlm.NeuronType.Excitatory)

print(f"Created neural network with {num_regions} regions")
print(f"Total neurons: {brain.total_neurons}")
print(f"Total synapses: {brain.total_synapses}")
```

## Migration Guide

### From Basic to Enhanced API

**Before:**
```python
# Basic usage
brain = nlm.create_brain()
brain.initialize()

# Manual property access
width = world.getWidth()
height = world.getHeight()
brain.initialize()
```

**After:**
```python
# Enhanced usage with context managers
with nlm.BrainContextManager(brain) as brain_obj:
    brain_obj.initialize()

# Pythonic property access
width = world.width
height = world.height
```

### Best Practices

1. **Use Context Managers**: Always use context managers for resource management
2. **Leverage Properties**: Use property access instead of method calls where available
3. **Error Handling**: Always wrap configuration access in try-except blocks
4. **Convenience Functions**: Use provided convenience functions for common operations
5. **Validation**: Use validation functions to catch potential issues early

## API Reference

### Core Classes

- **Brain**: Neural simulation brain class
- **Config**: Configuration manager
- **SimpleWorld**: 2D simulation world
- **AgentBrain**: Brain-world interface
- **Action**: Motor action representation
- **SensoryPercept**: Sensory input data
- **Vision**: Visual sensory input
- **Audio**: Audio sensory input
- **InternalSignals**: Internal neural signals

### Factory Functions

- `create_default_config()`: Create default configuration
- `create_brain(config)`: Create brain with configuration
- `create_simple_world()`: Create simulation world
- `create_agent_brain(brain)`: Create agent brain interface
- `create_demo_scenario()`: Create complete demo scenario

### Convenience Functions

- `create_brain_with_config(dict)`: Create brain from dictionary
- `initialize_brain_neural_network(brain, num_regions, neurons_per_region)`: Initialize network
- `inject_current_to_pattern(brain, type, amplitude, duration)`: Inject current pattern
- `validate_brain_state(brain)`: Validate brain state
- `get_simulation_stats(brain, world)`: Get simulation statistics
- `benchmark_simulation_step(brain, world, iterations)`: Benchmark performance

### Enumeration Helpers

- `neuron_type_name(type)`: Get neuron type as string
- `synapse_type_name(type)`: Get synapse type as string

## Conclusion

The enhanced Python bindings for NLM provide a comprehensive, Pythonic API for neural simulation frameworks. The improvements include:

- **Enhanced Property Access**: Intuitive property-style access to common attributes
- **Context Managers**: Clean resource management with automatic cleanup
- **Convenience Functions**: Simplified common operations
- **Better Error Handling**: Comprehensive error handling with descriptive exceptions
- **Type Safety**: Improved type checking and validation
- **Documentation**: Extensive examples and comprehensive documentation

These enhancements make the NLM framework more accessible and powerful for Python users, enabling faster development and more intuitive usage of neural simulation capabilities.
