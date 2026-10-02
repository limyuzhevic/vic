# NLM Python Library - Comprehensive Examples and Documentation

## Overview

The NLM (Neural Learning Machine) Python library provides a high-level interface to the C++ neural simulation framework, making it easy to create, simulate, and analyze artificial neural systems.

## Installation

```bash
pip install pynlm

# Or install from source
pip install -e .
```

## Basic Usage Examples

### 1. Simple Brain Simulation

```python
import pynlm

# Create a default configuration
config = pynlm.create_default_config()

# Create and initialize a brain
brain = pynlm.create_brain(config)
brain.initialize()

# Run a simple simulation
for i in range(100):
    brain.step(i)

# Get brain statistics
print(f"Total neurons: {brain.get_total_neuron_count()}")
print(f"Total synapses: {brain.get_total_synapse_count()}")
print(f"Total spikes: {brain.get_total_spike_count()}")
print(f"Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
```

### 2. Complete Agent Simulation

```python
import pynlm

# Create a complete simulation
brain, world, agent = pynlm.create_simple_simulation()

# Run an episode
result = pynlm.run_episode(brain, world, agent, 1000)

print(f"Episode completed!")
print(f"Total spikes: {result['total_spikes']}")
print(f"Final developmental stage: {result['final_stage']}")
print(f"Final reward level: {result['reward_level']:.3f}")
```

### 3. Using Context Managers for Automatic Cleanup

```python
import pynlm

# Create configuration
config = pynlm.create_default_config()

# Use context manager for automatic cleanup
with pynlm.brain_wrapper(pynlm.create_brain(config)) as brain_wrapper:
    # Initialize and run simulation
    brain_wrapper.run(1000, 0.001)
    
    # Get statistics
    stats = brain_wrapper.get_stats()
    print(f"Brain statistics: {stats}")

# Brain is automatically cleaned up here
```

## Advanced Examples

### 4. Enhanced Simulation with Advanced Features

```python
import pynlm

# Create an advanced simulation with enhanced features
brain, world, agent, config = pynlm.create_advanced_simulation()

# Enable all subsystems
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)
agent.enable_curiosity(True)

# Run simulation with monitoring
for step in range(500):
    world.update(0.1)
    
    # Get sensor input
    percept = world->get_sensory_percept()
    
    # Process sensory input
    agent.process_sensory_input(percept)
    
    # Brain computation
    brain.step(step)
    
    # Get action
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())
    
    # Apply neuromodulation if available
    if agent.get_neuromodulation_level() > 0:
        reward = world.get_sensory_percept().get_internal()[0] if world.get_sensory_percept().get_internal() else 0.0
        agent.apply_reward_modulation(reward, 0.0)
        agent.update_development(0.1)
    
    # Monitor progress
    if step % 100 == 0:
        stats = brain.get_stats()
        print(f"Step {step}: {stats['total_spikes']} spikes, "
              f"{stats['firing_neurons']} firing neurons, "
              f"stage {stats['developmental_stage']}")

# Save final state
pynlm.save_brain_state(brain, "brain_checkpoint.bin")
print("Brain state saved!")
```

### 5. Batch Processing

```python
import pynlm

# Create multiple simulation configurations
simulations = []

for seed in [42, 123, 456, 789]:
    config = pynlm.create_default_config()
    config.set("random_seed", seed)
    config.set("neuron_count", 500)
    
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    world = pynlm.create_world()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = pynlm.create_agent(brain)
    agent.initialize(world)
    
    simulations.append((brain, world, agent))

# Run all simulations in batch
results = pynlm.run_batch_simulation(simulations, 200, 10)

print(f"Ran {len(results)} batch simulations")
for i, result in enumerate(results):
    print(f"Episode {i}: {result['simulation_0']['final_spikes']} spikes")
```

### 6. Performance Benchmarking

