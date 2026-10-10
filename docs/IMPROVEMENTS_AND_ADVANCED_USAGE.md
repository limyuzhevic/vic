# NLM - Improvements and Advanced Usage Guide

## Overview

This document provides comprehensive information about the improvements made to the NLM project, including:

1. **Enhanced Configuration System**
2. **Improved Python Bindings**
3. **New Utility Functions and Advanced Features**
4. **Best Practices and Examples**

## 1. Enhanced Configuration System

### 1.1 JSON and YAML Support

The configuration system now supports multiple file formats:

- **JSON (`.json`)**: Modern, widely-used format with native Python support
- **YAML (`.yaml`, `.yml`)**: Human-readable format for configuration files
- **Simple (`.cfg`, `.txt`)**: Original key=value format for backward compatibility

#### Example JSON Configuration

```json
{
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.15,
        "connection_probability": 0.08,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.1,
        "v_thresh": -55.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.002
        },
        "structural": {
            "enable": true
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true
        },
        "novelty": {
            "enable": true
        }
    },
    "simulation": {
        "timestep": 0.001,
        "random_seed": 42
    }
}
```

#### Example YAML Configuration

```yaml
brain:
  neuron_count: 2000
  synapse_density: 0.15
  connection_probability: 0.08
  initial_weight_mean: 0.5
  initial_weight_std: 0.1
  v_thresh: -55.0
  v_rest: -70.0
  v_reset: -75.0
  tau_mem: 20.0
  tau_ref: 2.0

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
    tau_plus: 20.0
    tau_minus: 20.0
  hebbian:
    enable: true
    learning_rate: 0.002
  structural:
    enable: true

neuromod:
  dopamine:
    scale: 1.0
  curiosity:
    enable: true
  novelty:
    enable: true

simulation:
  timestep: 0.001
  random_seed: 42
```

#### Loading Configurations

```python
import pynlm

# Load from JSON file
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")

# Load from YAML file
config.loadFromFile("config.yaml")

# Load from simple format (backward compatibility)
config.loadFromFile("config.cfg")

# Load from command line arguments
config.loadFromArgs(3, ["program", "--brain.neuron_count=1000", "--simulation.timestep=0.01"])
```

### 1.2 Configuration Validation

The configuration system includes comprehensive validation:

- **Type validation**: Ensures values are of the correct type
- **Range validation**: Validates value ranges for known parameters
- **Cross-parameter validation**: Ensures parameter combinations are valid

#### Validation Examples

```python
import pynlm

config = pynlm.createLearningConfig()

# Try to set invalid value (will raise exception)
config.set("brain.neuron_count", -100)  # Invalid: negative neuron count

# Valid value set successfully
config.set("brain.neuron_count", 1000)  # Valid

# Range validation for probabilities
config.set("brain.synapse_density", 1.5)  # Invalid: > 1.0
config.set("brain.synapse_density", 0.5)  # Valid
```

### 1.3 Configuration Summary

Get a detailed summary of current configuration:

```python
import pynlm

config = pynlm.createLearningConfig()
summary = config.summary()
print(summary)
```

## 2. Enhanced Python Bindings

### 2.1 Vision, Audio, and InternalSignals Classes

The sensory input classes now have complete implementations:

#### Vision Class

```python
import pynlm
import numpy as np

# Create vision input with specified dimensions
vision = pynlm.Vision(64, 64, 3)  # 64x64 RGB image

# Set image data (normalized to [0, 1])
image_data = np.random.rand(64 * 64 * 3).astype(np.float32)
vision.setData(image_data)

# Access image properties
print(f"Width: {vision.getWidth()}")
print(f"Height: {vision.getHeight()}")
print(f"Channels: {vision.getChannels()}")
print(f"Total dimensions: {vision.getDimensions()}")

# Get data for processing
pixel_data = vision.getData()
```

#### Audio Class

```python
import pynlm
import numpy as np

# Create audio input
audio = pynlm.Audio(sampleRate=44100, numSamples=4410)  # 0.1 seconds of audio

# Set audio data
samples = np.random.randn(4410).astype(np.float32) * 0.1
audio.setData(samples)

# Set sample rate
audio.setSampleRate(48000)

print(f"Sample rate: {audio.getSampleRate()}")
print(f"Number of samples: {audio.getNumSamples()}")
print(f"Total dimensions: {audio.getDimensions()}")
```

#### InternalSignals Class

