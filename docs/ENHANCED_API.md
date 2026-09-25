# Enhanced NLM Python API Documentation

## Overview

The Enhanced NLM (Neural Learning Machine) Python API provides a user-friendly interface for working with the NLM neural simulation framework. It builds upon the original basic bindings and adds high-level abstractions, simplified interfaces, and example scripts for common use cases.

## Key Features

### 1. Simple One-Liner Operations

The enhanced API allows common tasks to be completed with minimal code:

```python
import enhanced_pynlm as nlm

# Create and initialize a brain
brain = nlm.createBrain()
brain.initialize()

# Run simulation
brain.simulate(100)

# Get statistics
stats = brain.getStats()
print(f"Neurons: {stats['total_neurons']}, Firing: {stats['firing_neurons']}")
```

### 2. High-Level Abstractions

#### Brain Class

```python
import enhanced_pynlm as nlm

# Create brain with automatic initialization
brain = nlm.Brain()
brain.initialize()

# Simple simulation
brain.step(0, [0.5, 0.8, 0.3, 0.1, 0.9])  # step with sensory input
brain.simulate(100)  # run 100 steps

# Save with automatic filename
brain.save("my_brain")
```

#### World Class

```python
import enhanced_pynlm as nlm

# Create world with custom dimensions
world = nlm.World()
world.setup(width=30, height=30, visionWidth=12, visionHeight=12)
world.setAgentPosition(15.0, 15.0)

# Update world and get observation
observation = world.update(0.1)
print(f"Observation shape: {len(observation)}")
```

#### Agent Class

```python
import enhanced_pynlm as nlm

# Create agent with brain
agent = nlm.Agent(brain)
agent.initialize()
agent.enableAll()  # Enable all learning features

# Run complete episode
results = agent.runEpisode(maxSteps=500)
print(f"Average reward: {results['total_reward']}")
print(f"Final position: ({results['final_x']}, {results['final_y']})")
```

### 3. Command-Line Interface Helpers

The enhanced API includes command-line interface functions:

```python
import enhanced_pynlm as nlm

# Quick simulation with command-line helpers
brain = nlm.createBrain("config.json")
world = nlm.createWorld(20, 20)
stats = nlm.runSimulation(brain, 100)
```

### 4. Complete Experiment Examples

#### Example 1: Simple Brain Simulation

```python
import enhanced_pynlm as nlm

# Create and run a simple simulation
brain = nlm.Brain()
brain.initialize()
brain.simulate(1000)

# Save results
brain.save("simple_simulation")

# Display results
stats = brain.getStats()
print(f"Simulation completed with {stats['total_neurons']} neurons")
print(f"Total spikes: {stats['total_spikes']}")
print(f"Development stage: {stats['development_stage']}")
```

#### Example 2: Complete Agent-World Experiment

```python
import enhanced_pynlm as nlm

# Setup environment
brain = nlm.Brain()
world = nlm.World()
world.setup(width=25, height=25)

# Create and initialize agent
agent = nlm.Agent(brain)
agent.initialize()
agent.enableAll()

# Run exploration episode
print("Starting agent exploration...")
results = agent.runEpisode(maxSteps=1000)

# Display results
print(f"Exploration completed!")
print(f"Average reward per step: {results['total_reward']}")
print(f"Final position: ({results['final_x']:.1f}, {results['final_y']:.1f})")
print(f"Success rate: {100 * results['final_x'] / 25:.1f}%")
```

#### Example 3: Learning with Memory

```python
import enhanced_pynlm as nlm

# Create brain with memory support
brain = nlm.Brain()
brain.initialize()

# Simulate with learning
print("Running learning simulation...")
for step in range(500):
    # Inject some sensory input
    sensory_input = [0.5 * (step % 10) / 10 for _ in range(8)]
    brain.step(step, sensory_input)
    
    # Print progress
    if step % 100 == 0:
        stats = brain.getStats()
        print(f"Step {step}: {stats['firing_neurons']} firing neurons")

# Check memory state
stats = brain.getStats()
if 'working_memory_traces' in stats:
    print(f"Working memory traces: {stats['working_memory_traces']}")
if 'episodic_memory_episodes' in stats:
    print(f"Episodic memory episodes: {stats['episodic_memory_episodes']}")

brain.save("learning_simulation")
```

### 5. Advanced Usage

For advanced users, the enhanced API also provides access to the underlying objects:

```python
import enhanced_pynlm as nlm

# Access underlying C++ objects
brain = nlm.Brain()
brain.initialize()

# Get the underlying brain object
underlying_brain = brain.getBrain()

# Use it with the original API (advanced usage)
underlying_brain.logStatus()

# Configure custom settings
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
```

## Installation

The enhanced API is available as part of the NLM Python package:

```bash
pip install enhanced-pynlm
```

Or install from source:

```bash
pip install -e .
```

## Module Structure

The enhanced module provides two interfaces:

1. **Basic API** (`nlm`): Original bindings, direct C++ access
2. **Enhanced API** (`enhanced_pynlm`): User-friendly abstractions

## Available Classes

### Brain

| Method | Description |
|--------|-------------|
| `initialize()` | Initialize the brain with configuration |
| `step(step, input)` | Run one simulation step with optional input |
| `simulate(steps)` | Run multiple simulation steps |
| `getStats()` | Get brain statistics as dictionary |
| `save(name)` | Save brain state with automatic filename |
| `getBrain()` | Get underlying brain object for advanced use |

### World

