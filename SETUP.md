# Enhanced NLM Python Bindings - Setup Guide

## Quick Start

This guide provides quick instructions for using the Enhanced NLM Python bindings.

## Prerequisites

1. **Install NLM**: Build and install the NLM C++ library
2. **Install Python Dependencies**: `pip install pybind11 numpy`
3. **Build Python Bindings**: Run CMake to build the enhanced Python module

## Installation

### 1. Build NLM Library

```bash
# Clone and build NLM
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### 2. Install Python Bindings

```bash
# Install with pip (this will build the Python module)
cd /workspace/app
pip install -e .
```

### 3. Alternative: Manual Build

```bash
# If you want to build the Python module manually
cd /workspace/app
mkdir build_python
cd build_python
cmake -DNLM_BUILD_PYTHON_BINDINGS=ON ..
make -j4
```

## Quick Examples

### Example 1: Simple Brain Simulation

```python
import nlm

# Create and initialize brain
brain = nlm.create_brain()
brain.initialize()

# Run simulation
brain.run_simulation(1000)

# Get statistics
stats = brain.get_statistics()
print(f"Simulation completed: {stats['total_spikes']} spikes")
```

### Example 2: Agent in a World

```python
import nlm

# Create brain with learning preset
brain = nlm.create_enhanced_brain(preset="learning")
brain.initialize()

# Create world and agent
world = nlm.create_enhanced_world()
agent = nlm.create_enhanced_agent(brain)

# Initialize agent with world
agent.initialize(world)

# Configure world
world.configure_world(20, 20, 8, 8)
world.set_agent_start(10.0, 10.0)

# Enable learning
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)

# Run interaction loop
for step in range(100):
    world.update_world(0.1)
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    brain.step(step)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())
```

### Example 3: Configuration Builder

```python
import nlm

# Use configuration builder for custom setup
config = (nlm.ConfigBuilder()
          .set_neuron_count(2000)
          .set_connection_probability(0.08)
          .set_simulation_timestep(0.01)
          .set_random_seed(12345)
          .enable_plasticity(True)
          .set_learning_rate(0.005)
          .preset("learning")
          .build())

brain = nlm.create_brain(config)
brain.initialize()
brain.run_simulation(100)
```

## Using Presets

The enhanced API provides several preset configurations for common use cases:

### Available Presets

- **default**: Standard configuration with balanced parameters
- **fast**: Low neuron count, high timestep for quick testing
- **detailed**: High neuron count, low timestep for detailed simulation
- **learning**: Strong plasticity for rapid learning
- **stable**: Weak plasticity for stable behavior

### Using Presets

```python
import nlm

# Create brain with fast preset (quick testing)
brain = nlm.create_enhanced_brain(preset="fast")

# Create brain with learning preset (exploration)
brain = nlm.create_enhanced_brain(preset="learning")

# Create brain with stable preset (stable behavior)
brain = nlm.create_enhanced_brain(preset="stable")

# Create brain with detailed preset (high fidelity)
brain = nlm.create_enhanced_brain(preset="detailed")
```

## Configuration Builder

The configuration builder provides a fluent API for creating custom configurations.

```python
import nlm

# Create custom configuration using builder
config = (nlm.ConfigBuilder()
          .set_neuron_count(3000)                    # 3000 neurons
          .set_connection_probability(0.12)         # Higher connectivity
          .set_simulation_timestep(0.001)           # Standard timestep
          .set_random_seed(42)                       # Reproducible results
          .enable_plasticity(True)                   # Enable plasticity
          .set_learning_rate(0.008)                  # Higher learning rate
          .preset("learning")                        # Apply learning preset
          .build())

# Create brain with custom config
brain = nlm.create_brain(config)
brain.initialize()
```

## Statistics Collection

The enhanced API provides built-in statistics collection for monitoring simulations.

```python
import nlm

brain = nlm.create_brain()
brain.initialize()
brain.run_simulation(1000)

# Get comprehensive statistics
stats = brain.get_statistics()
print("=== Brain Statistics ===")
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total synapses: {stats['total_synapses']}")
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
print(f"E/I ratio: {stats['excitation_inhibition_ratio']:.2f}")
print(f"Developmental stage: {stats['developmental_stage']}")
print(f"Total spikes: {stats['total_spikes']}")

# Agent statistics
agent_stats = agent.get_agent_statistics()
print("\n=== Agent Statistics ===")
print(f"Curiosity level: {agent_stats['curiosity_level']:.2f}")
print(f"Novelty level: {agent_stats['novelty_level']:.2f}")
print(f"Prediction error: {agent_stats['prediction_error']:.2f}")
print(f"Neuromodulation level: {agent_stats['neuromodulation_level']:.2f}")
```

## State Management

The enhanced API provides easy save/load functionality for state management.

```python
import nlm

brain = nlm.create_brain()
brain.initialize()
brain.run_simulation(500)

# Save brain state
brain.save_state("brain_checkpoint.nml")
print("Brain state saved to brain_checkpoint.nml")

