# NLM Python API Reference

This document provides comprehensive reference for the NLM Python API, including all functions, classes, and usage examples.

## Overview

NLM (Neural Learning Machine) is an experimental artificial developmental brain simulation framework. The Python API provides access to the full NLM simulation capabilities through a clean, Pythonic interface.

## Installation

```bash
pip install pynlm
```

## Core Modules

### pynlm

Main module containing all NLM classes and functions.

### nlm.classes

Collection of core NLM classes for brain simulation, memory systems, and agent control.

## Configuration

### createDefaultConfig()

Creates a default NLM configuration with standard settings.

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()
```

### createEnhancedConfig()

Creates an enhanced configuration with advanced settings.

```python
import pynlm

# Create enhanced configuration
config = pynlm.createEnhancedConfig(
    brain_type="standard",
    neuron_count=5000,
    connection_probability=0.1,
    stdp_ltp_weight=0.01,
    stdp_ltd_weight=0.012
)
```

### createPresetConfig()

Creates preset configurations for common use cases.

```python
import pynlm

# Create different preset configurations
config_small = pynlm.createPresetConfig("small")
config_medium = pynlm.createPresetConfig("medium")
config_large = pynlm.createPresetConfig("large")
config_experimental = pynlm.createPresetConfig("experimental")
```

### Config Class

Configuration management for NLM systems.

#### Methods

- `has(key)` - Check if a configuration key exists
- `getKeys()` - Get all configuration keys
- `set(key, value)` - Set a configuration value
- `saveToFile(filepath)` - Save configuration to file
- `loadFromFile(filepath)` - Load configuration from file
- `loadFromArgs(argc, argv)` - Load from command line arguments
- `clear()` - Clear all configuration entries
- `summary()` - Get configuration summary
- `validate()` - Validate configuration and return errors

#### Properties

- `brain.neuron_count` - Number of neurons in the brain
- `brain.connection_probability` - Connection probability between neurons
- `brain.region_count` - Number of brain regions
- `plasticity.stdp.enable` - Enable/disable STDP plasticity
- `plasticity.hebbian.enable` - Enable/disable Hebbian plasticity
- `neuromodulation.curiosity.enabled` - Enable/disable curiosity modulation

## Brain Simulation

### createBrain()

Creates a new brain instance with the given configuration.

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
```

### Brain Class

The main brain simulation class containing all neural components.

#### Core Methods

- `initialize()` - Initialize the brain with current configuration
- `step(step_number)` - Perform a simulation step
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file

#### Sensor-Motor Interface

- `receiveSensoryInput(input)` - Inject sensory input into the brain
- `injectCurrent(neuron_id, current)` - Inject current into specific neuron
- `injectCurrentToNeurons(neuron_type, current)` - Inject current to neurons of a type
- `produceAction()` - Produce motor action based on neural activity

#### Accessors

- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getActiveNeuronCount()` - Get active neuron count
- `getFiringNeuronCount()` - Get currently firing neuron count
- `getAverageFiringRate()` - Get average firing rate
- `getExcitationInhibitionRatio()` - Get E/I balance ratio
- `getTotalSpikeCount()` - Get total spike count
- `getDevelopmentalStage()` - Get current developmental stage

#### Integrated Systems

- `getWorkingMemory()` - Get working memory system
- `getEpisodicMemory()` - Get episodic memory system
- `getAssociativeMemory()` - Get associative memory system
- `getPredictionSystem()` - Get prediction system
- `getPlanner()` - Get neural planner
- `getConceptFormation()` - Get concept formation system
- `getAttention()` - Get attention system
- `getDevelopmentSystem()` - Get development system
- `getDopamine()` - Get dopamine neuromodulator
- `getCuriosity()` - Get curiosity neuromodulator

## Environment and Agent

### createSimpleWorld()

Creates a simple 2D world for NLM simulation.

```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()

# Configure world
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
```

### SimpleWorld Class

Simulation environment with sensory input and motor output capabilities.

#### Core Methods

- `configure(width, height, vision_width, vision_height)` - Configure world dimensions
- `reset()` - Reset world to initial state
- `update(timestep)` - Update world state
- `applyMotorCommand(action, time)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept from world
- `getAgentBody()` - Get agent body state

#### Accessors

- `getWidth()` - Get world width
- `getHeight()` - Get world height
- `getSimulationTime()` - Get current simulation time

### createAgentBrain()

Creates an agent brain interface connecting brain to world.

```python
import pynlm

# Create agent brain
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)
```

### AgentBrain Class

Agent interface for processing sensory input and producing motor output.

#### Core Methods

- `initialize(world)` - Initialize with world interface
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command from brain activity
- `applyRewardModulation(reward, predicted_reward)` - Apply reward-based learning
- `updateDevelopment(timestep)` - Update development system
- `reset()` - Reset for new episode

#### Neuromodulation Control

- `enableRewardModulation(enable)` - Enable/disable reward modulation
- `enableStructuralPlasticity(enable)` - Enable/disable structural plasticity
- `enableDevelopment(enable)` - Enable/disable development
- `enableCuriosity(enable)` - Enable/disable curiosity

#### Accessors

- `getNeuromodulationLevel()` - Get current neuromodulation level
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level
- `getPredictionError()` - Get prediction error

