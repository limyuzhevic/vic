# NLM Enhanced Python Wrapper - Complete Documentation

## Overview

The NLM Enhanced Python Wrapper provides a comprehensive, Pythonic interface for the NLM (Neural Learning Machine) neural simulation framework. It builds upon the low-level C++ bindings (pynlm) to provide an enhanced user experience with better error handling, configuration validation, simulation utilities, and development tools.

## Quick Start

```python
import pynlm
from python import nlm_enhanced_wrapper as nlm

# Create a brain with configuration
brain = nlm.create_brain({
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15
})

# Initialize the brain
brain.initialize()

# Create world and agent
world = nlm.create_world()
world.configure(20, 20, 8, 8)
agent = nlm.create_agent_brain(brain, world)
agent.initialize(world)

# Enable learning
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)

# Run simulation
results = nlm.run_simulation(brain, world, agent, 100)

print(f"Simulation completed: {results['steps_completed']} steps")
print(f"Final firing rate: {results['max_firing_rate']:.3f}")
print(f"Learning detected: {results['learning_detected']}")
```

## Installation

The enhanced Python wrapper is included with the NLM codebase:

```bash
# Navigate to the NLM project
cd /path/to/nlm

# Install the Python package in development mode
pip install -e python/

# Or, use directly from source
import sys
sys.path.insert(0, 'python')
import pynlm
from python import nlm_enhanced_wrapper as nlm
```

## Core Components

### 1. Error Handling Classes

```python
from python.nlm_enhanced_wrapper import (
    NLMError,
    ConfigurationError,
    SimulationError
)
```

These custom exception classes provide better error context and type checking:

- `NLMError`: Base exception for all NLM-related errors
- `ConfigurationError`: Specific to configuration validation errors
- `SimulationError`: Specific to simulation execution errors

### 2. Enum Classes

```python
from python.nlm_enhanced_wrapper import ActionType, DevelopmentalStage

# Action types
ActionType.MOVE_FORWARD      # pynlm.ActionType.MoveForward
ActionType.TURN_LEFT        # pynlm.ActionType.TurnLeft
ActionType.INTERACT         # pynlm.ActionType.Interact

# Developmental stages
DevelopmentalStage.INITIAL        # pynlm.DevelopmentalStage.Initial
DevelopmentalStage.CRITICAL_PERIOD  # pynlm.DevelopmentalStage.CriticalPeriod
DevelopmentalStage.ADULT          # pynlm.DevelopmentalStage.Adult
```

### 3. Configuration Validation

```python
from python.nlm_enhanced_wrapper import ConfigValidator

# Validate configuration
config = {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15
}

validated_config = ConfigValidator.validate_config(config)
```

The `ConfigValidator` class:
- Validates all configuration parameters against a schema
- Provides default values for missing parameters
- Performs type checking and range validation
- Raises `ConfigurationError` for invalid configurations

### 4. Wrapper Classes

#### SensoryPerceptWrapper

```python
percept_wrapper = sensory_percept_wrapper(percept)

# Access sensory data as numpy arrays
vision = percept_wrapper.get_vision()
touch = percept_wrapper.get_touch()
internal = percept_wrapper.get_internal()

# Get shape information
shape = percept_wrapper.get_vision_shape()
timestamp = percept_wrapper.get_timestamp()
```

#### ActionWrapper

```python
action_wrapper = ActionWrapper(action)

# Access action properties
action_type = action_wrapper.get_type()  # ActionType enum
parameters = action_wrapper.get_parameters()  # numpy array
name = action_wrapper.get_name()  # human-readable string

# Modify action
action_wrapper.set_type(ActionType.TURN_LEFT)
action_wrapper.set_parameters([1.0, 2.0, 3.0])
```

#### WorldWrapper

```python
world_wrapper = WorldWrapper(world)

# Configure world
world_wrapper.configure(20, 20, 8, 8)

# Update world
world_wrapper.update(0.1)

# Get sensory input
percept = world_wrapper.get_sensory_percept()

# Get agent state
agent_body = world_wrapper.get_agent_body()

# Control simulation
world_wrapper.reset()
```

#### AgentWrapper

