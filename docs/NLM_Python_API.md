# NLM Python Bindings - Advanced API Reference

This document provides comprehensive reference for the advanced Python API of the NLM (Neural Learning Machine) Python bindings.

## Overview

The NLM Python bindings provide a powerful interface to the advanced neural simulation framework implemented in C++. The API is designed to be both intuitive for beginners and flexible for advanced users who need fine-grained control over neural computation.

## Core Classes

### Brain

The main interface for neural computation and simulation.

#### Creation and Configuration

```python
# Default configuration
brain = nlm.Brain()  # Creates brain with default settings

# Configuration from file
brain = nlm.createBrainWithConfig("configs/my_config.json")

# Custom configuration
config = nlm.Config()
config.set("neuron_count", 1000)
brain = nlm.createBrain(config)

# Factory methods
brain = nlm.createBrainWithDefaults(neuron_count=500, region_count=3)
```

#### Core Methods

```python
brain.initialize()                    # Initialize brain with configuration
brain.step(step)                      # Advance simulation by one step
brain.step(step, current_time)         # Advance with timestamp
brain.reset()                          # Reset brain state
brain.save(filepath)                  # Save brain state to file
brain.load(filepath)                  # Load brain state from file
brain.logStatus()                      # Print brain status information
```

#### Statistics and Information

```python
brain.getTotalNeuronCount()            # Total number of neurons
brain.getTotalSynapseCount()            # Total number of synapses
brain.getActiveNeuronCount()           # Currently active neurons
brain.getFiringNeuronCount()           # Neurons firing this step
brain.getAverageFiringRate()           # Average firing rate across all neurons
brain.getExcitationInhibitionRatio()  # E/I balance ratio
brain.getTotalSpikeCount()             # Total spikes generated
brain.getPendingSpikeEventCount()      # Pending spike events
brain.getRegionCount()                 # Number of neural regions
```

#### Sensory Processing

```python
brain.receiveSensoryInput(percept)    # Inject sensory input into brain
brain.injectCurrent(neuron_id, current) # Inject current into specific neuron
brain.injectCurrentToNeurons(type, current)  # Inject to all neurons of type
```

#### Action and Motor Control

```python
action = brain.produceAction()          # Generate motor action based on neural activity
brain.applyNeuromodulation(signal)       # Apply neuromodulatory effects
brain.applyPlasticity()                  # Apply plasticity rules
brain.develop()                         # Apply developmental changes
```

#### Memory Systems

```python
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Working memory
working_memory.getCapacity()            # Current capacity
working_memory.getActiveTraces()        # Number of active traces
working_memory.storeToNeuron(id, value)  # Store information to neuron
working_memory.update(dt)               # Update working memory

# Episodic memory
episodic_memory.getMaxEpisodes()        # Maximum episodes
episodic_memory.getEpisodeCount()       # Current episodes
episodic_memory.storeEpisode(episode)   # Store new episode
episodic_memory.consolidate(ratio)      # Consolidate memories
```

#### Neuromodulation Systems

```python
dopamine = brain.getDopamine()           # Reward prediction error system
curiosity = brain.getCuriosity()        # Exploration motivation
novelty = brain.getNovelty()             # Novelty detection
prediction_error = brain.getPredictionErrorSignal()  # Prediction error

# Access neuromodulation levels
print(f"Dopamine level: {dopamine.getLevel()}")
print(f"Curiosity level: {curiosity.getLevel()}")
print(f"Novelty level: {novelty.getLevel()}")
```

### Sensory Inputs

#### Vision

```python
# Create vision sensory input
vision = nlm.createVision(width, height, channels)

# Set data
vision.setData(data_vector)  # Set all pixel data

# Access properties
print(f"Width: {vision.getWidth()}")
print(f"Height: {vision.getHeight()}")
print(f"Channels: {vision.getChannels()}")
print(f"Dimensions: {vision.getDimensions()}")
print(f"Data: {vision.getData()}")

# Clone
cloned_vision = vision.clone()
```

#### Audio

```python
# Create audio sensory input
audio = nlm.createAudio(sample_rate, num_samples)

# Set data
audio.setData(data_vector)
audio.setSampleRate(rate)

# Access properties
print(f"Sample rate: {audio.getSampleRate()}")
print(f"Num samples: {audio.getNumSamples()}")
print(f"Dimensions: {audio.getDimensions()}")
print(f"Data: {audio.getData()}")

# Clone
cloned_audio = audio.clone()
```

