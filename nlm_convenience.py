# NLM Convenience Module

This module provides advanced convenience functions for NLM Python users, making it easier to create complex simulations and work with the brain system.

## Installation

The convenience module is part of the pynlm package. After installing NLM:

```bash
pip install -e .
```

Import the convenience module:

```python
from nlm_convenience import NLMBuilder, SimulationRunner, BrainProfiler, NLMTools
```

## NLMBuilder Class

The `NLMBuilder` class provides a fluent API for creating complete NLM simulation systems with all desired features.

### Basic Usage

```python
from nlm_convenience import NLMBuilder

# Create a complete NLM system
builder = (NLMBuilder()
    .set_brain_size(2000, 2)
    .set_plasticity(True, True, True, True)
    .set_neuromodulation(True, True, True, True)
    .set_developmental(True)
    .build())

brain = builder.brain
agent = builder.agent
world = builder.world
```

### Advanced Configuration

```python
from nlm_convenience import NLMBuilder

# Create system with all features enabled
brain_system = (NLMBuilder()
    .set_brain_size(5000, 3)
    .set_plasticity(True, hebbian=True, stdp=True, structural=True)
    .set_neuromodulation(dopamine=True, curiosity=True, novelty=True)
    .set_developmental(True)
    .set_world(width=30, height=30, vision_width=12, vision_height=12)
    .build())

# Enable all advanced features
builder.enable_all_features()
```

### Available Methods

- `set_brain_size(neuron_count, region_count)` - Set brain size and region count
- `set_plasticity(enable, hebbian, stdp, structural)` - Enable plasticity mechanisms
- `set_neuromodulation(dopamine, curiosity, novelty, reward)` - Enable neuromodulation systems
- `set_developmental(enable)` - Enable developmental system
- `set_world(width, height, vision_width, vision_height)` - Configure world environment
- `build()` - Build and return the complete NLM system
- `enable_all_features()` - Enable all advanced features

## SimulationRunner Class

The `SimulationRunner` class provides comprehensive simulation tracking and metrics collection.

### Basic Usage

```python
from nlm_convenience import SimulationRunner, NLMBuilder

# Create simulation system
builder = NLMBuilder().set_brain_size(1000).build()
brain = builder.brain
agent = builder.agent
world = builder.world

# Create simulation runner
runner = SimulationRunner()

# Run simulation with full tracking
metrics = runner.run_episode(
    brain, world, agent,
    max_steps=2000,
    enable_learning=True
)

print(f"Total spikes: {metrics['total_spikes']}")
print(f"Average firing rate: {metrics['average_firing_rate']:.2f} Hz")
print(f"Total reward: {metrics['total_reward']:.2f}")
```

### Metrics Available

```python
metrics = {
    'total_spikes': 12345,              # Total spikes during simulation
    'initial_firing': 5,                # Initial firing neuron count
    'final_firing': 125,                # Final firing neuron count
    'average_firing': 62.5,             # Average firing rate across all steps
    'total_reward': 456.7,              # Total reward accumulated
    'steps_completed': 1000,            # Number of steps run
    'average_reward': 0.4567,           # Average reward per step (if learning enabled)
}
```

### Step Statistics

```python
step_stats = runner.get_step_stats()
print(f"Max spikes per step: {step_stats['max_spikes_per_step']}")
print(f"Min spikes per step: {step_stats['min_spikes_per_step']}")
print(f"Average spikes per step: {step_stats['avg_spikes_per_step']:.2f}")
print(f"Positive reward steps: {step_stats['positive_reward_steps']}")
```

## BrainProfiler Class

The `BrainProfiler` class provides tools for analyzing brain performance and dynamics.

### Usage Example

```python
from nlm_convenience import BrainProfiler

# Create and initialize a brain
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()

# Profile brain activity over a window
profile = BrainProfiler.analyze_spike_patterns(brain, window_size=100)

print("Brain Profile:")
print(f"  Average spike rate: {profile['average_spike_rate']:.2f}")
print(f"  Average firing rate: {profile['average_firing_rate']:.2f} Hz")
print(f"  Burst count: {profile['burst_count']}")
print(f"  E/I ratio: {profile['excitatory_inhibition_ratio']:.2f}")
print(f"  Total neurons: {profile['total_neurons']}")
print(f"  Total synapses: {profile['total_synapses']}")
```

