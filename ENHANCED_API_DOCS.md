# Enhanced NLM Python API Documentation

This document provides comprehensive information about the enhanced NLM Python API, including new features, improvements, and examples.

## Overview

The enhanced NLM Python API provides significant improvements over the original bindings:

- **Better error handling** with custom `NLMError` exceptions
- **Convenience methods** for common operations
- **Enhanced configuration presets** for different use cases
- **Statistical analysis tools** for performance monitoring
- **High-level agent interfaces** for easier simulation control
- **Backward compatibility** with original API

## Installation

### With Enhanced Bindings (Recommended)

```bash
# Install with enhanced bindings
pip install --force-reinstall . --no-build-isolation

# Or for development
pip install -e . --no-build-isolation
```

### With Original Bindings

```bash
# Install with original bindings
pip install --force-reinstall . --no-build-isolation
```

## Quick Start Guide

### Basic Usage with Enhanced API

```python
import pynlm

# Create enhanced configuration
config = pynlm.ConfigWrapper()
config.setNeuronCount(1000)
config.setSynapseDensity(0.1)

# Create brain with enhanced features
brain = pynlm.createEnhancedBrain(config)
brain.initialize()

# Run simulation with progress logging
brain.runSteps(500, logProgress=True)

# Get statistics
stats = brain.getStatistics()
print(f"Average firing rate: {stats['average_firing_rate']}")
print(f"Total spikes: {stats['total_spikes']}")
```

### Advanced Usage with Presets

```python
import pynlm

# Create brain with preset configuration
brain = pynlm.createBrainWithPreset("advanced")
brain.initialize()

# Run advanced simulation
brain.runSteps(2000, logProgress=True)

# Access configuration
config = brain.getBrain().getConfig()
print(f"Neuron count: {config.getNeuronCount()}")
print(f"Plasticity enabled: {config.hasPlasticityEnabled()}")
```

### Complete Agent Example

```python
import pynlm

# Setup configuration and brain
config = pynlm.createConfigWithPreset("beginner")
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with enhanced features
agent = pynlm.createEnhancedAgent(brain)

# Setup world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent
agent.initializeWorld(world)

# Run episode with learning
rewards = agent.runEpisode(world, maxSteps=1000, enableLearning=True)

print(f"Episode completed with {len(rewards)} steps")
print(f"Average reward: {sum(rewards) / len(rewards):.3f}")
```

## Enhanced Classes

### ConfigWrapper

The enhanced configuration class provides convenience methods and better error handling.

#### Creation
```python
# Default configuration
config = pynlm.ConfigWrapper()

# From file with error handling
config = pynlm.ConfigWrapper("config.json")
```

#### Convenience Methods
```python
config.setNeuronCount(1000)           # Set neuron count
config.setSynapseDensity(0.1)          # Set synapse density
config.setConnectionProbability(0.05) # Set connection probability
config.enablePlasticity()              # Enable all plasticity
config.enableNeuromodulation()         # Enable neuromodulation

# Check settings
if config.hasPlasticityEnabled():
    print("Plasticity is enabled")

# Get values
neuron_count = config.getNeuronCount()
synapse_density = config.getSynapseDensity()
```

### BrainWrapper

The enhanced brain class provides convenience methods for simulation and statistical analysis.

#### Initialization
```python
config = pynlm.ConfigWrapper()
brain = pynlm.createEnhancedBrain(config)

# Initialize with all features enabled
brain.initialize(enableRewardModulation=True,
                 enableCuriosity=True,
                 enableDevelopment=True)
```

#### Simulation Methods
```python
# Run multiple steps with optional progress logging
brain.runSteps(100, logProgress=True)

# Get comprehensive statistics
stats = brain.getStatistics()
print(f"Neurons: {stats['total_neurons']}")
print(f"Synapses: {stats['total_synapses']}")
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Average firing rate: {stats['average_firing_rate']}")
print(f"Total spikes: {stats['total_spikes']}")
```

#### Development Methods
```python
# Advance through developmental stages
brain.advanceDevelopmentalStage()
print(f"Current stage: {brain.getBrain().getDevelopmentalStage()}")
```

### AgentWrapper

The enhanced agent class provides high-level interfaces for episode management and learning.

#### Initialization
```python
config = pynlm.createConfigWithPreset("advanced")
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createEnhancedAgent(brain)
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()

agent.initializeWorld(world)
```

#### Episode Management
```python
# Run complete episode with learning
rewards = agent.runEpisode(world, maxSteps=500, enableLearning=True)

print(f"Episode completed with {len(rewards)} steps")
print(f"Total reward: {sum(rewards):.3f}")
print(f"Average reward: {sum(rewards) / len(rewards):.3f}")
```

## Configuration Presets

The enhanced API provides convenience presets for different use cases:

### Beginner
- 500 neurons
- Single region
- Low connection probability (0.05)
- All features enabled
- Ideal for testing and learning

### Advanced
- 5000 neurons
- Three regions
- Medium connection probability (0.1)
- All features enabled
- Good for realistic simulations

### Research
- 20000 neurons
- Five regions
- High connection probability (0.2)
- All features enabled
- For intensive research simulations

#### Usage
```python
# Create brain with preset
brain = pynlm.createBrainWithPreset("advanced")
brain.initialize()

# Access configuration
config = brain.getBrain().getConfig()
print(f"Preset: {config.getNeuronCount()} neurons, {config.getRegionCount()} regions")
```

## Error Handling

The enhanced API provides comprehensive error handling:

### Custom Exceptions
```python
try:
    config = pynlm.ConfigWrapper("nonexistent_config.json")
except pynlm.NLMError as e:
    print(f"Configuration error: {e}")
```

