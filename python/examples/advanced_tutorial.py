# Example: Advanced Tutorial

This module provides advanced tutorials for working with the NLM Python Advanced API.

Based on the basic usage examples, this tutorial covers more advanced topics
and real-world applications.

## Installation

Make sure you have the NLM Python Advanced API installed:

```bash
pip install pynlm
```

## Prerequisites

This tutorial assumes you have:
- Basic Python knowledge
- Understanding of neural networks
- Some experience with NLM concepts

## Tutorial Overview

1. **Brain Configuration and Presets**
   - Understanding different configuration presets
   - Custom brain creation
   - Performance considerations

2. **World Simulation**
   - Creating complex environments
   - Object management
   - Physics simulation

3. **Agent Training**
   - Reinforcement learning concepts
   - Training loops
   - Performance optimization

4. **Advanced Analysis**
   - Brain connectivity analysis
   - Performance benchmarking
   - Statistical analysis

## Section 1: Brain Configuration and Presets

### 1.1 Understanding Presets

The NLM Python API provides several predefined configuration presets:

```python
from advanced_nlm import AdvancedBrainUtils

# Available presets
presets = ["minimal", "balanced", "learning", "fast", "high_performance"]

# Create brain with specific preset
brain = AdvancedBrainUtils.create_brain_with_preset_config("learning")
```

### 1.2 Preset Characteristics

| Preset | Neurons | Connection Probability | Learning Features |
|--------|---------|------------------------|-------------------|
| Minimal | 500 | 0.05 | Basic |
| Balanced | 2000 | 0.10 | All basic features |
| Learning | 3000 | 0.15 | All learning features |
| Fast | 1000 | 0.08 | Optimized for speed |
| High Performance | 5000 | 0.20 | All features optimized |

### 1.3 Custom Configuration

Create a custom brain with specific parameters:

```python
from advanced_nlm import AdvancedBrainUtils

# Create brain with custom parameters
brain = AdvancedBrainUtils.create_brain_with_preset_config("custom")

# Then modify configuration
brain.getConfig().setNeuronCount(10000)
brain.getConfig().setConnectionProbability(0.2)
brain.getConfig().set("stdp_ltp_weight", 0.03)
```

## Section 2: World Simulation

### 2.1 Creating Complex Worlds

Create worlds with various objects and terrain features:

```python
from advanced_nlm import AdvancedWorldUtils

# Create rich world with terrain and objects
world = AdvancedWorldUtils.create_rich_world(
    width=30, height=30,
    vision_width=8, vision_height=8
)
```

### 2.2 Object Management

Add and manage objects in the world:

```python
# Add resources
world.createObject(pynlm.WorldObject(
    x=10, y=10,
    type=pynlm.WorldObjectType.Resource,
    value=2.0,
    radius=0.5
))

# Add walls
for x in range(30):
    world.createObject(pynlm.WorldObject(
        x=x, y=0,
        type=pynlm.WorldObjectType.Wall,
        value=1.0,
        radius=0.2
    ))

# Add hazards
world.createObject(pynlm.WorldObject(
    x=25, y=25,
    type=pynlm.WorldObjectType.Hazard,
    value=0.8,
    radius=0.5
))
```

### 2.3 World Physics

World simulation includes:
- Agent movement and physics
- Object collision detection
- Energy and health systems
- Velocity and acceleration

## Section 3: Agent Training

### 3.1 Training Framework

The training framework provides:
- Episode-based training
- Reward systems
- Learning curve tracking
- Performance optimization

```python
from advanced_nlm import AdvancedTrainingUtils

# Train agent
stats = AdvancedTrainingUtils.train_agent_world(
    world=world,
    brain=brain,
    num_episodes=100,
    steps_per_episode=200
)
```

### 3.2 Training Statistics

The training statistics include:
- Episode rewards
- Running averages
- Convergence detection
- Performance curves

```python
# Learning curve analysis
learning_curve = AdvancedTrainingUtils.create_learning_curve_data(stats)

print(f"Best reward: {learning_curve['best_reward']:.2f}")
print(f"Final reward: {learning_curve['final_reward']:.2f}")
print(f"Reward improvement: {learning_curve['final_reward'] - learning_curve['rewards'][0]:.2f}")
print(f"Converged: {learning_curve['convergence_episode'] != -1}")
```

## Section 4: Advanced Analysis

### 4.1 Brain Analysis

Analyze brain connectivity and performance:

```python
from advanced_nlm import AdvancedBrainUtils

# Analyze brain
analysis = AdvancedBrainUtils.analyze_brain_connectivity(brain)

print(f"Connection density: {analysis['connection_density']:.4f}")
print(f"Firing rate: {analysis['average_firing_rate']:.2f}")
print(f"E/I ratio: {analysis['excitation_inhibition_ratio']:.2f}")
print(f"Active neurons: {analysis['active_neurons_percentage']:.1f}%")
```

### 4.2 Performance Benchmarking

Benchmark brain performance:

