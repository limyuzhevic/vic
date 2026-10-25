# NLM (Neural Learning Machine) - Comprehensive User Guide

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. This guide provides comprehensive documentation for users of all skill levels, from complete beginners to advanced researchers.

## Table of Contents

1. [Quick Start for Beginners](#quick-start-for-beginners)
2. [Intermediate Usage](#intermediate-usage)
3. [Advanced Features and Customization](#advanced-features-and-customization)
4. [API Reference](#api-reference)
5. [Configuration Management](#configuration-management)
6. [Simulation Examples](#simulation-examples)
7. [Troubleshooting](#troubleshooting)
8. [Performance Optimization](#performance-optimization)
9. [Research Applications](#research-applications)
10. [Installation and Setup](#installation-and-setup)

## Quick Start for Beginners

### What is NLM?

NLM simulates a brain with neurons that can learn, remember experiences, and make decisions. Think of it as creating a virtual brain that develops over time through interaction with an environment.

### Basic Concepts

- **Brain**: A collection of virtual neurons that process information
- **World**: The environment where the brain operates
- **Agent**: The connection between brain and world that handles sensory input and motor output

### Quick Start Example

```python
import pynlm

# Create a simple brain
brain = pynlm.create_brain(1000)  # 1000 neurons
brain.initialize()

# Run a simulation
spikes_generated = brain.run_steps(1000)

print(f"Done! Your brain thought {brain.get_total_spike_count()} times")
print(f"Generated {spikes_generated} spikes during simulation")
```

### Step-by-Step Tutorial

1. **Import the library**
   ```python
   import pynlm
   ```

2. **Create your brain**
   ```python
   # Create with default configuration
   brain = pynlm.create_brain()
   
   # Or with custom neuron count
   brain = pynlm.create_brain(neuron_count=2000)
   ```

3. **Initialize the brain**
   ```python
   brain.initialize()
   ```

4. **Run simulation**
   ```python
   # Run a simple simulation
   brain.run_steps(1000)
   
   # Or with custom timestep
   brain.run_steps(500, timestep=0.005)
   ```

5. **Check results**
   ```python
   print(f"Total neurons: {brain.get_neuron_count()}")
   print(f"Total spikes: {brain.get_total_spike_count()}")
   print(f"Average firing rate: {brain.get_firing_rate():.2f} Hz")
   print(f"Development stage: {brain.get_development_stage()}")
   ```

### Creating a Complete Agent System

```python
import pynlm

# Create world with environment
world = pynlm.create_world(width=20.0, height=20.0)
world.configure(vision_width=8, vision_height=8)

# Create brain
brain = pynlm.create_brain(neuron_count=1000)
brain.initialize()

# Create agent to connect brain and world
agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Configure learning
agent.configure_learning(reward_modulation=True, 
                       curiosity=True, 
                       development=True)

# Run complete simulation
steps = pynlm.run_agent_simulation(world, agent, 2000)

print(f"Simulation completed: {steps} steps")
print(f"Brain stats: {brain.get_stats()}")
print(f"Agent stats: {agent.get_stats()}")
```

## Intermediate Usage

### Working with Advanced Brain Features

#### Development Stages

NLM brains go through developmental stages. You can monitor and control them:

```python
brain = pynlm.create_brain()
brain.initialize()

print(f"Initial stage: {brain.get_development_stage()}")

# Simulate development
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.get_development_stage()}")

# Manually set developmental stage
brain.set_development_stage(pynlm.DevelopmentalStage.Maturation)
```

#### State Management

Save and load brain states for persistent simulations:

```python
# Save brain state
brain.save_state("my_brain.pkl")

# Load brain state
brain.load_state("my_brain.pkl")

# Create snapshots for experiment control
snapshot = brain.create_snapshot()
brain.restore_from_snapshot(snapshot)
```

### Advanced Simulation Controls

#### Batch Processing

Run multiple simulations with different configurations:

```python
# Generate different configurations
configs = pynlm.generate_experimental_configs(num_configs=5, base_neurons=1000)

# Run batch simulation
results = pynlm.run_batch_simulation(configs, steps_per_sim=1000)

# Analyze results
analysis = pynlm.analyze_simulation_performance(results)
print(f"Average performance: {analysis['avg_firing_rate']:.2f} Hz")
print(f"Consistency score: {analysis['performance_consistency']:.3f}")
```

#### Parallel Processing

Speed up simulations using multiple threads:

```python
# Parallel simulation
parallel_results = pynlm.run_parallel_simulation(
    configs, 
    steps_per_sim=1000, 
    num_threads=4
)

print(f"Completed {len(parallel_results)} parallel simulations")
```

#### Learning Experiment Controller

Run structured learning experiments:

```python
# Setup learning experiment
brain, world, agent = pynlm.create_research_environment(neuron_count=5000)

# Run exploration phase
exploration_results = pynlm.run_learning_experiment(
    brain, agent, world, 
    exploration_steps=1000, 
    exploitation_steps=2000
)

print(f"Learning efficiency: {exploration_results['learning_efficiency']:.3f}")
print(f"Spike improvement: {exploration_results['spike_improvement_percent']:.1f}%")
```

### Advanced Configuration

#### Configuration Presets

```python
# Get different configuration presets
minimal_config = pynlm.create_minimal_config()
research_config = pynlm.create_research_config()
performance_config = pynlm.create_high_performance_config()

# Use configuration
brain = pynlm.create_brain(research_config)
brain.initialize()
```

#### Configuration Validation

Validate your configurations:

```python
config = pynlm.create_brain_config(2000)

# Add validation rules
config.add_validation_rule(pynlm.ValidationRule("neuron_count", "int", "Number of neurons", 1, 10000))
config.add_validation_rule(pynlm.ValidationRule("simulation_timestep", "double", "Timestep", 0.0001, 0.01))

# Check if configuration is valid
if config.validate():
    print("Configuration is valid!")
else {
    print("Validation errors:", config.get_validation_errors())
}
```

## Advanced Features and Customization

### Context Managers and Resource Management

Python bindings provide context managers for automatic resource cleanup:

```python
# The enhanced brain class provides context management
with pynlm.EnhancedBrain(brain) as brain_helper:
    brain_helper.initialize()
    results = brain_helper.run_steps(1000)
    # Brain automatically cleaned up when exiting context

# World simulation context
with pynlm.EnhancedWorld(world) as world_helper:
    world_helper.reset()
    world_helper.update(0.1)
    # World automatically reset when exiting context
```

### Error Handling and Recovery

Advanced error handling for robust simulations:

```python
import pynlm

# Safe execution with error handling
def safe_simulation():
    try:
        brain = pynlm.create_brain(1000)
        brain.initialize()
        return brain.run_steps(1000)
    except Exception as e:
        print(f"Simulation failed: {e}")
        return 0

# Execute with retry logic
pynlm.SimulationController.execute_with_retry(
    safe_simulation,
    max_retries=3,
    delay_ms=100.0
)
```

### Performance Monitoring

Monitor simulation performance in real-time:

```python
# Create simulation monitor
monitor = pynlm.SimulationMonitor()

# Record checkpoints
monitor.record_checkpoint("brain_initialization")
brain.initialize()

monitor.record_checkpoint("simulation_start")
brain.run_steps(1000)

monitor.record_checkpoint("simulation_complete")

# Get performance report
report = monitor.generate_report()
print(report)

# Benchmark performance
benchmark = pynlm.SimulationMonitor.benchmark_simulation(
    lambda: brain.run_steps(1000),
    num_runs=10,
    benchmark_name="brain_simulation"
)

print(f"Average simulation time: {benchmark['avg_time_seconds']:.3f}s")
print(f"Performance score: {benchmark['performance_score']:.3f}")
```

### Visualization and Data Export

Export simulation data for analysis:

```python
# Generate visualization data
viz_data = pynlm.SimulationController.generate_visualization_data(
    brain.get_stats(),
    agent.get_stats(),
    world.get_stats()
)

# Save to file
with open("simulation_data.json", "w") as f:
    f.write(viz_data)

# Create analysis plots (requires matplotlib)
import matplotlib.pyplot as plt

# Extract metrics for plotting
spike_counts = [result["total_spikes"] for result in parallel_results]
times = list(range(len(spike_counts)))

plt.figure(figsize=(12, 5))
plt.subplot(1, 2, 1)
plt.plot(times, spike_counts)
plt.title("Spike Count Over Time")
plt.xlabel("Simulation Step")
plt.ylabel("Total Spikes")

plt.subplot(1, 2, 2)
firing_rates = [result["avg_firing_rate"] for result in parallel_results]
plt.plot(times, firing_rates)
plt.title("Firing Rate Over Time")
plt.xlabel("Simulation Step")
plt.ylabel("Firing Rate (Hz)")

plt.tight_layout()
plt.savefig("simulation_analysis.png")
plt.show()
```

### Research Applications

#### Lifelong Learning Experiment

```python
import pynlm

# Create research environment
brain, world, agent = pynlm.create_research_environment(
    neuron_count=10000,
    world_width=30.0
)

# Configure for lifelong learning
agent.configure_learning(
    reward_modulation=True,
    curiosity=True,
    development=True,
    structural_plasticity=True
)

# Run extended simulation (conceptual - would be many more steps)
monitor = pynlm.SimulationMonitor()

for episode in range(10):  # 10 learning episodes
    monitor.record_checkpoint(f"episode_{episode}_start")
    
    # Run single episode
    steps = pynlm.run_agent_simulation(world, agent, 500)
    
    monitor.record_checkpoint(f"episode_{episode}_end")
    
    # Record metrics
    brain_stats = brain.get_stats()
    agent_stats = agent.get_stats()
    
    monitor.record_metric(f"episode_{episode}_spikes", brain_stats["total_spikes"])
    monitor.record_metric(f"episode_{episode}_firing_rate", brain_stats["avg_firing_rate"])
    monitor.record_metric(f"episode_{episode}_curiosity", agent_stats["curiosity_level"])

# Generate comprehensive report
report = monitor.generate_report()
print(report)

# Analyze learning progression
exploration_results = pynlm.run_learning_experiment(
    brain, agent, world,
    exploration_steps=1000,
    exploitation_steps=3000
)

print("Learning Experiment Results:")
print(f"Spike improvement: {exploration_results['spike_improvement_percent']:.1f}%")
print(f"Learning efficiency: {exploration_results['learning_efficiency']:.3f}")
```

#### Social Learning Simulation

```python
# Social learning with multiple agents
agents = []
world = pynlm.create_large_world(50.0, 50.0)

# Create multiple agents with different characteristics
for i in range(5):
    brain = pynlm.EnhancedBrain.create_default_brain(2000)
    agent = pynlm.EnhancedAgentBrain.create_for_brain(brain.get_raw_brain())
    agent.configure_learning(True, True, True, True)
    agents.append((brain, agent))

# Simulate social interaction
for step in range(1000):
    world.update(0.1)
    
    for brain, agent in agents:
        # Each agent observes world state
        percept = world.get_percept()
        agent.process_percept(percept)
        
        # Each agent decides action
        action = agent.decode_action()
        world.apply_action(action, world.get_simulation_time())
        
        # Agents can learn from each other (simplified)
        if step % 10 == 0:  # Imitation learning opportunity
            # Agent i observes agent j's action
            observing_agent = agents[step % len(agents)][1]
            imitating_agent = agents[(step + 1) % len(agents)][1]
            
            # Simple imitation: use observing agent's action
            pass

print("Social learning simulation completed")
```

## API Reference

### Core Classes

#### EnhancedBrain
A Pythonic interface to the NLM brain with enhanced functionality.

**Methods:**
- `initialize()` - Initialize the brain
- `step(step, time=None)` - Perform simulation step
- `run_steps(num_steps, timestep=0.001)` - Run multiple steps
- `get_stats()` - Get brain statistics
- `save_state(filepath)` - Save brain state
- `load_state(filepath)` - Load brain state
- `get_neuron_count()` - Get total neuron count
- `get_total_spike_count()` - Get total spike count
- `get_firing_rate()` - Get average firing rate
- `get_development_stage()` - Get developmental stage
- `set_development_stage(stage)` - Set developmental stage

#### EnhancedWorld
Represents the simulation environment with enhanced features.

**Methods:**
- `configure(width, height, vision_width, vision_height)` - Configure world dimensions
- `reset()` - Reset world state
- `update(timestep)` - Update world state
- `apply_action(action, time)` - Apply action to world
- `get_percept()` - Get current sensory percept
- `get_agent_body()` - Get agent body state
- `get_width()`, `get_height()` - Get world dimensions

#### EnhancedAgentBrain
Connects brain and world with enhanced cognitive features.

**Methods:**
- `initialize(world)` - Initialize with world
- `process_percept(percept)` - Process sensory input
- `decode_action()` - Decode motor command
- `apply_reward(reward, predicted_reward)` - Apply reward modulation
- `update_development(timestep)` - Update development system
- `configure_learning()` - Configure learning subsystems
- `get_stats()` - Get agent statistics

### Factory Functions

#### Basic Creation
- `pynlm.create_brain(neuron_count=1000)` - Create brain with default config
- `pynlm.create_world(width, height, vision_width, vision_height)` - Create world
- `pynlm.create_agent_brain(brain)` - Create agent brain interface

#### Enhanced Creation
- `pynlm.EnhancedBrain.create_default_brain(neuron_count=1000)` - Create enhanced brain
- `pynlm.EnhancedWorld.create_standard_world(width=20.0, height=20.0)` - Create standard world
- `pynlm.EnhancedAgentBrain.create_for_brain(brain)` - Create enhanced agent

#### Simulation Helpers
- `pynlm.run_agent_simulation(world, agent, max_steps, timestep)` - Run agent simulation
- `pynlm.run_batch_simulation(configs, steps_per_sim)` - Run batch simulation
- `pynlm.run_parallel_simulation(configs, steps_per_sim, num_threads)` - Run parallel simulation
- `pynlm.run_learning_experiment(brain, agent, world, exploration_steps, exploitation_steps)` - Run learning experiment

### Configuration Classes

#### ValidationRule
Defines validation rules for configuration values.

**Parameters:**
- `key` - Configuration key
- `type` - Value type ("int", "double", "string", "bool", "array")
- `description` - Description of the parameter
- `min_value` - Minimum allowed value (for numeric types)
- `max_value` - Maximum allowed value (for numeric types)
- `required` - Whether the key is required

### Enumerations

#### DevelopmentalStage
- `Initial` - Initial developmental stage
- `CriticalPeriod` - Critical period for learning
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

#### NeuronType
- `Excitatory` - Excitatory neurons
- `Inhibitory` - Inhibitory neurons
- `Modulatory` - Modulatory neurons
- `Sensory` - Sensory neurons
- `Motor` - Motor neurons
- `Internal` - Internal neurons

#### ActionType
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `Look` - Look
- `Interact` - Interact with environment
- `Eat`, `Drink`, `Rest`, `Wait` - Other actions

## Configuration Management

### Configuration File Formats

#### JSON Configuration
```json
{
    "neuron_count": 1000,
    "region_count": 1,
    "simulation_timestep": 0.001,
    "random_seed": 42,
    "connection_probability": 0.1,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "development_enabled": true,
    "reward_modulation_enabled": true
}
```

#### Command Line Arguments
```bash
./nlm_simulation --neuron_count=2000 --simulation_timestep=0.005 --random_seed=123
```

### Configuration Presets

#### createDefaultConfig()
Creates a standard configuration for general use.

#### createMinimalConfig()
Creates a minimal configuration for quick testing.

#### createResearchConfig()
Creates a configuration optimized for research experiments.

#### createHighPerformanceConfig()
Creates a configuration optimized for performance.

### Configuration Validation

Validate configuration before use:

```python
config = pynlm.create_brain_config(1000)

# Add validation rules
config.add_validation_rule(pynlm.ValidationRule("neuron_count", "int", "Number of neurons", 1, 100000))
config.add_validation_rule(pynlm.ValidationRule("simulation_timestep", "double", "Timestep", 0.0001, 0.01))

# Validate
if config.validate():
    print("Configuration is valid")
else:
    print("Validation errors:", config.get_validation_errors())
```

## Simulation Examples

### Example 1: Simple Brain Simulation
```python
import pynlm

# Create and run simple brain
brain = pynlm.create_brain(500)
brain.initialize()

for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.get_firing_rate():.2f} Hz")

print(f"Final firing rate: {brain.get_firing_rate():.2f} Hz")
```

### Example 2: Complete Agent System
```python
import pynlm

# Setup environment
world = pynlm.create_world(30.0, 30.0)
world.configure(16, 16)

# Setup brain
brain = pynlm.create_brain(2000)
brain.initialize()

# Setup agent
agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Run simulation
pynlm.run_agent_simulation(world, agent, 2000)

# Print results
print(f"Simulation completed in {agent.get_stats()['total_spikes']} spikes")
```

### Example 3: Learning Experiment
```python
import pynlm

# Create research environment
brain, world, agent = pynlm.create_research_environment(neuron_count=5000)

# Run learning experiment
results = pynlm.run_learning_experiment(
    brain, agent, world,
    exploration_steps=1000,
    exploitation_steps=3000
)

print("Learning Experiment Results:")
print(f"Spike improvement: {results['spike_improvement_percent']:.1f}%")
print(f"Learning efficiency: {results['learning_efficiency']:.3f}")
```

### Example 4: Batch Processing
```python
import pynlm

# Generate different experimental configurations
configs = pynlm.generate_experimental_configs(num_configs=10)

# Run batch simulation
results = pynlm.run_batch_simulation(configs, steps_per_sim=500)

# Analyze results
analysis = pynlm.analyze_simulation_performance(results)
print(f"Average firing rate: {analysis['avg_firing_rate']:.2f} Hz")
print(f"Performance consistency: {analysis['performance_consistency']:.3f}")
```

### Example 5: Performance Benchmarking
```python
import pynlm
import time

# Benchmark brain simulation
brain = pynlm.create_brain(1000)
benchmark = pynlm.SimulationMonitor.benchmark_simulation(
    lambda: brain.run_steps(1000),
    num_runs=20,
    benchmark_name="brain_simulation"
)

print(f"Benchmark Results:")
print(f"Average time: {benchmark['avg_time_seconds']:.3f}s")
print(f"Performance score: {benchmark['performance_score']:.3f}")
print(f"Standard deviation: {benchmark['std_time_seconds']:.3f}s")
```

## Troubleshooting

### Common Issues

#### "Brain not initialized"
**Cause:** Brain.initialize() was not called.
**Solution:** Call brain.initialize() before running simulation steps.

```python
brain = pynlm.create_brain()
# Missing: brain.initialize()
brain.run_steps(100)  # This will fail
```

**Correct usage:**
```python
brain = pynlm.create_brain()
brain.initialize()
brain.run_steps(100)  # This works
```

#### "Zero neurons in brain"
**Cause:** Invalid neuron count or configuration.
**Solution:** Check configuration and use valid neuron count.

```python
# Invalid configuration
brain = pynlm.create_brain(neuron_count=0)  # This will fail
```

**Correct usage:**
```python
# Valid configuration
brain = pynlm.create_brain(neuron_count=100)  # This works
```

#### "Simulation not progressing"
**Cause:** Brain is stuck or not receiving input.
**Solution:** Check sensory input connections and injection current.

#### Memory issues
**Cause:** Too many neurons or insufficient system memory.
**Solution:** Reduce neuron count or use high-performance configuration.

### Error Handling Examples

#### Try-Catch Block
```python
import pynlm

try:
    brain = pynlm.create_brain(1000)
    brain.initialize()
    results = brain.run_steps(1000)
    print(f"Success: {results} spikes")
except Exception as e:
    print(f"Error: {e}")
```

#### Safe Execution
```python
import pynlm

# Safe execution with error handling
def safe_simulation():
    try:
        brain = pynlm.create_brain(1000)
        brain.initialize()
        return brain.run_steps(1000)
    except Exception as e:
        print(f"Simulation failed: {e}")
        return 0

# Execute with retry logic
pynlm.SimulationController.execute_with_retry(
    safe_simulation,
    max_retries=3,
    delay_ms=100.0
)
```

### Debugging Tips

#### Print Brain State
```python
brain = pynlm.create_brain(100)
brain.initialize()

print("=== Brain State ===")
print(f"Neurons: {brain.get_neuron_count()}")
print(f"Synapses: {brain.get_total_synapse_count()}")
print(f"Active neurons: {brain.get_active_neuron_count()}")
print(f"Firing neurons: {brain.get_firing_neuron_count()}")
print(f"Total spikes: {brain.get_total_spike_count()}")
print(f"Development stage: {brain.get_development_stage()}")
```

#### Monitor Simulation Progress
```python
import pynlm
import time

brain = pynlm.create_brain(500)
brain.initialize()

start_time = time.time()
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        elapsed = time.time() - start_time
        print(f"Step {step}: {brain.get_firing_rate():.2f} Hz, "
              f"Elapsed: {elapsed:.2f}s")
```

## Performance Optimization

### Tips for Faster Simulations

1. **Use appropriate neuron count:** Match neuron count to experiment requirements
2. **Optimize timestep:** Use larger timesteps for less detailed but faster simulations
3. **Disable unnecessary features:** Disable development, curiosity, or structural plasticity if not needed
4. **Use high-performance configuration:** `pynlm.create_high_performance_config()`

### Configuration for Performance

```python
# High-performance configuration
brain = pynlm.create_brain(config=pynlm.create_high_performance_config())
brain.initialize()

# Performance tips
brain.set_enabled(True)  # Ensure simulation is enabled
brain.run_steps(1000, timestep=0.01)  # Use larger timestep
```

### Memory Management

```python
import pynlm

# Create brain with memory-efficient configuration
brain = pynlm.create_brain(config=pynlm.create_minimal_config())
brain.initialize()

# Monitor memory usage
stats = brain.get_stats()
print(f"Estimated memory usage: {stats['memory_usage_mb']:.2f} MB")

# Clean up resources
brain.reset()
del brain  # Explicit cleanup
```

### Parallel Processing for Large-Scale Simulations

```python
import pynlm

# Generate many configurations
configs = pynlm.generate_experimental_configs(num_configs=50)

# Run parallel simulation
start_time = time.time()
results = pynlm.run_parallel_simulation(
    configs, 
    steps_per_sim=200, 
    num_threads=8
)
end_time = time.time()

print(f"Completed {len(results)} simulations in {end_time - start_time:.2f}s")
print(f"Average time per simulation: {(end_time - start_time) / len(results):.3f}s")
```

## Research Applications

### 1. Cognitive Development Studies

Study how neural systems develop and change over time:

```python
import pynlm

# Create brain for development study
brain = pynlm.EnhancedBrain.create_default_brain(1000)
brain.initialize()

# Track development over time
development_stages = []
firing_rates = []

for step in range(5000):  # Simulate 5 seconds at 0.001 timestep
    brain.step(step)
    
    if step % 100 == 0:
        development_stages.append(brain.get_development_stage())
        firing_rates.append(brain.get_firing_rate())

# Analyze development
print(f"Development stages observed: {len(set(development_stages))}")
print(f"Firing rate range: {min(firing_rates):.3f} - {max(firing_rates):.3f} Hz")
```

### 2. Learning and Memory Research

Investigate learning mechanisms and memory formation:

```python
import pynlm

# Create brain for learning study
brain, world, agent = pynlm.create_research_environment(
    neuron_count=3000,
    world_width=25.0
)

# Run learning experiment
learning_results = pynlm.run_learning_experiment(
    brain, agent, world,
    exploration_steps=2000,
    exploitation_steps=6000
)

# Analyze learning curves
print("=== Learning Analysis ===")
print(f"Total exploration time: {learning_results['exploration_spikes']} spikes")
print(f"Total exploitation time: {learning_results['exploitation_spikes']} spikes")
print(f"Learning efficiency: {learning_results['learning_efficiency']:.3f}")
print(f"Spike improvement: {learning_results['spike_improvement_percent']:.1f}%")
```

### 3. Neuromodulation Studies

Study the role of neuromodulators in learning and behavior:

```python
import pynlm

# Create brain with neuromodulation
brain, world, agent = pynlm.create_research_environment(
    neuron_count=2000,
    world_width=20.0
)

# Track neuromodulation levels
reward_levels = []
curiosity_levels = []
novelty_levels = []

for step in range(3000):
    world.update(0.1)
    
    percept = world.get_percept()
    agent.process_percept(percept)
    
    brain.step(step, world.get_simulation_time())
    
    action = agent.decode_action()
    world.apply_action(action, world.get_simulation_time())
    
    reward = percept.get_internal()[0] if percept.get_internal() else 0.0
    agent.apply_reward_modulation(reward, 0.0f)
    
    if step % 100 == 0:
        stats = agent.get_stats()
        reward_levels.append(stats["neuromodulation_level"])
        curiosity_levels.append(stats["curiosity_level"])
        novelty_levels.append(stats["novelty_level"])

# Analyze neuromodulation patterns
print("=== Neuromodulation Analysis ===")
print(f"Average reward level: {sum(reward_levels)/len(reward_levels):.3f}")
print(f"Average curiosity level: {sum(curiosity_levels)/len(curiosity_levels):.3f}")
print(f"Average novelty level: {sum(novelty_levels)/len(novelty_levels):.3f}")
```

### 4. Social Learning Research

Study social learning and imitation behavior:

```python
import pynlm

# Create multiple agents for social learning
agents = []
world = pynlm.create_large_world(50.0, 50.0)

for i in range(3):
    brain = pynlm.EnhancedBrain.create_default_brain(1000)
    agent = pynlm.EnhancedAgentBrain.create_for_brain(brain.get_raw_brain())
    agent.configure_learning(True, True, True, True)
    agents.append((brain, agent))

# Simulate social interaction
for step in range(1000):
    world.update(0.1)
    
    for i, (brain, agent) in enumerate(agents):
        # Each agent observes world and other agents
        percept = world.get_percept()
        agent.process_percept(percept)
        
        # Social learning: agents can observe and imitate others
        if step % 20 == 0 and i > 0:
            # Imitation learning opportunity
            observing_agent = agents[i][1]
            model_agent = agents[i-1][1]
            
            # Simple imitation: use model agent's action patterns
            # In a real implementation, this would involve more sophisticated mechanisms
            pass
        
        # Each agent acts
        action = agent.decode_action()
        world.apply_action(action, world.get_simulation_time())

print("Social learning simulation completed")
```

### 5. Decision Making Studies

Study decision-making under uncertainty and risk:

```python
import pynlm
import random

# Create decision-making environment
brain, world, agent = pynlm.create_research_environment(
    neuron_count=2500,
    world_width=30.0
)

# Track decision-making statistics
decision_accuracy = []
reaction_times = []
curiosity_driven = []

for step in range(2000):
    # Introduce uncertainty in environment
    uncertainty = random.random() > 0.7  # 30% chance of uncertainty
    
    world.update(0.1)
    
    percept = world.get_percept()
    agent.process_percept(percept)
    
    brain.step(step, world.get_simulation_time())
    
    action = agent.decode_action()
    world.apply_action(action, world.get_simulation_time())
    
    reward = percept.get_internal()[0] if percept.get_internal() else 0.0
    agent.apply_reward_modulation(reward, 0.0f)
    
    # Record metrics
    if step % 100 == 0:
        stats = agent.get_stats()
        curiosity_driven.append(stats["curiosity_level"] > 0.5)
        decision_accuracy.append(reward > 0.1)  # Simple accuracy metric

print("=== Decision Making Analysis ===")
print(f"Decision accuracy: {sum(decision_accuracy)/len(decision_accuracy)*100:.1f}%")
print(f"Curiosity-driven decisions: {sum(curiosity_driven)/len(curiosity_driven)*100:.1f}%")
```

## Installation and Setup

### Quick Installation

#### Prerequisites
- Python 3.8 or higher
- C++ compiler with C++20 support
- CMake 3.16 or higher

#### Installation Steps

1. **Clone the repository**
   ```bash
   git clone https://github.com/nlm-project/nlm.git
   cd nlm
   ```

2. **Build the C++ library**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

3. **Install Python bindings**
   ```bash
   pip install -e .
   ```

4. **Verify installation**
   ```python
   import pynlm
   print("NLM installed successfully!")
   brain = pynlm.create_brain(100)
   brain.initialize()
   print("Brain created and initialized successfully")
   ```

### Development Installation

For development with debugging features:

```bash
cd /path/to/nlm
mkdir debug_build
cd debug_build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-g -O0"
make -j4
cd ..
pip install -e .[dev]
```

### Alternative Installation Methods

#### Using Conda
```bash
conda create -n nlm-env python=3.9
conda activate nlm-env
pip install scikit-build-core pybind11
# Build and install as above
```

#### Using Docker
```bash
# Dockerfile available in repository
# Use: docker build -t nlm .
# Run: docker run -it nlm
```

### Environment Setup

#### Environment Variables
```bash
# Set NLM environment variables
export NLM_LOG_LEVEL=INFO
export NLM_SIMULATION_TIMESTEP=0.001
export NLM_DEFAULT_NEURON_COUNT=1000
```

#### Configuration Files
Create `~/.nlm/config.json` for default configuration:
```json
{
    "neuron_count": 1000,
    "region_count": 1,
    "simulation_timestep": 0.001,
    "random_seed": 42
}
```

### Testing and Validation

#### Run Tests
```bash
# Run Python tests
pytest tests/

# Run C++ tests
./build/nlm_test
```

#### Example Test Scripts
```bash
# Simple brain test
python3 examples/simple_brain.py

# Agent simulation test
python3 examples/agent_simulation.py

# Learning experiment test
python3 examples/learning_experiment.py
```

### Troubleshooting Installation

#### Build Issues
```bash
# If CMake fails, ensure you have:
- C++20 compiler support
- Python development headers
- Required system dependencies

# For Ubuntu/Debian:
sudo apt-get update
sudo apt-get install build-essential cmake python3-dev
```

#### Python Import Issues
```python
# Check installation
python3 -c "import pynlm; print(pynlm.__version__)"

# If this fails, check:
# 1. Python path
# 2. pip installation
# 3. Library dependencies
```

#### Performance Issues
```python
# For better performance:
# 1. Use larger timestep: brain.run_steps(1000, timestep=0.01)
# 2. Disable unnecessary features: agent.configure_learning(False, False, False, False)
# 3. Use high-performance config: brain = pynlm.create_brain(config=pynlm.create_high_performance_config())
```

## Additional Resources

### Documentation
- **Online Documentation**: https://nlm.readthedocs.io
- **Architecture Guide**: docs/ARCHITECTURE.md
- **Science Documentation**: docs/SCIENCE.md
- **Phase 6 Audit**: docs/PHASE6_FINAL_AUDIT.md

### Examples
- **Basic Examples**: examples/
- **Research Examples**: examples/research/
- **Tutorial Examples**: examples/tutorials/

### Support
- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Discussion Forum**: https://github.com/nlm-project/nlm/discussions
- **Documentation**: https://nlm.readthedocs.io

### Citation

If you use NLM in your research, please cite:

```bibtex
@misc{nlm2026,
  title={NLM (熙然): Neural Learning Machine - An Experimental Artificial Developmental Brain},
  author={NLM Authors},
  year={2026},
  url={https://github.com/nlm-project/nlm}
}
```

### Acknowledgments

NLM is developed by the NLM Research Team and contributors. Special thanks to:
- All contributors to the NLM project
- The open-source community for inspiration and tools
- Research collaborators and funding agencies

### License

NLM is licensed under the MIT License. See LICENSE file for details.

---

*Last updated: 2026-10-25*
*Version: 1.0.0*

This guide is continuously updated with new features and examples. For the latest documentation, visit the official NLM documentation website.