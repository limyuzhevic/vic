# NLM High-Level API Reference Guide

## Overview

The NLM (Neural Learning Machine) High-Level API provides simplified interfaces for common neural simulation use cases. This module abstracts away the complexity of the underlying NLM brain simulation framework while maintaining full access to advanced features when needed.

## Key Benefits

1. **Simplified Usage**: Common tasks require only a few lines of code
2. **Consistent Interface**: Unified API across different use cases
3. **Progressive Complexity**: Start simple, add complexity as needed
4. **Backward Compatible**: Existing code continues to work unchanged
5. **Well-Documented**: Comprehensive examples and documentation

## Quick Start

### Basic Neural Simulation

```python
import nlm

# Create and run a simple neural simulation
result = nlm.create_simple_neural_simulation(
    neuron_count=1000,
    steps=100
)

print(f"Simulation completed with {result.avg_firing_rate:.2f} Hz firing rate")
print(f"Total spikes: {result.total_spikes}")
```

### Agent Training

```python
import nlm

# Train an agent using reinforcement learning
training_result = nlm.create_agent_training_simulation(
    neuron_count=2000,
    episodes=50,
    steps_per_episode=100
)

print(f"Training completed: {training_result.experience_gained} experience points")
print(f"Final performance: {training_result.performance_metrics['final_avg_reward']:.3f}")
```

### Experiment Setup

```python
import nlm

# Run a custom experiment with specific configuration
experiment_config = {
    'neuron_count': 1500,
    'max_steps': 500,
    'enable_development': True,
    'world_size': 15
}

experiment_result = nlm.create_experiment_simulation(
    experiment_config,
    "memory_development_test",
    max_steps=200
)

print(f"Experiment '{experiment_result['experiment_name']}' completed")
print(f"Total reward: {experiment_result['total_reward']:.2f}")
```

## API Reference

### Core Classes

#### NLMAPI
The main API class providing factory methods and convenience functions.

**Methods:**

- `NLMAPI.create_simple_brain(neuron_count)` - Create brain for basic simulation
- `NLMAPI.create_agent_training_brain(neuron_count, enable_learning)` - Create brain optimized for training
- `NLMAPI.create_experiment_setup(experiment_config)` - Create custom experiment setup
- `run_basic_simulation(steps)` - Run neural simulation
- `run_agent_simulation(steps, enable_world)` - Run agent-world simulation
- `train_agent(episodes, steps_per_episode, exploration_rate, learning_callback)` - Train agent
- `run_experiment(experiment_name, max_steps)` - Run pre-configured experiment
- `analyze_simulation_data(data)` - Analyze simulation results
- `create_visualization_data(brain, world)` - Prepare data for visualization

**Static Methods:**

- `NLMAPI.create_default_config()` - Create default configuration
- `NLMAPI.create_brain_config(**kwargs)` - Create custom brain configuration

### Convenience Functions

#### Basic Simulation

```python
# One-line simulation
result = nlm.create_simple_neural_simulation(neuron_count=500, steps=50)
```

#### Agent Training

```python
# High-level training
result = nlm.create_agent_training_simulation(
    neuron_count=1000,
    episodes=20,
    steps_per_episode=50
)
```

#### Experiment Setup

```python
# Custom experiment
result = nlm.create_experiment_simulation(
    experiment_config,
    "test_name",
    max_steps=100
)
```

### Result Types

#### SimulationResult
Container for simulation performance metrics.

**Attributes:**

- `total_reward`: Average reward per step
- `avg_firing_rate`: Average neural firing rate
- `total_spikes`: Total spike count
- `active_neurons`: Number of active neurons
- `episodic_episodes`: Number of episodic memory episodes
- `novelty_level`: Current novelty level
- `curiosity_level`: Current curiosity level
- `dopamine_level`: Current neuromodulation level
- `execution_time`: Total simulation time
- `steps_completed`: Number of steps executed
- `metadata`: Additional metadata dictionary

#### TrainingResult
Container for agent training metrics and learning curve.

**Attributes:**

- `performance_metrics`: Dictionary of performance metrics
- `experience_gained`: Total experience points earned
- `learning_curve`: List of episode metrics
- `final_behavior`: Final behavioral state
- `convergence_score`: Learning convergence score

