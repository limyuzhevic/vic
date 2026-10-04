# NLM Advanced User Guide

This guide provides comprehensive documentation for advanced NLM (熙然) usage, including Python bindings, simulation control, and best practices for working with the neural simulation framework.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Python API Reference](#python-api-reference)
3. [Advanced Simulation Control](#advanced-simulation-control)
4. [Learning and Development](#learning-and-development)
5. [Memory Systems](#memory-systems)
6. [Neuromodulation](#neuromodulation)
7. [Visualization and Export](#visualization-and-export)
8. [Troubleshooting](#troubleshooting)
9. [Performance Optimization](#performance-optimization)

## Quick Start

This guide assumes basic familiarity with the NLM concepts. If you're new, start with `easy_usage.md` first.

### Creating Your First Brain

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.get_total_spike_count()}")
print(f"Firing neurons: {brain.get_firing_neuron_count()}")
```

### Agent in World Simulation

```python
import pynlm

# Create all components
brain = pynlm.create_brain(pynlm.create_default_config())
world = pynlm.create_simple_world()
world.configure(width=15.0, height=15.0, vision_width=10, vision_height=10)
world.reset()

agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Enable advanced features
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_structural_plasticity(True)

# Run simulation with logging
for step in range(500):
    world.update(0.1)
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    brain.step(step)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.get_firing_neuron_count()} neurons firing")
```

## Python API Reference

### Core Classes

#### Config
Configuration management for NLM brain parameters.

**Methods:**
- `create_default_config()` - Create configuration with sensible defaults
- `create_development_config()` - Create configuration optimized for development learning
- `config.set(key, value, source)` - Set configuration value
- `config.get_or(key, default)` - Get configuration value with default
- `config.load_from_file(filename)` - Load from file
- `config.save_to_file(filename)` - Save to file
- `config.get_keys()` - Get all configuration keys
- `config.summary()` - Get configuration summary

**Common Configuration Keys:**
- `random_seed` - Random seed for reproducibility
- `simulation_timestep` - Time step duration
- `neuron_count` - Number of neurons
- `region_count` - Number of brain regions
- `connection_probability` - Probability of connections between neurons
- `replay_interval` - Memory replay interval
- `consolidation_interval` - Memory consolidation interval

#### Brain
The main neural brain simulation class.

**Methods:**
- `brain.initialize()` - Initialize the brain
- `brain.step(step, time=None)` - Run simulation step
- `brain.get_total_neuron_count()` - Get total neuron count
- `brain.get_total_synapse_count()` - Get total synapse count
- `brain.get_firing_neuron_count()` - Get current firing neurons
- `brain.get_total_spike_count()` - Get total spike count
- `brain.get_average_firing_rate()` - Get average firing rate
- `brain.get_excitation_inhibition_ratio()` - Get E/I ratio
- `brain.get_developmental_stage()` - Get developmental stage
- `brain.set_developmental_stage(stage)` - Set developmental stage
- `brain.reset()` - Reset brain state
- `brain.save(filepath)` - Save brain state
- `brain.load(filepath)` - Load brain state
- `brain.log_status()` - Log brain status
- `brain.receive_sensory_input(input)` - Receive sensory input
- `brain.inject_current(neuron_id, current)` - Inject current
- `brain.inject_current_to_neurons(neuron_type, current)` - Inject to neuron type

#### SimpleWorld
The environment simulation world.

**Methods:**
- `world.configure(width, height, vision_width, vision_height)` - Configure world
- `world.reset()` - Reset world
- `world.update(timestep)` - Update world state
- `world.get_sensory_percept()` - Get current sensory percept
- `world.get_agent_body()` - Get agent body state
- `world.apply_motor_command(action, time)` - Apply motor action
- `world.set_random_seed(seed)` - Set random seed
- `world.get_random_seed()` - Get random seed
- `world.is_valid_position(x, y)` - Check valid position
- `world.add_object(object)` - Add object to world
- `world.remove_object(x, y)` - Remove object

#### SensoryPercept
Agent sensory input data.

**Methods:**
- `percept.get_vision()` - Get vision data (grayscale grid)
- `percept.set_vision(data)` - Set vision data
- `percept.get_touch()` - Get touch/proximity data
- `percept.set_touch(data)` - Set touch data
- `percept.get_internal()` - Get internal homeostatic signals
- `percept.set_internal(data)` - Set internal signals
- `percept.get_proprioception()` - Get proprioception data
- `percept.set_proprioception(data)` - Set proprioception
- `percept.get_audio()` - Get audio data
- `percept.set_audio(data)` - Set audio data
- `percept.get_all_signals()` - Get all signals concatenated
- `percept.get_timestamp()` - Get timestamp
- `percept.set_timestamp(time)` - Set timestamp

#### AgentBrain
Bridge between brain and world.

**Methods:**
- `agent.initialize(world)` - Initialize with world
- `agent.get_sensory_input_size()` - Get expected sensory input size
- `agent.get_motor_output_size()` - Get expected motor output size
- `agent.process_sensory_input(percept)` - Process sensory input
- `agent.decode_motor_command()` - Decode motor command
- `agent.apply_reward_modulation(reward, predicted_reward)` - Apply reward modulation
- `agent.update_development(timestep)` - Update development system
- `agent.get_developmental_stage()` - Get developmental stage
- `agent.get_neuromodulation_level()` - Get neuromodulation level
- `agent.get_curiosity_level()` - Get curiosity level
- `agent.get_novelty_level()` - Get novelty level
- `agent.get_prediction_error()` - Get prediction error
- `agent.reset()` - Reset agent for new episode
- `agent.enable_reward_modulation(enable)` - Enable/disable reward modulation
- `agent.enable_structural_plasticity(enable)` - Enable/disable structural plasticity
- `agent.enable_development(enable)` - Enable/disable development
- `agent.enable_curiosity(enable)` - Enable/disable curiosity

### Context Manager

```python
# Context manager for clean simulation control
brain = pynlm.create_brain(pynlm.create_default_config())
world = pynlm.create_simple_world()
agent = pynlm.create_agent_brain(brain)

with pynlm.SimulationContext(brain) as sim:
    sim.run(1000, world, agent)
```

### Event System

```python
# Add callbacks for simulation events
brain = pynlm.create_brain(pynlm.create_default_config())

def on_step(step):
    print(f"Step {step} completed")

pynlm.add_step_callback(brain, on_step)
```

## Advanced Simulation Control

### Speed Control

```python
brain = pynlm.create_brain(pynlm.create_default_config())
pynlm.set_simulation_speed(brain, 10.0)  # Run 10x faster than real-time
```

### Time Management

```python
# Control simulation with custom time steps
brain = pynlm.create_brain(pynlm.create_default_config())

for step in range(1000):
    time = step * 0.001  # 1ms time steps
    brain.step(step, time)
```

### Batch Processing

```python
# Process multiple simulations with different seeds
import numpy as np

results = []
for seed in range(10):
    brain = pynlm.create_brain(pynlm.create_default_config())
    pynlm.create_default_config().set("random_seed", seed, pynlm.ConfigSource.RUNTIME)
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    # Collect results
    results.append({
        'seed': seed,
        'total_spikes': brain.get_total_spike_count(),
        'firing_rate': brain.get_average_firing_rate(),
        'e_i_ratio': brain.get_excitation_inhibition_ratio()
    })
```

## Learning and Development

### Reward-Based Learning

```python
import pynlm

# Create agent with reward learning
brain = pynlm.create_brain(pynlm.create_default_config())
agent = pynlm.create_agent_brain(brain)
agent.enable_reward_modulation(True)  # Enable reward-based learning

# Simulate reward learning
for step in range(500):
    # Get reward from environment
    reward = world.get_current_reward()  # Your environment reward function
    
    # Process sensory input
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    
    # Brain processes and learns
    brain.step(step)
    
    # Apply reward modulation
    predicted_reward = agent.get_prediction_error()
    agent.apply_reward_modulation(reward, predicted_reward)
```

### Development Simulation

```python
import pynlm

# Create development-oriented brain
config = pynlm.create_development_config()
brain = pynlm.create_brain(config)

# Track development stages
stages = ["Initial", "CriticalPeriod", "Maturation", "Adult"]
for stage in stages:
    brain.set_developmental_stage(stage)
    
    # Simulate development at this stage
    for step in range(500):
        brain.step(step)
        
        # Log developmental changes
        if step % 100 == 0:
            print(f"Stage {stage}, Step {step}: "
                  f"Plactivity: {brain.get_developmental_plasticity():.3f}")
```

### Curiosity-Driven Exploration

```python
import pynlm

# Create curious agent
brain = pynlm.create_brain(pynlm.create_default_config())
agent = pynlm.create_agent_brain(brain)
agent.enable_curiosity(True)

# Simulate curiosity-driven exploration
for step in range(1000):
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    
    # High curiosity leads to exploration
    if agent.get_curiosity_level() > 0.5:
        print("Agent exploring new stimuli!")
    
    brain.step(step)
    action = agent.decode_motor_command()
    world.apply_motor_command(action)
```

## Memory Systems

### Working Memory

```python
import pynlm

# Access working memory
brain = pynlm.create_brain(pynlm.create_default_config())
working_memory = brain.get_working_memory()

# Store information
pattern = [0.1, 0.5, 0.8, 0.3, 0.6]
working_memory.store(pattern, strength=0.8)

# Retrieve information
retrieved = working_memory.retrieve()
print(f"Retrieved pattern: {retrieved}")
```

### Episodic Memory

```python
# Access episodic memory
brain = pynlm.create_brain(pynlm.create_default_config())
episodic_memory = brain.get_episodic_memory()

# Store episode
history = {
    'action': 'move_forward',
    'reward': 1.0,
    'novelty': 0.3,
    'timestamp': 100
}
episodic_memory.store_episode(history)

# Replay episodes
for episode in episodic_memory.get_episodes_for_replay(5):
    episodic_memory.replay_episode(episode)
```

### Semantic Memory

```python
# Access semantic memory
brain = pynlm.create_brain(pynlm.create_default_config())
semantic_memory = brain.get_semantic_memory()

# Learn concept
concept = {'type': 'predator', 'threat_level': 0.8, 'distance': 5.0}
semantic_memory.learn_concept(concept, importance=0.9)

# Retrieve similar concepts
related = semantic_memory.find_similar_concepts({'type': 'predator', 'threat_level': 0.5})
print(f"Similar concepts: {related}")
```

## Neuromodulation

### Dopamine System

```python
import pynlm

# Access dopamine system
brain = pynlm.create_brain(pynlm.create_default_config())
dopamine = brain.get_dopamine()

# Process reward prediction error
reward = 1.0
predicted = 0.5
error = reward - predicted
dopamine.signalRewardPredictionError(error)

# Get dopamine level
level = dopamine.get_level()
print(f"Dopamine level: {level:.3f}")

# Get plasticity factor
factor = dopamine.get_plasticity_factor()
print(f"Plasticity factor: {factor:.3f}")
```

### Acetylcholine System

```python
# Access acetylcholine system
brain = pynlm.create_brain(pynlm.create_default_config())
acetylcholine = brain.get_acetylcholine()

# Process attention signals
acetylcholine.signal_attention(0.7, "visual_focus")
level = acetylcholine.get_level()
```

### Novelty Detection

```python
# Access novelty detection
brain = pynlm.create_brain(pynlm.create_default_config())
novelty = brain.get_novelty()

# Detect novel sensory input
sensory_input = [0.1, 0.2, 0.9, 0.3]  # High contrast in middle
novelty_level = novelty.detect_novelty(sensory_input)

# Get current novelty
current_novelty = novelty.get_level()
```

## Visualization and Export

### Basic Visualization

```python
import pynlm

brain = pynlm.create_brain(pynlm.create_default_config())
# Run simulation first
for step in range(100):
    brain.step(step)

# Create visualization
pynlm.plot_spike_activity(brain, "spike_raster.png")
```

### Network Analysis

```python
import pynlm
import matplotlib.pyplot as plt

brain = pynlm.create_brain(pynlm.create_default_config())
# Run simulation
for step in range(200):
    brain.step(step)

# Analyze network dynamics
stats = pynlm.analyze_network_dynamics(brain)

# Plot results
plt.figure(figsize=(12, 8))

plt.subplot(2, 2, 1)
plt.hist([s['total_spikes'] for s in stats], bins=20)
plt.title('Total Spikes Distribution')

plt.subplot(2, 2, 2)
plt.scatter([s['e_i_ratio'] for s in stats], 
           [s['firing_rate'] for s in stats])
plt.title('E/I Ratio vs Firing Rate')

plt.tight_layout()
plt.savefig('network_analysis.png')
```

### Export Capabilities

```python
import pynlm
import json

# Save simulation results
brain = pynlm.create_brain(pynlm.create_default_config())
world = pynlm.create_simple_world()

results = pynlm.run_simulation(brain, world, 500)
pynlm.export_results(results, "simulation_results.json")

# Import from saved configuration
config = pynlm.import_config("saved_config.json")
brain2 = pynlm.create_brain(config)
```

## Troubleshooting

### Common Issues

#### "Brain isn't doing anything"

**Problem**: Brain appears inactive with no spikes or activity.

**Solution**:
```python
import pynlm

brain = pynlm.create_brain(pynlm.create_default_config())

# Always initialize before using
brain.initialize()

# Check if configuration is set properly
print("Neuron count:", brain.get_total_neuron_count())
print("Synapse count:", brain.get_total_synapse_count())

# Inject some current to get activity
brain.inject_current_to_neurons(pynlm.NeuronType.EXCITATORY, 10.0)

# Run simulation
for step in range(100):
    brain.step(step)
    if brain.get_firing_neuron_count() > 0:
        print(f"Activity detected at step {step}")
        break
```

#### "Agent isn't moving"

**Problem**: Agent produces no actions or doesn't move.

**Solution**:
```python
import pynlm

brain = pynlm.create_brain(pynlm.create_default_config())
world = pynlm.create_simple_world()
agent = pynlm.create_agent_brain(brain)

# Enable motor output
agent.enable_structural_plasticity(True)

# Check motor neuron activity
print(f"Motor output size: {agent.get_motor_output_size()}")

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    brain.step(step)
    
    action = agent.decode_motor_command()
    print(f"Step {step}: Action = {action}")
    
    if action != pynlm.ActionType.WAIT:
        world.apply_motor_command(action, world.get_simulation_time())
        break
```

#### "Everything is 0"

**Problem**: All values are zero, no activity detected.

**Solution**:
```python
import pynlm

brain = pynlm.create_brain(pynlm.create_default_config())

# Brains need time to "warm up"
for step in range(1000):
    brain.step(step)
    if brain.get_total_spike_count() > 0:
        print(f"Warm-up completed at step {step}")
        break

# Check connectivity
print(f"Connected neurons: {brain.get_total_neuron_count()}")
print(f"Total synapses: {brain.get_total_synapse_count()}")
```

### Performance Tips

#### Optimize Large Simulations

```python
# Use development config for faster startup
config = pynlm.create_development_config()
config.set("neuron_count", 2000, pynlm.ConfigSource.RUNTIME)
config.set("region_count", 2, pynlm.ConfigSource.RUNTIME)

# Batch processing for multiple runs
import time
start_time = time.time()

for i in range(10):
    brain = pynlm.create_brain(pynlm.create_default_config())
    for step in range(100):
        brain.step(step)
    
print(f"Processed 10 simulations in {time.time() - start_time:.2f} seconds")
```

#### Memory Management

```python
import pynlm
import gc

# Use context managers for automatic cleanup
for iteration in range(5):
    with pynlm.SimulationContext(brain) as sim:
        sim.run(500, world, agent)
    
    # Explicit cleanup
    del brain, world, agent
    gc.collect()
```

## Pattern Templates

### Template 1: Learning Agent

```python
def run_learning_agent(seed=42, steps=1000):
    """Template for running a learning agent"""
    import pynlm
    
    # Configure for learning
    config = pynlm.create_default_config()
    config.set("random_seed", seed, pynlm.ConfigSource.RUNTIME)
    config.set("development_enabled", True, pynlm.ConfigSource.RUNTIME)
    
    # Create agent
    brain = pynlm.create_brain(config)
    world = pynlm.create_simple_world()
    agent = pynlm.create_agent_brain(brain)
    
    # Enable all learning features
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    agent.enable_structural_plasticity(True)
    
    # Run simulation
    for step in range(steps):
        world.update(0.1)
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        brain.step(step)
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'total_spikes': brain.get_total_spike_count(),
        'final_firing_rate': brain.get_average_firing_rate()
    }
```

### Template 2: Developmental Brain

```python
def run_development_simulation(seed=42, stages=4):
    """Template for developmental brain simulation"""
    import pynlm
    
    # Create development-oriented brain
    config = pynlm.create_development_config()
    config.set("random_seed", seed, pynlm.ConfigSource.RUNTIME)
    
    brain = pynlm.create_brain(config)
    
    # Developmental stages
    developmental_stages = [
        pynlm.DevelopmentalStage.Initial,
        pynlm.DevelopmentalStage.CriticalPeriod,
        pynlm.DevelopmentalStage.Maturation,
        pynlm.DevelopmentalStage.Adult
    ]
    
    results = {}
    for i, stage in enumerate(developmental_stages[:stages]):
        brain.set_developmental_stage(stage)
        stage_results = []
        
        for step in range(500):
            brain.step(step)
            stage_results.append({
                'step': step,
                'firing_rate': brain.get_average_firing_rate(),
                'plasticity': brain.get_developmental_plasticity(),
                'spike_count': brain.get_total_spike_count()
            })
        
        results[stage.name] = stage_results
    
    return results
```

### Template 3: Experimental Control

```python
def run_experiments(param_grid, base_config):
    """Template for parameter sweep experiments"""
    import pynlm
    import itertools
    
    results = []
    
    # Generate all parameter combinations
    param_names = list(param_grid.keys())
    param_values = list(param_grid.values())
    
    for params in itertools.product(*param_values):
        param_dict = dict(zip(param_names, params))
        
        # Create experiment configuration
        config = pynlm.create_default_config()
        for key, value in base_config.items():
            config.set(key, value, pynlm.ConfigSource.Default)
        for key, value in param_dict.items():
            config.set(key, value, pynlm.ConfigSource.RUNTIME)
        
        # Run experiment
        brain = pynlm.create_brain(config)
        world = pynlm.create_simple_world()
        agent = pynlm.create_agent_brain(brain)
        
        # Execute simulation
        for step in range(200):
            world.update(0.1)
            percept = world.get_sensory_percept()
            agent.process_sensory_input(percept)
            brain.step(step)
            action = agent.decode_motor_command()
            world.apply_motor_command(action, world.get_simulation_time())
        
        # Record results
        results.append({
            'parameters': param_dict,
            'total_spikes': brain.get_total_spike_count(),
            'final_firing_rate': brain.get_average_firing_rate(),
            'e_i_ratio': brain.get_excitation_inhibition_ratio(),
            'development_stage': brain.get_developmental_stage().name
        })
    
    return results
```

## Best Practices

### 1. Configuration Management

```python
# Always create configurations explicitly
config = pynlm.create_default_config()
config.set("random_seed", 123, pynlm.ConfigSource.RUNTIME)

# Use specific configuration types for different scenarios
development_config = pynlm.create_development_config()
learning_config = pynlm.create_learning_config()
```

### 2. Resource Management

```python
# Use context managers for resource management
with pynlm.SimulationContext(brain) as sim:
    sim.run(1000, world, agent)

# Always clean up resources
if 'brain' in locals():
    del brain
if 'world' in locals():
    del world
if 'agent' in locals():
    del agent
```

### 3. Error Handling

```python
import pynlm
import traceback

def safe_simulation_run(config, steps=100):
    """Safely run simulation with error handling"""
    try:
        brain = pynlm.create_brain(config)
        world = pynlm.create_simple_world()
        agent = pynlm.create_agent_brain(brain)
        
        for step in range(steps):
            world.update(0.1)
            percept = world.get_sensory_percept()
            agent.process_sensory_input(percept)
            brain.step(step)
            action = agent.decode_motor_command()
            world.apply_motor_command(action, world.get_simulation_time())
        
        return {'success': True, 'brain': brain, 'world': world, 'agent': agent}
        
    except Exception as e:
        print(f"Simulation failed: {e}")
        traceback.print_exc()
        return {'success': False, 'error': str(e)}
```

### 4. Reproducibility

```python
import pynlm

# Set seed for reproducibility
config = pynlm.create_default_config()
config.set("random_seed", 42, pynlm.ConfigSource.RUNTIME)

# Run identical simulations multiple times
results = []
for seed in [42, 42, 42]:  # Same seed for reproducibility
    config.set("random_seed", seed, pynlm.ConfigSource.RUNTIME)
    brain = pynlm.create_brain(config)
    
    for step in range(100):
        brain.step(step)
    
    results.append(brain.get_total_spike_count())

print(f"Results: {results}")
print(f"All identical: {len(set(results)) == 1}")
```

## Advanced Topics

### 1. Event-Driven Programming

```python
import pynlm

# Create callback system
brain = pynlm.create_brain(pynlm.create_default_config())

class SimulationObserver:
    def on_simulation_start(self):
        print("Simulation started")
    
    def on_simulation_end(self, final_stats):
        print(f"Simulation ended: {final_stats}")
    
    def on_step_complete(self, step, brain_state):
        if step % 100 == 0:
            print(f"Step {step} completed")

observer = SimulationObserver()

# Integration with NLM event system
# (This would require additional NLM framework integration)
```

### 2. Custom Brain Architectures

```python
# Extend NLM with custom components
import pynlm

class CustomBrain(pynlm.Brain):
    def __init__(self, config):
        super().__init__(config)
        self.custom_components = []
    
    def add_custom_component(self, component):
        self.custom_components.append(component)
    
    def custom_processing(self, data):
        # Implement custom neural processing
        result = data
        for component in self.custom_components:
            result = component.process(result)
        return result
```

### 3. Parallel Processing

```python
import pynlm
import threading
import queue

# Parallel simulation execution
def run_simulation_in_thread(config, results_queue, thread_id):
    """Run simulation in separate thread"""
    brain = pynlm.create_brain(config)
    
    for step in range(100):
        brain.step(step)
    
    results_queue.put({
        'thread_id': thread_id,
        'total_spikes': brain.get_total_spike_count(),
        'firing_rate': brain.get_average_firing_rate()
    })

# Create multiple threads
threads = []
result_queue = queue.Queue()

for i in range(5):
    thread = threading.Thread(
        target=run_simulation_in_thread,
        args=(pynlm.create_default_config(), result_queue, i)
    )
    threads.append(thread)
    thread.start()

# Wait for all threads to complete
for thread in threads:
    thread.join()

# Collect results
while not result_queue.empty():
    result = result_queue.get()
    print(f"Thread {result['thread_id']}: {result}")
```

## API Summary

### Quick Reference

| Component | Class | Key Methods |
|-----------|-------|-------------|
| Configuration | Config | set(), get_or(), load_from_file(), save_to_file() |
| Brain | Brain | initialize(), step(), get_total_neuron_count(), reset() |
| World | SimpleWorld | configure(), update(), get_sensory_percept(), apply_motor_command() |
| Agent | AgentBrain | initialize(), process_sensory_input(), decode_motor_command(), reset() |
| Sensory | SensoryPercept | get_vision(), set_vision(), get_touch(), get_all_signals() |

### Enums

| Enum | Values |
|------|--------|
| NeuronType | Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal |
| DevelopmentalStage | Initial, CriticalPeriod, Maturation, Adult, Aging |
| ConfigSource | Default, File, CommandLine, Runtime |
| ActionType | MoveForward, MoveBackward, TurnLeft, TurnRight, Interact, LookLeft, LookRight, Wait |

### Common Error Codes

| Error Code | Description |
|------------|-------------| 
| INIT_FAILED | Brain initialization failed |
| STEP_FAILED | Simulation step failed |
| CONFIG_ERROR | Configuration error |
| WORLD_ERROR | World simulation error |
| AGENT_ERROR | Agent processing error |

## Migration Guide

### From Phase 2 to Phase 6

**Changes Needed:**
1. Update configuration keys for new Phase 6 systems
2. Add neuromodulation support for advanced learning
3. Implement memory systems for experience storage
4. Add development systems for lifespan learning
5. Enable prediction and cognition systems

**Example Migration:**

```python
# OLD Phase 2 code
import pynlm

brain = pynlm.createBrain(pynlm.createConfig())
brain.initialize()

for step in range(100):
    brain.step(step)

# NEW Phase 6 code
import pynlm

# Use enhanced configuration
config = pynlm.create_default_config()
config.set("random_seed", 42, pynlm.ConfigSource.Runtime);
config.set("development_enabled", true, pynlm.ConfigSource.Runtime);
config.set("neuromodulation_enabled", true, pynlm.ConfigSource.Runtime);

brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)

# Access advanced features
print(f"Development stage: {brain.get_developmental_stage()}")
print(f"Neuromodulation level: {brain.get_neuromodulation_level()}")
print(f"Memory capacity: {brain.get_working_memory().get_capacity()}")
```

### From C++ to Python

**Key Differences:**
1. **Memory Management**: Python uses garbage collection
2. **Type System**: Python is dynamically typed
3. **Error Handling**: Python exceptions vs C++ error codes
4. **Integration**: Python bindings vs direct C++ API

**Migration Tips:**
- Use Python context managers for resource management
- Leverage Python's rich ecosystem for data analysis
- Utilize Python's event system for callbacks
- Take advantage of Python's interactive debugging

## Conclusion

The NLM Python bindings provide comprehensive access to Phase 6 neural simulation capabilities. This guide covers everything from basic usage to advanced topics, including best practices, troubleshooting, and migration strategies.

The NLM framework offers:
- **Complete Neural Computation**: Real LIF neuron dynamics, STDP, structural plasticity
- **Integrated Brain Systems**: Memory, prediction, cognition, neuromodulation
- **Advanced Learning**: Reward-based, curiosity-driven, developmental learning
- **Flexible API**: Pythonic interface with extensive customization options
- **Production Ready**: Robust error handling, resource management, documentation

Whether you're building simple neural models or complex brain simulations, NLM provides the tools you need. Start with the basic examples and gradually explore the advanced features as you become familiar with the framework.

**Happy simulating!** 🐭🧠