# Load brain state
loaded_brain = nlm.create_brain()
loaded_brain.load_state("brain_checkpoint.nml")
print("Brain state loaded successfully")

# Verify loaded brain
loaded_stats = loaded_brain.get_statistics()
print(f"Loaded brain has {loaded_stats['total_neurons']} neurons")
```

## Error Handling

The enhanced API provides comprehensive error handling with informative error messages.

```python
import nlm

try:
    # Create brain with invalid configuration
    brain = nlm.create_brain(neuron_count=-100)  # Invalid
    brain.initialize()
except nlm.InvalidArgumentError as e:
    print(f"Invalid argument: {e}")
except nlm.OutOfRangeError as e:
    print(f"Out of range: {e}")
except nlm.NLMError as e:
    print(f"NLM error: {e}")
```

## Validation

You can validate configurations before using them.

```python
import nlm

# Create configuration and validate it
config = nlm.create_brain()
valid, errors = nlm.validate_config(config)
if not valid:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")
```

## Constants Access

The enhanced API provides access to NLM constants.

```python
import nlm

print("=== NLM Constants ===")
print(f"Default timestep: {nlm.DEFAULT_TIMESTEP}")
print(f"Brain threshold: {nlm.BRAIN_V_THRESH} mV")
print(f"Brain resting potential: {nlm.BRAIN_V_REST} mV")
print(f"Brain reset potential: {nlm.BRAIN_V_RESET} mV")
print(f"Membrane time constant: {nlm.BRAIN_TAU_MEM} ms")
print(f"Refractory period: {nlm.BRAIN_TAU_REF} ms")
```

## Advanced Features

### Configuration from JSON

```python
import nlm
import json

# Configuration in JSON format
json_config = '''
{
    "brain.neuron_count": 1500,
    "brain.connection_probability": 0.1,
    "plasticity.stdp.learning_rate": 0.008,
    "simulation.timestep": 0.001,
    "random.seed": 12345
}
'''

# Create configuration from JSON
config = nlm.create_config_from_json(json_config)
brain = nlm.create_brain(config)
brain.initialize()
```

### Serialization

```python
import nlm

config = nlm.create_brain()

# Serialize configuration
serialized = nlm.serialize_config(config)
print(f"Serialized config: {serialized}")

# Deserialize configuration
deserialized_config = nlm.deserialize_config(serialized)
deserialized_brain = nlm.create_brain(deserialized_config)
```

### Example Scenarios

The enhanced API provides example scenarios for common use cases:

```python
import nlm

# Learning scenario (for exploration tasks)
learning_brain = nlm.create_enhanced_brain(preset="learning")

# Stable scenario (for stable behavior)
stable_brain = nlm.create_enhanced_brain(preset="stable")

# Fast scenario (for quick prototyping)
fast_brain = nlm.create_enhanced_brain(preset="fast")

# Detailed scenario (for high-fidelity simulations)
detailed_brain = nlm.create_enhanced_brain(preset="detailed")
```

## Backward Compatibility

The enhanced API maintains backward compatibility with the original NLM API.

### Original API (Still Available)
```python
import nlm.old_api

# Access old API functions
brain = nlm.old_api.create_brain()
world = nlm.old_api.create_world()
agent = nlm.old_api.create_agent(brain)
```

### Enhanced API (Recommended)
```python
import nlm

# Use enhanced API functions
brain = nlm.create_brain()
world = nlm.create_world()
agent = nlm.create_agent(brain)
```

## Performance Tips

1. **Reuse Configurations**: Create configurations once and reuse them
2. **Use Presets**: Use presets for common scenarios
3. **Batch Operations**: Process multiple steps in batches
4. **Monitor Statistics**: Use statistics to monitor simulation progress
5. **Save Frequently**: Save brain state periodically for long-running simulations

## Troubleshooting

### Common Issues

1. **Import Error**: If you can't import `nlm`, ensure the Python bindings are built correctly.

```bash
cd /workspace/app
pip install -e .
```

2. **Missing Dependencies**: Ensure you have required Python dependencies.

```bash
pip install pybind11 numpy
```

3. **Build Errors**: If there are build errors, check the CMake output.

```bash
cd /workspace/app
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

4. **Runtime Errors**: Check error messages for specific issues.

### Getting Help

- **Documentation**: See `ENHANCED_PYTHON_API.md` for comprehensive documentation
- **Examples**: See `examples/enhanced_api_examples.py` for examples
- **Issues**: Report issues with detailed error messages
- **Community**: Join NLM community for support

## Conclusion

The Enhanced NLM Python API provides a modern, Pythonic interface to the NLM neural simulation framework with:

1. **Simplified API**: Easy-to-use functions and builders
2. **Preset Configurations**: Pre-configured scenarios for common use cases
3. **Enhanced Error Handling**: Automatic validation and informative error messages
4. **Statistics Collection**: Built-in monitoring and reporting
5. **State Management**: Easy save/load functionality
6. **Backward Compatibility**: Legacy API for existing code
7. **Advanced Features**: Configuration builders, JSON support, serialization

The enhanced API makes it easier to get started with NLM while providing powerful tools for advanced users and complex simulations.