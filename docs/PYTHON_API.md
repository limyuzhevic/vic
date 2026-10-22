# NLM Python Bindings Documentation

## Overview

NLM (Neural Learning Machine) is an experimental computational brain project that creates neural systems beginning in primitive developmental states and acquiring increasingly complex abilities through interaction with an environment.

The Python bindings provide a Python interface to the NLM C++ neural simulation framework, enabling users to create and simulate artificial neural systems with memory, prediction, cognition, and neuromodulation capabilities.

## Installation

### Prerequisites

- Python 3.8 or higher
- pip
- A C++ compiler (GCC, Clang, or MSVC)

### Installation from Source

1. Clone the NLM repository
2. Build the C++ library and Python bindings:

```bash
mkdir build
cd build
cmake ..
make -j4
```

3. Install the Python package:

```bash
pip install -e .
```

### Using PyPI (if available)

```bash
pip install nlm
```

## Quick Start

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.create_brain_default()
brain.initialize()

# Create a simple world
world = pynlm.create_simple_world()

# Create an agent brain interface
agent = pynlm.create_agent_brain_default()
agent.initialize(world)

print(f"Created brain with {brain.get_total_neuron_count()} neurons")
print(f"Created agent with sensory input size: {agent.get_sensory_input_size()}")
```

### Simulation Loop

```python
import pynlm
import time

# Setup simulation
brain = pynlm.create_brain_default()
brain.initialize()

world = pynlm.create_simple_world()
agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Run simulation
print("Starting simulation...")
for step in range(100):
    # Get sensory input from world
    percept = world.get_sensory_percept()
    
    # Process sensory input through agent
    agent.process_sensory_input(percept)
    
    # Get action from brain
    action = agent.decode_motor_command()
    
    # Apply action to world
    world.apply_motor_command(action, step * 0.001)
    world.update(step * 0.001)
    
    # Check for learning
    if step % 100 == 0:
        agent.apply_reward_modulation(0.1, 0.05)
        brain.log_status()

print(f"Simulation completed. Total spikes: {brain.get_total_spike_count()}")
```

## API Reference

### Core Classes

#### Config
Configuration system for the NLM neural simulation framework.

**Methods:**
- `Config()` - Create an empty configuration
- `load_from_file(filepath)` - Load from JSON file
- `load_from_args(argc, argv)` - Load from command line
- `save_to_file(filepath)` - Save to JSON file
- `has(key)` - Check if key exists
- `get_keys()` - Get all configuration keys
- `clear()` - Clear all entries
- `summary()` - Get summary string

**Properties:**
- `neuron_count` - Number of neurons to create
- `simulation_timestep` - Time step in seconds
- `connection_probability` - Connection probability between neurons

**Examples:**
```python
import pynlm

# Default configuration
config = pynlm.Config()

# Load from file
config.load_from_file("config.json")

# Set values
config.set_neuron_count(1000)
config.set_simulation_timestep(0.001)

# Save
config.save_to_file("my_config.json")
```

#### Brain
Central neural simulation brain class.

**Methods:**
- `Brain(config)` - Create brain with configuration
- `initialize()` - Initialize the brain
- `step(current_step)` - Perform a simulation step
- `step(current_step, current_time)` - Perform step with timestamp
- `receive_sensory_input(input)` - Inject sensory input
- `inject_current(neuron, current)` - Inject current into specific neuron
- `inject_current_to_neurons(type, current)` - Inject to all neurons of type
- `produce_action()` - Produce motor action
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state
- `add_region(name)` - Add neural region
- `get_region(id)` - Get region by ID
- `get_total_neuron_count()` - Get total neuron count
- `get_total_synapse_count()` - Get total synapse count
- `get_active_neuron_count()` - Get active neuron count
- `get_firing_neuron_count()` - Get firing neuron count
- `get_total_spike_count()` - Get total spike count
- `get_developmental_stage()` - Get developmental stage
- `set_developmental_stage(stage)` - Set developmental stage
- `get_config()` - Get configuration
- `log_status()` - Log brain status

**Convenience Methods:**
- `get_spike_system()` - Get spike system
- `get_stdp()` - Get STDP plasticity
- `get_hebbian()` - Get Hebbian plasticity
- `get_structural_plasticity()` - Get structural plasticity
- `get_prediction_system()` - Get prediction system
- `get_working_memory()` - Get working memory
- `get_episodic_memory()` - Get episodic memory
- `get_associative_memory()` - Get associative memory
- `get_planner()` - Get neural planner
- `get_concept_formation()` - Get concept formation
- `get_attention()` - Get attentional selection
- `get_development_system()` - Get development system
- `get_dopamine()` - Get dopamine neuromodulator
- `get_curiosity()` - Get curiosity neuromodulator
- `get_novelty()` - Get novelty neuromodulator
- `get_prediction_error_signal()` - Get prediction error
- `get_random_generator()` - Get random generator

**Examples:**
```python
import pynlm

