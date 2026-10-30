# NLM Python Package - Enhanced Wrapper

This directory contains the enhanced Python interface for the NLM (Neural Learning Machine) neural simulation framework.

## Overview

The enhanced Python wrapper provides a comprehensive, Pythonic API for NLM that builds upon the low-level C++ bindings (pynlm) to offer better usability, error handling, and development tools.

## Package Structure

```
python/
├── __init__.py              # Package initialization
├── nlm_enhanced_wrapper.py   # Core enhanced wrapper implementation
├── nlm_documentation.md     # Complete documentation
└── README.md               # This README file
```

## Installation

### Using pip (Development Mode)

```bash
cd /path/to/nlm
pip install -e python/
```

### Using from Source

Add the python directory to your Python path:

```python
import sys
sys.path.insert(0, '/path/to/nlm/python')

import pynlm
from python import nlm_enhanced_wrapper as nlm
```

## Quick Start Examples

### Example 1: Basic Usage

```python
import pynlm
from python import nlm_enhanced_wrapper as nlm

# Create brain with configuration
brain = nlm.create_brain({
    "neuron_count": 1000,
    "region_count": 2
})

# Initialize brain
brain.initialize()

# Create world and agent
world = nlm.create_world()
world.configure(20, 20, 8, 8)
agent = nlm.create_agent_brain(brain, world)
agent.initialize(world)

# Run simulation
results = nlm.run_simulation(brain, world, agent, 100)

print(f"Simulation completed: {results['steps_completed']} steps")
print(f"Max firing rate: {results['max_firing_rate']:.3f}")
```

### Example 2: Agent with Learning

```python
import pynlm
from python import nlm_enhanced_wrapper as nlm

# Setup with configuration
config = {
    "neuron_count": 1000,
    "region_count": 2,
    "random_seed": 123
}

brain = nlm.create_brain(config)
brain.initialize()
world = nlm.create_world()
world.configure(15, 15, 8, 8)
world.reset()
agent = nlm.create_agent_brain(brain, world)
agent.initialize(world)

# Enable learning features
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_structural_plasticity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())

print("Agent simulation complete!")
```

## Core Features

### 1. Enhanced Error Handling

```python
from python.nlm_enhanced_wrapper import (
    NLMError,
    ConfigurationError,
    SimulationError
)

# Better error handling with specific exception types
try:
    config = {"invalid": "value"}
    validated = nlm.ConfigValidator.validate_config(config)
except ConfigurationError as e:
    print(f"Configuration error: {e}")
except NLMError as e:
    print(f"NLM error: {e}")
```

### 2. Configuration Validation

```python
from python.nlm_enhanced_wrapper import ConfigValidator

# Automatic validation with defaults
config = {"neuron_count": 500}  # Missing parameters get defaults
validated = ConfigValidator.validate_config(config)

# Full validation with error reporting
config = {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15
}

validated = ConfigValidator.validate_config(config)
# All required parameters present, all values valid
```

### 3. Pythonic Wrapper Classes

#### SensoryPerceptWrapper

```python
# Access sensory data as numpy arrays
percept = world.get_sensory_percept()

vision = percept.get_vision()  # numpy array (width, height, 3)
touch = percept.get_touch()   # numpy array (8,)
internal = percept.get_internal()  # numpy array (4,)

# Get metadata
timestamp = percept.get_timestamp()
shape = percept.get_vision_shape()
```

#### ActionWrapper

```python
# Handle actions with enum support
action = agent.decode_motor_command()
action_type = action.get_type()  # ActionType enum
parameters = action.get_parameters()  # numpy array

# Use enum for type checking
from python.nlm_enhanced_wrapper import ActionType
if action_type == ActionType.TURN_LEFT:
    # Handle turn left action
    pass
```

#### WorldWrapper

```python
# World management with Pythonic API
world = nlm.create_world()
world.configure(20, 20, 8, 8)
world.reset()

# Update world
world.update(0.1)

# Get agent state
agent_body = world.get_agent_body()
print(f"Agent energy: {agent_body['energy']}")
```

#### AgentWrapper

```python
# Enhanced agent interface with development support
agent = nlm.create_agent_brain(brain, world)
agent.initialize(world)

# Process sensory input
percept = world.get_sensory_percept()
agent.process_sensory_input(percept)

# Decode actions
action = agent.decode_motor_command()
agent.apply_motor_command(action)

# Control development
agent.update_development(0.1)
agent.set_development_stage(nlm.DevelopmentalStage.ADULT)

# Feature toggles
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
```

#### BrainWrapper

```python
# Enhanced brain interface with monitoring
brain = nlm.create_brain()
brain.initialize()

# Step through simulation
brain.step(0)
brain.step(1, time=0.1)

# Get statistics
stats = {
    'neuron_count': brain.get_total_neuron_count(),
    'firing_rate': brain.get_average_firing_rate(),
    'active_neurons': brain.get_active_neuron_count(),
    'total_spikes': brain.get_total_spike_count()
}
```

### 4. Simulation Utilities

#### Event System

```python
from python.nlm_enhanced_wrapper import add_simulation_callback

def log_step(step_data):
    print(f"Step {step_data['step']}: "
          f"Firing rate: {step_data['firing_rate']:.3f}")

add_simulation_callback(log_step)
```

#### Batch Processing

