# NLM Python API - Enhanced Documentation

This document provides comprehensive documentation for the enhanced NLM Python API, including all new features, advanced usage patterns, and best practices.

## Overview

The NLM (Neural Learning Machine) Python API provides Python bindings for the C++ neural simulation framework. This enhanced version includes:

- **Advanced Brain Simulation**: Enhanced brain control with multiple simulation modes
- **Comprehensive Configuration**: Phase-based configuration presets and advanced settings
- **Intelligent Agent System**: Sophisticated agent-world interaction capabilities
- **Performance Monitoring**: Built-in metrics and performance tracking
- **Error Handling**: Robust exception handling with detailed error reporting
- **Learning System Monitoring**: Real-time tracking of learning mechanisms
- **Multi-Phase Support**: Pre-configured settings for all development phases

## Installation

```bash
# Install from source
cd /path/to/nlm
pip install .

# Or install in development mode
pip install -e .
```

## Quick Start Examples

### 1. Basic Brain Simulation

```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete. Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### 2. Phase-Specific Configuration

```python
import pynlm

# Create brain configured for Phase 6 (Final Integration)
brain = pynlm.createBrainFromPhase(6, scale=1)  # Use 2000 neurons
brain.initialize()

# Enable all learning systems
brain.enableAllPlasticity()
brain.enableAllNeuromodulation()

print("Phase 6 brain initialized with all systems enabled")
```

### 3. Complete Agent-World Simulation

```python
import pynlm
import time

def runCompleteEpisode():
    # Setup brain with Phase 4 configuration (emerging cognition)
    brain = pynlm.createBrainFromPhase(4, scale=1)
    brain.initialize()
    
    # Setup world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all adaptive systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run episode
    for step in range(500):
        world.update(0.1)
        
        # Get sensory input and process it
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action and apply it
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update development
        agent.updateDevelopment(0.1)
        
        if step % 100 == 0:
            print(f"Step {step}: Firing={brain.getFiringNeuronCount()}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}")
    
    return brain, agent, world

# Run the complete episode
brain, agent, world = runCompleteEpisode()
```

### 4. Performance Monitoring

```python
import pynlm
import time

def simulateWithMonitoring(duration, phase=6):
    brain = pynlm.createBrainFromPhase(phase)
    brain.initialize()
    
    start_time = time.time()
    step_count = 0
    
    while (time.time() - start_time) < duration:
        brain.step(step_count)
        step_count += 1
        
        if step_count % 100 == 0:
            metrics = brain.getPerformanceMetrics()
            print(f"Step {step_count}: "
                  f"Neurons={metrics['total_neurons']}, "
                  f"Firing={metrics['firing_neurons']}, "
                  f"Rate={metrics['average_firing_rate']:.3f}")
    
    elapsed = time.time() - start_time
    print(f"Simulation complete: {step_count} steps in {elapsed:.2f}s "
          f"({step_count/elapsed:.1f} steps/s)")
    
    return brain

# Run with monitoring
brain = simulateWithMonitoring(10.0, phase=4)  # 10 seconds, Phase 4
```

### 5. Learning System Analysis

```python
import pynlm

def analyzeLearningSystems(brain):
    """Analyze and report on all learning mechanisms"""
    print("=== Learning System Analysis ===")
    
    # Get comprehensive statistics
    learning_stats = brain.getLearningStats()
    memory_stats = brain.getMemoryStats()
    
    print(f"\nPlasticity Mechanisms:")
    if "stdp" in learning_stats:
        stdp = learning_stats["stdp"]
        print(f"  STDP: rate={stdp['learning_rate']:.4f}, "
              f"LTP={stdp['potentiation_strength']:.4f}, "
              f"LTD={stdp['depression_strength']:.4f}")
    
    if "hebbian" in learning_stats:
        hebbian = learning_stats["hebbian"]
        print(f"  Hebbian: rate={hebbian['learning_rate']:.4f}, "
              f"max_change={hebbian['weight_change_max']:.4f}")
    
    print(f"\nMemory Systems:")
    if "working_memory" in memory_stats:
        wm = memory_stats["working_memory"]
        print(f"  Working Memory: {wm['active_traces']}/{wm['capacity']} "
              f"traces ({wm['active_traces_percentage']:.1f}%)")
    
    if "episodic_memory" in memory_stats:
        em = memory_stats["episodic_memory"]
        print(f"  Episodic Memory: {em['episode_count']}/{em['max_capacity']} "
              f"episodes ({em['episode_percentage']:.1f}%)")
    
    return learning_stats, memory_stats

