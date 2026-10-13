# NLM Enhanced Python API Reference

## Overview

This document provides comprehensive reference documentation for the enhanced Python bindings (`pynlm_enhanced`) of the NLM framework. These bindings provide Pythonic APIs for easy configuration, brain control, and simulation management.

## Module Structure

The enhanced Python bindings are organized into the following modules:

### Core Classes

1. **PythonConfig** - Python-style configuration management
2. **PythonBrain** - Enhanced brain interface with monitoring
3. **PythonAgent** - Python-friendly agent control
4. **PythonSimpleWorld** - Enhanced world simulation
5. **PythonDevConsole** - Interactive development console

### Submodules

1. **nlm.simulation** - Simulation utilities and helpers
2. **nlm.types** - Python type definitions for NLM identifiers
3. **nlm.exceptions** - Custom exception classes

## PythonConfig Class

### Description

Python-style configuration management with enhanced type safety and Python-friendly APIs.

### Constructor

```python
def __init__(self, filepath="")
```

**Parameters**:
- `filepath` (str): Path to configuration file (optional)

**Example**:
```python
# Create with default configuration
config = PythonConfig()

# Create with file configuration
config = PythonConfig("configs/default.cfg")
```

### Methods

#### Configuration Loading

```python
def load_from_file(self, filepath)
```

Load configuration from file.

**Parameters**:
- `filepath` (str): Path to configuration file

**Returns**:
- bool: True if successful, False otherwise

```python
def load_from_dict(self, settings)
```

Load configuration from dictionary.

**Parameters**:
- `settings` (dict): Dictionary of configuration settings

**Returns**:
- bool: True if successful, False otherwise

**Example**:
```python
config = PythonConfig()
settings = {
    'neuron_count': 500,
    'connection_probability': 0.15,
    'simulation_timestep': 0.001,
    'random_seed': 42,
    'enable_reward_modulation': True,
    'enable_curiosity': True
}
config.load_from_dict(settings)
```

#### Value Access

```python
def get_int(self, key, default_value=0)
```

Get integer value with Python-friendly default.

**Parameters**:
- `key` (str): Configuration key
- `default_value` (int): Default value (optional)

**Returns**:
- int: Integer value or default

```python
def get_float(self, key, default_value=0.0)
```

Get float value with Python-friendly default.

**Parameters**:
- `key` (str): Configuration key
- `default_value` (float): Default value (optional)

**Returns**:
- float: Float value or default

```python
def get_bool(self, key, default_value=False)
```

Get boolean value with Python-friendly default.

**Parameters**:
- `key` (str): Configuration key
- `default_value` (bool): Default value (optional)

**Returns**:
- bool: Boolean value or default

```python
def get_string(self, key, default_value="")
```

Get string value with Python-friendly default.

**Parameters**:
- `key` (str): Configuration key
- `default_value` (str): Default value (optional)

**Returns**:
- str: String value or default

```python
def get_vector(self, key)
```

Get vector value.

**Parameters**:
- `key` (str): Configuration key

**Returns**:
- list[float]: Vector values or empty list

#### Value Setting

```python
def set_int(self, key, value)
```

Set integer value.

**Parameters**:
- `key` (str): Configuration key
- `value` (int): Integer value

```python
def set_float(self, key, value)
```

Set float value.

**Parameters**:
- `key` (str): Configuration key
- `value` (float): Float value

```python
def set_bool(self, key, value)
```

Set boolean value.

**Parameters**:
- `key` (str): Configuration key
- `value` (bool): Boolean value

```python
def set_string(self, key, value)
```

Set string value.

**Parameters**:
- `key` (str): Configuration key
- `value` (str): String value

#### Configuration Information

```python
def get_stats(self)
```

Get configuration statistics.

**Returns**:
- dict: Dictionary containing configuration statistics

```python
def validate(self)
```

Validate configuration and return errors.

**Returns**:
- list[str]: List of validation errors (empty if valid)

```python
def get_help(self, key="")
```

Get help for configuration key.

**Parameters**:
- `key` (str): Configuration key (optional)

**Returns**:
- str: Help text

```python
def summary(self)
```

Get configuration summary.

**Returns**:
- str: Configuration summary

### PythonBrain Class

### Description

Enhanced brain interface with Python-friendly APIs for brain control and monitoring.

### Constructor

```python
def __init__(self, config)
```

**Parameters**:
- `config` (PythonConfig): Configuration object

### Methods

```python
def initialize(self)
```

Initialize the brain with current configuration.

**Returns**:
- bool: True if successful, False otherwise

```python
def reset(self)
```

Reset brain to initial state.

```python
def get_total_neuron_count(self)
```

Get total neuron count.

**Returns**:
- int: Total neuron count

```python
def get_total_synapse_count(self)
```

Get total synapse count.

