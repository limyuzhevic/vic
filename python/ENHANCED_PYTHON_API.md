"""
Enhanced NLM Python API Reference
===================================

This document provides comprehensive reference for the Enhanced NLM Python API.

The enhanced API provides a modern, Pythonic interface to the NLM (Neural Learning Machine)
neural simulation framework while maintaining backward compatibility.

Table of Contents
================

1. Overview
2. Configuration API
3. Brain API
4. Agent API
5. World API
6. Convenience Functions
7. Constants
8. Error Handling
9. Examples
10. Migration Guide

1. Overview
============

The Enhanced NLM Python API provides a high-level interface to the NLM neural simulation framework
with the following key features:

- **Simplified Configuration**: Factory functions and builders for easy setup
- **Preset Configurations**: Pre-configured scenarios for common use cases
- **Enhanced Wrapper Classes**: Intuitive APIs for brain, agent, and world interaction
- **Comprehensive Error Handling**: Automatic validation and informative error messages
- **Statistics Collection**: Built-in monitoring and reporting
- **State Management**: Easy save/load functionality
- **Backward Compatibility**: Legacy API maintained for existing code

The enhanced API is designed to be:
- **Pythonic**: Follows Python best practices and conventions
- **Intuitive**: Easy to learn and use for beginners
- **Powerful**: Provides advanced features for power users
- **Robust**: Comprehensive error handling and validation
- **Flexible**: Supports both simple and complex use cases

2. Configuration API
====================

The configuration API provides multiple ways to create and manage brain configurations.

2.1. Factory Functions
--------------------

### nlm.create_default_config()
Create a default configuration with sensible defaults.

**Returns**: `nlm.Config` object
**Example**:
```python
import nlm

config = nlm.create_default_config()
```

### nlm.create_config(neuron_count, connection_probability, simulation_timestep, random_seed)
Create a configuration with specific parameters.

**Parameters**:
- `neuron_count`: Number of neurons (default: 1000)
- `connection_probability`: Connection probability (default: 0.05)
- `simulation_timestep`: Simulation timestep (default: 0.001)
- `random_seed`: Random seed (default: 42)

**Returns**: `nlm.Config` object
**Example**:
```python
import nlm

config = nlm.create_config(2000, 0.08, 0.002, 12345)
```

### nlm.create_config_from_json(json_string)
Create configuration from JSON string.

**Parameters**:
- `json_string`: JSON configuration string

**Returns**: `nlm.Config` object
**Example**:
```python
import nlm

json_config = '{"brain.neuron_count": 1500, "brain.connection_probability": 0.1}'
config = nlm.create_config_from_json(json_config)
```

2.2. Configuration Builder
------------------------

### nlm.ConfigBuilder()
Create a configuration builder for fluent API.

**Returns**: `nlm.ConfigBuilder` object
**Example**:
```python
import nlm

config = (nlm.ConfigBuilder()
          .set_neuron_count(2000)
          .set_connection_probability(0.08)
          .set_simulation_timestep(0.01)
          .set_random_seed(12345)
          .enable_plasticity(True)
          .set_learning_rate(0.005)
          .preset("learning")
          .build())
```

### ConfigBuilder Methods

- `set_neuron_count(count)`: Set number of neurons
- `set_connection_probability(prob)`: Set connection probability
- `set_simulation_timestep(timestep)`: Set simulation timestep
- `set_random_seed(seed)`: Set random seed
- `enable_plasticity(enable)`: Enable/disable plasticity
- `set_learning_rate(rate)`: Set STDP learning rate
- `preset(preset_name)`: Apply a preset configuration
- `build()`: Build and return the configuration

2.3. Preset Configurations
------------------------

### nlm.apply_preset(config, preset_name)
Apply a preset configuration to an existing configuration.

**Parameters**:
- `config`: `nlm.Config` object
- `preset_name`: Name of preset ("default", "fast", "detailed", "learning", "stable")

**Example**:
```python
import nlm

config = nlm.create_default_config()
nlm.apply_preset(config, "learning")
```

### Available Presets

- **default**: Standard configuration with balanced parameters
- **fast**: Low neuron count, high timestep for quick testing
- **detailed**: High neuron count, low timestep for detailed simulation
- **learning**: Strong plasticity for rapid learning
- **stable**: Weak plasticity for stable behavior

### nlm.get_available_presets()
Get list of available preset names.

**Returns**: List of preset names
**Example**:
```python
import nlm

presets = nlm.get_available_presets()
print(presets)  # ["default", "fast", "detailed", "learning", "stable"]
```

2.4. Configuration Validation
-----------------------------

### nlm.validate_config(config)
Validate configuration and return errors if any.

**Parameters**:
- `config`: `nlm.Config` object to validate

**Returns**: Tuple (bool, list) where bool indicates if valid, list contains error messages
**Example**:
```python
import nlm

config = nlm.create_config(1000, 0.05)
valid, errors = nlm.validate_config(config)
if not valid:
    print("Validation errors:", errors)
```

### nlm.serialize_config(config)
Serialize configuration to string.

**Parameters**:
- `config`: `nlm.Config` object to serialize

**Returns**: Serialized configuration string
**Example**:
```python
import nlm

config = nlm.create_config(1000, 0.05)
serialized = nlm.serialize_config(config)
```

### nlm.deserialize_config(serialized)
Deserialize configuration from string.

**Parameters**:
- `serialized`: Serialized configuration string

**Returns**: `nlm.Config` object
**Example**:
```python
import nlm

serialized = "brain.neuron_count = 1000\nbrain.connection_probability = 0.05"
config = nlm.deserialize_config(serialized)
```

3. Brain API
============

The Brain API provides access to neural simulation functionality.

3.1. Basic Brain Operations
--------------------------

### nlm.create_brain(neuron_count, connection_probability, simulation_timestep, random_seed)
Create a brain with specified configuration.

**Parameters**:
- `neuron_count`: Number of neurons (default: 1000)
- `connection_probability`: Connection probability (default: 0.05)
- `simulation_timestep`: Simulation timestep (default: 0.001)
- `random_seed`: Random seed (default: 42)

**Returns**: `nlm.Brain` object
**Example**:
```python
import nlm

brain = nlm.create_brain(1000, 0.05)
```

### brain.initialize()
Initialize the brain.

**Example**:
```python
brain.initialize()
```

### brain.run_simulation(steps, log_progress)
Run simulation for specified number of steps.

**Parameters**:
- `steps`: Number of simulation steps
- `log_progress`: Log progress every 100 steps (default: False)

**Example**:
```python
brain.run_simulation(1000, log_progress=True)
```

### brain.step(step_number)
Run a single simulation step.

**Parameters**:
- `step_number`: Current step number

**Example**:
```python
brain.step(0)
```

3.2. Brain Statistics
--------------------

### brain.get_statistics()
Get brain statistics as dictionary.

**Returns**: Dictionary containing brain statistics
**Keys**:
- `total_neurons`: Total number of neurons
- `total_synapses`: Total number of synapses
- `firing_neurons`: Number of currently firing neurons
- `average_firing_rate`: Average firing rate (Hz)
- `excitation_inhibition_ratio`: E/I balance
- `developmental_stage`: Current developmental stage
- `total_spikes`: Total number of spikes
- `concept_count`: Number of learned concepts (if concept formation enabled)
- `planner_available`: Whether planner is available (if enabled)

**Example**:
```python
import nlm

brain = nlm.create_brain()
brain.initialize()

stats = brain.get_statistics()
print(f"Neurons: {stats['total_neurons']}")
print(f"Spikes: {stats['total_spikes']}")
print(f"Firing: {stats['firing_neurons']}")
```

3.3. Brain State Management
--------------------------

### brain.save_state(filename)
Save brain state to file.

**Parameters**:
- `filename`: File to save state to

**Example**:
```python
brain.save_state("brain_checkpoint.nml")
```

### brain.load_state(filename)
Load brain state from file.

**Parameters**:
- `filename`: File to load state from

**Example**:
```python
brain.load_state("brain_checkpoint.nml")
```

### brain.reset()
Reset brain to initial state.

**Example**:
```python
brain.reset()
```

3.4. Brain Properties
---------------------

### brain.get_total_neuron_count()
Get total number of neurons.

**Returns**: Number of neurons
**Example**:
```python
neurons = brain.get_total_neuron_count()
```

### brain.get_total_synapse_count()
Get total number of synapses.

**Returns**: Number of synapses
**Example**:
```python
synapses = brain.get_total_synapse_count()
```

### brain.get_firing_neuron_count()
Get number of currently firing neurons.

**Returns**: Number of firing neurons
**Example**:
```python
firing = brain.get_firing_neuron_count()
```

### brain.get_average_firing_rate()
Get average firing rate.

**Returns**: Average firing rate (Hz)
**Example**:
```python
rate = brain.get_average_firing_rate()
```

### brain.get_excitation_inhibition_ratio()
Get excitation-inhibition balance.

**Returns**: E/I ratio
**Example**:
```python
ei_ratio = brain.get_excitation_inhibition_ratio()
```

### brain.get_developmental_stage()
Get current developmental stage.

**Returns**: Developmental stage (0=Initial, 1=CriticalPeriod, 2=Maturation, 3=Adult, 4=Aging)
**Example**:
```python
stage = brain.get_developmental_stage()
```

### brain.get_total_spike_count()
Get total number of spikes.

**Returns**: Total spike count
**Example**:
```python
spikes = brain.get_total_spike_count()
```

4. Agent API
============

The Agent API provides access to agent functionality for world interaction.

4.1. Basic Agent Operations
--------------------------

### nlm.create_agent(brain)
Create an agent for a brain.

**Parameters**:
- `brain`: `nlm.Brain` object to control

**Returns**: `nlm.Agent` object
**Example**:
```python
import nlm

brain = nlm.create_brain()
agent = nlm.create_agent(brain)
```

### agent.initialize(world)
Initialize agent with world.

**Parameters**:
- `world`: `nlm.World` object to interact with

**Example**:
```python
agent.initialize(world)
```

### agent.process_sensory_input(percept)
Process sensory input from world.

**Parameters**:
- `percept`: Dictionary containing sensory input

**Returns**: Dictionary containing processed state
**Example**:
```python
percept = world.get_sensory_percept()
processed = agent.process_sensory_input(percept)
```

### agent.decode_motor_command()
Decode motor command from brain activity.

**Returns**: Motor command as string ("MoveForward", "MoveBackward", "TurnLeft", "TurnRight", "Interact", "Wait")
**Example**:
```python
action = agent.decode_motor_command()
```

4.2. Agent Statistics
--------------------

### agent.get_agent_statistics()
Get agent statistics as dictionary.

**Returns**: Dictionary containing agent statistics
**Keys**:
- `curiosity_level`: Curiosity level (0.0 to 1.0)
- `novelty_level`: Novelty level (0.0 to 1.0)
- `prediction_error`: Prediction error
- `neuromodulation_level`: Neuromodulation level
- `developmental_stage`: Current developmental stage
- `motor_activities`: Dictionary of motor group firing rates

**Example**:
```python
stats = agent.get_agent_statistics()
print(f"Curiosity: {stats['curiosity_level']}")
print(f"Novelty: {stats['novelty_level']}")
```

4.3. Agent Control
-----------------

### agent.enable_reward_modulation(enable)
Enable/disable reward modulation.

**Parameters**:
- `enable`: Boolean indicating whether to enable reward modulation

**Example**:
```python
agent.enable_reward_modulation(True)
```

### agent.enable_structural_plasticity(enable)
Enable/disable structural plasticity.

**Parameters**:
- `enable`: Boolean indicating whether to enable structural plasticity

**Example**:
```python
agent.enable_structural_plasticity(True)
```

### agent.enable_development(enable)
Enable/disable development.

**Parameters**:
- `enable`: Boolean indicating whether to enable development

**Example**:
```python
agent.enable_development(True)
```

### agent.enable_curiosity(enable)
Enable/disable curiosity.

**Parameters**:
- `enable`: Boolean indicating whether to enable curiosity

**Example**:
```python
agent.enable_curiosity(True)
```

### agent.apply_reward_modulation(reward, predicted_reward)
Apply reward modulation.

**Parameters**:
- `reward`: Actual reward received
- `predicted_reward`: Predicted reward

**Example**:
```python
agent.apply_reward_modulation(10.0, 5.0)
```

### agent.update_development(timestep)
Update agent development.

**Parameters**:
- `timestep`: Time step for development update

**Example**:
```python
agent.update_development(0.1)
```

4.4. Agent Properties
---------------------

### agent.get_curiosity_level()
Get curiosity level.

**Returns**: Curiosity level (0.0 to 1.0)
**Example**:
```python
curiosity = agent.get_curiosity_level()
```

### agent.get_novelty_level()
Get novelty level.

**Returns**: Novelty level (0.0 to 1.0)
**Example**:
```python
novelty = agent.get_novelty_level()
```

### agent.get_prediction_error()
Get prediction error.

**Returns**: Prediction error
**Example**:
```python
error = agent.get_prediction_error()
```

5. World API
============

The World API provides access to world simulation functionality.

5.1. Basic World Operations
--------------------------

### nlm.create_world()
Create a world.

**Returns**: `nlm.World` object
**Example**:
```python
import nlm

world = nlm.create_world()
```

### world.configure_world(width, height, vision_width, vision_height)
Configure world dimensions.

**Parameters**:
- `width`: World width
- `height`: World height
- `vision_width`: Vision width (default: 8)
- `vision_height`: Vision height (default: 8)

**Example**:
```python
world.configure_world(20, 20, 8, 8)
```

### world.set_agent_start(x, y)
Set agent's starting position.

**Parameters**:
- `x`: X coordinate
- `y`: Y coordinate

**Example**:
```python
world.set_agent_start(10.0, 10.0)
```

### world.reset_world()
Reset world to initial state.

**Example**:
```python
world.reset_world()
```

### world.update_world(timestep)
Update world state.

**Parameters**:
- `timestep`: Time step for update

**Example**:
```python
world.update_world(0.1)
```

5.2. World Interaction
---------------------

### world.get_sensory_percept()
Get sensory percept as dictionary.

**Returns**: Dictionary containing sensory percept
**Keys**:
- `vision`: List of vision values
- `touch`: List of touch values
- `internal`: List of internal values
- `proprioception`: List of proprioception values
- `text`: Text content (if any)
- `simulation_time`: Current simulation time
- `agent_position`: Dictionary with x, y, angle

**Example**:
```python
percept = world.get_sensory_percept()
print(f"Vision: {percept['vision']}")
print(f"Touch: {percept['touch']}")
```

### world.apply_motor_command(command, time)
Apply motor command to world.

**Parameters**:
- `command`: Motor command as string
- `time`: Current time

**Example**:
```python
world.apply_motor_command("MoveForward", world.get_simulation_time())
```

5.3. World Statistics
---------------------

### world.get_world_statistics()
Get world statistics as dictionary.

**Returns**: Dictionary containing world statistics
**Keys**:
- `width`: World width
- `height`: World height
- `vision_width`: Vision width
- `vision_height`: Vision height
- `simulation_time`: Current simulation time
- `max_energy`: Maximum energy
- `energy_decay_rate`: Energy decay rate

**Example**:
```python
stats = world.get_world_statistics()
print(f"Width: {stats['width']}")
print(f"Height: {stats['height']}")
print(f"Time: {stats['simulation_time']}")
```

5.4. World Configuration
------------------------

### world.set_random_seed(seed)
Set random seed for reproducible results.

**Parameters**:
- `seed`: Random seed

**Example**:
```python
world.set_random_seed(12345)
```

6. Convenience Functions
=======================

### nlm.create_enhanced_brain(neuron_count, connection_probability, simulation_timestep, random_seed, preset)
Create a brain with enhanced wrapper and preset.

**Parameters**:
- `neuron_count`: Number of neurons (default: 1000)
- `connection_probability`: Connection probability (default: 0.05)
- `simulation_timestep`: Simulation timestep (default: 0.001)
- `random_seed`: Random seed (default: 42)
- `preset`: Preset name (default: "default")

**Returns**: `nlm.Brain` object with enhanced wrapper
**Example**:
```python
import nlm

brain = nlm.create_enhanced_brain(preset="learning")
```

### nlm.create_enhanced_agent(brain)
Create an enhanced agent for a brain.

**Parameters**:
- `brain`: `nlm.Brain` object to control

**Returns**: `nlm.Agent` object with enhanced wrapper
**Example**:
```python
import nlm

brain = nlm.create_brain()
agent = nlm.create_enhanced_agent(brain)
```

### nlm.create_enhanced_world()
Create an enhanced world.

**Returns**: `nlm.World` object with enhanced wrapper
**Example**:
```python
import nlm

world = nlm.create_enhanced_world()
```

### nlm.get_version()
Get NLM version information.

**Returns**: Version string
**Example**:
```python
import nlm

version = nlm.get_version()
print(f"NLM version: {version}")
```

7. Constants
============

### nlm.DEFAULT_TIMESTEP
Default simulation timestep.

**Value**: 0.001

### nlm.BRAIN_V_THRESH
Brain threshold potential.

**Value**: -50.0 (mV)

### nlm.BRAIN_V_REST
Brain resting potential.

**Value**: -70.0 (mV)

### nlm.BRAIN_V_RESET
Brain reset potential.

**Value**: -75.0 (mV)

### nlm.BRAIN_TAU_MEM
Brain membrane time constant.

**Value**: 20.0 (ms)

### nlm.BRAIN_TAU_REF
Brain refractory period.

**Value**: 2.0 (ms)

### nlm.BRAIN_INITIAL_WEIGHT_MEAN
Initial synaptic weight mean.

**Value**: 0.5

### nlm.BRAIN_INITIAL_WEIGHT_STD
Initial synaptic weight standard deviation.

**Value**: 0.1
8. Error Handling
=================

### Exceptions

The enhanced NLM API raises the following exceptions:

- `nlm.NLMError`: Base exception class
- `nlm.InvalidArgumentError`: Invalid argument exception
- `nlm.OutOfRangeError`: Out of range exception

**Example**:
```python
import nlm

try:
    brain = nlm.create_brain(1000, 0.05)
    brain.initialize()
except nlm.NLMError as e:
    print(f"NLM error: {e}")
except nlm.InvalidArgumentError as e:
    print(f"Invalid argument: {e}")
except nlm.OutOfRangeError as e:
    print(f"Out of range: {e}")
```

9. Examples
===========

### Simple Simulation
```python
import nlm

# Create and initialize brain
brain = nlm.create_brain()
brain.initialize()

# Run simulation
brain.run_simulation(1000)

# Get statistics
stats = brain.get_statistics()
print(f"Simulation completed: {stats['total_spikes']} spikes")
```

### Agent-World Interaction
```python
import nlm

# Create brain and agent
brain = nlm.create_enhanced_brain(preset="learning")
brain.initialize()

world = nlm.create_enhanced_world()
agent = nlm.create_enhanced_agent(brain)

agent.initialize(world)
world.configure_world(20, 20, 8, 8)
world.set_agent_start(10.0, 10.0)

# Enable learning
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_development(True)

# Run interaction loop
for step in range(100):
    world.update_world(0.1)
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    brain.step(step)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())
    agent.apply_reward_modulation(0.0)
```

### Configuration Builder
```python
import nlm

# Use configuration builder for custom setup
config = (nlm.ConfigBuilder()
          .set_neuron_count(2000)
          .set_connection_probability(0.08)
          .set_simulation_timestep(0.01)
          .set_random_seed(12345)
          .enable_plasticity(True)
          .set_learning_rate(0.005)
          .preset("learning")
          .build())

brain = nlm.create_brain(config)
brain.initialize()
brain.run_simulation(100)
```

10. Migration Guide
===================

### From Old API to Enhanced API

The enhanced API maintains backward compatibility with the old NLM API while providing additional functionality and improved usability.

#### Old API (Original)
```python
import nlm

# Old way: complex initialization
config = nlm.create_default_config()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)

brain = nlm.create_brain(config)
brain.initialize()
brain.run_simulation(1000)
```

#### New API (Enhanced)
```python
import nlm

# New way: simple initialization
brain = nlm.create_brain(neuron_count=1000, learning_rate=0.001)
brain.initialize()
brain.run_simulation(1000)
```

#### Old API (Legacy Functions)
```python
import nlm.old_api

# Access old API functions
brain = nlm.old_api.create_brain()
world = nlm.old_api.create_world()
agent = nlm.old_api.create_agent(brain)
```

#### New API (Enhanced Functions)
```python
import nlm

# Use enhanced API functions
brain = nlm.create_enhanced_brain()
world = nlm.create_enhanced_world()
agent = nlm.create_enhanced_agent(brain)
```

### Migration Benefits

1. **Simplified API**: Reduced number of function calls and parameters
2. **Better Defaults**: Sensible defaults for common use cases
3. **Preset Configurations**: Pre-configured scenarios for common use cases
4. **Enhanced Error Handling**: Automatic validation and informative error messages
5. **Configuration Builder**: Fluent API for complex configurations
6. **Statistics Collection**: Built-in monitoring and reporting
7. **State Management**: Easy save/load functionality
8. **Backward Compatibility**: Legacy API maintained for existing code

### Migration Checklist

- [ ] Review existing code for direct function calls
- [ ] Replace simple function calls with enhanced versions
- [ ] Keep old-style code for complex configurations
- [ ] Use configuration builder for custom setups
- [ ] Use presets for common scenarios
- [ ] Leverage enhanced error handling
- [ ] Use statistics collection for monitoring
- [ ] Implement state management for long-running simulations

## Conclusion

The Enhanced NLM Python API provides a modern, Pythonic interface to the NLM neural simulation framework that:

1. **Simplifies Configuration**: Factory functions and builders for easy setup
2. **Provides Presets**: Pre-configured scenarios for common use cases
3. **Enhances Usability**: Intuitive APIs for brain, agent, and world interaction
4. **Improves Robustness**: Comprehensive error handling and validation
5. **Enables Monitoring**: Statistics collection and development tracking
6. **Maintains Compatibility**: Legacy API for existing code
7. **Supports Advanced Features**: Pythonic design for complex use cases

The enhanced API makes it easier to get started with NLM while providing powerful tools for advanced users and complex simulations.