### Profile Metrics

```python
profile = {
    'average_spike_rate': 12.5,
    'average_firing_rate': 8.3,
    'burst_count': 45,
    'firing_rate_variance': 2.1,
    'excitatory_inhibition_ratio': 1.8,
    'total_neurons': 1000,
    'total_synapses': 25000,
}
```

### Memory Profile

```python
memory_profile = BrainProfiler.profile_memory_usage(brain)
if 'working_memory_traces' in memory_profile:
    print(f"Working memory traces: {memory_profile['working_memory_traces']}")
if 'episodic_memory_episodes' in memory_profile:
    print(f"Episodic memory episodes: {memory_profile['episodic_memory_episodes']}")
```

### Brain Comparison

```python
BrainProfiler.compare_brain_states(brain1, brain2)
comparison = {
    'neuron_count_match': True,
    'synapse_count_match': False,
    'firing_rate_similarity': 0.85,
    'structural_similarity': 0.92,
}
```

## NLMTools Class

The `NLMTools` class provides utility functions for common NLM operations.

### Key Features

#### Brain State Export/Import

```python
# Save brain state
NLMTools.export_brain_state(brain, "checkpoint.bin")

# Load brain state
loaded_brain = NLMTools.import_brain_state("checkpoint.bin")
```

#### Pre-trained Brain Creation

```python
# Create a pre-trained brain
pretrained_brain = NLMTools.create_pretrained_brain("model.bin")

# Create new brain with brief training
brain = NLMTools.create_pretrained_brain()
for i in range(100):
    brain.step(i)
```

#### Configuration Benchmarking

```python
from nlm_convenience import NLMTools

# Benchmark different configurations
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)

metrics = NLMTools.benchmark_configuration(config, steps=1000)
print(f"Configuration: {metrics['configuration']}")
print(f"Steps per second: {metrics['steps_per_second']:.2f}")
print(f"Total spikes: {metrics['total_spikes']}")
print(f"Average firing rate: {metrics['average_firing_rate']:.2f} Hz")
```

#### Configuration Validation

```python
from nlm_convenience import NLMTools

config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("connection_probability", 0.1)

is_valid, errors = NLMTools.validate_configuration(config)
if is_valid:
    print("Configuration is valid")
else:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")
```

## Complete Integration Example

```python
"""
Complete example demonstrating all convenience features.
"""

import pynlm
from nlm_convenience import NLMBuilder, SimulationRunner, BrainProfiler

# 1. Create complete NLM system with all features
print("Creating NLM system...")
system = (NLMBuilder()
    .set_brain_size(2000, 2)
    .set_plasticity(True, True, True, True)
    .set_neuromodulation(True, True, True, True)
    .set_developmental(True)
    .set_world(20, 20, 8, 8)
    .build())

brain = system.brain
agent = system.agent
world = system.world

# 2. Run simulation with full tracking
print("Running simulation...")
runner = SimulationRunner()
metrics = runner.run_episode(
    brain, world, agent,
    max_steps=1000,
    enable_learning=True
)

# 3. Analyze brain activity
print("Profiling brain activity...")
profile = BrainProfiler.analyze_spike_patterns(brain, window_size=100)

# 4. Display results
print("\n" + "="*50)
print("SIMULATION RESULTS")
print("="*50)
print(f"Total spikes: {metrics['total_spikes']}")
print(f"Average firing rate: {metrics['average_firing_rate']:.2f} Hz")
print(f"Total reward: {metrics['total_reward']:.2f}")
print(f"Steps completed: {metrics['steps_completed']}")

if 'average_reward' in metrics:
    print(f"Average reward per step: {metrics['average_reward']:.4f}")

print("\nBRAIN PROFILE")
print("="*50)
print(f"Average spike rate: {profile['average_spike_rate']:.2f}")
print(f"E/I ratio: {profile['excitatory_inhibition_ratio']:.2f}")
print(f"Burst count: {profile['burst_count']}")
print(f"Firing rate variance: {profile['firing_rate_variance']:.2f}")

# 5. Run step statistics analysis
print("\nSTEP STATISTICS")
print("="*50)
step_stats = runner.get_step_stats()
print(f"Max spikes per step: {step_stats['max_spikes_per_step']}")
print(f"Min spikes per step: {step_stats['min_spikes_per_step']}")
print(f"Average spikes per step: {step_stats['avg_spikes_per_step']:.2f}")
print(f"Positive reward steps: {step_stats['positive_reward_steps']}")

# 6. Save brain state
print("\nSaving brain state...")
NLMTools.export_brain_state(brain, "simulation_checkpoint.bin")

print("\n" + "="*50)
print("All convenience features demonstrated successfully!")
print("="*50)
```