## Advanced Utility Functions

### runAgentSimulation()

Run a complete agent simulation loop.

```python
import pynlm

# Create all components
config = pynlm.createPresetConfig("medium")
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Initialize
brain.initialize()
agent.initialize(world)

# Run simulation
energy, firing_rate, neuromod_level = pynlm.runAgentSimulation(
    brain, world, agent, num_steps=1000
)
```

### validateConfig()

Validate configuration and return errors.

```python
import pynlm

config = pynlm.createEnhancedConfig(...)
errors = pynlm.validateConfig(config)

if errors:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")
```

### exportConfigToDict()

Export configuration as Python dictionary.

```python
import pynlm

config = pynlm.createPresetConfig("large")
config_dict = pynlm.exportConfigToDict(config)

# Modify configuration
config_dict["brain.neuron_count"] = 20000

# Import back
new_config = pynlm.importConfigFromDict(config_dict)
```

## Usage Examples

### Example 1: Basic Brain Simulation

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
```

### Example 2: Complete Agent Simulation

```python
import pynlm

# Setup configuration and create components
config = pynlm.createEnhancedConfig("experimental")
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Initialize all systems
brain.initialize()
agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation loop
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step)
    
    # Get action and apply to world
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Optional: apply reward
    if step % 100 == 0:
        agent.applyRewardModulation(0.5, 0.0)
        
    # Print progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Energy: {world.getAgentBody().energy}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")

print("\nSimulation complete!")
```

### Example 3: Configuration Management

```python
import pynlm

# Create different configurations
presets = ["small", "medium", "large", "experimental"]
for preset in presets:
    print(f"\n{preset.upper()} PRESET:")
    config = pynlm.createPresetConfig(preset)
    print(f"  Neurons: {config.get('brain.neuron_count', 0)}")
    print(f"  Connection probability: {config.get('brain.connection_probability', 0.0)}")
    
    # Validate configuration
    errors = pynlm.validateConfig(config)
    if errors:
        print(f"  WARNING: {len(errors)} validation errors")

# Custom configuration
custom_config = pynlm.createEnhancedConfig(
    brain_type="custom",
    neuron_count=3000,
    connection_probability=0.05,
    stdp_ltp_weight=0.005,
    stdp_ltd_weight=0.01
)
```

### Example 4: Advanced Brain Features

```python
import pynlm

# Create experimental configuration
config = pynlm.createPresetConfig("experimental")
brain = pynlm.createBrain(config)
brain.initialize()

# Access integrated systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Check system status
print("=== System Status ===")
print(f"Working memory capacity: {working_memory.getActiveTraces()}")
print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount()}")
print(f"Association count: {associative_memory.getAssociationCount()}")

# Enable development
brain.setDevelopmentalStage("Adult")
development = brain.getDevelopmentSystem()

# Run development updates
for step in range(1000):
    if step % 100 == 0:
        development.update(brain, None, 10.0)  # 10 time units
        print(f"Step {step}, Developmental stage: {brain.getDevelopmentalStage()}")
```

## Error Handling

NLM provides standard Python exceptions:

- `RuntimeError` - General runtime errors
- `ValueError` - Invalid parameter values
- `FileNotFoundError` - Missing configuration files
- `MemoryError` - Insufficient memory for operations

## Performance Considerations

### Memory Management
- Use `reset()` to clear brain state between simulations
- Large brain configurations may require significant memory
- Monitor memory usage with `logStatus()` method

### Simulation Speed
- Adjust `simulation_timestep` in configuration for trade-off between speed and accuracy
- Use `brain.logStatus()` to monitor simulation performance
- Consider using preset configurations for optimized performance

### Scaling
- Small configurations: < 1000 neurons (quick testing)
- Medium configurations: 1000-10000 neurons (research)
- Large configurations: 10000-100000 neurons (production)
- Experimental configurations: optimized for learning and development

## Migration Guide

### From C++ to Python

1. **Configuration**: Replace C++ config file loading with `createDefaultConfig()` or preset configurations
2. **Brain Creation**: Use `createBrain(config)` instead of `Brain(config)` constructor
3. **Memory Systems**: Access through `brain.getWorkingMemory()` etc.
4. **Experiments**: Use `runAgentSimulation()` for complete agent control
5. **Error Handling**: Use Python exceptions instead of C++ error codes

### From v0.1 to v0.2

- Added `createEnhancedConfig()` for advanced configuration
- Added `validateConfig()` for configuration validation
- Added `exportConfigToDict()` and `importConfigFromDict()` for JSON-like configuration
- Added `createPresetConfig()` for standardized configurations
- Enhanced error handling and validation

## Limitations

- Maximum neuron count: 100,000 (memory constraint)
- Maximum simulation time: 10,000 steps per episode
- Synchronous simulation (all components updated per step)
- Single-threaded execution (future versions may add multi-threading)

## Future Enhancements

- GPU acceleration for large-scale simulations
- Asynchronous simulation for real-time applications
- Distributed computing for multi-agent systems
- Visualization APIs for real-time monitoring
- Extended world models (3D, physics-based)
- Advanced learning algorithms integration

## License

MIT License. See LICENSE file for details.

## Copyright

© 2026 NLM Project. All rights reserved.