```python
import pynlm

# Create internal signals (e.g., from brain state)
internal = pynlm.InternalSignals()

# Add various internal signals
internal.addSignal(1.0)   # Allostatic load
internal.addSignal(0.5)   # Metabolic state
internal.addSignal(-0.2)  # Attention level
internal.addSignal(0.8)   # Excitatory drive

print(f"Number of signals: {internal.getDimensions()}")
print(f"All signals: {internal.getData()}")

# Clear signals when starting new episode
internal.clearSignals()
```

### 2.2 Configuration Presets

Three predefined configuration presets are available:

#### Learning Configuration

```python
import pynlm

config = pynlm.createLearningConfig()
print("Created learning-optimized configuration")
```

#### High Performance Configuration

```python
import pynlm

config = pynlm.createHighPerformanceConfig()
print("Created high-performance configuration")
```

#### Developmental Configuration

```python
import pynlm

config = pynlm.createDevelopmentalConfig()
print("Created developmental configuration")
```

## 3. New Utility Functions and Advanced Features

### 3.1 Batch Operations

Process multiple brains simultaneously for better performance:

#### Batch Brain Steps

```python
import pynlm

# Create multiple brains
brains = [
    pynlm.createBrain(pynlm.createLearningConfig()),
    pynlm.createBrain(pynlm.createLearningConfig()),
    pynlm.createBrain(pynlm.createLearningConfig()),
]

# Initialize all brains
for brain in brains:
    brain.initialize()

# Batch process multiple steps
pynlm.batchBrainSteps(brains, 100)  # All brains step together

# Get batch statistics
stats = pynlm.batchGetBrainStats(brains)
print(stats)
```

#### Batch Save/Load

```python
import pynlm

# Create brains
brains = [
    pynlm.createBrain(pynlm.createLearningConfig()),
    pynlm.createBrain(pynlm.createLearningConfig()),
]

# Initialize and simulate
for i, brain in enumerate(brains):
    brain.initialize()
    for step in range(100):
        brain.step(step)

# Batch save all brains
pynlm.batchSaveBrains(brains, "checkpoint")

# Clear and reload from batch
brains.clear()
brains = pynlm.createBrainPool(brains, pynlm.createLearningConfig(), 2)  # Placeholder for batch load
```

### 3.2 Agent Pool and World Pool

Create pools of agents and worlds for distributed simulation:

#### Agent Pool

```python
import pynlm

# Create a base brain
base_brain = pynlm.createBrain(pynlm.createLearningConfig())
base_brain.initialize()

# Create agent pool with 10 agents sharing the same brain
agents = pynlm.createAgentPool(base_brain, 10)

# Initialize all agents with their own worlds
for i, agent in enumerate(agents):
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)

# Simulate with all agents
for step in range(50):
    for agent in agents:
        world = agent.getBrain().getWorld()  # Get agent's world
        if world:
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            agent.getBrain().step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
```

#### World Pool

```python
import pynlm

# Create multiple worlds for parallel simulation
worlds = pynlm.createWorldPool(5, 30, 30, 12, 12)  # 5 worlds, 30x30, 12x12 vision

# Initialize all worlds
for world in worlds:
    world.reset()

# Run parallel episode across all worlds
agents = []
for world in worlds:
    brain = pynlm.createBrain(pynlm.createLearningConfig())
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    agents.append(agent)
    agent.initialize(world)

# Run parallel episode
pynlm.runParallelEpisode(agents, worlds, 100)
```

### 3.3 Parallel Episode Simulation

Run complex multi-agent simulations efficiently:

```python
import pynlm

# Setup simulation with multiple brains, agents, and worlds
brains = [pynlm.createBrain(pynlm.createHighPerformanceConfig()) for _ in range(4)]
agents = [pynlm.createAgentBrain(brain) for brain in brains]
worlds = [pynlm.createSimpleWorld() for _ in range(4)]

# Configure all worlds
for world in worlds:
    world.configure(width=40, height=40, visionWidth=16, visionHeight=16)
    world.reset()

# Initialize all agents with worlds
for agent, world in zip(agents, worlds):
    agent.initialize(world)

# Enable learning capabilities
for agent in agents:
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)

# Run complex parallel episode
pynlm.runParallelEpisode(brains, agents, worlds, 500)

# Analyze results
for i, brain in enumerate(brains):
    print(f"Brain {i}: {brain.getTotalSpikeCount()} spikes, "
          f"{brain.getFiringNeuronCount()} firing neurons, "
          f"{brain.getAverageFiringRate():.3f} firing rate")
```