## Error Handling

The convenience module includes comprehensive error handling:

### Exception Safety

All convenience classes use try-catch blocks to ensure safe operation:

```python
# Safety in brain building
try:
    system = builder.build()
except Exception as e:
    print(f"Failed to build system: {e}")
    # Recovery or graceful exit

# Safety in simulation
try:
    metrics = runner.run_episode(brain, world, agent)
except Exception as e:
    print(f"Simulation failed: {e}")
    # Continue with alternative approach
```

### Input Validation

The module validates inputs and provides meaningful error messages:

```python
# Invalid brain size will raise informative error
try:
    builder.set_brain_size(-1, 1)
except ValueError as e:
    print(f"Invalid brain size: {e}")
```

## Performance Considerations

### Memory Usage

- `NLMBuilder`: Minimal memory footprint (only stores configuration)
- `SimulationRunner`: Stores step history (may be large for long simulations)
- `BrainProfiler`: Light weight, no persistent storage

### Computational Overhead

- `NLMBuilder`: Negligible (fluent API only)
- `SimulationRunner`: Small overhead for tracking and metrics
- `BrainProfiler`: Moderate overhead for analysis

### Recommended Usage

```python
# For short-term simulations (interactive, testing)
runner = SimulationRunner()

# For long-term simulations (data collection, research)
# Avoid storing all step history if memory is a concern
```

## Migration Guide

### From Basic API to Convenience API

**Basic API:**

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(1000):
    brain.step(step)
```

**Convenience API:**

```python
import pynlm
from nlm_convenience import NLMBuilder

brain_system = (NLMBuilder()
    .set_brain_size(1000)
    .build())

brain = brain_system.brain

for step in range(1000):
    brain.step(step)
```

### Key Differences

1. **Configuration Management**: Builder pattern vs manual config setting
2. **Feature Enable/Disable**: Built-in plasticity, neuromodulation, development
3. **Simulation Tracking**: Automatic metrics collection vs manual tracking
4. **Error Handling**: Comprehensive safety checks vs basic null checks
5. **Code Organization**: Modular design vs procedural code

## License

This convenience module is part of the NLM project and is licensed under the same terms as the main NLM library.

## Dependencies

- pynlm (main NLM Python bindings)
- Python 3.8+

## Future Enhancements

Planned improvements for future versions:

1. **Configuration Templates**: Pre-built configuration profiles
2. **Advanced Analytics**: Statistical analysis and pattern detection
3. **Visualization Tools**: Plotting and visualization functions
4. **Cloud Integration**: Cloud-based simulation execution
5. **Distributed Computing**: Multi-brain simulation capabilities
6. **Machine Learning**: Integration with ML frameworks
7. **Plugin System**: Extensible architecture for custom components

## Support

For issues and questions:
- Check the main NLM documentation
- Submit issues to the NLM GitHub repository
- Join the NLM community forums
- Review examples in this module

---

*This convenience module is designed to make NLM more accessible to users while maintaining full access to the underlying power and flexibility of the system.*