**Returns**:
- int: Total synapse count

```python
def get_active_neuron_count(self)
```

Get active neuron count.

**Returns**:
- int: Active neuron count

```python
def get_firing_neuron_count(self)
```

Get firing neuron count.

**Returns**:
- int: Firing neuron count

```python
def get_average_firing_rate(self)
```

Get average firing rate.

**Returns**:
- float: Average firing rate

```python
def get_excitation_inhibition_ratio(self)
```

Get excitation/inhibition ratio.

**Returns**:
- float: Excitation/inhibition ratio

```python
def get_total_spike_count(self)
```

Get total spike count.

**Returns**:
- int: Total spike count

```python
def get_developmental_stage(self)
```

Get developmental stage.

**Returns**:
- float: Developmental stage

```python
def set_developmental_stage(self, stage)
```

Set developmental stage.

**Parameters**:
- `stage` (float): Developmental stage

```python
def get_developmental_stage_name(self)
```

Get developmental stage name.

**Returns**:
- str: Developmental stage name

```python
def receive_sensory_input(self, input)
```

Inject sensory input.

**Parameters**:
- `input` (SensoryInput): Sensory input object

```python
def produce_action(self)
```

Produce motor action.

**Returns**:
- Action: Motor action object

```python
def step(self, step_number, time=0.0)
```

Perform simulation step.

**Parameters**:
- `step_number` (int): Step number
- `time` (float): Current time (optional)

```python
def get_brain_summary(self)
```

Get brain summary as dictionary.

**Returns**:
- dict: Brain summary

### PythonAgent Class

### Description

Python-friendly agent interface for controlling brain behavior and interaction with world.

### Constructor

```python
def __init__(self, brain)
```

**Parameters**:
- `brain` (PythonBrain): Brain object

### Methods

```python
def initialize(self, world)
```

Initialize with world.

**Parameters**:
- `world` (PythonSimpleWorld): World object

```python
def process_sensory_input(self, percept)
```

Process sensory percept and inject into brain.

**Parameters**:
- `percept` (SensoryPercept): Sensory percept object

```python
def decode_motor_command(self)
```

Decode brain motor activity into motor command.

**Returns**:
- Action: Motor command object

```python
def apply_reward_modulation(self, reward, predicted_reward=0.0)
```

Apply reward-based neuromodulation.

**Parameters**:
- `reward` (float): Reward value
- `predicted_reward` (float): Predicted reward (optional)

```python
def update_development(self, timestep)
```

Update development system.

**Parameters**:
- `timestep` (float): Time step

```python
def enable_reward_modulation(self, enable)
```

Enable/disable reward modulation.

**Parameters**:
- `enable` (bool): Enable flag

```python
def enable_structural_plasticity(self, enable)
```

Enable/disable structural plasticity.

**Parameters**:
- `enable` (bool): Enable flag

```python
def enable_development(self, enable)
```

Enable/disable development.

**Parameters**:
- `enable` (bool): Enable flag

```python
def enable_curiosity(self, enable)
```

Enable/disable curiosity.

**Parameters**:
- `enable` (bool): Enable flag

```python
def get_developmental_stage(self)
```

Get developmental stage.

**Returns**:
- float: Developmental stage

```python
def get_neuromodulation_level(self)
```

Get neuromodulation level.

**Returns**:
- float: Neuromodulation level

```python
def get_curiosity_level(self)
```

Get curiosity level.

**Returns**:
- float: Curiosity level

```python
def get_novelty_level(self)
```

Get novelty level.

**Returns**:
- float: Novelty level

```python
def get_prediction_error(self)
```

Get prediction error.

**Returns**:
- float: Prediction error

```python
def get_agent_summary(self)
```

Get agent summary as dictionary.

**Returns**:
- dict: Agent summary

### PythonSimpleWorld Class

### Description

Python-friendly simple world for NLM simulation.

### Constructor

```python
def __init__(self)
```

### Methods

```python
def configure(self, width, height, vision_width, vision_height)
```

Configure world dimensions and vision.

**Parameters**:
- `width` (int): World width
- `height` (int): World height
- `vision_width` (int): Vision width
- `vision_height` (int): Vision height

```python
def configure(self, width, height)
```

Configure world dimensions with default vision.

**Parameters**:
- `width` (int): World width
- `height` (int): World height

```python
def reset(self)
```

Reset world to initial state.

```python
def set_agent_start(self, x, y)
```

Set agent start position.

**Parameters**:
- `x` (float): X position
- `y` (float): Y position

```python
def update(self, timestep)
```

Update world by timestep.

**Parameters**:
- `timestep` (float): Time step

```python
def apply_motor_command(self, action, current_time)
```

Apply motor command and get result.

**Parameters**:
- `action` (Action): Motor command object
- `current_time` (float): Current time

**Returns**:
- ActionResult: Action result object

