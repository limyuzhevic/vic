# Comprehensive NLM Python Bindings - Build Instructions

## Overview

This repository contains pynlm, a Python binding for NLM (Neural Learning Machine), an experimental computational brain project. The Python bindings provide a comprehensive interface for creating and interacting with neural brains, agents, and simulation environments.

## Installation and Building

### Prerequisites

- Python 3.8 or higher
- C++ compiler with C++20 support
- CMake 3.16 or higher
- pybind11

### Building the Python Module

1. **Standard Build**

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

2. **With Python Tests**

```bash
# Build the main module
make pynlm

# Build and run Python tests
python -m pytest python/test_pynlm.py -v
```

3. **Installation**

```bash
# Install the module
pip install python/dist/pynlm*.whl

# Or install in development mode
pip install -e python/
```

## Key Features

### 1. Pythonic API Design

The bindings are designed with Python best practices in mind:

- **Context managers** for resource management
- **Property access** for intuitive object interaction
- **Type hints** and comprehensive documentation
- **Error handling** with clear Python exceptions
- **Convenience methods** for common operations

### 2. Core Components

#### Configuration
```python
import pynlm as nlm

# Create default configuration
config = nlm.create_config()

# Load from file
config.load_from_file("config.json")

# Save to file
config.save_to_file("my_config.json")
```

#### Brain
```python
# Create brain with configuration
brain = nlm.create_brain(config)
brain.initialize()

# Perform simulation steps
brain.step(1)
brain.step(2, 0.5)  # With timestamp

# Get brain information
print(f"Neurons: {brain.get_total_neuron_count()}")
print(f"Regions: {brain.get_region_count()}")
```

#### AgentBrain
```python
# Create agent brain interface
agent = nlm.create_agent_brain(brain)
agent.initialize(world)

# Process sensory input
percept = world.get_sensory_percept()
agent.process_sensory_input(percept)

# Get motor command
action = agent.decode_motor_command()
```

#### World
```python
# Create and configure world
world = nlm.create_world()
world.configure(100, 100, 20, 20)

# Get agent state
body = world.get_agent_body()
print(f"Position: ({body.x}, {body.y})")

# Update simulation
world.update(1)
```

### 3. Advanced Features

#### Context Managers
```python
# Automatic resource management
with nlm.BrainContext(config) as brain:
    brain.initialize()
    # Brain automatically reset on exit
```

#### Development Stages
```python
# Create brains for different developmental stages
for stage in [nlm.DevelopmentalStage.Initial,
              nlm.DevelopmentalStage.Maturation,
              nlm.DevelopmentalStage.Adult]:
    brain = nlm.create_development_stage_brain(stage)
    brain.initialize()
```

#### Convenience Functions
```python
# Quick prototyping setup
agent, world = nlm.quick_prototype()

# Complete experimental setup
config, world, agent = nlm.create_experiment_setup(50, 50, 10, 10)

# Advanced agent with all systems
agent = nlm.create_advanced_agent(config, world)
```

#### Performance Optimization
```python
# Optimize brain for performance
optimized_brain = nlm.optimize_brain_for_performance(brain)

# Enable memory optimizations
optimized_brain = nlm.enable_memory_optimization(optimized_brain)
```

### 4. Debugging and Monitoring

```python
# Debug brain connections
nlm.debug_brain_connections(brain)

# Analyze brain efficiency
efficiency = nlm.analyze_brain_efficiency(brain)
print(f"Efficiency: {efficiency * 100:.1f}%")

# Monitor brain health
status = nlm.monitor_brain_health(brain)
print(f"Status: {status}")
```

### 5. Enum and Type Utilities

```python
# Convert between enum values and strings
print(nlm.neuron_type_to_string(nlm.NeuronType.Excitatory))
print(nlm.string_to_neuron_type("inhibitory"))

# Print all enum values
nlm.print_enum_values()
```

### 6. Simulation Examples

#### Basic Simulation Loop
```python
import pynlm as nlm

# Setup
world = nlm.create_world()
world.configure(100, 100, 20, 20)

brain = nlm.create_brain()
brain.initialize()

agent = nlm.create_agent_brain(brain)
agent.initialize(world)

# Main simulation loop
for step in range(100):
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    action = agent.decode_motor_command()
    world.update(step)
```

#### Advanced Simulation with All Systems
```python
import pynlm as nlm

# Create complete experimental setup
config, world, agent = nlm.create_experiment_setup(80, 80, 15, 15)

# Enable all systems
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)
agent.enable_curiosity(True)

# Run simulation with custom behavior
for step in range(50):
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    
    # Custom action selection based on neuromodulation
    curiosity = agent.get_curiosity_level()
    neuromodulation = agent.get_neuromodulation_level()
    
    if curiosity > 0.7:
        action = agent.decode_motor_command()  # Explore
    else:
        action = nlm.Action(nlm.ActionType.Look)  # Exploit
    
    agent.apply_reward_modulation(1.0, agent.get_prediction_error())
    world.update(step)
    agent.update_development(step)
```

## Testing

### Running Tests

```bash
# Run Python bindings tests
python -m pytest python/test_pynlm.py -v

# Run examples
python examples/quick_start.py
python examples/advanced_examples.py
```

### Test Coverage

The test suite includes:

- Configuration tests
- Brain functionality tests
- World interaction tests
- Action and neuron ID tests
- Convenience function tests
- Debug and utility function tests
- Experimental setup tests

## Examples Directory

The examples directory contains:

1. **quick_start.py** - Basic usage examples and getting started guide
2. **advanced_examples.py** - Advanced scenarios and performance optimization
3. Other example scripts for specific use cases

## API Reference

### Classes

#### Config
- `create_config()` - Create default configuration
- `load_from_file(filepath)` - Load configuration from JSON
- `save_to_file(filepath)` - Save configuration to JSON
- `get_keys()` - Get all configuration keys

#### Brain
- `create_brain(config)` - Create brain with configuration
- `initialize()` - Initialize the brain
- `step(currentStep, currentTime=None)` - Perform simulation step
- `receive_sensory_input(input)` - Inject sensory input
- `produce_action()` - Generate motor action
- `reset()` - Reset brain state

#### AgentBrain
- `create_agent_brain(brain)` - Create agent brain interface
- `initialize(world)` - Initialize with world
- `process_sensory_input(percept)` - Process sensory percept
- `decode_motor_command()` - Decode motor command
- `apply_reward_modulation(reward, predictedReward)` - Apply reward modulation

#### SimpleWorld
- `create_world()` - Create new world
- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `get_sensory_percept()` - Get current sensory input
- `get_agent_body()` - Get agent physical state
- `update(timestep)` - Update world state

#### Action
- `Action(type, parameters=None)` - Create action
- `get_type()` - Get action type
- `get_parameters()` - Get action parameters
- `get_name()` - Get human-readable name

#### WorldObject
- `WorldObject(x, y, type, value=0.0, radius=0.5)` - Create world object

#### SensoryPercept
- Contains vision, touch, internal, and audio sensory data

### Enums

#### NeuronType
- Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal

#### SynapseType
- Excitatory, Inhibitory, Modulatory, Electrical, GapJunction

#### DevelopmentalStage
- Initial, CriticalPeriod, Maturation, Adult, Aging

#### ActionType
- MoveForward, MoveBackward, MoveLeft, MoveRight, TurnLeft, TurnRight, etc.

#### WorldObjectType
- Empty, Resource, Hazard, Wall, Marker

### Functions

#### Factory Functions
- `create_config()` - Create default configuration
- `create_brain(config)` - Create brain
- `create_world()` - Create world
- `create_agent_brain(brain)` - Create agent brain

#### Convenience Functions
- `create_advanced_agent(config, world)` - Create advanced agent
- `create_training_environment(width, height, vision_width, vision_height)` - Create training environment
- `create_experiment_setup(width, height, vision_width, vision_height)` - Create complete experimental setup

#### Utilities
- `create_development_stage_brain(stage)` - Create brain with specific stage
- `quick_prototype()` - Quick prototyping setup
- `optimize_brain_for_performance(brain)` - Optimize brain for performance
- `enable_memory_optimization(brain)` - Enable memory optimizations

#### Debugging
- `debug_brain_connections(brain)` - Debug brain connections
- `analyze_brain_efficiency(brain)` - Analyze brain efficiency
- `monitor_brain_health(brain)` - Monitor brain health
- `print_brain_summary(brain)` - Print brain summary

#### String Conversion
- `neuron_type_to_string(type)` - Convert NeuronType to string
- `synapse_type_to_string(type)` - Convert SynapseType to string
- `action_type_to_string(type)` - Convert ActionType to string
- `world_object_type_to_string(type)` - Convert WorldObjectType to string

#### String to Enum
- `string_to_neuron_type(str)` - Convert string to NeuronType
- `string_to_synapse_type(str)` - Convert string to SynapseType
- `string_to_action_type(str)` - Convert string to ActionType
- `string_to_world_object_type(str)` - Convert string to WorldObjectType

#### Simulation
- `run_simulation(brain, world, steps)` - Run complete simulation
- `safe_step(brain, world, steps)` - Safe simulation with error handling

## Performance Considerations

### Memory Management

- Use context managers for automatic resource cleanup
- Reuse brain instances across simulations when possible
- Use `optimize_brain_for_performance()` for large-scale simulations

### Simulation Speed

- For high-performance simulations, use the C++ API directly
- The Python bindings provide good performance for most use cases
- Consider using `enable_memory_optimization()` for memory-constrained environments

### Large-Scale Simulations

- Use batch processing for multiple scenarios
- Implement checkpointing for long-running simulations
- Monitor brain efficiency regularly

## Troubleshooting

### Common Issues

1. **Module not found**
   ```bash
   pip install pynlm
   ```

2. **Import errors**
   - Ensure all dependencies are installed
   - Check that the module was built successfully

3. **Performance issues**
   - Profile your simulation to identify bottlenecks
   - Consider using optimized brain configurations

4. **Memory issues**
   - Use context managers to ensure proper cleanup
   - Enable memory optimizations for large simulations

### Getting Help

- Check the documentation in the docs/ directory
- Read the examples in the examples/ directory
- Use `nlm.debug_brain_connections()` for debugging
- Monitor brain health with `nlm.monitor_brain_health()`

## License

MIT License. See LICENSE file for details.

## Authors

NLM Authors. See README.md for more information.

## Version

0.1.0

This document provides a comprehensive guide for using the NLM Python bindings. For more detailed information about specific features or API documentation, refer to the docstrings in the Python module or the online documentation.