```python
agent_wrapper = AgentWrapper(agent, brain_wrapper, world_wrapper)

# Process sensory input
agent_wrapper.process_sensory_input(percept_wrapper)

# Get actions
action = agent_wrapper.decode_motor_command()

# Apply learning
agent_wrapper.apply_reward_modulation(reward, predicted_reward)

# Control development
agent_wrapper.update_development(0.1)
agent_wrapper.set_development_stage(DevelopmentalStage.ADULT)

# Feature toggles
agent_wrapper.enable_reward_modulation(True)
agent_wrapper.enable_curiosity(True)
```

#### BrainWrapper

```python
brain_wrapper = BrainWrapper(brain)

# Initialize with configuration
brain_wrapper.initialize({
    "neuron_count": 1000,
    "region_count": 2
})

# Control simulation
brain_wrapper.step(0)
brain_wrapper.step(1, time=0.1)

# Inject currents
brain_wrapper.inject_current(0, 50.0)
brain_wrapper.inject_current_to_neurons("excitatory", 30.0)

# Get statistics
neurons = brain_wrapper.get_total_neuron_count()
synapses = brain_wrapper.get_total_synapse_count()
firing_rate = brain_wrapper.get_average_firing_rate()
```

### 5. Factory Functions

```python
from python.nlm_enhanced_wrapper import (
    create_brain,
    create_world,
    create_agent_brain
)

# Create components with validation
brain = create_brain({"neuron_count": 1000})
world = create_world()
agent = create_agent_brain(brain, world)
```

### 6. Simulation Utilities

```python
from python.nlm_enhanced_wrapper import (
    run_simulation,
    save_simulation_results,
    load_simulation_results,
    add_simulation_callback,
    remove_simulation_callback
)

# Run complete simulation with callbacks
def step_callback(step_data):
    print(f"Step {step_data['step']}: {step_data['firing_rate']:.3f} Hz")

add_simulation_callback(step_callback)

results = run_simulation(brain, world, agent, 100)

# Save/load results
save_simulation_results(results, "simulation_results.json")
loaded_results = load_simulation_results("simulation_results.json")
```

## Configuration Schema

The enhanced wrapper validates configuration parameters against the following schema:

| Parameter | Type | Minimum | Maximum | Default | Description |
|-----------|------|---------|---------|---------|-------------|
| neuron_count | int | 10 | 10000 | 500 | Number of neurons in brain |
| region_count | int | 1 | 100 | 1 | Number of brain regions |
| connection_probability | float | 0.0 | 1.0 | 0.15 | Probability of synaptic connections |
| random_seed | int | 0 | 2147483647 | 42 | Random seed for reproducibility |
| simulation_timestep | float | 0.0001 | 1.0 | 0.001 | Simulation time step (seconds) |
| stdp_ltp_weight | float | 0.0 | 1.0 | 0.02 | STDP long-term potentiation weight |
| stdp_ltd_weight | float | 0.0 | 1.0 | 0.015 | STDP long-term depression weight |
| stdp_tau | float | 1.0 | 1000.0 | 20.0 | STDP time constant |
| synaptogenesis_rate | float | 0.0 | 1.0 | 0.0001 | Rate of new synapse formation |
| pruning_rate | float | 0.0 | 1.0 | 0.00001 | Rate of synapse removal |

## Usage Examples

### Example 1: Silent Brain

```python
import pynlm
from python import nlm_enhanced_wrapper as nlm

# Simplest possible example
brain = nlm.create_brain()
brain.initialize()

for i in range(10):
    brain.step(i)

print(f"Silent brain test done! Neurons: {brain.get_total_neuron_count()}")
```

### Example 2: Brain Watching a World

```python
import pynlm
from python import nlm_enhanced_wrapper as nlm

# Setup
brain = nlm.create_brain()
brain.initialize()
world = nlm.create_world()
world.configure(20, 20, 8, 8)
world.reset()
agent = nlm.create_agent_brain(brain, world)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(i)

print(f"Watched world for 30 steps")
print(f"Firing rate: {brain.get_average_firing_rate():.3f}")
```

### Example 3: Complete Agent with Learning