## 4. Best Practices and Examples

### 4.1 Configuration Management Best Practices

```python
import pynlm

def createOptimizedConfig():
    """Create a configuration optimized for specific use case"""
    config = pynlm.createLearningConfig()
    
    # Fine-tune parameters for specific requirements
    config.set("brain.neuron_count", 1500)  # Adjust neuron count
    config.set("simulation.timestep", 0.001)  # Fine-tune timestep
    config.set("plasticity.stdp.learning_rate", 0.002)  # Adjust learning rate
    
    return config

def validateConfiguration(config):
    """Validate configuration parameters"""
    required_keys = ["brain.neuron_count", "brain.synapse_density", 
                     "simulation.timestep", "random_seed"]
    
    for key in required_keys:
        if not config.has(key):
            raise ValueError(f"Missing required configuration key: {key}")
    
    # Validate value ranges
    if config.getOr("brain.neuron_count", 0) <= 0:
        raise ValueError("Neuron count must be positive")
    
    if config.getOr("brain.synapse_density", 0.0) < 0.0 or config.getOr("brain.synapse_density", 0.0) > 1.0:
        raise ValueError("Synapse density must be between 0.0 and 1.0")
    
    return True
```

### 4.2 Error Handling Best Practices

```python
import pynlm
import traceback

def safeSimulation(config, steps=100):
    """Run simulation with comprehensive error handling"""
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps):
            brain.step(step)
            
            # Optional: Add periodic checkpointing
            if step % 100 == 0:
                brain.save(f"checkpoint_step_{step}.bin")
        
        return brain
        
    except Exception as e:
        print(f"Simulation error: {e}")
        traceback.print_exc()
        return None

# Usage with error handling
try:
    config = pynlm.createLearningConfig()
    brain = safeSimulation(config, 200)
    
    if brain is not None:
        print(f"Simulation completed successfully!")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
    else:
        print("Simulation failed!")
        
except Exception as e:
    print(f"Configuration error: {e}")
```

### 4.3 Performance Optimization Examples

```python
import pynlm
import time

def optimizeForPerformance():
    """Optimize configuration for maximum performance"""
    start_time = time.time()
    
    # Use high-performance configuration
    config = pynlm.createHighPerformanceConfig()
    
    # Create multiple brains for parallel processing
    brains = [pynlm.createBrain(config) for _ in range(5)]
    
    # Initialize all brains
    for brain in brains:
        brain.initialize()
    
    # Batch process for efficiency
    pynlm.batchBrainSteps(brains, 1000)
    
    end_time = time.time()
    
    print(f"Processed {len(brains)} brains in {end_time - start_time:.2f} seconds")
    print(f"Average steps per second: {(1000 * len(brains)) / (end_time - start_time):.0f}")
    
    return brains

def memoryEfficientSimulation():
    """Use memory-efficient approaches for large simulations"""
    config = pynlm.createDevelopmentalConfig()  # Smaller configuration
    
    # Use single brain with multiple episodes
    brain = pynlm.createBrain(config)
    
    for episode in range(10):
        brain.reset()
        brain.initialize()
        
        for step in range(100):
            brain.step(step)
            
            # Optional: Save checkpoint periodically
            if step % 50 == 0:
                brain.save(f"episode_{episode}_step_{step}.bin")
        
        print(f"Episode {episode + 1} completed")
    
    return brain
```

## 5. Migration Guide

### 5.1 From Simple Configuration to Enhanced System

If you were using the simple key=value configuration format, migration is straightforward:

#### Old Configuration Format

```
brain.neuron_count = 1000
brain.synapse_density = 0.1
simulation.timestep = 0.001
random_seed = 42
```

#### New Configuration (JSON)

```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1
    },
    "simulation": {
        "timestep": 0.001,
        "random_seed": 42
    }
}
```

### 5.2 From Basic Python API to Enhanced API

#### Old Code

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
for step in range(100):
    brain.step(step)
```

#### New Code with Best Practices

```python
import pynlm

# Use predefined optimized configuration
config = pynlm.createLearningConfig()
brain = pynlm.createBrain(config)
brain.initialize()
for step in range(100):
    brain.step(step)

# Add error handling and validation
try:
    if not validateConfiguration(config):
        raise ValueError("Invalid configuration")
        
    brain = pynlm.createBrain(config)
    brain.initialize()
    for step in range(100):
        brain.step(step)
        
except Exception as e:
    print(f"Simulation error: {e}")
    # Handle error or fallback