# Create brain with custom config
config = pynlm.Config()
config.set_neuron_count(500)
config.set_simulation_timestep(0.001)
brain = pynlm.create_brain(config)

# Initialize and run
brain.initialize()

# Run several steps
for step in range(100):
    brain.step(step, step * 0.001)

# Get statistics
print(f"Neurons: {brain.get_total_neuron_count()}")
print(f"Synapses: {brain.get_total_synapse_count()}")
print(f"Spikes: {brain.get_total_spike_count()}")
print(f"Development stage: {brain.get_developmental_stage()}")
```

#### SimpleWorld
Simple 2D world for NLM agent simulation.

**Methods:**
- `SimpleWorld()` - Create empty world
- `configure(width, height, vision_width, vision_height)` - Configure world
- `reset()` - Reset world
- `set_agent_start(x, y)` - Set agent start position
- `update(timestep)` - Update world state
- `apply_motor_command(cmd, current_time)` - Apply motor command
- `get_sensory_percept()` - Get sensory percept
- `get_agent_body()` - Get agent body state
- `add_object(obj)` - Add world object
- `remove_object(x, y)` - Remove object at position
- `is_valid_position(x, y)` - Check if position is valid
- `get_width()` - Get world width
- `get_height()` - Get world height
- `get_max_energy()` - Get max energy
- `set_max_energy(e)` - Set max energy
- `get_energy_decay_rate()` - Get energy decay rate
- `set_energy_decay_rate(r)` - Set energy decay rate
- `get_simulation_time()` - Get simulation time
- `set_random_seed(seed)` - Set random seed

**Convenience Methods:**
- `get_vision()` - Get vision percept
- `get_agent_position()` - Get agent position as (x, y) tuple
- `get_agent_orientation()` - Get agent orientation
- `add_resource(x, y, value)` - Add resource object
- `add_hazard(x, y, value)` - Add hazard object
- `add_wall(x, y, radius)` - Add wall object
- `find_nearest(type)` - Find nearest object of given type

**Examples:**
```python
import pynlm

# Create and configure world
world = pynlm.create_simple_world()
world.configure(width=100, height=100, vision_width=16, vision_height=16)

# Set agent start position
world.set_agent_start(50, 50)

# Add objects
world.add_resource(25, 25, 1.0)  # Resource
world.add_hazard(75, 75, -0.5)    # Hazard
world.add_wall(0, 50, 2.0)        # Wall

# Run simulation
for step in range(100):
    percept = world.get_sensory_percept()
    world.update(0.001)

# Get agent state
body = world.get_agent_body()
print(f"Agent position: ({body.x}, {body.y})")
print(f"Agent energy: {body.energy}")
```

#### AgentBrain
Agent brain interface connecting NLM brain to world.

**Methods:**
- `AgentBrain(brain)` - Create with brain
- `initialize(world)` - Initialize with world
- `get_sensory_input_size()` - Get expected sensory input size
- `get_motor_output_size()` - Get expected motor output size
- `process_sensory_input(percept)` - Process sensory input
- `decode_motor_command()` - Decode motor command
- `apply_reward_modulation(reward, predicted_reward)` - Apply reward modulation
- `update_development(timestep)` - Update development
- `get_developal_stage()` - Get developmental stage
- `get_neuromodulation_level()` - Get neuromodulation level
- `get_curiosity_level()` - Get curiosity level
- `get_novelty_level()` - Get novelty level
- `get_prediction_error()` - Get prediction error
- `reset()` - Reset agent
- `get_brain()` - Get underlying brain

**Configuration Methods:**
- `enable_reward_modulation(enable)` - Enable/disable reward modulation
- `enable_structural_plasticity(enable)` - Enable/disable structural plasticity
- `enable_development(enable)` - Enable/disable development
- `enable_curiosity(enable)` - Enable/disable curiosity
- `is_reward_modulation_enabled()` - Check if reward modulation enabled
- `is_structural_plasticity_enabled()` - Check if structural plasticity enabled
- `is_development_enabled()` - Check if development enabled
- `is_curiosity_enabled()` - Check if curiosity enabled

**Convenience Methods:**
- `get_curiosity_drive()` - Get curiosity drive
- `get_novelty_sensitivity()` - Get novelty sensitivity
- `get_learning_rate()` - Get learning rate
- `should_explore()` - Check if should explore
- `explore()` - Generate exploration command
- `create_episode(world)` - Create new episode
- `process_episode_step(world, timestep)` - Process episode step

**Examples:**
```python
import pynlm

