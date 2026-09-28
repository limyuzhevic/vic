# NLM Python Bindings - Enhanced Interface

A comprehensive Pythonic interface for the NLM (Neural Learning Machine) neural simulation framework.

## Overview

This document describes the enhanced Python bindings for NLM, featuring Pythonic convenience methods, improved error handling, and comprehensive type improvements.

## Key Enhancements

### 1. Pythonic Convenience Methods

#### Property Accessors
Instead of the C++ style `brain.getTotalNeuronCount()`, you can now use:
- `brain.neuron_count` - Total neuron count
- `brain.synapse_count` - Total synapse count  
- `brain.active_neuron_count` - Active neuron count
- `brain.firing_neuron_count` - Currently firing neurons
- `brain.average_firing_rate` - Average firing rate
- `brain.ei_ratio` - Excitation/inhibition ratio
- `brain.total_spike_count` - Total spike count
- `world.getTime()` - Simulation time (alias)

#### Context Manager Support
Brains can now be used with context managers for automatic resource cleanup:

```python
with pynlm.createBrain(config) as brain:
    brain.initialize()
    # Brain will be automatically reset when exiting context
```

#### Factory Methods with Sensible Defaults

```python
# Create a default configuration
config = pynlm.createDefaultConfig()

# Create optimized brain for real-time simulation
brain = pynlm.createOptimizedBrain()

# Create a standard world
world = pynlm.createDefaultWorld()

# Create a simulation environment with custom settings
env = pynlm.createSimulationEnvironment(width=20, height=20, visionWidth=8, visionHeight=8)

# Create a complete simulation setup
setup = pynlm.createSimulationSetup()
```

#### Enhanced Simulation Methods

```python
# Run multiple steps efficiently
brain.stepSim(steps=1000, timestep=0.1)

# Run a complete episode with brain and world
pynlm.runSimulationEpisode(brain, world, steps=1000, timestep=0.1)

# Process a complete agent episode
agent.processEpisode(world, steps=500, timestep=0.1)
```

### 2. Improved Error Handling

#### Custom Exception Types
- `pynlm.RuntimeError` - Runtime errors
- `pynlm.InvalidArgumentError` - Invalid argument errors
- `pynlm.OutOfRangeError` - Out of range errors

#### Enhanced Validation

```python
# Validate configuration
validation = pynlm.validate_config(config)
print(f"Configuration valid: {validation['valid']}")
print(f"Message: {validation['message']}")

# Validate brain statistics
validation = pynlm.validate_simulation_state(brain, world)
```

#### Better Error Messages

```python
# Invalid arguments raise descriptive errors
try:
    world = pynlm.create_simulated_environment()
    result = world.applyMotorCommand(None, 0.0)
except pynlm.InvalidArgumentError as e:
    print(f"Error: {e}")
```

### 3. Enhanced Brain API

#### Statistics Aggregation

```python
# Get comprehensive statistics as a dictionary
stats = brain.get_statistics()
print(f"Total neurons: {stats['neuron_count']}")
print(f"Firing percentage: {stats['firing_percentage']:.1f}%")

# Analyze brain statistics
analysis = pynlm.analyze_brain_statistics(brain)
print(f"Average firing rate: {analysis['average_firing_rate']:.3f}")
print(f"Excitatory synapse percentage: {analysis['excitatory_synapse_percentage']:.1f}%")
```

#### Configuration Helpers

```python
# Get brain configuration as dictionary
brain_config = pynlm.createPyConfig()
brain_config.set("brain.neuron_count", 2000)

# Set brain configuration from Python dictionary
pynlm.set_brain_config(brain_config, {"neuron_count": 1500, "synapse_density": 0.12})
```

### 4. Enhanced World API

#### World Management

```python
# Create environment with objects
env = pynlm.create_simulated_environment(width=30, height=30)

# Add objects with helper methods
env.add_resource(x=5.0, y=5.0, value=1.0)
env.add_hazard(x=25.0, y=25.0, damage=-0.5)
env.add_wall(x=15.0, y=15.0, radius=0.5)

# Update world efficiently
env.updateWorld(steps=100, timestep=0.1)
```

#### Simulation Control

```python
# Run complete episode
env.reset()
env.setAgentStart(5.0, 5.0)
for _ in range(500):
    env.update(0.1)
    # ... your logic here
```

### 5. Enhanced Agent API

#### Agent Management

```python
# Create and configure agent
agent = pynlm.createAgentBrain(brain)
agent.enableAllSystems()  # Enable all subsystems
agent.disableAllSystems() # Disable all subsystems

# Get system status
status = agent.getSystemStatus()
print(f"Reward modulation: {status['reward_modulation']}")
print(f"Curiosity level: {status['curiosity_level']:.3f}")
```

#### Episode Processing

```python
# Process complete episode with world
agent.processEpisode(world, steps=500, timestep=0.1)
```

### 6. Type Improvements

#### Type Conversion Utilities

