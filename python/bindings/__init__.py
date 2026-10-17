# Basic Python Interface for NLM

This module provides a simple Python interface to the NLM C++ neural simulation framework.
It includes basic type definitions and core functionality with comprehensive error handling.

## Overview

This is a minimal implementation that provides:
- Basic type definitions and enumerations
- Simple error handling with custom exceptions
- Core utility functions
- Basic validation

The implementation is designed to be simple, efficient, and Pythonic while providing
a solid foundation for more advanced functionality.

## Installation

```bash
pip install -e /workspace/app/python/bindings
```

## Usage Examples

### Basic Usage

```python
import nlm

# Create a brain
brain = nlm.create_brain()
brain.initialize()

# Create a world for simulation
world = nlm.create_simple_world()
world.configure(100, 100, 20, 20)
world.set_agent_start(50, 50)

# Run simulation
for step in range(100):
    world.update(0.001)
    action = brain.produce_action()
    world.apply_motor_command(action, step * 0.001)

print("Simulation completed!")
```

### Configuration

```python
import nlm

# Create configuration
config = nlm.create_default_config()

# Set configuration values
config.set("neuron_count", 1000)
config.set("region_count", 5)
config.set("connection_probability", 0.1)

# Save configuration
config.save_to_file("my_config.json")

# Load configuration
new_config = nlm.Config()
new_config.load_from_file("my_config.json")
```

### Error Handling

```python
import nlm

try:
    # Create brain with invalid configuration
    config = nlm.Config()
    config.set("neuron_count", -100)  # Invalid: negative neurons
    
    brain = nlm.create_brain(config)
    brain.initialize()
    
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
except nlm.NeuronError as e:
    print(f"Neuron error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Core Classes

### Config

```python
# Create configuration
config = nlm.Config()

# Set configuration values
config.set("neuron_count", 1000)
config.set("region_count", 5)
config.set("connection_probability", 0.1)
config.set("simulation_timestep", 0.001)
config.set("stdp_ltp_weight", 0.01)

# Check configuration has a key
has_connection_prob = config.has("connection_probability")

# Get all configuration keys
keys = config.get_keys()

# Clear configuration
config.clear()

# Save configuration
config.save_to_file("config.json")

# Load configuration
config.load_from_file("config.json")
```

### Brain

```python
# Create brain
brain = nlm.create_brain()
brain.initialize()

# Initialize with configuration
brain.initialize_with_config(config)

# Run simulation steps
brain.step(100)
brain.step(100, 1.234)  # With timestamp

# Receive sensory input
percept = world.get_sensory_percept()
brain.receive_sensory_input(percept)

# Get statistics
total_spikes = brain.get_total_spike_count()
active_neurons = brain.get_active_neuron_count()
avg_firing_rate = brain.get_average_firing_rate()
exc_inh_ratio = brain.get_excitation_inhibition_ratio()

# Check brain status
brain.log_status()

# Reset brain
brain.reset()

# Save/load brain state
brain.save("brain_state.nlm")
brain.load("brain_state.nlm")
```

### AgentBrain

```python
# Create agent brain
agent_brain = nlm.create_agent_brain(brain)

# Initialize with world
agent_brain.initialize(world)

# Process sensory input
agent_brain.process_sensory_input(percept)

# Get motor command
action = agent_brain.decode_motor_command()

# Apply reward modulation
agent_brain.apply_reward_modulation(reward, predicted_reward)

# Update development
agent_brain.update_development(timestep)

# Get developmental stage
stage = agent_brain.get_developmental_stage()

# Get neuromodulation levels
neuromodulation_level = agent_brain.get_neuromodulation_level()
curiosity_level = agent_brain.get_curiosity_level()
novelty_level = agent_brain.get_novelty_level()
prediction_error = agent_brain.get_prediction_error()
```

### SimpleWorld

```python
# Create world
world = nlm.create_simple_world()

# Configure world
world.configure(100, 100, 20, 20)  # width, height, vision_width, vision_height
world.set_agent_start(50, 50)      # Starting position

# Update world
world.update(0.001)  # timestep

# Apply motor command
world.apply_motor_command(action, 1.234)  # action, timestamp

# Get state
body = world.get_agent_body()
percept = world.get_sensory_percept()

# Get environment info
width = world.get_width()
height = world.get_height()
max_energy = world.get_max_energy()
energy_decay_rate = world.get_energy_decay_rate()

# Reset world
world.reset()

# Set random seed
world.set_random_seed(42)
```

### Action

```python
# Create action from type
action = nlm.Action(nlm.ActionType.MoveForward)

# Create action with parameters
action = nlm.Action(nlm.ActionType.Custom, [0.5, 0.5, 0.0])

# Get action type
type = action.get_type()

# Get action parameters
params = action.get_parameters()

# Get action name
name = action.get_name()

# Set action type
action.set_type(nlm.ActionType.MoveLeft)

# Set action parameters
action.set_parameters([0.3, 0.7, 0.0])

# Clone action
cloned_action = action.clone()
```

### NeuronId, SynapseId, etc.

```python
# Create neuron IDs
neuron_id = nlm.create_neuron_id(1)
invalid_neuron_id = nlm.INVALID_NEURON_ID

# Create synapse IDs
synapse_id = nlm.create_synapse_id(1)
invalid_synapse_id = nlm.INVALID_SYNAPSE_ID

# Create region IDs
region_id = nlm.create_region_id(1)
invalid_region_id = nlm.INVALID_REGION_ID

# Create population IDs
population_id = nlm.create_population_id(1)
invalid_population_id = nlm.INVALID_POPULATION_ID