# Create brain and agent
brain = pynlm.create_brain_default()
brain.initialize()

world = pynlm.create_simple_world()
agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Run episode with context manager
with pynlm.agent_context(agent) as agent:
    for step in range(50):
        # Get percept and process
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        
        # Get action
        action = agent.decode_motor_command()
        
        # Apply to world
        world.apply_motor_command(action, step * 0.001)
        world.update(0.001)
        
        # Check learning
        if step % 10 == 0:
            agent.apply_reward_modulation(0.1, 0.05)

# Agent automatically reset when exiting context
```

#### Action
Action representation for motor output.

**Methods:**
- `Action()` - Create empty action
- `Action(type)` - Create with type
- `Action(type, parameters)` - Create with type and parameters
- `get_type()` - Get action type
- `set_type(type)` - Set action type
- `get_parameters()` - Get parameters
- `set_parameters(params)` - Set parameters
- `get_name()` - Get action name
- `clone()` - Create copy

**Examples:**
```python
import pynlm

# Create actions
action1 = pynlm.Action(pynlm.ActionType.MoveForward)
action2 = pynlm.Action(pynlm.ActionType.Interact, [0.5, 0.8, 1.0])

print(f"Action 1: {action1.get_name()}")
print(f"Action 2: {action2.get_name()}")
print(f"Action 2 parameters: {action2.get_parameters()}")

# Clone
action3 = action2.clone()
```

#### WorldObject
World object representation.

**Methods:**
- `WorldObject()` - Create empty object
- `WorldObject(x, y, type, value, radius)` - Create with properties

**Properties:**
- `x` - X coordinate
- `y` - Y coordinate  
- `radius` - Radius
- `type` - Object type
- `value` - Value/magnitude
- `active` - Whether object is active

**Methods:**
- `is_colliding(x, y)` - Check collision with position
- `distance_to(other)` - Calculate distance to another object

**Examples:**
```python
import pynlm

# Create world object
obj = pynlm.WorldObject(10.0, 20.0, pynlm.WorldObjectType.Resource, 1.5, 0.5)

print(f"Object type: {obj.type}")
print(f"Object value: {obj.value}")
print(f"Is colliding with (10, 20): {obj.is_colliding(10, 20)}")
```

#### AgentBody
Agent body state in the world.

**Properties:**
- `x` - X position
- `y` - Y position
- `orientation` - Orientation in radians
- `velocity_x` - X velocity
- `velocity_y` - Y velocity
- `angular_velocity` - Angular velocity
- `energy` - Energy level (0-1)
- `health` - Health level (0-1)
- `age` - Age in simulation steps
- `is_moving` - Whether agent is moving
- `is_turning` - Whether agent is turning
- `last_action_time` - Time of last action

**Methods:**
- `reset()` - Reset body to initial state
- `set_position(x, y)` - Set position
- `set_velocity(vx, vy)` - Set velocity
- `update_position(dt)` - Update position based on velocity

**Examples:**
```python
import pynlm

# Create agent body
body = pynlm.AgentBody()
body.set_position(50, 50)
body.set_velocity(1.0, 0.5)
body.energy = 0.8
body.health = 0.9