#### InternalSignals

```python
# Create internal signals
internal = nlm.createInternalSignals()

# Add signals
internal.addSignal(value1)
internal.addSignal(value2)
internal.clearSignals()

# Access properties
print(f"Signals: {internal.getSignals()}")
print(f"Count: {internal.getSignalCount()}")
print(f"Dimensions: {internal.getDimensions()}")
```

### Action and Motor Output

```python
# Create action
action = nlm.Action(action_type)  # action_type from ActionType enum
action = nlm.Action(action_type, parameters)  # with parameters

# Access properties
print(f"Type: {action.getType()}")
print(f"Name: {action.getName()}")
print(f"Parameters: {action.getParameters()}")

# Modify
action.setType(new_type)
action.setParameters(new_params)

# Clone
cloned_action = action.clone()
```

### AgentBrain

```python
# Create agent brain interface
agent_brain = nlm.createAgentBrain(brain)

# Process sensory input
agent_brain.processSensoryInput(percept)

# Get motor commands
motor_command = agent_brain.decodeMotorCommand()

# Access neuromodulation levels
print(f"Reward modulation: {agent_brain.isRewardModulationEnabled()}")
print(f"Curiosity: {agent_brain.isCuriosityEnabled()}")
print(f"Development: {agent_brain.isDevelopmentEnabled()}")
```

### SimpleWorld

```python
# Create world
world = nlm.createSimpleWorld()

# Configure
world.configure(width, height, vision_width, vision_height)
world.setAgentStart(x, y)
world.setMaxEnergy(energy)
world.setRandomSeed(seed)

# Run simulation
world.reset()
world.update(timestep)

# Get state
percept = world.getSensoryPercept()
body = world.getAgentBody()
```

## Advanced API Features

### Factory Methods

```python
# Create brain with configuration from file
brain = nlm.createBrainWithConfig("path/to/config.json")

# Create brain with defaults
brain = nlm.createBrainWithDefaults(neurons=1000, regions=5)

# Create sensory inputs
vision = nlm.createVision(64, 64, 3)
audio = nlm.createAudio(44100, 2000)
internal = nlm.createInternalSignals()

# Create actions
action = nlm.createAction(ActionType.Interact, [0.5, 0.8, 0.3])
```

### Configuration Utilities

```python
# Create default configuration
config = nlm.createDefaultConfig()

# Validate configuration
try:
    nlm.validateConfig(config)
    print("Configuration is valid")
except Exception as e:
    print(f"Configuration error: {e}")

# Access constants
constants = nlm.getConstants()
print(f"Max neuron value: {constants['MAX_NEURON_VALUE']}")
```

### Utility Functions

```python
# Random ID generation
neuron_id = nlm.randomNeuronId()
synapse_id = nlm.randomSynapseId()

# Constants access
constants = nlm.getConstants()
for key, value in constants.items():
    print(f"{key}: {value}")
```

## Advanced Usage Patterns

### 1. Multi-Agent System

```python
# Create multiple brains
agents = []
for i in range(3):
    brain = nlm.createBrainWithDefaults(500, 2)
    agents.append(nlm.createAgentBrain(brain))

# Simulate multiple agents
for step in range(1000):
    for agent in agents:
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        command = agent.decodeMotorCommand()
        world.applyMotorCommand(command, step * 0.01)
```

### 2. Learning Experiment

```python
# Create experiment brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Enable plasticity
brain.getSTDP().setLTPWeight(0.02)
brain.getSTDP().setLTDWeight(0.015)

# Run learning experiment
for episode in range(100):
    # Reset and initialize
    brain.reset()
    brain.initialize()
    
    # Apply stimulus pattern
    for step in range(200):
        # Create input pattern
        vision = nlm.createVision(10, 10, 1)
        vision.setData([i * 0.1 for i in range(100)])
        
        # Inject into brain
        percept = nlm.SensoryPercept()
        percept.setVision(vision)
        brain.receiveSensoryInput(percept)
        
        # Step brain
        brain.step(step)
        
        # Check for learning
        if brain.getTotalSpikeCount() > 0:
            print(f"Episode {episode}, Step {step}: Learning detected")
            break
```

### 3. Memory Management