```python
import pynlm
from python import nlm_enhanced_wrapper as nlm

# Setup
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

# Enable learning
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_structural_plasticity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())

    if step % 20 == 0:
        print(f"Step {step}: {brain.get_firingNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

## Advanced Usage

### 1. Custom Simulation Loops

```python
def custom_simulation_loop(brain, world, agent, num_steps, timestep=0.1):
    """Custom simulation loop with additional logic"""
    results = {
        'steps_completed': 0,
        'spike_history': [],
        'action_history': [],
        'reward_history': []
    }
    
    for step in range(num_steps):
        # Custom logic can be inserted here
        if step % 10 == 0:
            print(f"Progress: {step}/{num_steps} steps")
        
        # Standard NLM loop
        world.update(timestep)
        agent.process_sensory_input(world.get_sensory_percept())
        brain.step(step)
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Collect custom data
        results['spike_history'].append(brain.get_totalSpikeCount())
        results['action_history'].append(action.get_type().name)
        results['reward_history'].append(agent.get_neuromodulation_level())
    
    results['steps_completed'] = num_steps
    return results
```

### 2. Batch Processing

```python
def batch_simulations(scenarios, num_steps_per_scenario=50):
    """Run multiple simulations with different configurations"""
    results = {}
    
    for scenario_name, config in scenarios.items():
        print(f"Running scenario: {scenario_name}")
        
        brain = nlm.create_brain(config)
        brain.initialize()
        world = nlm.create_world()
        world.configure(config.get('width', 20), config.get('height', 20), 
                       config.get('vision_width', 8), config.get('vision_height', 8))
        world.reset()
        
        agent = nlm.create_agent_brain(brain, world)
        agent.initialize(world)
        
        # Configure based on scenario
        if config.get('enable_reward_modulation', False):
            agent.enable_reward_modulation(True)
        if config.get('enable_curiosity', False):
            agent.enable_curiosity(True)
        
        # Run simulation
        scenario_results = nlm.run_simulation(
            brain, world, agent, num_steps_per_scenario
        )
        
        results[scenario_name] = scenario_results
    
    return results
```

### 3. Integration with Machine Learning

```python
def create_training_data(simulation, num_episodes=100):
    """Extract training data for machine learning models"""
    training_data = []
    
    for episode in range(num_episodes):
        # Create fresh simulation
        brain = nlm.create_brain()
        brain.initialize()
        world = nlm.create_world()
        world.configure(10, 10, 8, 8)
        world.reset()
        agent = nlm.create_agent_brain(brain, world)
        agent.initialize(world)
        
        episode_data = {
            'step_data': [],
            'final_state': None,
            'total_reward': 0
        }
        
        for step in range(50):
            # Get sensory input
            percept = world.get_sensory_percept()
            
            # Store features for ML model
            features = {
                'vision': percept.get_vision().flatten(),
                'touch': percept.get_touch(),
                'internal': percept.get_internal(),
                'firing_rate': brain.get_averageFiringRate(),
                'action': agent.decodeMotorCommand().get_type().name
            }
            
            episode_data['step_data'].append(features)
            
            # Apply action
            world.update(0.1)
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        training_data.append(episode_data)
    
    return training_data
```

## Performance and Monitoring

### 1. Event System

```python
from python.nlm_enhanced_wrapper import add_simulation_callback

def log_step(step_data):
    """Log step data for performance analysis"""
    print(f"Step {step_data['step']}: "
          f"Firing={step_data['firing_neurons']}, "
          f"Rate={step_data['firing_rate']:.3f}, "
          f"Action={step_data['action_type']}")

add_simulation_callback(log_step)
```

### 2. Memory Monitoring

```python
import psutil
import os

def monitor_memory(step_data):
    """Monitor memory usage during simulation"""
    process = psutil.Process(os.getpid())
    memory_info = process.memory_info()
    
    print(f"Step {step_data['step']}: "
          f"Memory usage: {memory_info.rss / 1024 / 1024:.2f} MB")
```

### 3. Performance Analysis

```python
from python.nlm_enhanced_wrapper import run_simulation
import time

