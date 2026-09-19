# NLM Python API Documentation

## Overview

This is the complete API documentation for the `pynlm` Python package, which provides Python bindings to the NLM (Neural Learning Machine) C++ neural simulation framework. NLM is an experimental computational brain project that simulates artificial developmental brains capable of learning, memory, and adaptive behavior.

The API has been enhanced with convenience functions, batch operations, validation, and error handling to make it more accessible for beginners while maintaining flexibility for advanced users.

## Quick Start for Beginners

### Simplest Possible Example

```python
import pynlm

# Create a complete simulation with one function call
brain, world, agent = pynlm.quickStartSimulation()

# Run for a few steps
for i in range(10):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

### Learning-Optimized Setup

```python
import pynlm

# Create learning-ready simulation
brain, world, agent = pynlm.quickStartLearning()

# Run learning episode
stats = pynlm.runEpisode(brain, world, agent, steps=1000, verbose=True)

print(f"Learning episode completed:")
print(f"  - Total spikes: {stats['spikes']}")
print(f"  - Firing rate: {stats['firing_rate']:.2f}")
print(f"  - Neuromodulation level: {stats['reward_modulation']:.2f}")
print(f"  - Curiosity level: {stats['curiosity']:.2f}")
```

## API Reference

### Factory Functions

#### `pynlm.createBrain(config=None)`

Creates a brain with configuration.

**Parameters:**
- `config` (optional): Configuration object. If None, creates default config.

**Returns:**
- Brain object

**Example:**
```python
import pynlm

# Create with default config
brain = pynlm.createBrain()

# Create with custom config
config = pynlm.createLearningConfig()
brain = pynlm.createBrain(config)
```

#### `pynlm.createSimpleWorld()`

Creates a new simple 2D world.

**Returns:**
- SimpleWorld object

**Example:**
```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
```

#### `pynlm.createAgentBrain(brain)`

Creates an agent brain interface.

**Parameters:**
- `brain`: Brain object to interface with

**Returns:**
- AgentBrain object

**Example:**
```python
import pynlm

brain = pynlm.createBrain()
agent = pynlm.createAgentBrain(brain)
```

### Convenience Functions

#### `pynlm.quickStartSimulation()`

Creates a complete working simulation with sensible defaults.

**Returns:**
- Tuple of (brain, world, agent)

**Recommended for:**
- Beginners
- Quick prototyping
- Testing

#### `pynlm.quickStartLearning()`

Creates simulation optimized for learning scenarios.

**Returns:**
- Tuple of (brain, world, agent)

**Features:**
- Learning-enabled configuration
- Reward modulation enabled
- Curiosity enabled
- Development enabled
- Added learning objects (resources, markers)

#### `pynlm.quickStartExploration()`

Creates simulation optimized for exploration and discovery.

**Returns:**
- Tuple of (brain, world, agent)

**Features:**
- Exploration-focused configuration
- Structural plasticity enabled
- Hazard and goal objects added
- High curiosity settings

### Preset Configurations

#### `pynlm.createDefaultConfig()`

Creates empty default configuration.

**Returns:**
- Config object

#### `pynlm.createDebugConfig()`

Creates debug-friendly configuration with verbose logging.

**Returns:**
- Config object

**Use case:**
- Debugging simulation issues
- Development and testing
- Detailed logging output

#### `pynlm.createProductionConfig()`

Creates production-optimized configuration.

**Returns:**
- Config object

**Use case:**
- Large-scale simulations
- Performance-critical applications
- Long-running experiments

#### `pynlm.createLearningConfig()`

Creates configuration optimized for learning.

**Returns:**
- Config object

**Features:**
- Learning neuromodulators enabled
- High plasticity settings
- Reasonable resource allocation

### Batch Operations

#### `pynlm.setConfigBatch(config, values)`

Sets multiple configuration values at once from a dictionary.

**Parameters:**
- `config`: Config object
- `values`: Dictionary of key-value pairs

**Returns:**
- Config object (for method chaining)

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()

# Set multiple values at once
pynlm.setConfigBatch(config, {
    'random_seed': 42,
    'neuron_count': 1000,
    'debug': True,
    'simulation_timestep': 0.001
})
```

#### `pynlm.configureWorldForSimulation(world, width, height, visionWidth, visionHeight)`

Configures world with learning-friendly defaults.

**Parameters:**
- `world`: SimpleWorld object
- `width`, `height`: World dimensions
- `visionWidth`, `visionHeight`: Vision dimensions

**Returns:**
- World object (for method chaining)

**Example:**
```python
import pynlm

world = pynlm.createSimpleWorld()
pynlm.configureWorldForSimulation(world, 15, 15, 6, 6)
```

#### `pynlm.initializeAgentComplete(agent, world)`

Initializes agent with world and returns reference.

**Parameters:**
- `agent`: AgentBrain object
- `world`: SimpleWorld object

**Returns:**
- AgentBrain object (for method chaining)

### Validation and Error Handling

#### `pynlm.validateConfig(config)`

Validates configuration and raises error if invalid.

**Parameters:**
- `config`: Config object