# Analyze learning systems
brain = pynlm.createBrainFromPhase(4)
brain.initialize()
# Run some learning steps first
for i in range(100):
    brain.step(i)

analysis = analyzeLearningSystems(brain)
```

### 6. Exception Handling

```python
import pynlm

def safeBrainOperation():
    """Demonstrate robust error handling"""
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # This should work fine
        for i in range(50):
            brain.step(i)
        
        print("Basic operation successful")
        
    except pynlm.InvalidArgumentError as e:
        print(f"Invalid argument: {e}")
    except pynlm.RuntimeError as e:
        print(f"Runtime error: {e}")
    except Exception as e:
        print(f"Unexpected error: {type(e).__name__}: {e}")
    
    # Try with invalid configuration
    try:
        bad_config = pynlm.createDefaultConfig()
        # This would trigger validation errors
        print("Testing configuration validation...")
        
    except Exception as e:
        print(f"Configuration validation caught: {e}")

safeBrainOperation()
```

### 7. Advanced Episode Control

```python
import pynlm

def runControlledEpisode(brain, world, agent, 
                         condition_fn, callback_fn):
    """Run episode with custom control logic"""
    step = 0
    while condition_fn(step):
        # Update world
        world.update(0.1)
        
        # Get sensory input and process it
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action and apply it
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # User callback for custom logic
        callback_fn(step)
        
        step += 1
    
    return step

