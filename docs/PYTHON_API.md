# NLM Python API Documentation

This document provides comprehensive documentation for the NLM Python bindings,
including the original API and the new convenience functions added for improved
usability and performance.

## Overview

The NLM Python bindings (`pynlm`) provide a Python interface to the NLM C++ neural
simulation framework. The library allows you to create and simulate brain-like
neural networks, interact with virtual environments, and study learning and
development.

## Quick Start

```python
# Import the library
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simulation
for step in range(100):
    brain.step(step)
    
print(f"Brain has {brain.getTotalNeuronCount()} neurons")
print(f"Generated {brain.getTotalSpikeCount()} spikes")
```

## Core Classes

### Brain Class
The main class representing a neural brain simulation.

#### Creation
```python
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
```

#### Initialization
```python
brain.initialize()  # Always call this before using the brain!
```

#### Simulation
```python
for step in range(1000):
    brain.step(step)
```

#### State Access
```python
brain.getTotalNeuronCount()        # Total neurons
brain.getTotalSynapseCount()       # Total synapses  
brain.getFiringNeuronCount()      # Currently firing
brain.getAverageFiringRate()        # Average firing rate
brain.getTotalSpikeCount()         # Total spikes generated
brain.getExcitationInhibitionRatio()  # E/I balance
```

#### State Management
```python
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state
brain.reset()                      # Reset to initial state
```

### Config Class
Configuration management for brain parameters.

#### Creation
```python
config = pynlm.createDefaultConfig()
```

#### Configuration
```python
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("random_seed", 42)
```

#### Query
```python
if config.has("brain.neuron_count"):
    count = config.getOr("brain.neuron_count", 1000)
```

#### File I/O
```python
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

### SimpleWorld Class
Virtual environment for the brain to interact with.

#### Creation
```python
world = pynlm.createSimpleWorld()
```

#### Configuration
```python
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
```

#### Control
```python
world.reset()
world.update(timestep)  # Advance simulation
world.setAgentStart(x, y)
```

#### State Access
```python
percept = world.getSensoryPercept()  # What the agent sees
body = world.getAgentBody()         # Agent's position and state
```

### AgentBrain Class
Bridge between brain and world.

#### Creation
```python
agent = pynlm.createAgentBrain(brain)
```

#### Initialization
```python
agent.initialize(world)
```

#### Processing
```python
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)  # Brain receives sensory input
action = agent.decodeMotorCommand()  # Brain decides action
```

#### Neuromodulation
```python
agent.applyRewardModulation(reward, predicted_reward)
```

#### Development
```python
agent.updateDevelopment(0.1)  # Brain matures over time
```

## New Convenience Functions

The following convenience functions have been added to improve usability and performance:

### Batch Operations

#### `batch_step(brain, steps)`
Perform multiple brain steps efficiently.

```python
# Run 100 steps at once
pynlm.batch_step(brain, 100)
```

#### `run_simulation_with_stats(brain, steps, verbose=False)`
Run simulation and return statistics for each step.

```python
# Run 500 steps and collect stats
stats = pynlm.run_simulation_with_stats(brain, 500, verbose=True)
for step_data in stats:
    print(f"Step {step_data['step']}: {step_data['firing_neurons']} firing neurons")
```

### Brain Statistics

#### `get_brain_stats(brain)`
Get comprehensive brain statistics as a dictionary.

```python
stats = pynlm.get_brain_stats(brain)
print(f"Neurons: {stats['neuron_count']}")
print(f"Firing rate: {stats['average_firing_rate']:.2f}")
```

### Configuration Helpers

#### `create_brain_with_params(neuron_count, region_count)`
Create a brain with specified parameters.

```python
# Custom-sized brain
brain = pynlm.create_brain_with_params(neuron_count=500, region_count=3)
brain.initialize()
```

#### `create_config_from_dict(config_dict)`
Create configuration from Python dictionary.

```python
config_dict = {
    "brain.neuron_count": 1500,
    "brain.synapse_density": 0.2,
    "random_seed": 123
}
config = pynlm.create_config_from_dict(config_dict)
```

#### `config_to_dict(config)`
Get configuration keys (values hidden for security).

```python
config_keys = pynlm.config_to_dict(config)
print(f"Config has {config_keys['key_count']} keys")
```

#### `validate_config(config)`
Validate configuration and return validation results.

```python
is_valid, errors = pynlm.validate_config(config)
if not is_valid:
    print(f"Validation errors: {errors}")
```

### World Statistics

#### `get_world_stats(world)`
Get world statistics as a dictionary.

```python
world_stats = pynlm.get_world_stats(world)
print(f"World size: {world_stats['width']}x{world_stats['height']}")
print(f"Vision: {world_stats['vision_width']}x{world_stats['vision_height']}")
```

### State Management

#### `is_brain_ready(brain)`
Check if brain is properly initialized and ready for use.

```python
if pynlm.is_brain_ready(brain):
    print("Brain is ready for simulation")
```

#### `get_all_region_ids(brain)`
Get all region IDs as a list.

```python
region_ids = pynlm.get_all_region_ids(brain)
print(f"Number of regions: {len(region_ids)}")
```

#### `safe_save(brain, filepath)`
Safely save brain state with error checking.

```python
pynlm.safe_save(brain, "my_brain.bin")
```

#### `safe_load(brain, filepath)`
Safely load brain state with error checking.

```python
pynlm.safe_load(brain, "my_brain.bin")
```

### Agent Operations

#### `batch_sensory_processing(agent, percepts)`
Process multiple sensory inputs efficiently.

```python
percepts = [world.getSensoryPercept() for _ in range(10)]
pynlm.batch_sensory_processing(agent, percepts)
```

## Advanced Usage Patterns

### Complete Agent Simulation

```python
def run_agent_simulation(brain, world, agent, num_steps=1000):
    """Complete agent simulation with brain and world."""
    
    # Enable learning and development features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get and apply action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log statistics periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Use the pattern
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

