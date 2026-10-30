"""
NLM Enhanced Python Bindings
===========================

This module provides enhanced Python bindings for the NLM (Neural Learning Machine) 
C++ neural simulation framework. It offers both the original bindings and new
Pythonic APIs with improved usability, error handling, and advanced features.

Key Features:
- Pythonic APIs with property access and method chaining
- Context managers for safe resource management
- Enhanced configuration system
- Brain simulation utilities
- Phase 6 integration test support
- Comprehensive error handling
- Debugging and inspection tools

Installation:
-------------
Build using CMake (requires C++ compiler and pybind11):

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

Or install via pip if available:
```bash
pip install pynlm
```

Basic Usage:
------------
```python
import pynlm as nlm

# Create brain with enhanced config
config = nlm.Config() \
    .set_int("neuron_count", 1000) \
    .set_float("connection_probability", 0.1)

brain = nlm.create_brain(config)
brain.initialize()

# Use context manager for safe simulation
with nlm.BrainSimulator(brain) as simulator:
    for step in range(100):
        simulator.step(step)
        print(f"Step {step}: firing rate = {simulator.get_brain().getAverageFiringRate():.2f}")

# Enhanced world setup
world = nlm.World()
world.configure(100, 100)
world.set_agent_start(50, 50)

# Create actions
action = nlm.Action(nlm.ActionType.MoveForward)
action = nlm.Action(nlm.ActionType.Custom, [0.5, 0.3, 0.8])

# ID classes with Pythonic properties
neuron_id = nlm.NeuronId(42)
print(f"Neuron ID value: {neuron_id.value}, index: {neuron_id.index()}")

# Check if valid
if neuron_id.is_valid():
    print("Neuron ID is valid")

# Convert to string
print(repr(neuron_id))
```

Advanced Usage:
---------------
```python
import pynlm as nlm
import numpy as np

# Run Phase 6 integration test
config = nlm.Phase6Config()
config.neuron_count = 500
config.max_steps = 1000
config.enable_checkpointing = True
config.enable_replay = True

print("Running Phase 6 integration...")
result = nlm.run_phase6_integration(config)

print(f"Total reward: {result.get_total_reward():.3f}")
print(f"Average firing rate: {result.get_avg_firing_rate():.3f}")
print(f"Episodic memory stored: {result.get_memory_episodes_stored():.0f}")

# Check integration status
print("\nIntegration Status:")
print(f"Working Memory: {'✓' if result.is_working_memory_integrated() else '✗'}")
print(f"Episodic Memory: {'✓' if result.is_episodic_memory_integrated() else '✗'}")
print(f"Neuromodulation: {'✓' if result.is_neuromodulation_integrated() else '✗'}")
print(f"Prediction: {'✓' if result.is_prediction_integrated() else '✗'}")
print(f"Development: {'✓' if result.is_development_integrated() else '✗'}")

# Run until condition
with nlm.BrainSimulator(brain) as sim:
    steps_taken = sim.run_until(
        lambda b: b.getAverageFiringRate() > 10.0,  # Stop when firing rate > 10
        max_steps=5000
    )
    print(f"Reached condition after {steps_taken} steps")

# Run multiple steps efficiently
firing_rates = sim.run_steps(100, time_step=0.001)
print(f"Average firing rate over 100 steps: {np.mean(firing_rates):.3f}")

# Simulation statistics
print(sim.get_stats())

# Run integration verification
if nlm.verify_phase6_integration():
    print("Phase 6 integration verified successfully!")
else:
    print("Phase 6 integration verification failed!")
```

Error Handling:
--------------
The enhanced bindings provide comprehensive error handling:

```python
import pynlm as nlm
import traceback

try:
    # Create invalid action type
    action = nlm.Action("InvalidActionType")  # Will raise ValueError
except nlm.NLMError as e:
    print(f"NLM Error: {e}")
except ValueError as e:
    print(f"Invalid argument: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
    traceback.print_exc()
```

Simulation Control:
------------------
The `BrainSimulator` context manager provides safe simulation control:

```python
# Safe simulation with automatic reset
with nlm.BrainSimulator(brain) as sim:
    # Your simulation code here
    for step in range(1000):
        sim.step(step)
    
    # Resources are automatically cleaned up

# Manual control
sim = nlm.BrainSimulator(brain, auto_reset=False)
sim.enter()  # or use with statement
sim.step(0)
sim.exit()   # or use with statement
```

Configuration Management:
-------------------------
The enhanced `Config` class supports method chaining:

```python
# Method chaining for config
config = nlm.Config() \
    .set_int("neuron_count", 1000) \
    .set_float("connection_probability", 0.1) \
    .set_float("stdp_ltp_weight", 0.02) \
    .set_float("stdp_ltd_weight", 0.015)

# Get values with defaults
neuron_count = config.get_int("neuron_count", 100)
connection_prob = config.get_float("connection_probability", 0.1)
```

Performance Monitoring:
----------------------
Monitor simulation performance:

```python
import time

start_time = time.time()

with nlm.BrainSimulator(brain) as sim:
    for step in range(1000):
        sim.step(step)

end_time = time.time()
print(f"Simulation took {end_time - start_time:.2f} seconds")

# Get detailed statistics
print(sim.get_stats())
```

Experiment Automation:
---------------------
Automate experiments:

```python
import pynlm as nlm

def run_experiment(neuron_counts=[100, 500, 1000], steps=1000):
    results = []
    
    for neuron_count in neuron_counts:
        config = nlm.Config() \
            .set_int("neuron_count", neuron_count) \
            .set_int("region_count", 1)
        
        brain = nlm.create_brain(config)
        brain.initialize()
        
        with nlm.BrainSimulator(brain) as sim:
            for step in range(steps):
                sim.step(step)
            
            results.append({
                'neuron_count': neuron_count,
                'avg_firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount()
            })
        
        brain.reset()
        del brain
    
    return results

# Run parameter sweep
results = run_experiment()
for result in results:
    print(f"Neurons: {result['neuron_count']}, "
          f"Firing: {result['avg_firing_rate']:.3f}, "
          f"Spikes: {result['total_spikes']}")
```

Visualizing Results:
-------------------
```python
import matplotlib.pyplot as plt
import numpy as np

# Create configuration and brain
config = nlm.create_test_config(neuron_count=200)
brain = nlm.create_brain(config)
brain.initialize()

# Collect firing rates
firing_rates = []
for step in range(100):
    brain.step(step)
    firing_rates.append(brain.getAverageFiringRate())

# Plot results
plt.figure(figsize=(10, 6))
plt.plot(firing_rates)
plt.xlabel('Step')
plt.ylabel('Average Firing Rate')
plt.title('Neural Activity Over Time')
plt.grid(True, alpha=0.3)
plt.show()

# Create world and visualize agent path
world = nlm.World()
world.configure(50, 50)
world.set_agent_start(5, 5)

agent_positions = [(5, 5)]
for step in range(50):
    action = nlm.Action(nlm.ActionType.MoveForward)
    world.apply_action(action, step * 0.1)
    body = world.get_agent_body()
    agent_positions.append((body.x, body.y))

# Plot agent trajectory
x, y = zip(*agent_positions)
plt.figure(figsize=(8, 8))
plt.plot(x, y, 'b-', linewidth=2, label='Agent path')
plt.scatter(x[0], y[0], c='green', s=100, label='Start')
plt.scatter(x[-1], y[-1], c='red', s=100, label='End')
plt.xlabel('X Position')
plt.ylabel('Y Position')
plt.title('Agent Trajectory')
plt.legend()
plt.grid(True, alpha=0.3)
plt.show()
```

API Reference:
--------------

Classes:
--------

BrainSimulator: Context manager for safe brain simulation
----------------------------------------------
- `__init__(brain, auto_reset=True)`
- `enter()` / `exit()` - Context manager support
- `step(step, time=0.0)` - Perform a simulation step
- `run_steps(num_steps, time_step=0.001)` - Run multiple steps
- `run_until(condition, max_steps=10000, time_step=0.001)` - Run until condition
- `get_brain()` - Get underlying brain object
- `get_stats()` - Get simulation statistics

Config: Enhanced configuration class
------------------------
- `__init__(filepath="")` - Create config, optionally load from file
- `set_int(key, value)` - Set integer value (method chaining)
- `set_float(key, value)` - Set float value (method chaining)
- `set_double(key, value)` - Set double value (method chaining)
- `set_string(key, value)` - Set string value (method chaining)
- `get_int(key, default_val=0)` - Get integer value
- `get_float(key, default_val=0.0)` - Get float value
- `get_double(key, default_val=0.0)` - Get double value
- `get_string(key, default_val="")` - Get string value
- `has(key)` - Check if key exists
- `get_keys()` - Get all keys
- `save(filepath)` - Save to file
- `load(filepath)` - Load from file
- `summary()` - Get summary string

Action: Action representation
---------------------
- `__init__(type=None, parameters=[])` - Create action
- `type` - Action type property
- `parameters` - Parameters list property
- `get_name()` - Get action name
- `clone()` - Clone action
- `str()` / `__repr__()` - String representations

World: Enhanced world simulation
-------------------------
- `__init__()` - Create world
- `configure(width, height, vision_width=32, vision_height=32)` - Configure dimensions
- `reset()` - Reset world
- `set_agent_start(x, y)` - Set agent start position
- `apply_action(action, current_time=0.0)` - Apply action
- `get_percept()` - Get percept
- `get_agent_body()` - Get agent body
- `add_object(obj)` - Add world object
- `remove_object(x, y)` - Remove object
- `is_valid_position(x, y)` - Check position validity
- `update(timestep=0.001)` - Update world
- `get_stats()` - Get world statistics

Phase6IntegrationResult: Integration test results
------------------------------------------------
- `get_total_reward()` - Get total reward
- `get_avg_firing_rate()` - Get average firing rate
- `get_memory_episodes_stored()` - Get stored episodes
- `get_novelty_level()` - Get novelty level
- `get_curiosity_level()` - Get curiosity level
- `get_dopamine_level()` - Get dopamine level
- `get_total_wall_clock_time()` - Get wall clock time
- `is_working_memory_integrated()` - Check working memory integration
- `is_episodic_memory_integrated()` - Check episodic memory integration
- `is_neuromodulation_integrated()` - Check neuromodulation integration
- `is_prediction_integrated()` - Check prediction integration
- `is_development_integrated()` - Check development integration
- `is_checkpointing_works()` - Check checkpointing
- `is_replay_works()` - Check replay
- `to_dict()` - Convert to Python dictionary

Enums:
------
- `NeuronType`: Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal
- `SynapseType`: Excitatory, Inhibitory, Modulatory, Electrical, GapJunction
- `DevelopmentalStage`: Initial, CriticalPeriod, Maturation, Adult, Aging
- `FiringState`: Resting, Active, Refractory, Inhibited
- `ActionType`: MoveForward, MoveBackward, MoveLeft, MoveRight, TurnLeft, TurnRight, Look, LookUp, LookDown, Interact, Eat, Drink, Rest, Wait, Custom
- `MotorCommand`: MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight, Interact, Wait
- `WorldObjectType`: Empty, Resource, Hazard, Wall, Marker

Functions:
----------
- `create_brain(config)` - Create brain with configuration
- `create_simple_world()` - Create simple world
- `create_agent_brain(brain)` - Create agent brain interface
- `create_default_config()` - Create default configuration
- `create_brain_simulator(brain, auto_reset=True)` - Create brain simulator
- `run_phase6_integration(config)` - Run Phase 6 integration test
- `verify_phase6_integration()` - Verify Phase 6 integration
- `create_test_config(neuron_count=100, region_count=1, connection_prob=0.1)` - Create test config
- `get_version()` - Get version string

Exceptions:
-----------
- `NLMError` - Base NLM error class
- `NLMInvalidArgument` - Invalid argument error

Version:
--------
NLM Enhanced Python Bindings v2.0

Copyright:
----------
MIT License - See LICENSE file for details
