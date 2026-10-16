# NLM Python API Reference

## Overview

The NLM (Neural Learning Machine) Python API provides a comprehensive interface to the experimental artificial developmental brain simulator. This reference document covers all Python classes, functions, and usage patterns.

## Installation

```bash
pip install pynlm
```

## Module Structure

### `pynlm` module

The main module provides:
- Brain simulation classes
- Configuration management
- World simulation
- Agent interfaces
- Utility functions
- Constants

## Core Classes

### Brain

The central neural simulation brain class implementing real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

#### Methods

```python
# Creation and initialization
brain = pynlm.create_brain(config)  # With custom config
brain = pynlm.create_brain()        # With default config
brain.initialize()

# Simulation control
brain.step(step_number)              # Basic step
brain.step(step_number, time)        # Step with timestamp
brain.reset()                        # Reset brain state
brain.save(filepath)                 # Save checkpoint
brain.load(filepath)                 # Load checkpoint

# Input/output
brain.receive_sensory_input(input)   # Inject sensory data
action = brain.produce_action()      # Get motor action
brain.inject_current(neuron, current) # Inject to specific neuron
brain.inject_current_to_neurons(type, current) # Inject to neuron type

# Statistics and querying
brain.get_total_neuron_count()        # Total neurons
brain.get_total_synapse_count()       # Total synapses
brain.get_active_neuron_count()       # Currently active neurons
brain.get_firing_neuron_count()       # Currently firing neurons
brain.get_average_firing_rate()       # Average firing rate
brain.get_excitation_inhibition_ratio() # E/I balance
brain.get_total_spike_count()         # Total spike count
brain.get_developmental_stage()       # Current developmental stage
brain.set_developmental_stage(stage)  # Set developmental stage

# Region management
region_id = brain.add_region("cortex") # Add new region
region = brain.get_region(region_id)    # Get region
brain.get_region_count()               # Get region count
brain.get_region_ids()                 # Get all region IDs
brain.get_regions()                    # Get all regions

# Access to subsystems
brain.get_config()                     # Get configuration
brain.get_working_memory()             # Get working memory
brain.get_episodic_memory()            # Get episodic memory
brain.get_associative_memory()         # Get associative memory
brain.get_prediction_system()          # Get prediction system
brain.get_planner()                    # Get neural planner
brain.get_concept_formation()          # Get concept formation
brain.get_attention()                  # Get attention system
brain.get_development_system()         # Get development system
brain.get_dopamine()                   # Get dopamine system
brain.get_curiosity()                  # Get curiosity system
brain.get_novelty()                    # Get novelty system
brain.get_prediction_error_signal()    # Get prediction error
brain.get_random_generator()           # Get random generator
brain.log_status()                     # Log brain status

# Convenience methods
brain.get_stats()                      # Get formatted statistics
brain.simulate_step(step, time)        # Simulate single step with return value
```

#### Properties

The Brain class provides Pythonic property access:

```python
# These are available as methods, with some properties for convenience
stats = brain.get_stats()  # Returns formatted string
```

#### Usage Examples

```python
# Example 1: Basic brain creation and simulation
import pynlm

# Create brain with default configuration
brain = pynlm.create_brain()
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.get_firing_neuron_count()} firing neurons")

# Example 2: Advanced brain with configuration
config = pynlm.create_default_config()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.1)

brain = pynlm.create_brain(config)
brain.initialize()

# Simulate and collect statistics
total_spikes = 0
for step in range(500):
    brain.step(step)
    total_spikes += brain.get_firing_neuron_count()

print(f"Average firing: {total_spikes / 500:.2f}")
print(f"Total neurons: {brain.get_total_neuron_count()}")
```

### Config

Configuration class for NLM system with Python-friendly interface.

#### Methods

```python
# Creation and basic operations
config = pynlm.Config()                    # Empty config
config = pynlm.create_default_config()     # With defaults

# File operations
config.load_from_file("config.json")       # Load from JSON
config.save_to_file("config.json")       # Save to JSON
config.load_from_args()                   # Load from command line

# Key operations
config.set("key", value)                  # Set value (supports string, float, int)
config.has("key")                         # Check if key exists
config.get_keys()                         # Get all keys
config.get("key")                         # Get as string
config.get_float("key")                   # Get as float
config.get_int("key")                     # Get as integer
config.clear()                            # Clear all entries

# Utility
config.summary()                          # Get summary string
```

#### Usage Examples

```python
config = pynlm.create_default_config()

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.learning_rate", 0.001)

# Query configuration
if config.has("brain.neuron_count"):
    neuron_count = config.get_int("brain.neuron_count")
    print(f"Brain has {neuron_count} neurons")

# Save and load
config.save_to_file("my_config.json")
new_config = pynlm.Config()
new_config.load_from_file("my_config.json")
```