| Method | Description |
|--------|-------------|
| `setup(width, height, visionWidth, visionHeight)` | Configure world dimensions |
| `setAgentPosition(x, y)` | Set agent starting position |
| `update(timestep)` | Update world and return observation |
| `applyAction(actionType, currentTime)` | Apply action to world |
| `getState()` | Get world state as dictionary |
| `getWorld()` | Get underlying world object |

### Agent

| Method | Description |
|--------|-------------|
| `initialize()` | Initialize agent with world |
| `runEpisode(maxSteps)` | Run complete agent episode |
| `enableAll()` | Enable all learning subsystems |

## Command-Line Functions

| Function | Description |
|----------|-------------|
| `createBrain(configFile)` | Create brain with optional config file |
| `createWorld(width, height, visionWidth, visionHeight)` | Create world with dimensions |
| `createAgentBrain(brain)` | Create agent brain interface |
| `runSimulation(brain, steps)` | Run simulation and return statistics |
| `runCompleteExperiment(brain, world, maxSteps)` | Run complete experiment |

## Type Constants

```python
# Neuron types
nlm.NeuronType.Excitatory
nlm.NeuronType.Inhibitory
nlm.NeuronType.Sensory
nlm.NeuronType.Motor

# Action types
nlm.ActionType.MoveForward
nlm.ActionType.TurnLeft
nlm.ActionType.Interact
nlm.ActionType.Wait
```

## Best Practices

### 1. Use the Enhanced API for Simple Tasks

For most common use cases, use the enhanced API:

```python
# Good
brain = nlm.Brain()
brain.initialize()
brain.simulate(100)

# Less efficient (still works)
import nlm
config = nlm.createDefaultConfig()
brain = nlm.Brain(config)
brain.initialize()
brain.step(0)
brain.step(1)
```

### 2. Save Results Regularly

Always save your progress:

```python
brain.save("checkpoints/simulation_001")
# ... continue simulation
brain.save("checkpoints/simulation_002")
```

### 3. Monitor Progress

Use the stats function to monitor your simulation:

```python
stats = brain.getStats()
print(f"Progress: Step {stats.get('step', 'unknown')}, "
      f"Neurons: {stats['total_neurons']}, "
      f"Firing: {stats['firing_neurons']}")
```

### 4. Use Appropriate World Size

Choose world sizes that balance realism and performance:

```python
# Good for most tasks
world.setup(width=20, height=20)

# Large world for complex navigation
world.setup(width=50, height=50, visionWidth=16, visionHeight=16)

# Small world for quick testing
world.setup(width=10, height=10)
```

## Troubleshooting

### Common Issues and Solutions

1. **Brain initialization fails**
   - Ensure you call `initialize()` before using the brain
   - Check that the configuration is valid

2. **Memory usage is high**
   - Use smaller world sizes
   - Limit the number of simulation steps
   - Save progress regularly to free memory

3. **Actions don't work correctly**
   - Ensure the agent is properly initialized
   - Check that the world has a valid agent position
   - Verify that the brain has motor neurons

4. **Sensor input not processed**
   - Ensure you're using the correct input format
   - Check that the brain has sensory neurons
   - Verify that you're calling `step()` with input

### Error Messages

If you encounter errors, check:

- Required method calls
- Input formats and dimensions
- Configuration file paths
- System resource availability

## Examples Directory

The enhanced API includes example scripts in the `examples/` directory:

- `simple_brain.py` - Basic brain simulation
- `agent_exploration.py` - Complete agent-world experiment
- `learning_simulation.py` - Advanced learning with memory
- `benchmark.py` - Performance benchmarking

These examples demonstrate common use cases and best practices.

## Performance Tips

### 1. Batch Operations

For large simulations, batch operations:

```python
# Process multiple steps at once
brain.simulate(1000)

# Process multiple inputs efficiently
inputs = [[i/100 for i in range(8)] for _ in range(100)]
for i, input_data in enumerate(inputs):
    brain.step(i, input_data)
```

### 2. Memory Management

Monitor and manage memory:

```python
# Get stats and monitor memory
stats = brain.getStats()
print(f"Working memory: {stats.get('working_memory_traces', 0)} traces")

# Save progress regularly
brain.save("checkpoints/simulation_001")
```

### 3. Use Appropriate Timesteps

Choose timesteps that balance accuracy and performance:

```python
# Use larger timesteps for faster simulation
brain.step(0, input, timestep=0.1)  # Faster
brain.step(0, input, timestep=0.001)  # More accurate but slower
```

## Migration Guide

### From Basic API to Enhanced API

If you're currently using the basic API, migrate to the enhanced API with these changes:

#### Before

```python
import nlm

config = nlm.createDefaultConfig()
brain = nlm.Brain(config)
brain.initialize()

for i in range(100):
    brain.step(i)

stats = {
    "neurons": brain.getTotalNeuronCount(),
    "spikes": brain.getTotalSpikeCount(),
    "firing": brain.getFiringNeuronCount()
}
```

#### After

```python
import enhanced_pynlm as nlm

brain = nlm.Brain()
brain.initialize()
brain.simulate(100)

stats = brain.getStats()
print(f"Neurons: {stats['total_neurons']}, "
      f"Spikes: {stats['total_spikes']}, "
      f"Firing: {stats['firing_neurons']}")
```

### Performance Comparison

The enhanced API provides similar performance with much simpler syntax. For most use cases, the performance difference is negligible.

## Conclusion

The Enhanced NLM Python API makes it easy to work with the NLM neural simulation framework without sacrificing power or flexibility. Whether you're doing quick experiments or running large-scale simulations, the enhanced API provides the right tools for the job.

Start with the simple one-liner operations and gradually move to more complex scenarios as you become more familiar with the framework.