```python
from advanced_nlm import AdvancedBrainUtils

# Benchmark with different configurations
configs = [
    {"neurons": 500, "conn": 0.05, "name": "Small"},
    {"neurons": 2000, "conn": 0.10, "name": "Medium"},
    {"neurons": 5000, "conn": 0.15, "name": "Large"},
]

results = {}
for config in configs:
    brain = AdvancedBrainUtils.create_brain_with_preset_config("custom")
    brain.getConfig().setNeuronCount(config['neurons'])
    brain.getConfig().setConnectionProbability(config['conn'])
    
    # Run benchmark
    results[config['name']] = AdvancedBrainUtils.benchmark_brain_performance(
        brain, num_steps=1000, num_runs=3
    )
```

## Section 5: Serialization

### 5.1 Exporting and Importing

Save and load brain states:

```python
from advanced_nlm import AdvancedSerializationUtils

# Export brain to JSON
AdvancedSerializationUtils.export_brain_to_json(brain, "my_brain.json")

# Import brain from JSON
brain_data = AdvancedSerializationUtils.import_brain_from_json("my_brain.json")
```

### 5.2 State Management

Manage brain states for:
- Experiment reproducibility
- Model persistence
- Transfer learning
- Backup and recovery

## Section 6: Real-World Application

### 6.1 Navigation Task

Create a navigation task where the agent:
- Explores the world
- Collects resources
- Avoids hazards
- Reaches goals

```python
# Create navigation task
world = AdvancedWorldUtils.create_rich_world(20, 20)

# Set goal
world.createObject(pynlm.WorldObject(
    x=18, y=18,
    type=pynlm.WorldObjectType.Marker,
    value=1.0,
    radius=0.3
))

# Set starting position
world.setAgentStart(2, 2)

# Create learning brain
brain = AdvancedBrainUtils.create_brain_with_preset_config("learning")
```

### 6.2 Training and Evaluation

Train and evaluate the navigation agent:

```python
from advanced_nlm import AdvancedTrainingUtils

# Train agent
stats = AdvancedTrainingUtils.train_agent_world(
    world=world,
    brain=brain,
    num_episodes=50,
    steps_per_episode=300
)

# Evaluate performance
print(f"Navigation success rate: {stats['success_rate']:.2%}")
print(f"Average path length: {stats['avg_path_length']:.1f}")
```

## Section 7: Advanced Features

### 7.1 Configuration Validation

Validate configuration settings:

```python
from advanced_nlm import pynlm

config = pynlm.createDefaultConfig()
config.setNeuronCount(10000)
config.setConnectionProbability(0.2)

# Validate configuration
is_valid, errors = pynlm.validateConfig(config)
if is_valid:
    print("Configuration is valid")
else:
    print(f"Configuration errors: {errors}")
```

### 7.2 Batch Processing

Process multiple brains efficiently:

```python
from advanced_nlm import pynlm

# Create multiple brains
brains = [pynlm.createBrain(pynlm.createDefaultConfig()) for _ in range(10)]

# Batch step all brains
steps = [1000, 2000, 1500, 2500, 1000, 3000, 2000, 1000, 500, 2000]
spike_counts = pynlm.batchStepBrains(brains, steps)
```

### 7.3 Performance Monitoring

Monitor system performance:

```python
from advanced_nlm import pynlm

system_info = pynlm.getSystemInfo()
print(f"Version: {system_info['version']}")
print(f"C++ Standard: {system_info['cpp_standard']}")
print(f"Build Type: {system_info['build_type']}")
```

## Section 8: Troubleshooting

### 8.1 Common Issues

| Issue | Solution |
|-------|----------|
| Slow simulation | Reduce neuron count, disable learning features |
| Memory issues | Monitor counts, garbage collection |
| Instability | Check neuron parameters, validate config |
| Import errors | Install pynlm, check build logs |

### 8.2 Error Handling

Handle errors gracefully:

```python
from advanced_nlm import safe_execute, NLMError

try:
    result = safe_execute(
        lambda: createBrainWithConfig(10000, 0.15)
    )
except NLMError as e:
    print(f"NLM Error: {e}")
```

## Section 9: Best Practices

### 9.1 Configuration Management

- Use predefined presets when possible
- Validate configuration before use
- Monitor performance with different configurations
- Keep documentation of configurations

### 9.2 Memory Management

- Monitor neuron and synapse counts
- Use garbage collection appropriately
- Implement checkpointing for long simulations
- Consider memory limits for production use

### 9.3 Performance Optimization

- Choose appropriate brain size for the task
- Disable unnecessary features
- Use batch operations when possible
- Monitor and profile performance regularly

### 9.4 Code Organization

- Use the provided utilities for common operations
- Document your configurations and results
- Use version control for brain states
- Implement proper error handling

## Section 10: Next Steps

### 10.1 Further Learning

- Read the complete NLM documentation
- Explore the C++ API for advanced features
- Contribute to the NLM project
- Join the NLM community

### 10.2 Advanced Features

- Implement custom plasticity rules
- Create complex world environments
- Develop advanced learning algorithms
- Implement real-time simulation

### 10.3 Research Applications

- Study neural learning dynamics
- Investigate brain connectivity patterns
- Analyze learning curves and convergence
- Explore transfer learning

## Conclusion

The NLM Python Advanced API provides a comprehensive set of tools for working with
neural simulations. This tutorial covers the essentials for getting started with
the API, including configuration, simulation, analysis, and advanced features.

For more detailed information, refer to the complete documentation and examples
in the NLM project repository.

---

*Last updated: September 30, 2026*
*NLM Project Team*