```python
def get_sensory_percept(self)
```

Get sensory percept from world.

**Returns**:
- SensoryPercept: Sensory percept object

```python
def get_agent_body(self)
```

Get agent body state.

**Returns**:
- AgentBody: Agent body object

```python
def add_world_object(self, obj)
```

Add world object.

**Parameters**:
- `obj` (WorldObject): World object

```python
def remove_world_object(self, x, y)
```

Remove world object.

**Parameters**:
- `x` (float): X position
- `y` (float): Y position

```python
def is_valid_position(self, x, y)
```

Check if position is valid.

**Parameters**:
- `x` (float): X position
- `y` (float): Y position

**Returns**:
- bool: True if position is valid

```python
def get_width(self)
```

Get world width.

**Returns**:
- int: World width

```python
def get_height(self)
```

Get world height.

**Returns**:
- int: World height

```python
def get_vision_width(self)
```

Get vision width.

**Returns**:
- int: Vision width

```python
def get_vision_height(self)
```

Get vision height.

**Returns**:
- int: Vision height

```python
def get_max_energy(self)
```

Get max energy.

**Returns**:
- int: Max energy

```python
def set_max_energy(self, e)
```

Set max energy.

**Parameters**:
- `e` (int): Max energy

```python
def get_energy_decay_rate(self)
```

Get energy decay rate.

**Returns**:
- float: Energy decay rate

```python
def set_energy_decay_rate(self, r)
```

Set energy decay rate.

**Parameters**:
- `r` (float): Energy decay rate

```python
def get_simulation_time(self)
```

Get simulation time.

**Returns**:
- float: Simulation time

```python
def get_vision_as_array(self)
```

Get vision as 2D array.

**Returns**:
- list[list[float]]: Vision as 2D array

```python
def get_world_summary(self)
```

Get world summary as dictionary.

**Returns**:
- dict: World summary

### PythonDevConsole Class

### Description

Python-friendly development console for runtime debugging and monitoring.

### Constructor

```python
def __init__(self)
```

### Methods

```python
def initialize(self, config, brain, development_system)
```

Initialize console with brain and config.

**Parameters**:
- `config` (PythonConfig): Configuration object
- `brain` (PythonBrain): Brain object
- `development_system` (DevelopmentSystem): Development system object

```python
def process_input(self, input)
```

Process console input and return success.

**Parameters**:
- `input` (str): Console input

**Returns**:
- bool: True if successful, False otherwise

```python
def get_history(self)
```

Get command history.

**Returns**:
- list[dict]: Command history

```python
def clear_history(self)
```

Clear command history.

```python
def is_active(self)
```

Check if console is active.

**Returns**:
- bool: True if console is active

```python
def stop(self)
```

Stop the development console.

### nlm.simulation Submodule

### Description

Simulation utilities for running NLM simulations with Pythonic APIs.

### Functions

```python
def run_simulation(brain, world, agent, num_steps, timestep=0.1, verbose=False)
```

Run a simple simulation loop.

**Parameters**:
- `brain` (PythonBrain): Brain object
- `world` (PythonSimpleWorld): World object
- `agent` (PythonAgent): Agent object
- `num_steps` (int): Number of steps
- `timestep` (float): Time step (optional)
- `verbose` (bool): Verbose output (optional)

**Returns**:
- list[dict]: Simulation results

```python
def run_learning_simulation(brain, world, agent, num_steps, reward_rate=0.01, timestep=0.1, verbose=False)
```

Run a learning simulation with reward modulation.

**Parameters**:
- `brain` (PythonBrain): Brain object
- `world` (PythonSimpleWorld): World object
- `agent` (PythonAgent): Agent object
- `num_steps` (int): Number of steps
- `reward_rate` (float): Reward rate (optional)
- `timestep` (float): Time step (optional)
- `verbose` (bool): Verbose output (optional)

**Returns**:
- list[dict]: Simulation results

```python
def run_exploration_simulation(brain, world, agent, num_steps, timestep=0.1, enable_curiosity=True, verbose=False)
```

Run an exploration simulation with curiosity.

**Parameters**:
- `brain` (PythonBrain): Brain object
- `world` (PythonSimpleWorld): World object
- `agent` (PythonAgent): Agent object
- `num_steps` (int): Number of steps
- `timestep` (float): Time step (optional)
- `enable_curiosity` (bool): Enable curiosity (optional)
- `verbose` (bool): Verbose output (optional)

**Returns**:
- list[dict]: Simulation results

## Ready-to-Use Examples

### Basic Simulation