```python
import pynlm
import time

# Create simulation
brain, world, agent = pynlm.create_simple_simulation()

# Benchmark different simulation lengths
benchmarks = []

for steps in [100, 500, 1000, 2000]:
    start_time = time.time()
    
    # Run simulation
    for i in range(steps):
        world.update(0.1)
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        brain.step(i)
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
    
    end_time = time.time()
    
    # Get benchmark results
    benchmark = pynlm.benchmark_simulation(brain, world, agent, steps)
    benchmark["wall_time"] = end_time - start_time
    benchmarks.append(benchmark)
    
    print(f"Steps: {steps}, "
          f"Time: {benchmark['elapsed_time_ms']:.0f}ms, "
          f"Rate: {benchmark['steps_per_second']:.0f} steps/sec, "
          f"Spikes: {benchmark['total_spikes']}")

# Analyze benchmark results
print("\nBenchmark Analysis:")
print("=================")
for bench in benchmarks:
    print(f"Steps: {bench['steps_completed']}, "
          f"Time: {bench['elapsed_time_ms']:.0f}ms, "
          f"Rate: {bench['steps_per_second']:.0f} steps/sec")
```

### 7. Memory Management

```python
import pynlm

# Create brain with memory monitoring
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Monitor memory usage
memory_info = pynlm.get_memory_usage(brain)
print(f"Memory usage information:")
for key, value in memory_info.items():
    print(f"  {key}: {value}")

# Optimize brain memory usage
pynlm.optimize_brain(brain)

# Check memory usage again
memory_info = pynlm.get_memory_usage(brain)
print(f"\nMemory usage after optimization:")
for key, value in memory_info.items():
    print(f"  {key}: {value}")
```

### 8. Checkpoint and Resume

```python
import pynlm

# Create simulation
brain, world, agent = pynlm.create_simple_simulation()

# Run simulation with checkpoint support
checkpoint_file = "simulation_checkpoint.bin"
result = pynlm.simulate_with_checkpoint(brain, world, agent, 500, checkpoint_file)

print(f"Checkpoint simulation completed!")
print(f"Loaded checkpoint: {result['checkpoint_loaded']}")
print(f"Saved checkpoint: {result['checkpoint_saved']}")
print(f"Final spikes: {result['final_spikes']}")

# Load checkpoint and continue simulation
print("\nLoading checkpoint and continuing simulation...")
loaded_brain = pynlm.load_brain_state(checkpoint_file, "configs/default.cfg")
if loaded_brain:
    # Continue simulation with loaded brain
    for i in range(200):
        world.update(0.1)
        # ... rest of simulation logic
        
    print("Successfully continued simulation from checkpoint!")
else:
    print("Failed to load checkpoint")
```

### 9. Data Export and Analysis

```python
import pynlm

# Create simulation
brain, world, agent = pynlm.create_simple_simulation()

# Run simulation
for i in range(100):
    world.update(0.1)
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    brain.step(i)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())

# Export simulation data
pynlm.export_simulation_data(brain, world, "simulation_data.txt")
print("Simulation data exported to 'simulation_data.txt'")

# Load and analyze exported data
with open("simulation_data.txt", "r") as file:
    data = file.read()
    print("Exported data preview:")
    print(data[:500] + "...")  # First 500 characters
```

## Configuration Examples

### 10. Custom Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.create_default_config()

# Set custom parameters
config.set("neuron_count", 2000)
config.set("synapse_density", 0.15)
config.set("random_seed", 12345)
config.set("simulation_timestep", 0.001)

# Save configuration
config.save_to_file("custom_config.json")
print("Custom configuration saved to 'custom_config.json'")

# Load configuration
loaded_config = pynlm.create_default_config()
loaded_config.load_from_file("custom_config.json")
print(f"Loaded configuration has {loaded_config.get_keys()} keys")
```

## Working with Enumerations

### 11. Using Neuron Types

```python
import pynlm

# Access neuron type enumerations
print("Neuron types:")
for name, neuron_type in pynlm.NEURON_TYPES.items():
    print(f"  {name}: {neuron_type}")

# Create neurons of specific types
config = pynlm.create_default_config()
config.set("neuron_count", 1000)

brain = pynlm.create_brain(config)
brain.initialize()

# Get all regions and their populations
for region_id in brain.get_region_ids():
    region = brain.get_region(region_id)
    print(f"Region {region_id}: {region.get_name()}")
    
    for pop_id in region.get_population_ids():
        population = region.get_population(pop_id)
        print(f"  Population {pop_id}: {population.get_neuron_type()}, "
              f"{population.get_neuron_count()} neurons")
```

### 12. Using Action Types

```python
import pynlm

# Access action type enumerations
print("Action types:")
for name, action_type in pynlm.ACTION_TYPES.items():
    print(f"  {name}: {action_type}")

# Create different action types
from pynlm import Action