### SimpleWorld

Simple 2D world for NLM simulation.

#### Methods

```python
# Creation and configuration
world = pynlm.create_simple_world()
world.configure(width, height, visionWidth, visionHeight)

# World control
world.reset()                     # Reset world
world.update(timestep)             # Update world state
set_agent_start(x, y)             # Set agent starting position
apply_motor_command(cmd, time)    # Apply motor command

# State access
get_sensory_percept()              # Get sensory input
world.get_agent_body()             # Get agent body state

# Object management
world.add_object(obj)              # Add world object
world.remove_object(x, y)          # Remove object at position
world.is_valid_position(x, y)      # Check position validity

# Properties
world.get_width()                  # World width
world.get_height()                 # World height
world.get_max_energy()             # Maximum energy
world.set_max_energy(value)        # Set maximum energy
world.get_energy_decay_rate()      # Energy decay rate
world.set_energy_decay_rate(value) # Set energy decay rate
world.get_simulation_time()        # Current simulation time
world.set_random_seed(seed)        # Set random seed
world.get_random_seed()            # Get random seed

# Additional utilities
world.get_world_objects()          # Get all objects (placeholder)
world.clear_objects()              # Clear all objects
```

#### Usage Examples

```python
# Create and configure world
world = pynlm.create_simple_world()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.set_agent_start(10.0, 10.0)

# Run simulation loop
for _ in range(100):
    world.update(0.1)
    percept = world.get_sensory_percept()
    
    # Add an object to the world
    obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0)
    world.add_object(obj)
```

### AgentBrain

Agent brain interface connecting NLM brain to world.

#### Methods

```python
# Creation and initialization
agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Configuration
agent.enable_reward_modulation(True/False)
agent.enable_structural_plasticity(True/False)
agent.enable_development(True/False)
agent.enable_curiosity(True/False)

# Query configuration
agent.is_reward_modulation_enabled()
agent.is_structural_plasticity_enabled()
agent.is_development_enabled()
agent.is_curiosity_enabled()

# Sensory processing
agent.process_sensory_input(percept)  # Process sensory input
agent.get_sensory_input_size()        # Get expected sensory size

# Motor decoding
agent.decode_motor_command()          # Get motor command from brain
agent.get_motor_output_size()         # Get expected motor size

# Neuromodulation
agent.apply_reward_modulation(reward, predicted_reward)
agent.get_neuromodulation_level()
agent.get_curiosity_level()
agent.get_novelty_level()
agent.get_prediction_error()

# Development
agent.update_development(timestep)
agent.get_developmental_stage()

# State management
agent.reset()                          # Reset for new episode
agent.get_brain()                      # Get underlying brain

# Statistics
agent.get_agent_stats()                # Get formatted agent statistics
```

#### Usage Examples

```python
# Complete agent example
import pynlm

def run_agent_simulation(num_steps=1000):
    # Setup
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    agent = pynlm.create_agent_brain(brain)
    world = pynlm.create_simple_world()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.set_agent_start(10.0, 10.0)
    
    agent.initialize(world)
    
    # Enable subsystems
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # Run simulation
    for step in range(num_steps):
        world.update(0.1)
        agent.process_sensory_input(world.get_sensory_percept())
        brain.step(step)
        world.apply_motor_command(agent.decode_motor_command(), world.get_simulation_time())
        
        # Apply reward modulation
        reward = world.get_sensory_percept().get_internal()[0] if world.get_sensory_percept().get_internal() else 0.0
        agent.apply_reward_modulation(reward, 0.0)
        
        agent.update_development(0.1)
        
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.get_firing_neuron_count()}")
            print(f"  Curiosity: {agent.get_curiosity_level():.3f}")
    
    print("Agent simulation complete!")

run_agent_simulation(1000)
```

### Vision

Vision sensory input class for visual perception.

#### Methods

```python
# Creation
vision = pynlm.Vision()                           # Empty
vision = pynlm.Vision(width, height)              # With dimensions
vision = pynlm.Vision(width, height, channels)    # With channels

# Data operations
vision.set_data(data)                # Set data from list/array
vision.get_data()                    # Get raw data as list
vision.get_width()                   # Get width
vision.get_height()                  # Get height
vision.get_channels()                # Get channels
vision.get_image_data()              # Get as numpy-like array
```

### Audio

Audio sensory input class.

#### Methods