def benchmark_simulation(brain, world, agent, num_steps):
    """Benchmark simulation performance"""
    start_time = time.time()
    start_memory = psutil.Process(os.getpid()).memory_info().rss
    
    results = run_simulation(brain, world, agent, num_steps)
    
    end_time = time.time()
    end_memory = psutil.Process(os.getpid()).memory_info().rss
    
    performance_stats = {
        'simulation_time': end_time - start_time,
        'memory_before': start_memory,
        'memory_after': end_memory,
        'memory_increase': end_memory - start_memory,
        'steps_per_second': num_steps / (end_time - start_time),
        'memory_per_step': (end_memory - start_memory) / num_steps
    }
    
    return results, performance_stats
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: "Configuration Error" when creating brain
**Solution:** Check that all configuration parameters are valid:

```python
# Check parameter ranges
config = {
    "neuron_count": 1000,  # Must be 10-10000
    "connection_probability": 0.15,  # Must be 0.0-1.0
}

# Or use defaults
brain = nlm.create_brain()  # Uses all defaults
```

#### Issue 2: Simulation fails with "RuntimeError"
**Solution:** Check that all components are properly initialized:

```python
# Always initialize components in correct order
brain = nlm.create_brain()
brain.initialize()  # Must be called first

world = nlm.create_world()
world.configure(10, 10, 8, 8)
world.reset()

agent = nlm.create_agent_brain(brain, world)
agent.initialize(world)  # Requires world and brain
```

#### Issue 3: Performance issues with large simulations
**Solution:** Optimize simulation parameters:

```python
# Use smaller simulation for testing
config = {
    "neuron_count": 100,  # Smaller for faster testing
    "region_count": 2,
}

# Or use batch processing
brain = nlm.create_brain(config)
brain.initialize()
```

### Error Messages

| Error Type | Message | Solution |
|------------|---------|----------|
| ConfigurationError | "Config parameter 'X' must be an integer" | Check parameter type and range |
| SimulationError | "Simulation failed at step X: Error message" | Check for exceptions in your simulation loop |
| NLMError | Generic NLM error | Check NLM documentation for specific error conditions |

## API Reference

### Class Reference

#### ConfigValidator
- `validate_config(config)`: Validates and normalizes configuration
- `DEFAULT_SCHEMA`: Schema definition for all configuration parameters

#### SensoryPerceptWrapper
- `get_vision()`: Returns vision data as numpy array
- `get_vision_shape()`: Returns vision data shape
- `get_touch()`: Returns touch data as numpy array
- `get_internal()`: Returns internal signals as numpy array
- `get_proprioception()`: Returns proprioception data as numpy array
- `get_all_signals()`: Returns all sensory signals as flattened numpy array
- `get_timestamp()`: Returns percept timestamp
- `set_timestamp()`: Sets percept timestamp

#### ActionWrapper
- `get_type()`: Returns action type as ActionType enum
- `set_type()`: Sets action type from ActionType enum
- `get_parameters()`: Returns action parameters as numpy array
- `set_parameters()`: Sets action parameters
- `get_name()`: Returns human-readable action name

#### WorldWrapper
- `configure()`: Configures world dimensions
- `reset()`: Resets world state
- `update()`: Updates world for one timestep
- `get_sensory_percept()`: Returns sensory percept as wrapper
- `get_agent_body()`: Returns agent body state as dictionary
- `add_world_object()`: Adds world object from dictionary
- `remove_world_object()`: Removes world object
- `is_valid_position()`: Checks if position is valid
- `get_stats()`: Returns world statistics
- `set_random_seed()`: Sets random seed for reproducibility

#### AgentWrapper
- `process_sensory_input()`: Processes sensory input
- `decode_motor_command()`: Decodes motor command
- `apply_reward_modulation()`: Applies reward neuromodulation
- `update_development()`: Updates development system
- `get_developmental_stage()`: Returns current developmental stage
- `get_neuromodulation_level()`: Returns neuromodulation level
- `get_curiosity_level()`: Returns curiosity level
- `get_novelty_level()`: Returns novelty level
- `get_prediction_error()`: Returns prediction error
- `reset()`: Resets agent for new episode
- `enable_reward_modulation()`: Enables/disables reward modulation
- And more feature toggles...