# Create various actions
actions = [
    Action(pynlm.ActionType.MoveForward),
    Action(pynlm.ActionType.TurnLeft),
    Action(pynlm.ActionType.Interact),
    Action(pynlm.ActionType.Custom, [0.5, 0.3, 0.7])
]

for i, action in enumerate(actions):
    print(f"Action {i}: type={action.get_type()}, "
          f"name='{action.get_name()}', "
          f"parameters={action.get_parameters()}")
```

## Error Handling

### 13. Error Handling Examples

```python
import pynlm

# Error handling examples
try:
    # Create brain and try to run invalid operation
    brain = pynlm.create_brain(pynlm.create_default_config())
    brain.initialize()
    
    # This should work fine
    brain.step(0)
    
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
    
except pynlm.InvalidArgumentError as e:
    print(f"Invalid argument error: {e}")
    
except pynlm.OutOfRangeError as e:
    print(f"Out of range error: {e}")
    
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Tips

### 14. Performance Optimization Tips

```python
import pynlm

# Performance optimization tips
tips = [
    "Use context managers for automatic cleanup",
    "Enable appropriate subsystems (don't enable unused features)",
    "Use batch processing for multiple simulations",
    "Monitor memory usage and optimize when needed",
    "Use checkpointing for long-running simulations",
    "Batch similar operations together",
    "Use efficient data structures for large simulations",
    "Consider parallel processing for independent simulations"
]

print("Performance Optimization Tips:")
print("=" * 40)
for i, tip in enumerate(tips, 1):
    print(f"{i}. {tip}")
```

## Integration Examples

### 15. Integration with External Systems

```python
import pynlm
import json
from datetime import datetime

# Example: Integration with external monitoring system
def monitor_simulation(brain, world, agent):
    """Monitor simulation and collect metrics"""
    metrics = {
        'timestamp': datetime.now().isoformat(),
        'neuron_count': brain.get_total_neuron_count(),
        'spike_count': brain.get_total_spike_count(),
        'firing_neurons': brain.get_firing_neuron_count(),
        'development_stage': brain.get_developmental_stage(),
        'world_time': world.get_simulation_time(),
        'curiosity_level': agent.get_curiosity_level(),
        'novelty_level': agent.get_novelty_level()
    }
    
    return metrics

# Run simulation with monitoring
brain, world, agent = pynlm.create_simple_simulation()
metrics_log = []

for step in range(100):
    world.update(0.1)
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    brain.step(step)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())
    
    # Collect metrics
    metrics = monitor_simulation(brain, world, agent)
    metrics_log.append(metrics)

# Export metrics to JSON
with open("simulation_metrics.json", "w") as f:
    json.dump(metrics_log, f, indent=2)

print(f"Collected {len(metrics_log)} metrics and saved to 'simulation_metrics.json'")
```

## Troubleshooting

### 16. Common Issues and Solutions

```python
import pynlm

# Common troubleshooting examples

def troubleshoot_simulation(config, brain, world, agent):
    """Troubleshoot common simulation issues"""
    
    print("Troubleshooting simulation...")
    
    # Check if brain was initialized
    if not brain:
        print("ERROR: Brain not created properly")
        return False
    
    # Check if world was configured
    if world.get_width() == 0 or world.get_height() == 0:
        print("ERROR: World dimensions not set properly")
        return False
    
    # Check if agent has sensory input
    percept = world.get_sensory_percept()
    if not percept.get_vision():
        print("WARNING: No visual input available")
        # Check other sensory modalities
        if not percept.get_touch() and not percept.get_internal():
            print("ERROR: No sensory input available")
            return False
    
    # Check if brain has neurons
    if brain.get_total_neuron_count() == 0:
        print("ERROR: No neurons in brain")
        return False
    
    # Run a minimal test
    try:
        brain.initialize()
        brain.step(0)
        world.update(0.1)
        print("All checks passed!")
        return True
    except Exception as e:
        print(f"ERROR during test run: {e}")
        return False

# Example usage
troubleshoot_simulation(config, brain, world, agent)
```

## Advanced Features

### 17. Advanced Feature Usage