### Graceful Degradation
```python
# Methods that can fail gracefully
if not brain.initialize():
    print("Failed to initialize brain")
    # Fallback to simpler configuration
    config = pynlm.ConfigWrapper()
    config.setNeuronCount(100)
    brain = pynlm.createEnhancedBrain(config)
    brain.initialize()
```

## Statistical Analysis

The enhanced API provides statistical tools for performance monitoring:

### Available Statistics
```python
stats = brain.getStatistics()

# Basic statistics
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total synapses: {stats['total_synapses']}")
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Active neurons: {stats['active_neurons']}")

# Performance metrics
print(f"Average firing rate: {stats['average_firing_rate']}")
print(f"Excitatory/inhibitory ratio: {stats['excitation_inhibition_ratio']}")
print(f"Total spikes: {stats['total_spikes']}")
```

### Statistical Tracking
```python
# Track statistics over time
statistics_log = []
for step in range(1000):
    brain.runSteps(10)
    stats = brain.getStatistics()
    statistics_log.append(stats)
    
    # Log significant changes
    if step % 100 == 0:
        print(f"Step {step}: {stats['total_spikes']} total spikes")
```

## Backward Compatibility

The enhanced API maintains full backward compatibility with the original NLM Python API:

### Original Classes Still Available
```python
# Original classes (for backward compatibility)
from pynlm import Config, Brain, AgentBrain, SimpleWorld, Vision, Action

# Original usage still works
config = Config()
brain = Brain(config)
brain.initialize()
brain.step(0)
```

### New Classes Added
```python
# Enhanced classes (new)
from pynlm import ConfigWrapper, BrainWrapper, AgentWrapper

# Enhanced usage
config = ConfigWrapper()
brain = BrainWrapper(config)
```

### Migration Guide

If you're migrating from the original API:

1. **Config → ConfigWrapper**: Use `ConfigWrapper` for convenience methods
2. **Brain → BrainWrapper**: Use `createEnhancedBrain()` for enhanced features
3. **AgentBrain → AgentWrapper**: Use `createEnhancedAgent()` for high-level interfaces
4. **Keep using original classes** if you don't need enhanced features

## Advanced Features

### Configuration from File
```python
# Load configuration with error handling
try:
    config = pynlm.ConfigWrapper("my_config.json")
    print("Configuration loaded successfully")
except pynlm.NLMError as e:
    print(f"Failed to load configuration: {e}")
    # Fallback to default
    config = pynlm.ConfigWrapper()
```

### Custom Configuration
```python
# Create custom configuration
config = pynlm.ConfigWrapper()

# Customize parameters
config.set("custom.parameter", "value")
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.005)

# Save custom configuration
config.saveToFile("custom_config.json")
```

### Multi-threaded Usage
```python
import threading

def run_simulation(brain, steps):
    brain.runSteps(steps)

# Run multiple simulations in parallel
brains = [pynlm.createEnhancedBrain(pynlm.ConfigWrapper()) for _ in range(3)]
threads = []

for brain in brains:
    brain.initialize()
    thread = threading.Thread(target=run_simulation, args=(brain, 100))
    threads.append(thread)
    thread.start()

# Wait for all threads to complete
for thread in threads:
    thread.join()
```

## Troubleshooting

### Common Issues

**"ModuleNotFoundError: No module named 'pynlm'"**

```bash
# Install properly
pip install --force-reinstall .
```

**"ImportError: libnlm_enhanced_python.so: cannot open shared object file"**

```bash
# Reinstall to build enhanced bindings
pip install --force-reinstall . --no-build-isolation
```

**"Brain initialization fails"**

```python
# Check configuration
config = pynlm.ConfigWrapper()
print(f"Neuron count: {config.getNeuronCount()}")
print(f"Region count: {config.getRegionCount()}")

# Try initialization
brain = pynlm.createEnhancedBrain(config)
if not brain.initialize():
    print("Initialization failed - trying with default config")
    config = pynlm.ConfigWrapper()
    brain = pynlm.createEnhancedBrain(config)
    brain.initialize()
```

### Performance Tips

- Use presets for optimal configuration
- Monitor statistics for performance bottlenecks
- Use `logProgress=True` for debugging
- Consider using smaller neuron counts for testing
- Enable/disable features as needed

## API Reference

### Classes

#### ConfigWrapper
- Methods: `loadFromFile()`, `saveToFile()`, `setNeuronCount()`, `enablePlasticity()`, etc.
- Properties: `neuron_count`, `synapse_density`, `connection_probability`

#### BrainWrapper
- Methods: `initialize()`, `runSteps()`, `getStatistics()`, `advanceDevelopmentalStage()`
- Properties: `total_neurons`, `total_synapses`, `firing_neurons`, etc.

#### AgentWrapper
- Methods: `initializeWorld()`, `runEpisode()`
- Properties: Access to underlying agent and brain

### Functions

#### Creation Functions
- `createConfigWithPreset(preset)`
- `createBrainWithPreset(preset)`
- `createEnhancedBrain(config)`
- `createEnhancedAgent(brain)`

#### Constants
- `INVALID_NEURON_ID`, `INVALID_SYNAPSE_ID`, `INVALID_REGION_ID`

## Conclusion

The enhanced NLM Python API provides a significant improvement over the original bindings while maintaining full backward compatibility. The enhanced features make it easier to:

- Set up and configure simulations
- Monitor performance with statistical analysis
- Run complete agent episodes
- Handle errors gracefully
- Customize simulations for different use cases

Whether you're a beginner looking for simple setup or an advanced researcher needing sophisticated control, the enhanced API provides the tools you need.

## Acknowledgments

This enhancement was developed to address the limitations of the original NLM Python bindings and provide a more Pythonic and user-friendly interface for the NLM neural simulation framework.