#### BrainWrapper
- `initialize()`: Initializes brain with configuration
- `step()`: Advances brain simulation
- `receive_sensory_input()`: Receives sensory input
- `inject_current()`: Injects current into specific neuron
- `inject_current_to_neurons()`: Injects current into neurons of specific type
- `produce_action()`: Produces motor action
- `reset()`: Resets brain state
- `save()`: Saves brain state to file
- `load()`: Loads brain state from file
- `add_region()`: Adds new neural region
- `get_region_ids()`: Gets all region IDs
- `get_total_neuron_count()`: Gets total neuron count
- `get_total_synapse_count()`: Gets total synapse count
- `get_active_neuron_count()`: Gets active neuron count
- `get_firing_neuron_count()`: Gets firing neuron count
- `get_average_firing_rate()`: Gets average firing rate
- `get_excitation_inhibition_ratio()`: Gets E/I balance ratio
- `get_total_spike_count()`: Gets total spike count
- `get_developmental_stage()`: Returns developmental stage
- `set_developmental_stage()`: Sets developmental stage

### Function Reference

#### Factory Functions
- `create_brain(config=None)`: Creates brain with optional configuration
- `create_world()`: Creates simple world
- `create_agent_brain(brain, world)`: Creates agent brain interface

#### Simulation Utilities
- `run_simulation(brain, world, agent, num_steps, timestep=0.1, callbacks_enabled=True)`: Runs complete simulation
- `save_simulation_results(results, filepath)`: Saves simulation results to JSON
- `load_simulation_results(filepath)`: Loads simulation results from JSON
- `add_simulation_callback(callback, event_type="step")`: Adds simulation callback
- `remove_simulation_callback(callback)`: Removes simulation callback

### Constants and Enums

#### ActionType
- `MOVE_FORWARD`: Move forward action
- `MOVE_BACKWARD`: Move backward action
- `TURN_LEFT`: Turn left action
- `TURN_RIGHT`: Turn right action
- And more action types...

#### DevelopmentalStage
- `INITIAL`: Initial developmental stage
- `CRITICAL_PERIOD`: Critical period stage
- `MATURATION`: Maturation stage
- `ADULT`: Adult stage
- `AGING`: Aging stage

## Migration Guide

### From pynlm to nlm_enhanced_wrapper

If you're currently using the raw pynlm module, here's how to migrate:

#### Before (pynlm only):

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)

world = pynlm.createSimpleWorld()
world.configure(10, 10, 8, 8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

#### After (with enhanced wrapper):

```python
import pynlm
from python import nlm_enhanced_wrapper as nlm

brain = nlm.create_brain()
brain.initialize()
brain.step(0)

world = nlm.create_world()
world.configure(10, 10, 8, 8)
agent = nlm.create_agent_brain(brain, world)
agent.initialize(world)
```

The enhanced wrapper provides the same core functionality but with:
- Better error handling and validation
- More Pythonic API with wrapper classes
- Additional utilities and helpers
- Example scripts and documentation

## Development Guidelines

### 1. Python Style Guidelines

- Use snake_case for variable and function names
- Use CamelCase for class names
- Type hints are recommended but not required
- Docstrings should follow Google/NumPy format

### 2. Configuration Best Practices

- Always validate configuration before use
- Use schema-defined parameter names
- Provide meaningful error messages
- Use defaults when parameters are not specified

### 3. Simulation Best Practices

- Initialize components in correct order
- Use consistent timestep throughout simulation
- Monitor performance for large simulations
- Use callbacks for logging and monitoring
- Save results regularly for long simulations

### 4. Testing Guidelines

- Write unit tests for all wrapper classes
- Test error handling scenarios
- Test edge cases and boundary conditions
- Include integration tests with actual NLM simulation
- Use temporary files and cleanup properly

## Acknowledgements

This enhanced Python wrapper builds upon the excellent work of the NLM development team:

- C++ core implementation by NLM team
- pybind11 bindings by NLM team
- Original documentation and examples

The enhanced wrapper extends this work with Pythonic APIs, better error handling, and additional development tools.

## License

This enhanced Python wrapper is part of the NLM project and is licensed under the MIT License. See LICENSE file for details.

For more information about the NLM project, visit https://github.com/nlm-project/nlm