```python
# Format IDs with custom prefixes
neuron_id = pynlm.to_neuron_id(123)
print(f"Formatted: {pynlm.format_neuron_id(neuron_id, prefix='N')}")
print(f"Standard: {pynlm.format_neuron_id(neuron_id)}")

# String conversion
print(f"As string: {pynlm.neuron_id_to_str(neuron_id)}")
```

#### Action Utilities

```python
# Create actions
move_action = pynlm.create_action(pynlm.ActionType.MoveForward)
look_action = pynlm.create_action(pynlm.ActionType.Look, [0.5, 0.3])

# Get action names
action_names = pynlm.get_action_names()
print(f"Available actions: {list(action_names)[:5]}...")

# Get action names
print(f"MoveForward: {pynlm.get_action_name(pynlm.ActionType.MoveForward)}")
```

#### Human-Readable Formatting

```python
# Format counts
print(f"Neurons: {pynlm.format_neuron_count(1500)}")
print(f"Spikes: {pynlm.format_spike_count(5000)}")
print(f"Synapses: {pynlm.format_synapse_count(12345)}")
```

#### Type Hints and Documentation

```python
# Get version information
version_info = pynlm.get_version_info()
print(f"Version: {version_info['version']}")
print(f"Build: {version_info['build']}")

# Get list of available features
features = pynlm.get_feature_list()
print(f"Number of features: {len(features)}")
for feature in features[:10]:
    print(f"  - {feature}")
```

### 7. Memory Management Utilities

```python
# Clean up resources
pynlm.cleanup_brain(brain)
pynlm.cleanup_world(world)
pynlm.cleanup_agent(agent)
```

### 8. Simulation Constants and Utilities

```python
# Get simulation constants
constants = pynlm.get_simulation_constants()
print(f"Default timestep: {constants['default_timestep']}")
print(f"Default neuron count: {constants['default_neuron_count']}")
```

## Example Usage

### Complete Agent Simulation Example

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with enhanced Pythonic interface."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.create_simulated_environment(width=20, height=20)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableAllSystems()
    
    # 7. Run simulation loop with enhanced methods
    for step in range(num_steps):
        # Update world efficiently
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command
        world.applyMotorCommand(motor_cmd, world.getTime())
        
        # Print progress periodically
        if step % 100 == 0:
            stats = brain.get_statistics()
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.firing_neuron_count}")
            print(f"  Neuron percentage: {stats['firing_percentage']:.1f}%")
            print(f"  Time: {world.getTime():.1f}")
    
    # 8. Cleanup with context managers
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        # ... additional processing
    
    return True

# Run the simulation
run_agent_simulation(1000)
```

### Configuration Management Example

```python
import pynlm
import json

# Create Python-friendly config
config = pynlm.createPyConfig()

# Set brain parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.08)

# Get brain configuration as dictionary
brain_params = pynlm.get_brain_config(config)
print(f"Brain parameters: {brain_params}")

# Validate configuration
validation = pynlm.validate_config(config)
if not validation['valid']:
    print(f"Configuration issues: {validation['message']}")

# Export configuration
with open("config.json", "w") as f:
    json.dump(pynlm.config_to_dict(config), f, indent=2)
```

## Installation

### From Source

```bash
# Install the package
cd /path/to/nlm
pip install -e .

# Or build and install
pip install build
python -m build
pip install dist/*.whl
```

### Using with NLM

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain created with {brain.neuron_count} neurons")

# Run simulation
for i in range(100):
    brain.step(i)
    if i % 10 == 0:
        print(f"Step {i}: {brain.firing_neuron_count} firing neurons")
```

## Features Summary

### Core Pythonic Features
- ✅ Pythonic property accessors
- ✅ Context manager support
- ✅ Factory methods with defaults
- ✅ String representations and debugging
- ✅ Enhanced error handling

### API Enhancements
- ✅ Statistics aggregation methods
- ✅ Simulation episode methods
- ✅ Configuration helper methods
- ✅ World simulation methods
- ✅ Agent management methods

### Type and Utility Improvements
- ✅ Type conversion utilities
- ✅ Human-readable formatting
- ✅ Action utilities
- ✅ Validation methods
- ✅ Memory management utilities

### Development Features
- ✅ Comprehensive documentation
- ✅ Type hints and introspection
- ✅ Feature discovery
- ✅ System information utilities

## Testing

Run the test suite to verify all Pythonic enhancements:

```bash
python test_bindings.py
```

## Requirements

- Python 3.8+
- C++20 compiler with pybind11 support
- scikit-build-core
- pytest (optional, for testing)

## Build Instructions

See the main README for detailed build instructions:
- [HOW_TO_USE.md](HOW_TO_USE.md) - Building and installation
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture
- [docs/SCIENCE.md](docs/SCIENCE.md) - Scientific background

## Support and Issues

For support and issue reporting:
- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Documentation: https://nlm.readthedocs.io

## License

MIT License - See LICENSE file for details.