run_agent_simulation(brain, world, agent, 1000)
```

### Brain Benchmarking

```python
def benchmark_brain(neuron_counts, steps_per_brain=100):
    """Benchmark multiple brain configurations."""
    results = {}
    
    for count in neuron_counts:
        # Create brain with specific neuron count
        brain = pynlm.create_brain_with_params(neuron_count=count)
        brain.initialize()
        
        # Run simulation and collect stats
        stats = pynlm.run_simulation_with_stats(brain, steps_per_brain)
        
        # Calculate average firing rate
        avg_firing = sum(s['firing_rate'] for s in stats) / len(stats)
        
        results[count] = {
            'avg_firing_rate': avg_firing,
            'final_spike_count': stats[-1]['spike_count'],
            'steps_completed': len(stats)
        }
        
        print(f"Neuron count {count}: {avg_firing:.3f} avg firing rate")
    
    return results

# Run benchmark
neuron_counts = [100, 500, 1000, 2000]
benchmark_results = benchmark_brain(neuron_counts)
```

### Configuration Comparison

```python
def compare_configurations(config_dict_list):
    """Compare different configuration settings."""
    from collections import defaultdict
    
    comparison = defaultdict(dict)
    
    for i, config_dict in enumerate(config_dict_list):
        config = pynlm.create_config_from_dict(config_dict)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        stats = pynlm.run_simulation_with_stats(brain, 200)
        
        # Store results
        comparison[i]['config'] = config_dict
        comparison[i]['avg_firing_rate'] = sum(
            s['firing_rate'] for s in stats
        ) / len(stats)
        comparison[i]['total_spikes'] = stats[-1]['spike_count']
    
    # Print comparison
    print("Configuration Comparison:")
    print("-" * 50)
    for i, results in comparison.items():
        print(f"Config {i}:")
        print(f"  Neuron count: {results['config'].get('brain.neuron_count', 'N/A')}")
        print(f"  Avg firing rate: {results['avg_firing_rate']:.3f}")
        print(f"  Total spikes: {results['total_spikes']}")
        print()
    
    return comparison
```

## Enumerations

The following enumerations are available for specifying types:

### NeuronType
- `NeuronType.Excitatory`
- `NeuronType.Inhibitory`
- `NeuronType.Modulatory`
- `NeuronType.Sensory`
- `NeuronType.Motor`
- `NeuronType.Internal`

### SynapseType
- `SynapseType.Excitatory`
- `SynapseType.Inhibitory`
- `SynapseType.Modulatory`
- `SynapseType.Electrical`
- `SynapseType.GapJunction`

### DevelopmentalStage
- `DevelopmentalStage.Initial`
- `DevelopmentalStage.CriticalPeriod`
- `DevelopmentalStage.Maturation`
- `DevelopmentalStage.Adult`
- `DevelopmentalStage.Aging`

### ActionType
- `ActionType.MoveForward`
- `ActionType.MoveBackward`
- `ActionType.TurnLeft`
- `ActionType.TurnRight`
- `ActionType.Look`
- `ActionType.Interact`
- `ActionType.Eat`
- `ActionType.Drink`
- `ActionType.Rest`
- `ActionType.Wait`
- `ActionType.Custom`

### WorldObjectType
- `WorldObjectType.Empty`
- `WorldObjectType.Resource`
- `WorldObjectType.Hazard`
- `WorldObjectType.Wall`
- `WorldObjectType.Marker`

## Error Handling

The Python bindings include comprehensive error handling:

1. **TypeError**: Invalid arguments passed to functions
2. **ValueError**: Invalid values (e.g., negative neuron counts)
3. **RuntimeError**: Runtime errors (e.g., file I/O errors)

Convenience functions like `safe_save()` and `safe_load()` include additional validation.

## Performance Tips

1. **Use batch operations**: For multiple steps, use `batch_step()` instead of individual `step()` calls.

2. **Collect statistics efficiently**: Use `run_simulation_with_stats()` to collect data without manual loops.

3. **Reuse configurations**: Create configuration objects and modify them for multiple brains.

4. **Enable appropriate features**: Only enable subsystems (reward modulation, curiosity, etc.) as needed.

5. **Monitor brain state**: Use `get_brain_stats()` to monitor brain performance without calling multiple methods.

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"
- Did you call `brain.initialize()`? Always call this before using the brain!

**"The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Ensure `world.update()` is being called in your simulation loop.

**"Everything is 0"
- Brains need time to "warm up" - try more steps.
- Some neurons need input to fire - make sure sensory input is connected.

**"ImportError: No module named 'pynlm'"
- Install the module: `pip install .` or `pip install -e .`

**"Segmentation fault"
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

### Getting Help

For help with NLM:

1. **Documentation**: Check `docs/ARCHITECTURE.md` for system architecture
2. **Examples**: See `easy_usage.md` and `HOW_TO_USE.md` for usage examples
3. **Science**: See `docs/SCIENCE.md` for scientific background
4. **Experiments**: See `docs/EXPERIMENTS.md` for experiment descriptions

## Version Information

This version of the NLM Python bindings includes:
- Core NLM API (original)
- Convenience functions for improved usability
- Comprehensive error handling
- Batch processing optimizations
- Configuration validation tools
- Enhanced documentation

The bindings are designed to be both easy to use for beginners and powerful for advanced users.

## License

MIT License - See `LICENSE` file for details.