```python
import pynlm_enhanced as nlm

# Create brain with configuration
config = nlm.PythonConfig({
    'neuron_count': 500,
    'connection_probability': 0.15,
    'simulation_timestep': 0.001,
    'random_seed': 42
})

brain = nlm.PythonBrain(config)
brain.initialize()

# Create world
world = nlm.PythonSimpleWorld()
world.configure(20, 20, 8, 8)

# Create agent
agent = nlm.PythonAgent(brain)
agent.initialize(world)

# Run simulation
results = nlm.simulation.run_simulation(brain, world, agent, 1000, verbose=True)

# Print results
for step in results:
    print(f"Step {step['step']}: "
          f"{step['neuron_count']} neurons, "
          f"{step['firing_neuron_count']} firing, "
          f"action: {step['action_type']}")
```

### Learning Simulation

```python
import pynlm_enhanced as nlm

# Create brain with learning features
config = nlm.PythonConfig({
    'neuron_count': 500,
    'connection_probability': 0.15,
    'simulation_timestep': 0.001,
    'enable_reward_modulation': True,
    'enable_curiosity': True,
    'random_seed': 42
})

brain = nlm.PythonBrain(config)
brain.initialize()

world = nlm.PythonSimpleWorld()
world.configure(10, 10, 6, 6)

agent = nlm.PythonAgent(brain)
agent.initialize(world)

# Run learning simulation
results = nlm.simulation.run_learning_simulation(
    brain, world, agent, 2000, reward_rate=0.02, verbose=True
)

# Analyze learning
rewards = [r['reward'] for r in results if 'reward' in r]
if rewards:
    print(f"Max reward: {max(rewards)}")
    print(f"Average reward: {sum(rewards)/len(rewards)}")
```

### Exploration Simulation

```python
import pynlm_enhanced as nlm

# Create brain with exploration features
config = nlm.PythonConfig({
    'neuron_count': 300,
    'connection_probability': 0.1,
    'simulation_timestep': 0.001,
    'enable_curiosity': True,
    'enable_novelty': True,
    'random_seed': 42
})

brain = nlm.PythonBrain(config)
brain.initialize()

world = nlm.PythonSimpleWorld()
world.configure(15, 15, 5, 5)

agent = nlm.PythonAgent(brain)
agent.initialize(world)

# Run exploration simulation
results = nlm.simulation.run_exploration_simulation(
    brain, world, agent, 1500, enable_curiosity=True, verbose=True
)

# Analyze exploration
for step in results:
    if step['curiosity_level'] > 0.5:
        print(f"Step {step['step']}: High curiosity {step['curiosity_level']:.2f}")
    if step['novelty_level'] > 0.3:
        print(f"Step {step['step']}: Novel experience {step['novelty_level']:.2f}")
```

### Development Console Usage

```python
import pynlm_enhanced as nlm

# Create brain and config
config = nlm.PythonConfig({'neuron_count': 500})
brain = nlm.PythonBrain(config)
brain.initialize()

# Create development console
console = nlm.PythonDevConsole()
console.initialize(config, brain, development_system)

# Start console (this will run interactively)
console.run(continuous=True)

# Or use in non-interactive mode
console.process_input("brain")  # Show brain status
console.process_input("set neuron_count 1000")  # Modify config
console.process_input("step 100")  # Advance simulation
console.process_input("status")  # Show comprehensive status
console.process_input("help")  # Show help
```

## Error Handling

### Custom Exceptions

The enhanced bindings include custom exception classes:

```python
from pynlm_enhanced.exceptions import (
    NLMError,
    ConfigurationError,
    SimulationError,
    BrainError
)
```

### Exception Handling Examples

```python
import pynlm_enhanced as nlm
from pynlm_enhanced.exceptions import ConfigurationError

try:
    config = nlm.PythonConfig({
        'neuron_count': 1000000,  # Too large
        'invalid_key': 'value'     # Invalid key
    })
    brain = nlm.PythonBrain(config)
    brain.initialize()
except ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.simulation.SimulationError as e:
    print(f"Simulation error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Considerations

### Memory Usage

- **Large networks**: Use `enable_memory_pools` configuration option
- **Sparse connectivity**: Use `enable_sparse_connectivity` for large networks
- **Simulation steps**: Monitor memory usage with development console

### Performance Tuning

```python
# Enable performance optimizations
config = nlm.PythonConfig({
    'enable_simd': True,
    'enable_multithreading': True,
    'enable_memory_pools': True,
    'enable_sparse_connectivity': True
})

# Reduce memory footprint
config.set_int('neuron_count', 1000)
config.set_float('simulation_timestep', 0.01)

# Enable development console for performance monitoring
console = nlm.PythonDevConsole()
console.initialize(config, brain, development_system)
```

## Installation

```bash
# Install using pip
pip install pynlm-enhanced

# Or install from source
pip install -e .
```

## Dependencies

- Python 3.7 or higher
- pybind11
- NumPy (optional, for array operations)

## License

MIT License - See LICENSE file for details

## Version

Version 1.0.0-enhanced