**Raises:**
- ValueError if configuration is invalid

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
config.set('neuron_count', -100)  # Invalid

try:
    pynlm.validateConfig(config)
except ValueError as e:
    print(f"Validation error: {e}")
```

#### `pynlm.configSummary(config)`

Gets human-readable summary of configuration.

**Parameters:**
- `config`: Config object

**Returns:**
- String summary

**Example:**
```python
import pynlm

config = pynlm.createLearningConfig()
summary = pynlm.configSummary(config)
print(summary)
```

#### `pynlm.validateSimulationSetup(brain, world, agent)`

Validates complete simulation setup.

**Parameters:**
- `brain`: Brain object
- `world`: SimpleWorld object
- `agent`: AgentBrain object

**Raises:**
- ValueError if setup is invalid

#### `pynlm.validateCompleteSetup(brain, world, agent)`

Validates complete learning setup with comprehensive checks.

**Parameters:**
- `brain`: Brain object
- `world`: SimpleWorld object
- `agent`: AgentBrain object

**Raises:**
- ValueError if setup is invalid

### Simulation Helpers

#### `pynlm.runEpisode(brain, world, agent, steps)`

Runs complete simulation episode and returns statistics.

**Parameters:**
- `brain`: Brain object
- `world`: SimpleWorld object
- `agent`: AgentBrain object
- `steps`: Number of steps to run

**Returns:**
- Dictionary with episode statistics

**Example:**
```python
import pynlm

brain, world, agent = pynlm.quickStartLearning()
stats = pynlm.runEpisode(brain, world, agent, steps=1000)

print(f"Episode completed in {stats['steps']} steps")
print(f"Total spikes: {stats['spikes']}")
print(f"Firing rate: {stats['firing_rate']:.2f}")
```

#### `pynlm.runEpisodeWithLogging(brain, world, agent, steps, verbose=False)`

Runs episode with optional verbose logging.

**Parameters:**
- `brain`: Brain object
- `world`: SimpleWorld object
- `agent`: AgentBrain object
- `steps`: Number of steps to run
- `verbose`: Enable verbose logging

**Returns:**
- Dictionary with comprehensive statistics

#### `pynlm.safeInitialize(brain)`

Safely initializes brain with validation and error handling.

**Parameters:**
- `brain`: Brain object

**Raises:**
- ValueError if initialization fails

#### `pynlm.safeWorldSetup(world, width, height, visionWidth, visionHeight)`

Sets up world with comprehensive validation.

**Parameters:**
- `world`: SimpleWorld object
- `width`, `height`: World dimensions
- `visionWidth`, `visionHeight`: Vision dimensions

**Raises:**
- ValueError if setup fails

### Monitoring and Debugging

#### `pynlm.getSystemStatus(brain, world, agent)`

Gets comprehensive system status as dictionary.

**Parameters:**
- `brain`: Brain object
- `world`: SimpleWorld object
- `agent`: AgentBrain object

**Returns:**
- Dictionary with system status

**Example:**
```python
import pynlm

brain, world, agent = pynlm.quickStartLearning()
status = pynlm.getSystemStatus(brain, world, agent)

print("System status:")
print(f"  - Neurons: {status['neurons']}")
print(f"  - Synapses: {status['synapses']}")
print(f"  - Spikes: {status['spikes']}")
print(f"  - Firing rate: {status['firing_rate']:.2f}")
print(f"  - Dopamine: {status['dopamine']:.2f}")
print(f"  - Curiosity: {status['curiosity']:.2f}")
```

#### `pynlm.getLearningStats(brain, agent)`

Gets learning and performance statistics.

**Parameters:**
- `brain`: Brain object
- `agent`: AgentBrain object

**Returns:**
- Dictionary with learning statistics

### Advanced Helpers

#### `pynlm.getLearningStats(brain, agent)`

Gets comprehensive learning and performance statistics.

**Parameters:**
- `brain`: Brain object
- `agent`: AgentBrain object

**Returns:**
- Dictionary with learning indicators

#### `pynlm.enableAllLearningFeatures(agent)`

Enables all learning features on agent.

**Parameters:**
- `agent`: AgentBrain object

**Returns:**
- AgentBrain object (for method chaining)

#### `pynlm.setupCompleteAgent(brain, world)`

Sets up complete agent simulation.

**Parameters:**
- `brain`: Brain object
- `world`: SimpleWorld object

**Returns:**
- Tuple of (brain, world, agent)

#### `pynlm.setupCompleteAgentWithConfig(config)`

Sets up complete agent simulation with custom configuration.

**Parameters:**
- `config`: Config object

**Returns:**
- Tuple of (brain, world, agent)

#### `pynlm.completeLearningSetup(config)`

Complete learning setup with validation and error handling.

**Parameters:**
- `config`: Config object

**Returns:**
- Tuple of (brain, world, agent)

## Best Practices

### 1. Always Validate Your Setup

```python
import pynlm

brain, world, agent = pynlm.quickStartLearning()

# Validate before use
try:
    pynlm.validateCompleteSetup(brain, world, agent)
    print("Setup is valid!")
except ValueError as e:
    print(f"Setup validation failed: {e}")