```python
# Creation
audio = pynlm.Audio(sample_rate, num_samples)

# Data operations
audio.set_data(data)                 # Set audio data
audio.set_sample_rate(rate)          # Set sample rate

audio.get_sample_rate()              # Get sample rate
audio.get_num_samples()              # Get number of samples
```

### InternalSignals

Internal signals sensory input class.

#### Methods

```python
# Creation
internal = pynlm.InternalSignals()

# Operations
internal.add_signal(value)           # Add signal value
internal.get_signals()               # Get all signals
clear_signals()                      # Clear all signals
```

### SensoryPercept

Sensory percept data container.

#### Methods

```python
# Creation
percept = pynlm.SensoryPercept()

# Vision
percept.get_vision()                 # Get vision data
percept.set_vision(vision)            # Set vision data
percept.get_vision_width()            # Get vision width
percept.get_vision_height()           # Get vision height

# Touch (placeholder)
percept.get_touch()                  # Get touch data
percept.set_touch(touch)              # Set touch data

# Internal signals
percept.get_internal()               # Get internal signals
percept.set_internal(internal)       # Set internal signals

# Proprioception (placeholder)
percept.get_proprioception()         # Get proprioception
percept.set_proprioception(proprioception) # Set proprioception

# Audio
percept.get_audio()                  # Get audio data
percept.set_audio(audio)              # Set audio data

# Common operations
percept.get_all_signals()             # Get all signals
percept.get_timestamp()              # Get timestamp
percept.set_timestamp(timestamp)      # Set timestamp

# Convenience methods
percept.add_vision_signal(data)       # Add vision data as signal
percept.add_internal_signal(value)    # Add internal signal
```

### Action

Action representation for motor output.

#### Methods

```python
# Creation
action = pynlm.Action()                              # Empty
action = pynlm.Action(action_type)                   # With type
action = pynlm.Action(action_type, parameters)      # With type and parameters

# Properties
action.get_type()                    # Get action type
action.set_type(type)                # Set action type

action.get_parameters()              # Get parameters
action.set_parameters(params)        # Set parameters

action.get_name()                    # Get action name

action.clone()                       # Clone action

# Representation
action.__repr__()                   # String representation
```

### WorldObject

World object representation.

#### Methods

```python
# Creation
obj = pynlm.WorldObject(x, y, obj_type, value=0.0, radius=0.5)

# Properties
obj.x                              # X position
obj.y                              # Y position
obj.radius                         # Radius
obj.type                           # Object type
obj.value                          # Object value
obj.active                         # Active state

# Representation
obj.__repr__()                      # String representation
```

### AgentBody

Agent body state representation.

#### Methods

```python
# Creation
body = pynlm.AgentBody()

# Properties
body.x                             # X position
body.y                             # Y position
body.orientation                   # Orientation
body.velocity_x                    # X velocity
body.velocity_y                    # Y velocity
body.angular_velocity               # Angular velocity
body.energy                        # Energy level
body.health                        # Health
body.age                           # Age
body.is_moving                    # Movement state
body.is_turning                    # Turning state
body.last_action_time              # Last action time

# Operations
body.reset()                        # Reset body state

# Methods
body.distance_to(x, y)              # Calculate distance to position
```

### ActionResult

Action result from world execution.

#### Methods

```python
# Creation
result = pynlm.ActionResult()                          # Empty
result = pynlm.ActionResult(reward, success, message="")  # With parameters

# Properties
result.reward                      # Reward value
result.success                     # Success flag
result.message                     # Result message

# Representation
result.__repr__()                   # String representation
```

## Enumerations

### NeuronType

```python
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal
```

### SynapseType

```python
pynlm.SynapseType.Excitatory
pynlm.SynapseType.Inhibitory
pynlm.SynapseType.Modulatory
pynlm.SynapseType.Electrical
pynlm.SynapseType.GapJunction
```

### DevelopmentalStage

```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

### FiringState

```python
pynlm.FiringState.Resting
pynlm.FiringState.Active
pynlm.FiringState.Refractory
pynlm.FiringState.Inhibited
```

### ActionType

```python
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.MoveLeft
pynlm.ActionType.MoveRight
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.LookUp
pynlm.ActionType.LookDown
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest
pynlm.ActionType.Wait
pynlm.ActionType.Custom
```

### MotorCommand

```python
pynlm.MotorCommand.MoveForward
pynlm.MotorCommand.MoveBackward
pynlm.MotorCommand.TurnLeft
pynlm.MotorCommand.TurnRight
pynlm.MotorCommand.LookLeft
pynlm.MotorCommand.LookRight
pynlm.MotorCommand.Interact
pynlm.MotorCommand.Wait
```

### WorldObjectType

```python
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

## Utility Functions