## Use Case Implementations

### Use Case 1: Basic Neural Simulation

**Goal**: Run a simple neural simulation with minimal setup

**Solution**: Use `create_simple_neural_simulation()`

```python
import nlm

# Run basic simulation
result = nlm.create_simple_neural_simulation(
    neuron_count=800,
    steps=100
)

print(f"Brain simulation results:")
print(f"  - Neurons simulated: {result.steps_completed}")
print(f"  - Average firing rate: {result.avg_firing_rate:.3f} Hz")
print(f"  - Total activity: {result.total_spikes} spikes")
```

**Key Features:**
- No world or agent required
- Simple brain initialization
- Automatic cleanup
- Performance metrics included

### Use Case 2: Agent Training

**Goal**: Train an agent to interact with a world environment

**Solution**: Use `create_agent_training_simulation()`

```python
import nlm

# Configure training
training_result = nlm.create_agent_training_simulation(
    neuron_count=1500,
    episodes=30,
    steps_per_episode=80,
    exploration_rate=0.2
)

print(f"Training completed:")
print(f"  - Episodes trained: {training_result.experience_gained // 80}")
print(f"  - Final performance: {training_result.performance_metrics['final_avg_reward']:.3f}")
print(f"  - Learning rate: {training_result.convergence_score:.3f}")
```

**Key Features:**
- Full agent-world interaction
- Reward-based learning
- Exploration-exploitation balance
- Progress monitoring

### Use Case 3: Experiment Setup

**Goal**: Set up and run custom experiments

**Solution**: Use `create_experiment_simulation()`

```python
import nlm

# Define experiment configuration
experiment_config = {
    'neuron_count': 1200,
    'max_steps': 300,
    'enable_development': True,
    'enable_curiosity': True,
    'world_size': 12
}

# Run experiment
experiment_result = nlm.create_experiment_simulation(
    experiment_config,
    "cognitive_development",
    max_steps=200
)

print(f"Experiment '{experiment_result['experiment_name']}' results:")
print(f"  - Steps completed: {experiment_result['steps_completed']}")
print(f"  - Total reward: {experiment_result['total_reward']:.2f}")
print(f"  - Developmental stage: {experiment_result['final_developmental_stage']}")
print(f"  - Novelty level: {experiment_result['final_novelty']}")
print(f"  - Curiosity level: {experiment_result['final_curiosity']}")
```

**Key Features:**
- Custom configuration
- Environment setup
- Advanced feature control
- Detailed results

### Use Case 4: Result Analysis

**Goal**: Analyze simulation results and extract insights

**Solution**: Use `analyze_simulation_data()`

```python
import nlm

# Create API instance
api = nlm.NLMAPI.create_simple_brain(500)

# Run simulation to generate data
result = api.run_basic_simulation(steps=50)

# Analyze for visualization
viz_data = api.create_visualization_data(api._brain)

print(f"Neural network analysis:")
print(f"  - Total neurons: {viz_data['neural_activity']['total_neurons']}")
print(f"  - Active neurons: {viz_data['neural_activity']['active_neurons']}")
print(f"  - Firing rate: {viz_data['neural_activity']['firing_rate']:.3f} Hz")
print(f"  - Excitatory/inhibitory ratio: {viz_data['neural_activity']['excitatory_inhibitory_ratio']:.3f}")
print(f"  - Memory episodes: {viz_data['memory']['episodic_memory_episodes']}")
```

**Key Features:**
- Statistical analysis
- Trend detection
- Pattern recognition
- Ready for visualization

### Use Case 5: Visualization

**Goal**: Prepare data for visualization and plotting

**Solution**: Use `create_visualization_data()`