```python
def run_multiple_scenarios(scenarios):
    """Run multiple simulation scenarios"""
    results = {}
    
    for name, config in scenarios.items():
        brain = nlm.create_brain(config)
        brain.initialize()
        world = nlm.create_world()
        world.configure(**config.get('world_config', {}))
        
        agent = nlm.create_agent_brain(brain, world)
        agent.initialize(world)
        
        # Configure agent based on scenario
        if config.get('enable_learning', False):
            agent.enable_reward_modulation(True)
        
        # Run simulation
        results[name] = nlm.run_simulation(
            brain, world, agent, config.get('steps', 100)
        )
    
    return results
```

#### Results Management

```python
from python.nlm_enhanced_wrapper import save_simulation_results, load_simulation_results

# Save simulation results
results = nlm.run_simulation(brain, world, agent, 100)
save_simulation_results(results, "experiment_results.json")

# Load and analyze
loaded_results = load_simulation_results("experiment_results.json")
print(f"Max firing rate: {loaded_results['max_firing_rate']}")
```

## Configuration Schema

The enhanced wrapper uses a strict configuration schema with validation:

| Parameter | Type | Range | Default | Description |
|-----------|------|-------|---------|-------------|
| neuron_count | int | 10-10000 | 500 | Number of neurons |
| region_count | int | 1-100 | 1 | Number of brain regions |
| connection_probability | float | 0.0-1.0 | 0.15 | Synaptic connection probability |
| random_seed | int | 0-2147483647 | 42 | Random seed for reproducibility |
| simulation_timestep | float | 0.0001-1.0 | 0.001 | Time step duration |
| And 6 more parameters... |

## Available Examples

The `EXAMPLE_SCRIPTS` dictionary provides ready-to-run examples:

```python
from python.nlm_enhanced_wrapper import EXAMPLE_SCRIPTS

# List available examples
print(list(EXAMPLE_SCRIPTS.keys()))
# ['silent_brain', 'brain_watching_world', 'complete_agent']

# Run an example
script = EXAMPLE_SCRIPTS['silent_brain']
# Execute using exec() or similar
```

### Example Scripts

1. **silent_brain**: Basic brain operation
2. **brain_watching_world**: Sensory processing in environment
3. **complete_agent**: Full agent with learning enabled

## API Documentation

### Import Statements

```python
import pynlm
from python import nlm_enhanced_wrapper as nlm
```

### Core Classes

- `ConfigValidator`: Configuration validation and schema system
- `SensoryPerceptWrapper`: Enhanced sensory perception interface
- `ActionWrapper`: Improved action handling with enum support
- `WorldWrapper`: Pythonic world interface with utilities
- `AgentWrapper`: Agent brain interface with development features
- `BrainWrapper`: Enhanced brain interface with monitoring

### Key Functions

- `create_brain(config=None)`: Creates brain with optional configuration
- `create_world()`: Creates simple world
- `create_agent_brain(brain, world)`: Creates agent brain interface
- `run_simulation(brain, world, agent, num_steps, ...)`: Runs complete simulation
- `save_simulation_results(results, filepath)`: Saves results to JSON
- `load_simulation_results(filepath)`: Loads results from JSON

### Error Classes

- `NLMError`: Base exception for all NLM-related errors
- `ConfigurationError`: Specific to configuration validation errors
- `SimulationError`: Specific to simulation execution errors

### Enums

- `ActionType`: Action types for motor control
- `DevelopmentalStage`: Brain development stages

## Testing

The package includes comprehensive unit tests:

```bash
python -m pytest tests/test_python_wrapper.py -v
```

Or run all tests:

```python
python -c "from tests.test_python_wrapper import run_all_tests; run_all_tests()"
```

## Development Guidelines

### 1. Python Best Practices

- Use snake_case for variables and functions
- Use CamelCase for class names
- Include docstrings for all public functions
- Use type hints where appropriate

### 2. Error Handling

```python
# Use specific exception types
from python.nlm_enhanced_wrapper import ConfigurationError

try:
    config = validate_config(user_input)
except ConfigurationError as e:
    log_error(f"Invalid configuration: {e}")
```

### 3. Performance

```python
# For large simulations, use batch processing
results = nlm.run_simulation(brain, world, agent, 10000)

# Monitor memory and performance
import time
start_time = time.time()
results = nlm.run_simulation(brain, world, agent, 1000)
elapsed = time.time() - start_time
print(f"Simulation took {elapsed:.2f} seconds")
```

## Migration from pynlm

If you're currently using the raw pynlm module, migration is simple:

### Before (pynlm only):

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)
```

### After (enhanced wrapper):

```python
import pynlm
from python import nlm_enhanced_wrapper as nlm

brain = nlm.create_brain()
brain.initialize()
brain.step(0)
```

The enhanced wrapper maintains full compatibility while providing:
- Better error handling
- More Pythonic API
- Additional utilities
- Comprehensive documentation
- Example scripts

## Acknowledgements

This enhanced Python wrapper extends the excellent work of the NLM development team:

- C++ core implementation by NLM team
- pybind11 bindings by NLM team
- Original documentation and examples

The enhanced wrapper provides a user-friendly interface to these powerful capabilities.

## License

Part of the NLM project, licensed under MIT License.

For more information, visit: https://github.com/nlm-project/nlm