# Update position
body.update_position(0.001)
print(f"New position: ({body.x}, {body.y})")
```

#### SensoryPercept
Sensory percept data from the world.

**Methods:**
- `SensoryPercept()` - Create empty percept
- `get_vision()` - Get vision data
- `set_vision(vision)` - Set vision data
- `get_vision_width()` - Get vision width
- `get_vision_height()` - Get vision height
- `get_touch()` - Get touch data
- `set_touch(touch)` - Set touch data
- `get_internal()` - Get internal signals
- `set_internal(internal)` - Set internal signals
- `get_proprioception()` - Get proprioception data
- `set_proprioception(proprioception)` - Set proprioception data
- `get_audio()` - Get audio data
- `set_audio(audio)` - Set audio data
- `get_all_signals()` - Get all signals as vector
- `get_timestamp()` - Get timestamp
- `set_timestamp(timestamp)` - Set timestamp

**Convenience Methods:**
- `to_dict()` - Convert to Python dictionary
- `from_dict(data)` - Create from dictionary

**Examples:**
```python
import pynlm

# Create percept
percept = pynlm.SensoryPercept()

# Set vision (assuming 16x16 vision)
vision_data = [0.1] * (16 * 16)
percept.set_vision(vision_data)

# Set other modalities
touch_data = [0.0] * 8
percept.set_touch(touch_data)

# Convert to dict
percept_dict = percept.to_dict()
print(f"Vision size: {len(percept_dict['vision'])}")

# Create from dict
new_percept = pynlm.SensoryPercept()
new_percept.from_dict(percept_dict)
```

### Enumerations

All enumerations are exposed as Python enums (e.g., `pynlm.NeuronType`, `pynlm.ActionType`, etc.)

**NeuronType:**
- `Excitatory`, `Inhibitory`, `Modulatory`, `Sensory`, `Motor`, `Internal`

**SynapseType:**
- `Excitatory`, `Inhibitory`, `Modulatory`, `Electrical`, `GapJunction`

**DevelopmentalStage:**
- `Initial`, `CriticalPeriod`, `Maturation`, `Adult`, `Aging`

**FiringState:**
- `Resting`, `Active`, `Refractory`, `Inhibited`

**ActionType:**
- `MoveForward`, `MoveBackward`, `MoveLeft`, `MoveRight`, `TurnLeft`, `TurnRight`,
  `Look`, `LookUp`, `LookDown`, `Interact`, `Eat`, `Drink`, `Rest`, `Wait`, `Custom`

**MotorCommand:**
- `MoveForward`, `MoveBackward`, `TurnLeft`, `TurnRight`, `LookLeft`, `LookRight`, `Interact`, `Wait`

**WorldObjectType:**
- `Empty`, `Resource`, `Hazard`, `Wall`, `Marker`

### Factory Functions

#### create_default_config()
```python
config = pynlm.create_default_config()
```

#### create_brain(config=None)
```python
# With custom config
brain = pynlm.create_brain(config)

# With default config
brain = pynlm.create_brain_default()
```

#### create_simple_world()
```python
world = pynlm.create_simple_world()
```

#### create_agent_brain(brain)
```python
agent = pynlm.create_agent_brain(brain)
```

#### create_agent_brain_default()
```python
agent = pynlm.create_agent_brain_default()
```

### Utility Functions

#### run_demo_simulation(steps, verbose=True)
Run a demonstration simulation.

#### run_learning_demo(steps, verbose=True)
Run a learning demonstration.

#### benchmark_plasticity()
Benchmark plasticity learning.

#### get_version()
Get version of NLM Python bindings.

#### get_build_info()
Get build information.

#### validate_brain_state(brain)
Validate brain state. Raises exception if invalid.

#### safe_brain_step(brain, step, time=0.0)
Safely perform brain step.

#### safe_brain_reset(brain)
Safely reset brain.

#### compare_brains(brains)
Compare multiple brains and return statistics.

### Context Managers

#### brain_context(brain)
```python
with pynlm.brain_context(brain) as brain:
    # Brain automatically reset on exit
    brain.initialize()
    brain.step(0, 0.0)
```

#### world_context(world)
```python
with pynlm.world_context(world) as world:
    # World automatically reset on exit
    world.update(0.001)
```

#### agent_context(agent)
```python
with pynlm.agent_context(agent) as agent:
    # Agent automatically reset on exit
    agent.initialize(world)
    agent.process_sensory_input(percept)
