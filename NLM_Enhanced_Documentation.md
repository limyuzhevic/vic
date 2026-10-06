# NLM (Neural Learning Machine) - Enhanced Documentation

# Table of Contents

1. [Introduction](#introduction)
2. [Python API Improvements](#python-api-improvements)
3. [Command Line Interface](#command-line-interface)
4. [Advanced Configuration Management](#advanced-configuration-management)
5. [Performance Monitoring & Profiling](#performance-monitoring--profiling)
6. [Error Handling & Exceptions](#error-handling--exceptions)
7. [Advanced Usage Examples](#advanced-usage-examples)
8. [Migration Guide](#migration-guide)
9. [Troubleshooting](#troubleshooting)
10. [Future Enhancements](#future-enhancements)

# Introduction

NLM (Neural Learning Machine) is an experimental artificial developmental brain project that simulates spiking neural networks. This document covers the enhanced features and improvements made to the NLM Python API, including better convenience methods, error handling, command-line interface, configuration management, and performance monitoring.

## Key Improvements

- **Enhanced Python API**: New convenience methods and better error handling
- **Command Line Interface**: Comprehensive CLI for simulation control
- **Advanced Configuration**: Schema validation, templates, and version control
- **Performance Monitoring**: Built-in benchmarking and profiling tools
- **Better Documentation**: Comprehensive examples and API reference
- **Error Handling**: Custom exception hierarchy for better debugging

# Python API Improvements

## 1. Enhanced Convenience Methods

The NLM Python API now includes many new convenience methods that simplify common tasks:

### Core Enhancement Functions

```python
# Create brain from dictionary configuration
import pynlm
config_dict = {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1
}
brain = pynlm.createBrainFromDict(config_dict)
brain.initialize()

# Run complete episode simulation
brain, agent, world = pynlm.createAgentEnvironment()
pynlm.runEpisode(brain, agent, world, 1000, enable_logging=True)

# Get statistics as Python dictionary
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
stats = pynlm.getBrainStats(brain)
print(f"Neurons: {stats['total_neurons']}")
print(f"Firing neurons: {stats['firing_neurons']}")
```

### Advanced Helper Functions

```python
# Create world objects with validation
data = pynlm.createWorldObjects([
    {"x": 0.0, "y": 0.0, "type": pynlm.WorldObjectType.Resource, "value": 1.0},
    {"x": 5.0, "y": 5.0, "type": pynlm.WorldObjectType.Hazard, "value": -0.5}
])

# Get complete sensory percept as dictionary
world = pynlm.createSimpleWorld()
percept = pynlm.getCompletePercept(world)
print(f"Vision width: {percept['vision_width']}")
print(f"Vision height: {percept['vision_height']}")

# Apply action with position validation
world = pynlm.createSimpleWorld()
action = pynlm.Action(pynlm.ActionType.MoveForward, [1.0, 1.0])
pynlm.applyActionSafely(world, action, 0.0)
```

### Configuration Convenience

```python
# Advanced configuration loading with defaults
config = pynlm.loadConfigWithDefaults(
    "config.json",
    {"neuron_count": 500, "connection_probability": 0.05}
)

# Run multiple episodes with statistics
brain, agent, world = pynlm.setupQuickExperiment()
results = pynlm.runEpisodes(brain, agent, world, 5, 500, collect_stats=True)
for stats in results:
    print(f"Episode {stats['episode']}: {stats['final_neurons_firing']} neurons firing")
```

## 2. Custom Exception Hierarchy

The NLM Python API now provides a comprehensive exception hierarchy for better error handling:

### Exception Classes

```python
import pynlm

# Custom exception types
try:
    # This will raise ConfigurationError if config is invalid
    brain = pynlm.createBrain(config)
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.SensoryError as e:
    print(f"Sensory error: {e}")
except pynlm.NLMError as e:
    print(f"NLM error: {e}")
```

### Error Handling Best Practices

```python
def safe_simulation(neuron_count, steps):
    try:
        # Initialize simulation
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(steps):
            # Your simulation logic here
            brain.step(step)
            
        return brain
        
    except pynlm.ConfigurationError as e:
        print(f"Invalid configuration: {e}")
        return None
    except pynlm.SimulationError as e:
        print(f"Simulation failed: {e}")
        return None
    except pynlm.SensoryError as e:
        print(f"Sensory processing error: {e}")
        return None
    except pynlm.ActionError as e:
        print(f"Action execution error: {e}")
        return None
    except pynlm.MemoryError as e:
        print(f"Memory allocation error: {e}")
        return None
    except pynlm.DevelopmentError as e:
        print(f"Development error: {e}")
        return None
    except pynlm.NLMError as e:
        print(f"General NLM error: {e}")
        return None
```

## 3. Performance Monitoring

Built-in performance monitoring and benchmarking:

```python
# Quick benchmark
result = pynlm.quickBenchmark(1000)
print(f"Steps/sec: {result['steps_per_second']:.1f}")
print(f"Average firing rate: {result['average_firing_rate']:.3f}")

# Full benchmark
results = pynlm.runBenchmarks()
for result in results:
    print(f"{result.neurons} neurons: {result.stepsPerSecond:.0f} steps/sec")

# Performance monitoring class
monitor = pynlm.PerformanceMonitor()
monitor.start()

# Your simulation code
import time
time.sleep(0.1)  # Simulate work

monitor.record_event("simulation_step")
monitor.stop()

# Get statistics
stats = monitor.get_stats()
for event_name, duration in stats:
    print(f"{event_name}: {duration:.3f}s")
```

# Command Line Interface

## Overview

The NLM command-line interface (CLI) provides comprehensive control over NLM simulations:

```bash
Usage: nlm_cli.py <command> [options]

Commands:
  simulate      Run a single simulation episode
  benchmark     Run performance benchmarks
  episodes      Run multiple episodes
  config        Configuration management
  help          Show help information
```

## Simulate Command

Run a complete simulation episode:

```bash
# Basic simulation
./nlm_cli.py simulate --neurons 1000 --steps 1000

# With neuromodulation enabled
./nlm_cli.py simulate --neurons 500 --steps 500 \
  --enable-reward-modulation --enable-curiosity

# With verbose output
./nlm_cli.py simulate --neurons 1000 --steps 1000 --verbose
```

## Benchmark Command

Run performance benchmarks:

```bash
# Quick benchmark
./nlm_cli.py benchmark --neurons 500

# Full benchmark
./nlm_cli.py benchmark --type full
```

## Episodes Command

Run multiple episodes:

```bash
# Run 5 episodes
./nlm_cli.py episodes --episodes 5 --neurons 200 --steps 500

# With statistics collection
./nlm_cli.py episodes --episodes 3 --neurons 300 --steps 1000 --collect-stats
```

## Config Command

Manage configuration files:

```bash
# Load configuration
./nlm_cli.py config load config.json

# Save configuration
./nlm_cli.py config save my_config.json

# Print current configuration
./nlm_cli.py config print
```

# Advanced Configuration Management

## Overview

The advanced configuration system provides:

- **Schema Validation**: Validate configurations against defined schemas
- **Configuration Templates**: Common setups and presets
- **Version Control**: Configuration history and versioning
- **Import/Export**: JSON serialization and versioning
- **Preset Management**: Save and load common configurations

## Basic Usage

```python
import advanced_config as ac

# Create advanced configuration
config = ac.create_default_config()

# Load from file
config.load("my_config.json")

# Set values
config.set("neuron_count", 1000)
config.set("connection_probability", 0.1)

# Validate
errors = config.validate()
if errors:
    print("Validation errors:", errors)
else:
    print("Configuration is valid")

# Save to file
config.save("my_config.json")
```

## Schema Validation

```python
# Configuration schema definition
schema = ac.ConfigSchema("NLM Configuration")
schema.add_field("neuron_count", int, True,
                "Total number of neurons", [100, 500, 1000, 2000])
schema.add_field("connection_probability", float, False,
                "Connection probability", [0.01, 0.05, 0.1, 0.2])

# Create config with schema
config = ac.ConfigManager(schema)
```

## Configuration Templates

```python
# Create configuration template
template = ac.ConfigTemplate("Quick Simulation",
                           "A fast simulation setup for testing")
template.add_config({
    "neuron_count": 200,
    "connection_probability": 0.05,
    "plasticity_stdp_enable": True
})

# Apply template to configuration
config = ac.ConfigManager()
final_config = template.merge_with(config.get_all())
```

## Preset Management

```python
# Save current configuration as preset
config.save_preset("quick_simulation",
                  "Quick simulation with 500 neurons",
                  tags=["quick", "simulation", "fast"])

# Load preset
config.create_from_preset("quick_simulation")

# List all presets
preset_manager = ac.PresetManager()
for name in preset_manager.get_preset_names():
    preset = preset_manager.load_preset(name)
    print(f"{name}: {preset.description}")
```

## Configuration History

```python
# Get configuration history
history = config.get_history()
for entry in history:
    print(f"Time: {time.ctime(entry['timestamp'])}")
    print(f"Changes: {entry['changes']}")

# Revert to previous version
config.revert_to_version(history_entry["timestamp"])
```

# Performance Monitoring & Profiling

## Performance Monitoring

The performance monitoring system provides real-time tracking of simulation performance:

```python
# Create performance monitor
monitor = pynlm.PerformanceMonitor()

# Start monitoring
monitor.start()

# Your simulation code
for step in range(1000):
    brain.step(step)
    monitor.record_event("simulation_step")

# Stop monitoring
monitor.stop()

# Get statistics
stats = monitor.get_stats()
for event_name, duration in stats:
    print(f"{event_name}: {duration:.3f}s")
```

## Benchmarking

```python
# Quick benchmark with specific neuron count
result = pynlm.quickBenchmark(1000)
print(f"Performance: {result['steps_per_second']:.1f} steps/sec")
print(f"Average firing rate: {result['average_firing_rate']:.3f}")

# Detailed benchmark results
results = pynlm.runBenchmarks()
print("Benchmark Results:")
for result in results:
    print(f"  {result.neurons} neurons: {result.stepsPerSecond:.0f} steps/sec, "
          f"{result.spikesPerStep:.2f} spikes/step, "
          f"{result.synapsesPerNeuron:.1f} synapses/neuron")
```

## Performance Metrics

The benchmark system measures:

- **Steps per second**: Simulation speed
- **Spikes per step**: Neural activity level
- **Synapses per neuron**: Network connectivity
- **Total spikes**: Overall neural activity
- **Benchmark time**: Total execution time

# Error Handling & Exceptions

## Custom Exception Hierarchy

```python
# All NLM exceptions inherit from NLMError
class NLMError(Exception):
    """Base class for all NLM errors"""
    pass

class ConfigurationError(NLMError):
    """Configuration-related errors"""
    pass

class SimulationError(NLMError):
    """Simulation-related errors"""
    pass

class SensoryError(NLMError):
    """Sensory input errors"""
    pass

class ActionError(NLMError):
    """Action execution errors"""
    pass

class MemoryError(NLMError):
    """Memory allocation errors"""
    pass

class DevelopmentError(NLMError):
    """Development-related errors"""
    pass
```

## Error Handling Best Practices

```python
def robust_simulation(neuron_count, steps):
    try:
        # Initialize simulation
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(steps):
            # Your simulation logic here
            brain.step(step)
            
        return brain, "Simulation completed successfully"
        
    except pynlm.ConfigurationError as e:
        return None, f"Invalid configuration: {e}"
    
    except pynlm.SimulationError as e:
        return None, f"Simulation failed: {e}"
    
    except pynlm.SensoryError as e:
        return None, f"Sensory processing error: {e}"
    
    except pynlm.ActionError as e:
        return None, f"Action execution error: {e}"
    
    except pynlm.MemoryError as e:
        return None, f"Memory allocation error: {e}"
    
    except pynlm.DevelopmentError as e:
        return None, f"Development error: {e}"
    
    except pynlm.NLMError as e:
        return None, f"NLM error: {e}"
    
    except Exception as e:
        return None, f"Unexpected error: {e}"
```

# Advanced Usage Examples

## Complex Simulation Setup

```python
def advanced_simulation(neuron_count=1000, enable_learning=True):
    """Advanced simulation with multiple features enabled"""
    
    # Create and configure brain
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", neuron_count)
    config.set("region_count", 2)
    config.set("connection_probability", 0.1)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent environment
    brain, agent, world = pynlm.createAgentEnvironment()
    
    # Configure with advanced settings
    agent.enableRewardModulation(enable_learning)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(enable_learning)
    
    # Create world and configure
    world.configure(30, 30, 12, 12)
    world.setMaxEnergy(200.0)
    world.setEnergyDecayRate(0.005)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # Add world objects
    resource = pynlm.createWorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 1.0)
    hazard = pynlm.createWorldObject(20.0, 20.0, pynlm.WorldObjectType.Hazard, -0.5)
    world.addObject(resource)
    world.addObject(hazard)
    
    # Run simulation with statistics collection
    for step in range(2000):
        # Update world
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Get and apply action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply neuromodulation
        if enable_learning:
            reward = 0.0
            internal = percept.getInternal()
            if internal and len(internal) > 0:
                reward = internal[0]
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Log statistics periodically
        if step % 200 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Return final statistics
    final_stats = {
        "final_neurons_firing": brain.getFiringNeuronCount(),
        "total_spikes": brain.getTotalSpikeCount(),
        "average_firing_rate": brain.getAverageFiringRate(),
        "curiosity_level": agent.getCuriosityLevel(),
        "novelty_level": agent.getNoveltyLevel(),
        "prediction_error": agent.getPredictionError()
    }
    
    return brain, agent, world, final_stats
```

## Batch Processing

```python
def batch_processing(neuron_configs, episodes_per_config):
    """Process multiple configurations in batch"""
    
    results = []
    
    for neurons in neuron_configs:
        print(f"Processing configuration with {neurons} neurons...")
        
        # Create brain for this configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", neurons)
        config.set("region_count", 1)
        config.set("connection_probability", 0.05)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent environment
        brain, agent, world = pynlm.createAgentEnvironment()
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run multiple episodes
        episode_results = []
        for episode in range(episodes_per_config):
            # Reset for new episode
            brain.reset()
            agent.reset()
            world.reset()
            
            # Run episode
            for step in range(500):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                reward = 0.0
                internal = percept.getInternal()
                if internal and len(internal) > 0:
                    reward = internal[0]
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
            
            # Collect episode statistics
            episode_stats = {
                "neurons": neurons,
                "episode": episode,
                "final_neurons_firing": brain.getFiringNeuronCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "curiosity_level": agent.getCuriosityLevel(),
                "novelty_level": agent.getNoveltyLevel()
            }
            episode_results.append(episode_stats)
        
        results.extend(episode_results)
    
    return results
```

## Configuration Comparison

```python
# Compare two configurations
config1 = {
    "neuron_count": 500,
    "connection_probability": 0.1,
    "plasticity_stdp_enable": True
}

config2 = {
    "neuron_count": 1000,
    "connection_probability": 0.05,
    "plasticity_hebbian_enable": True
}

# Use the configuration comparison functionality
from advanced_config import ConfigManager

manager1 = ConfigManager()
manager1._load_config(config1)

manager2 = ConfigManager()
manager2._load_config(config2)

diff = manager1.compare(manager2.get_all())
print("Differences between configurations:")
print("  Added:", diff["added"])
print("  Removed:", diff["removed"])
print("  Modified:", diff["modified"])
print("  Unchanged:", diff["unchanged"])
```

# Migration Guide

## From Basic to Enhanced API

### Changes Required

1. **Import Updates**: Update import statements to use enhanced APIs
2. **Configuration Loading**: Use new convenience functions
3. **Error Handling**: Implement custom exception handling
4. **Performance Monitoring**: Add performance tracking

### Migration Examples

#### Before (Basic API)
```python
import pynlm

# Basic usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)
```

#### After (Enhanced API)
```python
import pynlm

# Enhanced usage with error handling
try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    for step in range(100):
        brain.step(step)
        
    print("Simulation completed successfully")
    
except pynlm.NLMError as e:
    print(f"Simulation failed: {e}")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
```

#### Before (Basic Configuration)
```python
# Basic configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("connection_probability", 0.1)

brain = pynlm.createBrain(config)
```

#### After (Advanced Configuration)
```python
import advanced_config as ac

# Advanced configuration
config = ac.create_default_config()
config.set("neuron_count", 1000)
config.set("connection_probability", 0.1)

# Validate configuration
errors = config.validate()
if errors:
    print("Configuration errors:", errors)
else:
    # Convert to pynlm config
    pynlm_config = pynlm.createDefaultConfig()
    for key, value in config.get_all().items():
        pynlm_config.set(key, value)
    
    brain = pynlm.createBrain(pynlm_config)
```

# Troubleshooting

## Common Issues and Solutions

### Issue 1: pynlm module not found

**Symptom**: `ImportError: No module named 'pynlm'`

**Solution**:
```bash
# Install NLM Python bindings
pip install .

# Or install from source
pip install -e .
```

### Issue 2: Configuration validation errors

**Symptom**: Configuration validation fails with error messages

**Solution**:
```python
# Check validation errors
errors = config.validate()
if errors:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")
    
    # Fix common issues
    config.set("neuron_count", 500)  # Valid value
    config.set("connection_probability", 0.05)  # Valid value
```

### Issue 3: Simulation performance issues

**Symptom**: Slow simulation performance

**Solution**:
```python
# Enable performance monitoring
monitor = pynlm.PerformanceMonitor()
monitor.start()

# Your simulation code here
for step in range(1000):
    brain.step(step)

monitor.stop()

# Analyze performance
stats = monitor.get_stats()
for event_name, duration in stats:
    print(f"{event_name}: {duration:.3f}s")
```

### Issue 4: Memory issues

**Symptom**: High memory usage or memory allocation failures

**Solution**:
```python
# Monitor memory usage
memory_usage = pynlm.getMemoryUsage()
print(f"Current memory usage: {memory_usage:.2f} MB")

# Use smaller network configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)  # Smaller network
```

### Issue 5: Configuration file not found

**Symptom**: Cannot load configuration file

**Solution**:
```python
# Check if file exists
import os
if os.path.exists("config.json"):
    config = pynlm.loadConfigWithDefaults("config.json")
else:
    print("Configuration file not found")
    config = pynlm.createDefaultConfig()
```

## Debug Information

### Enable Verbose Output

```python
# Enable verbose output in CLI
./nlm_cli.py simulate --verbose

# Enable verbose output in Python
config = pynlm.createDefaultConfig()
config.set("log_level", "debug")
```

### Check Configuration

```python
# Print current configuration
config = pynlm.createDefaultConfig()
print(config.summary())
```

### List Available Features

```python
# List all available Python features
print("Available pynlm features:")
print("- createBrain()")
print("- createDefaultConfig()")
print("- createAgentBrain()")
print("- createSimpleWorld()")
print("- Performance monitoring")
print("- Custom exceptions")
print("- Convenience methods")
```

# Future Enhancements

## Planned Improvements

### 1. Enhanced Event System
- Add real-time event callbacks
- Implement event filtering and subscription
- Add event persistence and replay

### 2. Advanced Visualization
- Integration with plotting libraries (Matplotlib, Plotly)
- Real-time visualization of neural activity
- Network visualization tools

### 3. Machine Learning Integration
- Integration with scikit-learn for analysis
- Feature extraction and analysis tools
- Pattern recognition capabilities

### 4. Cloud and Distributed Computing
- Support for distributed simulations
- Cloud deployment capabilities
- Large-scale network support

### 5. Web Interface
- Web-based simulation control
- Real-time visualization dashboard
- Collaboration features

## Backward Compatibility

The enhanced NLM Python API maintains full backward compatibility:

- All existing functions continue to work unchanged
- New functions are additive only
- Exception hierarchy is backward compatible
- Configuration system maintains existing API

## Migration Timeline

- **Version 0.1.1**: Initial enhancement release
- **Version 0.2.0**: Advanced configuration management
- **Version 0.3.0**: Event system and visualization
- **Version 0.4.0**: Machine learning integration
- **Version 0.5.0**: Cloud and web interface

## Support

For support with the enhanced NLM features:

1. **Documentation**: Check this documentation for examples and API reference
2. **GitHub Issues**: Report bugs and feature requests
3. **Community**: Join the NLM community for discussions
4. **Contact**: Contact the development team for enterprise support

---

*Last updated: $(date)
Version: 0.1.0*