```python
import pynlm

# Advanced feature examples

# 1. Create a complex simulation with multiple regions
config = pynlm.create_default_config()
config.set("neuron_count", 3000)
config.set("region_count", 3)

brain = pynlm.create_brain(config)
brain.initialize()

# Add regions
region_ids = []
for i in range(3):
    region_id = brain.add_region(f"Region_{i}")
    region_ids.append(region_id)
    print(f"Created region {region_id}")

# 2. Create a simulation with advanced learning
brain, world, agent = pynlm.create_advanced_simulation()

# Enable all learning subsystems
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)
agent.enable_curiosity(True)

# 3. Run a complex benchmark
import time
start_time = time.time()

brain, world, agent = pynlm.create_simple_simulation()
steps = 1000

for i in range(steps):
    world.update(0.1)
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    brain.step(i)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())

end_time = time.time()

# Get detailed benchmark
benchmark = pynlm.benchmark_simulation(brain, world, agent, steps)
benchmark["real_time"] = end_time - start_time

print(f"Benchmark completed in {benchmark['elapsed_time_ms']:.0f}ms")
print(f"Real time: {benchmark['real_time']:.2f}s")
print(f"Rate: {benchmark['steps_per_second']:.0f} steps/sec")

# 4. Memory analysis and optimization
print("\nMemory Analysis:")
memory_info = pynlm.get_memory_usage(brain)
for key, value in memory_info.items():
    print(f"  {key}: {value}")

# 5. Data export for analysis
print("\nExporting simulation data...")
pynlm.export_simulation_data(brain, world, "final_simulation_data.txt")
print("Data exported successfully!")

# 6. Checkpoint management
checkpoint_file = "advanced_simulation_checkpoint.bin"
print(f"\nSaving checkpoint to {checkpoint_file}...")
pynlm.save_brain_state(brain, checkpoint_file)
print("Checkpoint saved!")

# 7. Resume from checkpoint
print("\nLoading checkpoint...")
loaded_brain = pynlm.load_brain_state(checkpoint_file, "configs/default.cfg")
if loaded_brain:
    print("Checkpoint loaded successfully!")
else:
    print("Failed to load checkpoint")
```

## Best Practices

### 18. Best Practices for NLM Python Development

```python
import pynlm

# Best practices documentation

best_practices = [
    "Always initialize brains before use",
    "Use context managers for resource management",
    "Monitor memory usage in long-running simulations",
    "Use checkpointing for long simulations",
    "Enable only necessary subsystems to improve performance",
    "Use batch processing for multiple simulations",
    "Validate configuration parameters before use",
    "Handle errors gracefully",
    "Use efficient data structures for large datasets",
    "Consider parallel processing for independent simulations",
    "Monitor simulation progress regularly",
    "Save intermediate results periodically",
    "Use appropriate time steps for your simulation",
    "Consider GPU acceleration if available",
    "Document your simulation parameters and results"
]

print("Best Practices for NLM Python Development:")
print("=" * 50)
for i, practice in enumerate(best_practices, 1):
    print(f"{i}. {practice}")
```

## API Reference

### 19. Key Classes and Functions

**Classes:**
- `Brain`: Main neural simulation brain class
- `Config`: Configuration management class
- `SimpleWorld`: 2D simulation world
- `AgentBrain`: Agent interface connecting brain to world
- `BrainWrapper`, `AgentWrapper`, `WorldWrapper`: Context manager wrappers
- `Action`: Motor action representation
- `SensoryPercept`: Sensory input data
- `WorldObject`: World object representation

**Key Functions:**
- `create_default_config()`: Create default configuration
- `create_brain(config)`: Create brain with configuration
- `create_world()`: Create simulation world
- `create_agent(brain)`: Create agent brain interface
- `create_simple_simulation()`: Create complete simulation
- `run_episode(brain, world, agent, steps)`: Run simulation episode
- `brain_wrapper(brain)`: Create brain wrapper with context manager
- `agent_wrapper(agent, world)`: Create agent wrapper with context manager
- `world_wrapper(world)`: Create world wrapper with context manager

**Enumerations:**
- `NeuronType`: Neuron type enumeration
- `ActionType`: Action type enumeration
- `WorldObjectType`: World object type enumeration
- `DevelopmentalStage`: Developmental stage enumeration
- `SynapseType`: Synapse type enumeration

## Conclusion

This comprehensive guide covers the main features and usage patterns of the NLM Python library. The library provides a powerful and flexible interface for creating and running neural simulations, with support for advanced features like learning, development, memory management, and checkpointing.

For more detailed information and examples, refer to the HOW_TO_USE.md and easy_usage.md files in the repository.