# Use IDs
print(f"Neuron ID: {neuron_id.index()}")
print(f"Synapse ID: {synapse_id.index()}")
```

## Type Definitions

```python
# Neuron types
neuron_type = nlm.NeuronType.Excitatory
neuron_type = nlm.NeuronType.Inhibitory
neuron_type = nlm.NeuronType.Modulatory
neuron_type = nlm.NeuronType.Sensory
neuron_type = nlm.NeuronType.Motor
neuron_type = nlm.NeuronType.Internal

# Synapse types
synapse_type = nlm.SynapseType.Excitatory
synapse_type = nlm.SynapseType.Inhibitory
synapse_type = nlm.SynapseType.Modulatory
synapse_type = nlm.SynapseType.Electrical
synapse_type = nlm.SynapseType.GapJunction

# Developmental stages
stage = nlm.DevelopmentalStage.Initial
stage = nlm.DevelopmentalStage.CriticalPeriod
stage = nlm.DevelopmentalStage.Maturation
stage = nlm.DevelopmentalStage.Adult
stage = nlm.DevelopmentalStage.Aging

# Firing states
state = nlm.FiringState.Resting
state = nlm.FiringState.Active
state = nlm.FiringState.Refractory
state = nlm.FiringState.Inhibited

# Action types
action_type = nlm.ActionType.MoveForward
action_type = nlm.ActionType.MoveBackward
# ... more action types

# Motor commands
motor_cmd = nlm.MotorCommand.MoveForward
motor_cmd = nlm.MotorCommand.TurnLeft
# ... more motor commands

# World object types
obj_type = nlm.WorldObjectType.Empty
obj_type = nlm.WorldObjectType.Resource
obj_type = nlm.WorldObjectType.Hazard
# ... more object types
```

## Constants

```python
# Common constants
DEFAULT_NEURON_COUNT = nlm.DEFAULT_NEURON_COUNT  # 1000
DEFAULT_REGION_COUNT = nlm.DEFAULT_REGION_COUNT  # 1
DEFAULT_CONNECTION_PROBABILITY = nlm.DEFAULT_CONNECTION_PROBABILITY  # 0.1
DEFAULT_SIMULATION_TIMESTEP = nlm.DEFAULT_SIMULATION_TIMESTEP  # 0.001
DEFAULT_REFACTORY_PERIOD = nlm.DEFAULT_REFACTORY_PERIOD  # 5
DEFAULT_RESTING_POTENTIAL = nlm.DEFAULT_RESTING_POTENTIAL  # -70.0
DEFAULT_THRESHOLD = nlm.DEFAULT_THRESHOLD  # -55.0
DEFAULT_RESET_POTENTIAL = nlm.DEFAULT_RESET_POTENTIAL  # -70.0
DEFAULT_LEAK_CONDUCTANCE = nlm.DEFAULT_LEAK_CONDUCTANCE  # 10.0
```

## Utility Functions

```python
# Type validation
nlm.validate_neuron_type(neuron_type)
nlm.validate_synapse_type(synapse_type)

# Value clamping
clamped_value = nlm.clamp_value(value, min_val, max_val)

# Array normalization
normalized = nlm.normalize_array(values, min_val, max_val)

# Object creation with validation
neuron_id = nlm.create_neuron_id(1)
synapse_id = nlm.create_synapse_id(1)
region_id = nlm.create_region_id(1)
```

## Error Handling

```python
import nlm

# Custom exceptions
class NLMError(Exception):
    """Base exception for all NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Error in configuration or setup."""
    pass

class NeuronError(NLMError):
    """Error related to neuron operations."""
    pass

class SynapseError(NLMError):
    """Error related to synapse operations."""
    pass

class SimulationError(NLMError):
    """Error during simulation."""
    pass

# Example usage
try:
    # Create brain with invalid configuration
    config = nlm.Config()
    config.set("neuron_count", -100)
    
    brain = nlm.create_brain(config)
    brain.initialize()
    
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
except nlm.NeuronError as e:
    print(f"Neuron error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Tips

### For Large Simulations

```python
# Use optimized configuration
brain = (nlm.create_brain()
         .with_neuron_count(10000)
         .with_development())

# Use batch operations
neurons = brain.get_all_neurons()
neurons.activate_all(1.0)
```

### For Real-time Applications

```python
# Target specific update rates
update_interval = 0.001  # 1kHz

# Optimize for real-time
brain = nlm.create_brain()
brain.initialize()

# Use time.sleep to maintain exact update rates
while running:
    loop_start = time.time()
    
    # Simulation steps...
    
    elapsed = time.time() - loop_start
    sleep_time = max(0, update_interval - elapsed)
    time.sleep(sleep_time)
```

## Migration Guide

### From C++ API

If migrating from the C++ API to Python, note these key differences:

1. **Memory Management**: Python handles memory automatically
2. **Error Handling**: Use try-catch with NLM exceptions
3. **Type Hints**: Use type annotations for better IDE support
4. **Documentation**: Access docstrings with help()

### Performance Considerations

1. **NumPy Integration**: Use NumPy for numerical operations
2. **Batch Processing**: Use batch operations for efficiency
3. **Caching**: Cache frequently accessed data
4. **Profiling**: Use Python profiling tools

## Conclusion

The NLM Python interface provides a simple, intuitive way to work with the neural simulation framework. With its comprehensive documentation, robust error handling, and performance optimizations, it's ideal for both beginners and advanced users.

Key features:
- **Simplicity**: Easy-to-use API with minimal boilerplate
- **Power**: Full access to all NLM features and capabilities
- **Performance**: Optimized for both small and large simulations
- **Flexibility**: Extensible design for custom applications
- **Reliability**: Robust error handling and comprehensive testing

Start exploring the power of neural simulation with the NLM Python interface today!