### Configuration

```python
pynlm.create_default_config()    # Create config with defaults
```

### Brain Creation

```python
pynlm.create_brain()             # Create with default config
pynlm.create_brain(config)        # Create with custom config
```

### World Creation

```python
pynlm.create_simple_world()       # Create and configure world
```

### Agent Creation

```python
pynlm.create_agent_brain(brain)   # Create agent interface
```

### Simulation Utilities

```python
pynlm.run_simulation(brain, num_steps)  # Run simulation
pynlm.run_agent_simulation(brain, world, num_steps)  # Run agent simulation
pynlm.create_simulation_context(brain)  # Create simulation context
```

## Constants

```python
pynlm.INVALID_NEURON_ID        # Invalid neuron ID constant
pynlm.INVALID_SYNAPSE_ID        # Invalid synapse ID constant
pynlm.INVALID_REGION_ID         # Invalid region ID constant
pynlm.INVALID_POPULATION_ID     # Invalid population ID constant
```

## Module Information

```python
pynlm.__version__           # Module version
pynlm.__author__            # Module author
pynlm.__description__        # Module description
```

## Context Manager Example

```python
# Using SimulationContext for controlled execution
import pynlm

# Create brain
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()

# Use context manager
with pynlm.create_simulation_context(brain) as brain_ref:
    # Brain is automatically initialized
    for step in range(100):
        brain_ref.step(step)
    
    # Context exit resets the brain

print(f"Total spikes: {brain.get_total_spike_count()}")
```

## Best Practices

### 1. Proper Initialization

```python
# Always initialize before use
brain = pynlm.create_brain(config)
brain.initialize()  # Required!

# Or use context manager for automatic initialization
with pynlm.create_simulation_context(brain):
    # Brain is initialized here
    pass
```

### 2. Error Handling

```python
try:
    brain = pynlm.create_brain()
    brain.initialize()
    brain.step(0)
except Exception as e:
    print(f"Error: {e}")
    # Handle error appropriately
```

### 3. Memory Management

```python
# Use context managers for automatic cleanup
with pynlm.create_simulation_context(brain):
    # Perform operations
    pass

# Explicit cleanup when not using context managers
brain.reset()
```

### 4. Configuration Best Practices

```python
# Use sensible defaults
config = pynlm.create_default_config()

# Customize as needed
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Save configurations for reproducibility
config.save_to_file("my_config.json")
```

### 5. Simulation Best Practices

```python
# Use appropriate step sizes
brain.step(step, time=step * 0.001)  # Realistic time steps

# Periodically log statistics
if step % 100 == 0:
    stats = brain.get_stats()
    print(stats)

# Handle developmental stages
if brain.get_developmental_stage() == pynlm.DevelopmentalStage.Adult:
    print("Brain has reached adult stage")
```

## Troubleshooting

### Common Issues and Solutions

**Q: "My brain isn't doing anything"**
```python
# Solution: Ensure brain is initialized
brain = pynlm.create_brain()
brain.initialize()  # This is required!
```

**Q: "The agent isn't moving"**
```python
# Solution: Check world updates
world.update(0.1)  # Must be called in simulation loop
world.apply_motor_command(action, world.get_simulation_time())
```

**Q: "Everything is 0"**
```python
# Solution: Give brain time to "warm up"
for step in range(1000):
    brain.step(step)
```

**Q: "ImportError: No module named 'pynlm'"**
```bash
# Solution: Install the module
pip install pynlm
```

**Q: "Segmentation faults"**
```python
# Solution: Check initialization order
brain = pynlm.create_brain()
brain.initialize()  # Must be called before step()
brain.step(0)
```

## Performance Tips

1. **Reuse configurations**: Load configurations from file instead of recreating them
2. **Use context managers**: They handle initialization and cleanup automatically
3. **Batch operations**: When possible, combine related operations
4. **Monitor statistics**: Use `brain.get_stats()` to monitor performance
5. **Save checkpoints**: Use `brain.save()` to save progress

## Advanced Usage

### 1. Custom Development Stages

```python
# Access and modify developmental stages
brain = pynlm.create_brain()
brain.initialize()

# Set custom developmental stage
brain.set_developmental_stage(pynlm.DevelopmentalStage.CriticalPeriod)

# Monitor development
print(f"Development stage: {brain.get_developal_stage()}")
```

### 2. Multi-agent Systems