```

### 2. Use Safe Initialization for Production

```python
import pynlm

brain = pynlm.createBrain()
try:
    pynlm.safeInitialize(brain)
    print("Brain initialized successfully!")
except ValueError as e:
    print(f"Initialization failed: {e}")
```

### 3. Batch Configuration for Efficiency

```python
import pynlm

config = pynlm.createDefaultConfig()
pynlm.setConfigBatch(config, {
    'random_seed': 123,
    'neuron_count': 500,
    'enable_reward_modulation': True,
    'enable_curiosity': True
})
```

### 4. Use Learning-Optimized Defaults

```python
import pynlm

# Quick start with learning features
brain, world, agent = pynlm.quickStartLearning()

# Enable additional features
pynlm.enableAllLearningFeatures(agent)
```

### 5. Monitor System Health

```python
import pynlm

brain, world, agent = pynlm.quickStartLearning()

# Check system status periodically
status = pynlm.getSystemStatus(brain, world, agent)
if status['learning_active'] and status['curiosity_high']:
    print("Learning is active and curiosity is high!")
```

## Complete Example

```python
import pynlm

# Create learning-optimized simulation
print("Creating learning simulation...")
brain, world, agent = pynlm.quickStartLearning()

# Validate setup
try:
    pynlm.validateCompleteSetup(brain, world, agent)
    print("Setup validated successfully!")
except ValueError as e:
    print(f"Setup error: {e}")
    exit(1)

# Run learning episode
print("Running learning episode...")
stats = pynlm.runEpisodeWithLogging(brain, world, agent, steps=2000, verbose=True)

# Display results
print("\n=== Learning Episode Results ===")
print(f"Steps completed: {stats['steps']}")
print(f"Total spikes: {stats['spikes']}")
print(f"Average firing rate: {stats['firing_rate']:.2f}")
print(f"Final neuromodulation level: {stats['reward_modulation']:.2f}")
print(f"Final curiosity level: {stats['curiosity']:.2f}")
print(f"Final novelty level: {stats['novelty_level']:.2f}")
print(f"Final prediction error: {stats['prediction_error']:.2f}")
print(f"Developmental stage: {stats['developmental_stage']}")

# Check learning indicators
print("\n=== Learning Indicators ===")
print(f"Learning active: {stats['learning_active']}")
print(f"Novelty significant: {stats['novelty_significant']}")
print(f"Curiosity high: {stats['curiosity_high']}")

print("\n=== Example Complete ===")
```

## Migration Guide

### From Basic API to Enhanced API

**Old Way (Basic API):**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for i in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

**New Way (Enhanced API):**
```python
import pynlm

brain, world, agent = pynlm.quickStartLearning()

# Or use the new batch setup
config = pynlm.createLearningConfig()
brain, world, agent = pynlm.setupCompleteAgentWithConfig(config)

# Run with statistics
stats = pynlm.runEpisode(brain, world, agent, steps=100)
```

### From Manual Validation to Automatic Validation

**Old Way (Manual checks):**
```python
if brain.getTotalNeuronCount() == 0:
    raise ValueError("Brain has no neurons")
if world.getWidth() == 0:
    raise ValueError("World has invalid dimensions")
```

**New Way (Automatic validation):**
```python
try:
    pynlm.validateCompleteSetup(brain, world, agent)
except ValueError as e:
    print(f"Validation error: {e}")
```

## Troubleshooting

### Common Issues and Solutions

#### 1. "Brain has no neurons"

**Problem:** Brain created but contains no neurons.

**Solution:** Use `pynlm.quickStartLearning()` or ensure brain is properly initialized.

#### 2. "World has invalid dimensions"

**Problem:** World created with zero dimensions.

**Solution:** Use `pynlm.safeWorldSetup()` or configure world properly.

#### 3. "Agent not properly connected to brain"

**Problem:** Agent brain interface not connected to brain.

**Solution:** Use `pynlm.setupCompleteAgent()` or ensure proper initialization sequence.

#### 4. "No learning features enabled"

**Problem:** Agent lacks learning capabilities.

**Solution:** Use `pynlm.enableAllLearningFeatures()` or enable features individually.

### Error Messages

The enhanced API provides detailed error messages:

- **Configuration validation:** "Configuration validation failed: max_neurons must be positive; simulation_timestep must be positive"
- **Setup validation:** "Complete setup validation failed: Brain has no neurons; Agent has no learning features enabled"
- **Initialization errors:** "Safe initialization failed: Brain initialization failed"

All error messages include specific details about what went wrong and how to fix it.

## Version Information

- **Package:** pynlm
- **Version:** Enhanced API (based on NLM 0.1.0)
- **Python:** 3.8+
- **Features:** All basic functionality + convenience functions + batch operations + validation + error handling + monitoring

## Acknowledgments

This enhanced API is based on the original NLM (Neural Learning Machine) framework and has been improved with:

- Convenience functions for beginners
- Batch operations for efficiency
- Configuration validation and error handling
- Comprehensive monitoring and debugging tools
- Learning-optimized presets
- Complete documentation and examples

The goal is to make NLM more accessible while maintaining full flexibility for advanced users.