```python
import nlm
import matplotlib.pyplot as plt

# Setup experiment
api = nlm.NLMAPI.create_agent_training_brain(800)

# Run simulation
sim_result = api.run_agent_simulation(steps=100, enable_world=True)

# Create visualization data
viz_data = api.create_visualization_data(api._brain, api._world)

# Example visualization
plt.figure(figsize=(12, 8))

# Plot neural activity
plt.subplot(2, 2, 1)
plt.hist(viz_data['neural_activity']['firing_rate'], bins=20, alpha=0.7)
plt.title('Neural Firing Rate Distribution')
plt.xlabel('Firing Rate (Hz)')
plt.ylabel('Frequency')

# Plot memory metrics
plt.subplot(2, 2, 2)
memory_metrics = [
    viz_data['memory']['working_memory_active_traces'],
    viz_data['memory']['episodic_memory_episodes']
]
plt.bar(['Working Memory', 'Episodic Memory'], memory_metrics, alpha=0.7)
plt.title('Memory System Activity')
plt.ylabel('Active Traces/Episodes')

# Plot neuromodulation
plt.subplot(2, 2, 3)
neuromod_data = {
    'Dopamine': viz_data['neuromodulation']['dopamine_level'],
    'Curiosity': viz_data['neuromodulation']['curiosity_level'],
    'Novelty': viz_data['neuromodulation']['novelty_level']
}
plt.bar(neuromod_data.keys(), neuromod_data.values(), alpha=0.7)
plt.title('Neuromodulation Levels')
plt.ylabel('Level (0-1)')

# Plot development
plt.subplot(2, 2, 4)
dev_stages = [0, 1, 2, 3, 4]
stage_names = ['Initial', 'Critical', 'Maturation', 'Adult', 'Aging']
plt.plot(dev_stages, [viz_data['development']['developmental_stage']] * 5, 'o-', linewidth=2)
plt.xticks(dev_stages, stage_names)
plt.title('Developmental Stage')
plt.ylabel('Stage')
plt.ylim(0, 4)

plt.tight_layout()
plt.show()
```

**Key Features:**
- Structured data format
- Multiple visualization types
- Statistical summaries
- Integration with plotting libraries

## Advanced Usage

### Custom API Instance

For advanced use cases requiring more control:

```python
import nlm

# Create custom API instance
api = nlm.NLMAPI.create_agent_training_brain(
    neuron_count=1000,
    enable_learning=True
)

# Custom training with callback

def training_monitor(episode, step, total_reward, current_reward):
    if episode % 10 == 0:
        print(f"Episode {episode}, Step {step}, Reward: {current_reward:.3f}")

training_result = api.train_agent(
    episodes=50,
    steps_per_episode=100,
    exploration_rate=0.1,
    learning_callback=training_monitor
)
```

### Integration with Existing Code

The high-level API maintains full backward compatibility with existing NLM code:

```python
import nlm

# Legacy code continues to work
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Can also use high-level API
legacy_result = nlm.run_legacy_simulation(brain, steps=100)
```

## Performance Considerations

1. **Memory Usage**: Larger brains require more memory
2. **Computation Time**: More neurons/steps = longer execution time
3. **World Complexity**: Simulating environments increases overhead
4. **Training Cost**: Training requires many simulation steps

## Error Handling

The high-level API provides comprehensive error handling:

```python
try:
    result = nlm.create_simple_neural_simulation(neuron_count=100, steps=50)
except Exception as e:
    print(f"Simulation failed: {e}")
```

Common errors:
- `ValueError`: Invalid parameters
- `RuntimeError`: Simulation failed
- `MemoryError`: Insufficient memory for requested size

## Best Practices

1. **Start Simple**: Begin with basic simulations
2. **Progressive Development**: Add features gradually
3. **Monitor Performance**: Track metrics during development
4. **Use Callbacks**: Implement callbacks for long-running operations
5. **Validate Parameters**: Check parameter ranges
6. **Handle Errors**: Implement robust error handling

## Limitations

1. **C++ Library Dependencies**: Requires NLM C++ library to be properly built
2. **Learning Curves**: Advanced features have learning curves
3. **Performance**: Large-scale simulations may require optimization
4. **Platform Compatibility**: Current support for Linux/Windows

## Future Enhancements

1. **Cloud Integration**: Distributed simulation support
2. **GPU Acceleration**: GPU-accelerated neural simulation
3. **Web Interfaces**: Web-based visualization and control
4. **Mobile Support**: Lightweight mobile versions
5. **Plugin Architecture**: Extensible plugin system

## Contributing

Contributions to the high-level API are welcome. Please follow these guidelines:

1. Add comprehensive examples
2. Include unit tests
3. Maintain backward compatibility
4. Follow existing coding standards
5. Document new features

## License

This high-level API is provided under the same license as the NLM project. Please see the LICENSE file for details.