```python
# Create multiple brains with shared configuration
config = pynlm.create_default_config()

brain1 = pynlm.create_brain(config)
brain2 = pynlm.create_brain(config)  # Share same config

brain1.initialize()
brain2.initialize()

# Run simulations in parallel
import threading

def simulate_brain(brain, steps):
    for step in range(steps):
        brain.step(step)

thread1 = threading.Thread(target=simulate_brain, args=(brain1, 1000))
thread2 = threading.Thread(target=simulate_brain, args=(brain2, 1000))

thread1.start()
thread2.start()

thread1.join()
thread2.join()
```

### 3. Advanced World Manipulation

```python
# Create and manipulate world objects
world = pynlm.create_simple_world()

# Add various objects
resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0)
hazard = pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Hazard, value=-5.0)

world.add_object(resource)
world.add_object(hazard)

# Check positions
if world.is_valid_position(7.0, 7.0):
    print("Position is valid")

# Remove objects
world.remove_object(5.0, 5.0)
```

## Integration with Other Libraries

### NumPy Integration

The NLM Python bindings work well with NumPy for data processing:

```python
import numpy as np
import pynlm

# Create vision data using NumPy
vision_data = np.random.randn(8, 8, 3).astype(np.float32)

vision = pynlm.Vision(8, 8, 3)
vision.set_data(vision_data.tolist())

# Get image data as array (for display/processing)
image_data = vision.get_image_data()  # Returns numpy-like array
```

### Visualization Integration

```python
import matplotlib.pyplot as plt
import pynlm

# Create world and simulate
world = pynlm.create_simple_world()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Run simulation for a few steps
for _ in range(10):
    world.update(0.1)

# Get and display vision data
percept = world.get_sensory_percept()
if percept.get_vision():
    # For visualization (placeholder - actual implementation may vary)
    pass
```

## Research Features

### 1. Detailed Logging

```python
# Enable detailed logging for research
import logging
logging.basicConfig(level=logging.INFO)

brain = pynlm.create_brain()
brain.initialize()
brain.log_status()  # Log detailed brain status
```

### 2. Checkpointing and Reproducibility

```python
# Save and load brain states for reproducibility
brain = pynlm.create_brain()
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Save checkpoint
brain.save("experiment_1_checkpoint.bin")

# Later, load and continue
brain2 = pynlm.create_brain()
brain2.load("experiment_1_checkpoint.bin")

# Continue simulation
for step in range(1000, 2000):
    brain2.step(step)
```

### 3. Developmental Studies

```python
# Study developmental effects
brain = pynlm.create_brain()
brain.initialize()

# Track developmental changes over time
development_stages = []
firing_rates = []

for step in range(5000):
    brain.step(step)
    
    development_stages.append(brain.get_developal_stage())
    firing_rates.append(brain.get_average_firing_rate())
    
    if step % 500 == 0:
        print(f"Step {step}: Stage {brain.get_developal_stage()}, "
              f"Firing rate: {brain.get_average_firing_rate():.3f}")
```

## Migration Guide

### From easy_usage.md

The `easy_usage.md` file provides basic tutorials. For comprehensive API reference, use this documentation.

### From Version 0.0.x

#### Changes in 0.1.0

1. **Enhanced Python bindings**: New Pythonic features and better organization
2. **Improved documentation**: Comprehensive API reference with examples
3. **New utility functions**: Context managers and convenience methods
4. **Better error handling**: More descriptive error messages
5. **Enhanced visualization**: Better support for data visualization

#### Migration Steps

1. Update import statements: `import pynlm` (unchanged)
2. Use new convenience methods: `pynlm.create_brain()` (unchanged, but more robust)
3. Update documentation: Use this API reference instead of `easy_usage.md`
4. Adopt new best practices: Use context managers, check initialization

## Future Enhancements

The following features are planned for future releases:

1. **Real-time visualization**: Built-in plotting and visualization tools
2. **Distributed computing**: Support for multi-GPU and distributed simulations
3. **Advanced machine learning integration**: Integration with popular ML libraries
4. **Custom neural dynamics**: Support for custom neuron models
5. **Export/import**: Standardized formats for brain models
6. **Web interface**: Browser-based visualization and control
7. **Plugin architecture**: Extensible plugin system for custom functionality

## Contributing

Contributions to improve the NLM Python API are welcome! Suggestions for:

- New API methods
- Performance improvements
- Documentation enhancements
- Bug fixes
- Example code

Please follow the existing code style and contribute via pull requests.

## Support

For questions, issues, or support, please:

1. Check the [GitHub Issues](https://github.com/nlm-project/nlm/issues)
2. Read the [Documentation](https://nlm.readthedocs.io)
3. Join the [Discussions](https://github.com/nlm-project/nlm/discussions)
4. File bug reports with detailed reproduction steps

The NLM Python API is actively maintained and improved. Your feedback helps make it better for everyone!