# NLM Advanced Python API

This module provides advanced utilities, examples, and best practices for using
the NLM (Neural Learning Machine) Python bindings.

## Installation

Install the NLM Python package:

```bash
pip install pynlm
```

Or install in development mode:

```bash
pip install -e .
```

## Quick Start

### Basic Usage

```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent Example

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run episode
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

## Advanced Features

### Configuration Presets

The NLM Python API provides convenient preset configurations:

```python
from advanced_nlm import AdvancedBrainUtils

# Create brain with different preset configurations
brain_minimal = AdvancedBrainUtils.create_brain_with_preset_config("minimal")
brain_balanced = AdvancedBrainUtils.create_brain_with_preset_config("balanced")
brain_learning = AdvancedBrainUtils.create_brain_with_preset_config("learning")
brain_fast = AdvancedBrainUtils.create_brain_with_preset_config("fast")
brain_high_performance = AdvancedBrainUtils.create_brain_with_preset_config("high_performance")
```

### World Creation

Create complex world environments:

```python
from advanced_nlm import AdvancedWorldUtils

# Create world with obstacles and resources
world = AdvancedWorldUtils.create_rich_world(width=30, height=30)

# Add custom objects and terrain
# ... (see world creation documentation)
```

### Brain Benchmarking

Benchmark brain performance:

```python
from advanced_nlm import AdvancedBrainUtils

# Create and benchmark brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run benchmark
results = AdvancedBrainUtils.benchmark_brain_performance(brain, num_steps=5000, num_runs=3)
print(f"Best performance: {results['best_run']['steps_per_second']:.0f} steps/second")
```

### Serialization

Save and load brain states:

```python
from advanced_nlm import AdvancedSerializationUtils

# Export brain to JSON
AdvancedSerializationUtils.export_brain_to_json(brain, "brain_state.json")

# Import brain from JSON
brain_data = AdvancedSerializationUtils.import_brain_from_json("brain_state.json")
```

### Agent Training

Train agents in complex environments:

```python
from advanced_nlm import AdvancedTrainingUtils

# Train agent
stats = AdvancedTrainingUtils.train_agent_world(world, brain, 
                                                num_episodes=100, 
                                                steps_per_episode=500)

# Get learning curve
learning_curve = AdvancedTrainingUtils.create_learning_curve_data(stats)
```

## Advanced Utilities

### Configuration Validation

Validate configuration settings:

```python
from advanced_nlm import pynlm

config = pynlm.createDefaultConfig()
config.setNeuronCount(3000)
config.setConnectionProbability(0.15)

# Validate configuration
is_valid, errors = pynlm.validateConfig(config)
if not is_valid:
    print(f"Configuration errors: {errors}")
```

### Brain Analysis

Analyze brain connectivity and performance:

```python
from advanced_nlm import AdvancedBrainUtils

# Analyze brain
analysis = AdvancedBrainUtils.analyze_brain_connectivity(brain)
print(f"Connection density: {analysis['connection_density']:.4f}")
print(f"Firing rate: {analysis['average_firing_rate']:.2f}")
print(f"E/I ratio: {analysis['excitation_inhibition_ratio']:.2f}")
```

### World Analysis

Analyze world state and agent position:

```python
from advanced_nlm import AdvancedWorldUtils

# Analyze world state
world_analysis = AdvancedWorldUtils.analyze_world_state(world)
print(f"Agent position: ({world_analysis['agent_position']['x']}, {world_analysis['agent_position']['y']})")
print(f"Agent energy: {world_analysis['agent_energy']}")
print(f"Agent health: {world_analysis['agent_health']}")
```

## Performance Optimization

### Batch Operations

Process multiple brains efficiently:

```python
from advanced_nlm import pynlm

# Create multiple brains
brains = [pynlm.createBrain(pynlm.createDefaultConfig()) for _ in range(10)]
for brain in brains:
    brain.initialize()

# Batch step all brains
steps = [1000, 2000, 1500, 2500, 1000, 3000, 2000, 1000, 500, 2000]
spike_counts = pynlm.batchStepBrains(brains, steps)

print("Batch processing completed successfully")
```

### System Information

Get system information:

```python
from advanced_nlm import pynlm

system_info = pynlm.getSystemInfo()
print(f"Version: {system_info['version']}")
print(f"C++ Standard: {system_info['cpp_standard']}")
print(f"Build Type: {system_info['build_type']}")
```

## Error Handling

### Common Errors and Solutions

**1. Import Errors**

```python
# Error: No module named 'pynlm'
# Solution: Install the package
pip install pynlm

# Error: Module not built
# Solution: Build from source
pip install -e .
```

**2. Configuration Errors**

```python
# Error: Configuration parameters out of range
# Solution: Use createDefaultConfig() for safe defaults
config = pynlm.createDefaultConfig()
config.setNeuronCount(10000)  # This will fail validation

# Solution: Validate configuration first
is_valid, errors = pynlm.validateConfig(config)
if is_valid:
    # Use configuration
    pass
else:
    print(f"Configuration errors: {errors}")
```

**3. Simulation Errors**

```python
# Error: Brain not initialized before stepping
# Solution: Always call initialize() before step()
brain.initialize()
brain.step(0)  # Now this works
```

## Troubleshooting

### Performance Issues

- **Symptom**: Slow brain simulation
  **Solution**: Reduce neuron count, disable learning features, use smaller configs

- **Symptom**: High memory usage
  **Solution**: Monitor neuron/synapse counts, consider checkpointing, garbage collection

- **Symptom**: Simulation instability
  **Solution**: Check neuron thresholds, ensure proper initialization, validate config

### API Usage Issues

- **Symptom**: Type conversion errors
  **Solution**: Use flexible data types, validate inputs, use proper conversions

- **Symptom**: Agent-world interaction problems
  **Solution**: Call agent.initialize(world) before use, reset world properly

### Learning Issues

- **Symptom**: No learning observed
  **Solution**: Verify plasticity is enabled, check neuromodulation levels

- **Symptom**: Poor convergence
  **Solution**: Increase episodes, adjust learning rates, enable multiple features

- **Symptom**: Overfitting
  **Solution**: Use smaller networks, add regularization, implement early stopping

## Advanced Examples

See the examples directory for advanced usage patterns and real-world applications.

```bash
cd examples/
ls -la
```

## References

- [NLM Documentation](https://nlm.readthedocs.io)
- [GitHub Repository](https://github.com/nlm-project/nlm)
- [Phase 6 Integration Demo](https://github.com/nlm-project/nlm/blob/main/src/experiments/Phase6Demo.cpp)

## License

This package is licensed under the MIT License.

For more detailed information and advanced usage, refer to the complete documentation in the NLM project repository.