def runUntilGoal(brain, world, agent, maxSteps=1000):
    """Run episode until goal is reached or max steps"""
    def goalReached(step):
        # Example goal: agent reaches edge of world
        x, y = world.getAgentBody().x, world.getAgentBody().y
        return step < maxSteps and x > 0 and x < world.getWidth() and y > 0 and y < world.getHeight()
    
    def logProgress(step):
        if step % 100 == 0:
            print(f"Progress: Step {step}, Position ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    return runControlledEpisode(brain, world, agent, goalReached, logProgress)

# Setup and run
brain = pynlm.createBrainFromPhase(3)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run with goal-based control
final_step = runUntilGoal(brain, world, agent, maxSteps=500)
print(f"Episode ended at step {final_step}")
```

## API Reference

### Core Classes

#### Brain Class

**Methods:**

- `__init__(config)` - Create brain with configuration
- `initialize()` - Initialize the brain with configuration  
- `step(currentStep[, currentTime])` - Perform simulation step
- `stepMultiple(steps, timestep=0.001)` - Perform multiple steps efficiently
- `receiveSensoryInput(input)` - Inject sensory input
- `injectCurrent(neuron, current)` - Inject current into specific neuron
- `injectCurrentToNeurons(type, current)` - Inject current to all neurons of type
- `injectCurrentToAll(current)` - Inject current to all neuron types
- `produceAction()` - Produce motor action
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file

**Properties:**

- `getTotalNeuronCount()` - Total neuron count
- `getTotalSynapseCount()` - Total synapse count
- `getActiveNeuronCount()` - Active neuron count
- `getFiringNeuronCount()` - Currently firing neurons
- `getAverageFiringRate()` - Average firing rate
- `getExcitationInhibitionRatio()` - E/I balance ratio
- `getTotalSpikeCount()` - Total spike count
- `getPendingSpikeEventCount()` - Pending spike events
- `getDevelopmentalStage()` - Current developmental stage
- `getPerformanceMetrics()` - Comprehensive performance metrics

#### Config Class

**Methods:**

- `__init__()` - Create default configuration
- `loadFromFile(filepath)` - Load from JSON file
- `loadFromArgs(argc, argv)` - Load from command line
- `saveToFile(filepath)` - Save to JSON file
- `has(key)` - Check if key exists
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all entries
- `summary()` - Get configuration summary
- `getInt(key, default)` - Get integer value
- `getFloat(key, default)` - Get float value
- `getString(key, default)` - Get string value
- `setInt(key, value)` - Set integer value
- `setFloat(key, value)` - Set float value
- `setString(key, value)` - Set string value
- `configureForPhase(phase, scale=1)` - Phase-based configuration
- `getPhaseSpecificConfig(phase)` - Get phase-specific config
- `validate()` - Validate configuration

#### AgentBrain Class

**Methods:**

- `__init__(brain)` - Create agent with brain
- `initialize(world)` - Initialize with world
- `getSensoryInputSize()` - Get expected sensory input size
- `getMotorOutputSize()` - Get expected motor output size
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predictedReward)` - Apply reward modulation
- `updateDevelopment(timestep)` - Update development
- `getDevelopmentalStage()` - Get developmental stage
- `getNeuromodulationLevel()` - Get neuromodulation level
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level
- `getPredictionError()` - Get prediction error
- `reset()` - Reset agent
- `enableRewardModulation(enable)` - Enable/disable reward modulation
- `enableStructuralPlasticity(enable)` - Enable/disable structural plasticity
- `enableDevelopment(enable)` - Enable/disable development
- `enableCuriosity(enable)` - Enable/disable curiosity

### Global Functions

#### Basic Creation

- `createDefaultConfig()` - Create default configuration
- `createBrain(config)` - Create brain with config
- `createBrainFromPhase(phase, scale=1)` - Create brain for specific phase
- `createSimpleWorld()` - Create simple world
- `createAgentBrain(brain)` - Create agent brain interface

#### Experiments

- `runPhase2Experiment(numSteps, scale=1)` - Run Phase 2 experiment
- `runPhase6IntegrationDemo()` - Run Phase 6 integration demo

#### System Information

- `getVersionInfo()` - Get version information
- `getSystemCapabilities()` - Get system capabilities

### Exception Classes

- `pynlm.RuntimeError` - General runtime errors
- `pynlm.InvalidArgumentError` - Invalid argument errors
- `pynlm.OutOfRangeError` - Index out of range errors
- `pynlm.LogicError` - Logic errors

## Best Practices

### 1. Configuration Management

```python
# Use phase-based configurations for consistency
brain = pynlm.createBrainFromPhase(4, scale=1)

# Always validate configurations
if not brain.getConfig().validate():
    raise ValueError("Invalid configuration")
```

### 2. Error Handling

```python
# Always handle potential exceptions
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.InvalidArgumentError:
    print("Check configuration parameters")
except pynlm.RuntimeError as e:
    print(f"Brain initialization failed: {e}")
```

### 3. Performance Monitoring

```python
# Monitor performance during long simulations
for step in range(total_steps):
    brain.step(step)
    
    if step % 100 == 0:
        metrics = brain.getPerformanceMetrics()
        print(f"Performance: {metrics['average_firing_rate']:.3f} Hz, "
              f"{metrics['firing_neurons']} neurons firing")
```

### 4. Memory Management

```python
# Be mindful of memory usage
if brain.getTotalNeuronCount() > 10000:
    print("Warning: Large brain simulation may use significant memory")
    
# Check memory system utilization
memory_stats = brain.getMemoryStats()
if "working_memory" in memory_stats:
    wm = memory_stats["working_memory"]
    if wm['active_traces_percentage'] > 90:
        print("Warning: Working memory near capacity")
```

## Advanced Features

### 1. Multi-Agent Systems

```python
# Create multiple agents sharing the same brain
brain = pynlm.createBrainFromPhase(3)
brain.initialize()

agent1 = pynlm.createAgentBrain(brain)
agent2 = pynlm.createAgentBrain(brain)

world1 = pynlm.createSimpleWorld()
world1.configure(width=20, height=20, visionWidth=8, visionHeight=8)

world2 = pynlm.createSimpleWorld()
world2.configure(width=20, height=20, visionWidth=8, visionHeight=8)

agent1.initialize(world1)
agent2.initialize(world2)

# Agents can share neural resources while having separate interfaces
```

### 2. Real-Time Monitoring

```python
import threading
import time

def monitorBrain(brain, interval=1.0):
    """Monitor brain metrics in real-time"""
    def monitor():
        while True:
            metrics = brain.getPerformanceMetrics()
            print(f"[{time.time():.1f}] "
                  f"Rate: {metrics['average_firing_rate']:.3f} Hz, "
                  f"Neurons: {metrics['total_neurons']}, "
                  f"Spikes: {metrics['total_spikes']}")
            time.sleep(interval)
    
    monitor_thread = threading.Thread(target=monitor, daemon=True)
    monitor_thread.start()
    return monitor_thread

# Start monitoring
monitor = monitorBrain(brain, interval=2.0)
```

### 3. Custom Learning Scenarios

```python
import pynlm

def createCustomLearningScenario(phase=4, custom_params=None):
    """Create a custom learning scenario with specific parameters"""
    brain = pynlm.createBrainFromPhase(phase)
    
    if custom_params:
        config = brain.getConfig()
        for key, value in custom_params.items():
            if isinstance(value, int):
                config.setInt(key, value)
            elif isinstance(value, float):
                config.setFloat(key, value)
            elif isinstance(value, str):
                config.setString(key, value)
    
    return brain

# Create custom scenario
custom_scenario = createCustomLearningScenario(
    phase=6,
    custom_params={
        "neuron_count": 3000,
        "connection_probability": 0.03,
        "enable_reward_modulation": True,
        "enable_curiosity": True,
        "enable_development": True
    }
)
```

## Migration Guide

### From Basic to Enhanced API

**Before (Basic):**

```python
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
for i in range(100):
    brain.step(i)
```

**After (Enhanced):**

```python
import pynlm

# Phase-based configuration (recommended)
brain = pynlm.createBrainFromPhase(4)
brain.initialize()

# Enhanced monitoring and control
for i in range(100):
    brain.step(i)
    if i % 10 == 0:
        metrics = brain.getPerformanceMetrics()
        print(f"Progress: {metrics['average_firing_rate']:.3f} Hz")
```

### Best Practices for Users

1. **Always use phase-based configurations** when creating brains
2. **Enable appropriate plasticity mechanisms** for your use case
3. **Monitor performance** during long simulations
4. **Handle exceptions** gracefully
5. **Validate configurations** before use
6. **Use the comprehensive API** for advanced features
7. **Consider memory usage** for large-scale simulations

## Troubleshooting

### Common Issues

**Issue: Brain initialization fails**

```python
# Check configuration
config = pynlm.createDefaultConfig()
print(config.summary())

# Ensure parameters are valid
if not config.validate():
    print("Configuration validation failed")
```

**Issue: Memory usage is high**

```python
brain = pynlm.createBrainFromPhase(6, scale=2)  # Try smaller scale
if brain.getTotalNeuronCount() > 5000:
    print("Consider using smaller scale or Phase 4")
```

**Issue: Performance is slow**

```python
# Check system capabilities
info = pynlm.getSystemCapabilities()
print(f"System supports up to: {info['phases_supported'][-1]} phases")

# Monitor performance
for i in range(100):
    brain.step(i)
    if i % 20 == 0:
        metrics = brain.getPerformanceMetrics()
        print(f"Step {i}: {metrics['total_spikes']} spikes")
```

## Examples Directory

For more examples, visit the examples directory in the repository:

- `examples/basic_usage.py` - Basic brain simulation
- `examples/agent_simulation.py` - Complete agent-world simulation
- `examples/learning_analysis.py` - Learning system analysis
- `examples/multi_phase_comparison.py` - Compare different phases
- `examples/performance_benchmarking.py` - Performance testing

## Future Enhancements

The NLM Python API is actively developed. Future enhancements may include:

- **Real-time visualization** with matplotlib integration
- **Advanced agent architectures** with multiple sensory modalities
- **Distributed simulation** support for large-scale systems
- **Neural network transfer learning** capabilities
- **Advanced optimization** for different hardware platforms
- **Extended documentation** with interactive examples
- **Community plugins** for additional functionality

## Support and Community

For support, visit the NLM project documentation or join the community forum. The enhanced Python API is part of the ongoing development of NLM Phase 6 (Final Integration).

---

*This documentation reflects the enhanced NLM Python API as of the current release. The API continues to evolve with new features and improvements.*