```python
# Configure memory systems
brain = nlm.createBrainWithDefaults(1000, 3)

# Set memory parameters
brain.getWorkingMemory().setCapacity(100)
brain.getEpisodicMemory().setMaxEpisodes(1000)

# Monitor memory usage
for step in range(1000):
    brain.step(step)
    
    # Log memory status
    working_mem = brain.getWorkingMemory()
    episodic_mem = brain.getEpisodicMemory()
    
    if working_mem and episodic_mem:
        print(f"Step {step}: "
              f"Working: {working_mem.getActiveTraces()}, "
              f"Episodic: {episodic_mem.getEpisodeCount()}")
```

### 4. Performance Monitoring

```python
import time

# Performance benchmarking
brains = []
for i in range(5):
    brain = nlm.createBrainWithDefaults(500, 2)
    brains.append(brain)

# Measure setup time
start_time = time.time()
for brain in brains:
    brain.initialize()
setup_time = time.time() - start_time

print(f"Setup time: {setup_time:.3f}s for {len(brains)} brains")

# Measure simulation time
total_steps = 0
for brain in brains:
    for step in range(100):
        start = time.time()
        brain.step(step)
        total_steps += 1
        elapsed = time.time() - start
        
        # Log if step takes too long
        if elapsed > 0.1:  # 100ms threshold
            print(f"Slow step detected: {elapsed:.3f}s")
```

## Error Handling and Validation

### Configuration Validation

```python
def validate_configuration(config):
    try:
        nlm.validateConfig(config)
        return True
    except Exception as e:
        print(f"Invalid configuration: {e}")
        return False

# Usage
config = nlm.createDefaultConfig()
if validate_configuration(config):
    brain = nlm.createBrain(config)
else:
    print("Using default configuration")
    brain = nlm.createBrainWithDefaults()
```

### Exception Handling

```python
try:
    brain = nlm.createBrainWithConfig("nonexistent_config.json")
except Exception as e:
    print(f"Failed to create brain: {e}")
    # Fallback to default configuration
    brain = nlm.createBrainWithDefaults()
```

## Best Practices

### 1. Memory Management

```python
# Use clone methods instead of direct copying
original_vision = nlm.createVision(64, 64, 3)
cloned_vision = original_vision.clone()

# Clean up when done
if 'large_data' in locals():
    del large_data
import gc
gc.collect()
```

### 2. Configuration Consistency

```python
# Always validate configurations
config = nlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 3)

if not nlm.validateConfig(config):
    raise ValueError("Invalid configuration")

brain = nlm.createBrain(config)
```

### 3. Performance Optimization

```python
# Batch operations when possible
vision_data = [i * 0.1 for i in range(10000)]
for i in range(0, len(vision_data), 1000):
    chunk = vision_data[i:i+1000]
    # Process chunk

# Use appropriate data types for performance
import array
efficient_data = array.array('f', vision_data)
```

### 4. Debugging and Monitoring

```python
# Enable logging
import logging
logging.basicConfig(level=logging.INFO)

# Monitor brain status
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        brain.logStatus()
```

## Migration Guide

### From C++ to Python

If you're migrating from C++ NLM to Python, here's what changed:

- **Memory management**: Python handles automatic garbage collection
- **Configuration**: Use `createBrainWithConfig()` instead of manual config loading
- **Error handling**: Use try-catch blocks for file operations
- **Debugging**: Use Python's interactive debugging tools

### Version Notes

- **Version 0.1.0**: Initial release with basic functionality
- **Version 0.2.0**: Added advanced factory methods and utilities
- **Version 0.3.0**: Enhanced memory systems and neuromodulation support

## Troubleshooting

### Common Issues

**Issue**: Configuration validation fails
**Solution**: Check `nlm.validateConfig()` for error messages and fix configuration

**Issue**: Brain initialization fails
**Solution**: Verify all required configuration parameters are set

**Issue**: Performance is slow
**Solution**: Reduce brain size, optimize data structures, or use batch operations

**Issue**: Memory usage is high
**Solution**: Reduce memory system capacities or clean up unused references

### Getting Help

- Check the NLM documentation for C++ API reference
- Review the example files for common patterns
- Use Python's help() function for class and method documentation
- Report issues with specific error messages and configuration

## Conclusion

The NLM Python bindings provide a comprehensive, flexible interface for neural simulation and computation. This document covers the essential API patterns and best practices for effective usage. The bindings are designed to be both accessible for beginners and powerful enough for advanced research and development applications.

For the most up-to-date information and additional examples, refer to the NLM project documentation and the example files included in this distribution.