```

## Examples Gallery

### Simple Navigation

```python
import pynlm

# Create world with obstacles
world = pynlm.create_simple_world()
world.configure(100, 100, 16, 16)
world.set_agent_start(10, 10)

# Add some obstacles
world.add_wall(50, 50, 3)
world.add_hazard(80, 80, -0.5)
world.add_resource(90, 90, 1.0)

# Create agent
brain = pynlm.create_brain_default()
brain.initialize()
agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Simple navigation loop
for step in range(200):
    # Get percept
    percept = world.get_sensory_percept()
    
    # Process
    agent.process_sensory_input(percept)
    action = agent.decode_motor_command()
    
    # Apply action
    world.apply_motor_command(action, step * 0.001)
    world.update(0.001)
    
    # Check if agent reached goal
    body = world.get_agent_body()
    if abs(body.x - 90) < 2 and abs(body.y - 90) < 2:
        print("Goal reached!")
        break
```

### Learning Environment

```python
import pynlm

# Create learning environment
world = pynlm.create_simple_world()
world.configure(200, 200, 16, 16)
world.set_agent_start(10, 10)

# Add learning objects
for i in range(10):
    x = i * 15 + 5
    y = i * 15 + 5
    world.add_resource(x, y, 0.5 + (i * 0.05))

# Create learning agent
brain = pynlm.create_brain_default()
brain.initialize()
agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Enable learning features
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_curiosity(True)

# Learning loop
print("Starting learning episode...")
for episode in range(5):
    # Create new episode
    with pynlm.agent_context(agent) as agent:
        for step in range(100):
            # Get percept
            percept = world.get_sensory_percept()
            
            # Process
            agent.process_sensory_input(percept)
            action = agent.decode_motor_command()
            
            # Apply
            world.apply_motor_command(action, step * 0.001)
            world.update(0.001)
            
            # Apply learning
            if step % 20 == 0:
                agent.apply_reward_modulation(0.1, 0.05)
                agent.update_development(0.001)
        
        print(f"Episode {episode + 1} completed")
    
    # Reset world for next episode
    world.reset()
    world.set_agent_start(10, 10)
```

### Multi-Agent Simulation

```python
import pynlm
from concurrent.futures import ThreadPoolExecutor

def run_agent_simulation(agent_id, brain_config):
    """Run simulation for a single agent."""
    brain = pynlm.create_brain(brain_config)
    brain.initialize()
    
    world = pynlm.create_simple_world()
    world.configure(100, 100, 16, 16)
    world.set_agent_start(agent_id * 20 + 10, agent_id * 20 + 10)
    
    agent = pynlm.create_agent_brain(brain)
    agent.initialize(world)
    
    # Run simulation
    for step in range(50):
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        action = agent.decode_motor_command()
        world.apply_motor_command(action, step * 0.001)
        world.update(0.001)
    
    return brain.get_total_spike_count(), brain.get_total_synapse_count()

# Create multiple agent configurations
configs = []
for i in range(3):
    config = pynlm.Config()
    config.set_neuron_count(200)
    config.set("random_seed", 42 + i)
    configs.append(config)

# Run simulations in parallel
print("Running multi-agent simulations...")
with ThreadPoolExecutor(max_workers=3) as executor:
    futures = [executor.submit(run_agent_simulation, i, config) 
               for i, config in enumerate(configs)]
    
    results = []
    for future in futures:
        spikes, synapses = future.result()
        results.append((spikes, synapses))
        print(f"Agent result: {spikes} spikes, {synapses} synapses")

print(f"Average spikes: {sum(r[0] for r in results) / len(results):.1f}")
```

## Advanced Usage

### Working with Sensory Data

```python
import pynlm

world = pynlm.create_simple_world()
percept = world.get_sensory_percept()

# Access different sensory modalities
vision_data = percept.get_vision()
touch_data = percept.get_touch()
internal_data = percept.get_internal()

print(f"Vision dimensions: {percept.get_vision_width()} x {percept.get_vision_height()}")
print(f"Vision size: {len(vision_data)} values")
print(f"Touch size: {len(touch_data)} values")
print(f"Internal size: {len(internal_data)} values")