```

## 6. Troubleshooting

### 6.1 Common Issues and Solutions

#### Issue: Configuration Loading Failed

**Problem**: Configuration file cannot be loaded.

**Solutions**:

1. **File not found**: Ensure the configuration file exists
   ```python
   # Check if file exists before loading
   import os
   if os.path.exists("config.json"):
       config.loadFromFile("config.json")
   ```

2. **Invalid JSON format**: Use a valid JSON editor or validator
   ```python
   # Use try-catch for JSON loading
   try:
       config.loadFromFile("config.json")
   except Exception as e:
       print(f"JSON error: {e}")
       # Fall back to default configuration
       config = pynlm.createDefaultConfig()
   ```

3. **Invalid configuration values**: Use validation
   ```python
   # Validate configuration before use
   if not validateConfiguration(config):
       print("Configuration validation failed")
       config = pynlm.createLearningConfig()  # Use safe default
   ```

#### Issue: Python Bindings Not Available

**Problem**: pynlm module not found or initialization failed.

**Solutions**:

1. **Installation issues**: Reinstall the package
   ```bash
   pip install --force-reinstall pynlm
   ```

2. **Missing dependencies**: Install required dependencies
   ```bash
   pip install pybind11 scikit-build-core numpy
   ```

3. **Import errors**: Check Python path
   ```python
   import sys
   print(sys.path)
   
   # Add current directory to Python path
   import os
   sys.path.append(os.path.dirname(os.path.abspath(__file__)))
   ```

#### Issue: Simulation Performance Problems

**Problem**: Simulation running slowly or consuming excessive memory.

**Solutions**:

1. **Use batch processing**: Process multiple brains in batches
   ```python
   # Use batch operations for better performance
   brains = [pynlm.createBrain(config) for _ in range(10)]
   pynlm.batchBrainSteps(brains, 100)
   ```

2. **Optimize configuration**: Use appropriate configuration for your hardware
   ```python
   # Choose configuration based on available memory
   if available_memory > 1024:  # MB
       config = pynlm.createHighPerformanceConfig()
   else:
       config = pynlm.createDevelopmentalConfig()
   ```

3. **Memory management**: Clear references when done
   ```python
   # Clear references to free memory
   del brain
   import gc
   gc.collect()
   ```

### 6.2 Debug Information

Add debug output to troubleshoot issues:

```python
import pynlm

# Create configuration with debug output
config = pynlm.createLearningConfig()
print("Configuration loaded successfully")
print(config.summary())

# Create brain with debug
brain = pynlm.createBrain(config)
print("Brain created successfully")

# Add debug logging
import logging
logging.basicConfig(level=logging.DEBUG)

# Run simulation with progress tracking
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getTotalSpikeCount()} spikes, "
              f"{brain.getFiringNeuronCount()} firing neurons")
```

## 7. Future Enhancements

### 7.1 Planned Features

1. **Advanced Visualization**: Integration with matplotlib and other visualization libraries
2. **Distributed Computing**: Support for multi-GPU and distributed simulations
3. **Machine Learning Integration**: Direct integration with scikit-learn and TensorFlow
4. **Web Interface**: Web-based visualization and control
5. **Cloud Computing**: Support for cloud-based simulation

### 7.2 Research and Development

The NLM project continues to evolve with new research areas:

- **Neuromodulation Research**: Advanced studies of dopamine, serotonin, and other neuromodulators
- **Development Research**: How developmental processes affect learning and behavior
- **Multi-Agent Systems**: Complex interactions between multiple intelligent agents
- **Real-World Applications**: Applying NLM principles to robotics, gaming, and education

## 8. Conclusion

The NLM project has been significantly enhanced with:

1. **Robust Configuration System**: JSON/YAML support with validation and error handling
2. **Complete Python Bindings**: Full implementations for all sensory input classes
3. **Advanced Utility Functions**: Batch operations, parallel simulation, and agent pools
4. **Comprehensive Documentation**: Examples, best practices, and troubleshooting
5. **Backward Compatibility**: Support for existing configuration formats and APIs

These improvements make NLM more powerful, easier to use, and suitable for a wider range of applications, from research to production deployments.

## References

- NLM Documentation: https://nlm.readthedocs.io
- NLM GitHub: https://github.com/nlm-project/nlm
- Python Bindings API: https://pynlm.readthedocs.io

---
*Document Version: 1.0*
*Last Updated: 2026-01-01*
*Authors: NLM Development Team*