# Process vision data (simple threshold)
threshold = 0.5
bright_pixels = [v for v in vision_data if v > threshold]
print(f"Bright pixels: {len(bright_pixels)}/{len(vision_data)}")
```

### Custom Action Types

```python
import pynlm

# Create custom action with parameters
custom_action = pynlm.Action(pynlm.ActionType.Custom, [0.1, 0.2, 0.3, 0.4, 0.5])

# Access parameters
params = custom_action.get_parameters()
print(f"Action type: {custom_action.get_type()}")
print(f"Parameters: {params}")

# Clone and modify
action2 = custom_action.clone()
params[0] = 0.9
action2.set_parameters(params)
print(f"Modified action: {action2.get_name()}")
```

### Performance Monitoring

```python
import pynlm
import time

brain = pynlm.create_brain_default()
brain.initialize()

# Run performance test
start_time = time.time()
steps = 1000

for step in range(steps):
    brain.step(step, step * 0.001)

end_time = time.time()
execution_time = end_time - start_time

# Get performance metrics
total_spikes = brain.get_total_spike_count()
spike_rate = total_spikes / execution_time if execution_time > 0 else 0
neurons = brain.get_total_neuron_count()
synapses = brain.get_total_synapse_count()

print(f"Performance Metrics:")
print(f"  Steps: {steps}")
print(f"  Execution time: {execution_time:.3f}s")
print(f"  Spike rate: {spike_rate:.1f} spikes/sec")
print(f"  Neurons: {neurons}")
print(f"  Synapses: {synapses}")
print(f"  Synapses per neuron: {synapses/neurons:.2f}")
```

### Error Handling

```python
import pynlm

brain = pynlm.create_brain_default()

# Safe brain operations
try:
    brain.initialize()
    print("Brain initialized successfully")
    
    for step in range(10):
        brain.step(step, step * 0.001)
    
    print("Steps completed successfully")
    
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")

# Validate brain state before critical operations
if pynlm.validate_brain_state(brain):
    print("Brain state is valid")
else:
    print("Brain state is invalid!")
```

## Migration Guide

### From Old API to New API

#### Brain Creation
**Old:**
```python
brain = pynlm.Brain()
brain.initialize()
```

**New:**
```python
brain = pynlm.create_brain_default()
brain.initialize()
```

#### Agent Creation
**Old:**
```python
agent = pynlm.AgentBrain(brain)
agent.initialize(world)
```

**New:**
```python
agent = pynlm.create_agent_brain(brain)
agent.initialize(world)
```

#### World Configuration
**Old:**
```python
world.configure(width, height, vision_width, vision_height)
```

**New:**
```python
world.configure(width, height, vision_width, vision_height)
```

#### Accessing Properties
**Old:**
```python
neuron_count = brain.config.get_or("neuron_count", 500)
```

**New:**
```python
neuron_count = brain.config.get_neuron_count()
```

#### Error Handling
**Old:**
```python
try:
    brain.initialize()
except Exception as e:
    print(f"Error: {e}")
```

**New:**
```python
try:
    brain.initialize()
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Error: {e}")
```

## Troubleshooting

### Common Issues

#### "ModuleNotFoundError: No module named 'pynlm'"
- Ensure the Python bindings are installed
- Try: `pip install -e .` from the build directory

#### "ImportError: libpynlm.so not found"
- Set LD_LIBRARY_PATH to include the bindings directory
- On Linux: `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/pynlm/bindings`

#### Performance Issues
- Check that the C++ library is compiled with optimizations (`-O3`)
- Ensure you have enough RAM for large brain simulations
- Consider reducing neuron count for testing

#### Memory Issues
- Monitor memory usage during large simulations
- Use context managers to ensure proper cleanup
- Consider saving brain state to disk periodically

#### Build Issues
- Ensure all dependencies are installed
- Check compiler compatibility
- Verify CMake version requirements

### Getting Help

- Documentation: https://nlm.readthedocs.io
- Examples: See the examples directory
- Issues: Report on GitHub repository
- Community: Join the NLM discussion forum

## Acknowledgments

This project builds on the foundation of:
- The NLM C++ neural simulation framework
- Pybind11 for Python bindings
- CMake for build system
- Various scientific computing libraries

The NLM project is part of the broader research effort to create artificial developmental brain systems that learn from experience.

## License

MIT License

See LICENSE file for details.
