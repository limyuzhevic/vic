# NLM Documentation and Examples

## Overview
This document provides comprehensive documentation for the NLM (Neural Learning Machine) framework, including usage examples, best practices, and detailed API references.

## Table of Contents

1. [Getting Started](#1-getting-started)
2. [Python API Documentation](#2-python-api-documentation)
3. [Command-Line Interface Usage](#3-command-line-interface-usage)
4. [Advanced Usage Examples](#4-advanced-usage-examples)
5. [Best Practices](#5-best-practices)
6. [Troubleshooting](#6-troubleshooting)
7. [Migration Guide](#7-migration-guide)
8. [API Reference](#8-api-reference)

## 1. Getting Started

### 1.1 Installation

The NLM framework is installed as a Python package named `pynlm`. If you haven't installed it yet:

```bash
# Install from pip (if available)
pip install pynlm

# Or install from source
pip install -e .
```

### 1.2 Basic Usage

The simplest way to get started with NLM is to create a basic brain and run a simple simulation:

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain with the configuration
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run a few simulation steps
for i in range(10):
    brain.step(i)

# Print statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

### 1.3 Running Simulations

For more complex simulations with world and agent components:

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation completed!")
print(f"Final firing rate: {brain.getAverageFiringRate()}")
```

## 2. Python API Documentation

### 2.1 Core Classes

#### Brain
The main neural simulation engine.

**Constructor:**
```python
brain = pynlm.createBrain(config)
```

**Key Methods:**
- `initialize()` - Initialize the brain with configuration
- `step(step, time)` - Perform a simulation step
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file

**Key Properties:**
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getAverageFiringRate()` - Get average firing rate
- `getTotalSpikeCount()` - Get total spike count

#### AgentBrain
Interface between NLM brain and simulation world.

**Constructor:**
```python
agent = pynlm.createAgentBrain(brain)
```

**Key Methods:**
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode brain activity into motor command
- `applyRewardModulation(reward, predicted_reward)` - Apply reward-based learning

#### SimpleWorld
2D simulation world for NLM agents.

**Constructor:**
```python
world = pynlm.createSimpleWorld()
```

**Key Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `reset()` - Reset world to initial state
- `update(timestep)` - Update world by timestep
- `applyMotorCommand(cmd, time)` - Apply motor command

### 2.2 Context Managers

The enhanced NLM API provides context managers for automatic resource management:

```python
import pynlm

# Create instances
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Use context managers
with brain.create_context_manager() as ctx:
    world.configure(20, 20, 8, 8)
    world.reset()
    
    with agent.create_context_manager(world) as agent_ctx:
        for step in range(50):
            world.update(0.1)
            agent_ctx.step(step, step * 0.1)
            
            # Get statistics
            stats = ctx.getStatistics()
            print(f"Step {step}: {stats['firing_neurons']} neurons firing")
```

### 2.3 Advanced Python Features

#### Type Conversions
The API supports automatic type conversions for convenience:

```python
# Type conversions in context manager
brain = pynlm.createBrain(pynlm.createDefaultConfig())
with brain.create_context_manager() as ctx:
    # Get statistics as dictionary
    stats = ctx.getStatistics()
    print(f"Neurons: {stats['total_neurons']}")
    print(f"Firing neurons: {stats['firing_neurons']}")
    
    # Convert vision data to array
    percept = world.getSensoryPercept()
    vision_data = percept.getVision()
    if vision_data:
        # Access as Python list
        data_array = vision_data.getData()
        print(f"Vision data shape: {len(data_array)}")
```

#### Error Handling
```python
import pynlm

try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    with brain.create_context_manager() as ctx:
        # This will raise a BrainError if something goes wrong
        ctx.initialize()
        ctx.step(0)
except pynlm.BrainError as e:
    print(f"Brain error: {e}")
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## 3. Command-Line Interface Usage

### 3.1 Basic Commands

#### Running a Simulation
```bash
# Basic simulation
nlm run config.json --steps 1000 --output results.json

# Simulation with learning enabled
nlm run config.json --steps 5000 --enable-learning --output learning_results.json

# Simulation with world interaction
nlm run config.json --steps 2000 --world-width 30 --world-height 30 --output world_results.json
```

#### Getting Brain Status
```bash
# Get basic status
nlm status --brain-id 0

# Get detailed status in JSON format
nlm status --brain-id 0 --format json --detailed

# Get statistics only
nlm status --brain-id 0 --stats
```

#### Analyzing Neural Activity
```bash
# Analyze connectivity
nlm analyze connectivity --brain-id 0 --output connectivity.json

# Analyze learning process
nlm analyze learning --brain-id 0 --window-size 100 --output learning_analysis.json

# Analyze activity patterns
nlm analyze activity --brain-id 0 --time-range 0 1000 --output activity_patterns.json
```

### 3.2 Configuration Management

#### Setting Configuration
```bash
# Set individual values
nlm config --set neuron_count 1000
nlm config --set connection_probability 0.2
nlm config --set development_stage 1

# List all configuration keys
nlm config --list

# Get specific configuration value
nlm config --get neuron_count

# Validate configuration
nlm config --validate
```

#### Working with Configuration Files
```bash
# Save current configuration
nlm config --file current_config.json --copy my_config.json

# Load configuration
nlm config --file my_config.json --set neuron_count 500

# Merge configuration
nlm config --file base.json --merge overrides.json
```

### 3.3 Model Management

#### Saving and Loading Models
```bash
# Save brain model
nlm save model.nld --format hdf5 --compress --include-synapses

# Load brain model
nlm load model.nld --format hdf5 --verify

# Export to different formats
nlm export model.json model.csv --format csv --include-neurons

# Import from different formats
nlm import model.csv --format csv --into-brain-id 0 --validate
```

### 3.4 Performance Analysis
```bash
# Performance benchmarking
nlm benchmark --brain-id 0 --steps 1000 --repetitions 3 --output benchmark.json

# Performance profiling
nlm profile --brain-id 0 --steps 500 --include-memory --output profile.json

# Generate statistics
nlm statistics --brain-id 0 --output stats.json --include-learning
```

## 4. Advanced Usage Examples

### 4.1 Advanced Simulation Control
```python
import pynlm
import time

# Create simulation with context managers
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("connection_probability", 0.1)

brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Real-time control example
class RealTimeController:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.is_paused = False
        self.step_count = 0
    
    def pause(self):
        self.is_paused = True
    
    def resume(self):
        self.is_paused = False
    
    def step(self):
        if not self.is_paused:
            self.world.update(0.1)
            
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            self.brain.step(self.step_count, self.step_count * 0.1)
            self.step_count += 1

controller = RealTimeController(brain, world, agent)

# Run with real-time control
with brain.create_context_manager() as brain_ctx:
    world.configure(20, 20, 8, 8)
    world.reset()
    
    with agent.create_context_manager(world) as agent_ctx:
        for i in range(100):
            if i % 10 == 0:
                controller.pause()
                time.sleep(0.1)  # Simulate pause
                controller.resume()
            
            agent_ctx.step(i, i * 0.1)
            
            # Get real-time statistics
            stats = brain_ctx.getStatistics()
            print(f"Step {i}: {stats['firing_neurons']} neurons firing")
```

### 4.2 Neural Analysis
```python
import pynlm
import json

# Create brain for analysis
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Run simulation with learning
with brain.create_context_manager() as ctx:
    brain.initialize()
    
    # Learning phase
    for step in range(1000):
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)
        brain.step(step, step * 0.001)
    
    # Analyze learning
    analysis_results = {
        'initial_spikes': brain.getTotalSpikeCount(),
        'final_spikes': brain.getTotalSpikeCount(),
        'learning_detected': False,
        'weight_changes': 0.0
    }
    
    print("Learning Analysis:")
    print(json.dumps(analysis_results, indent=2))

# Advanced connectivity analysis
with brain.create_context_manager() as ctx:
    # Compute connectivity matrix
    connectivity = ctx.get_connectivity_matrix()
    
    # Identify network communities
    communities = ctx.identify_communities(connectivity)
    
    print(f"Found {len(communities)} communities")
    for i, community in enumerate(communities):
        print(f"Community {i}: {len(community)} neurons")
```

### 4.3 Model Persistence
```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Run simulation
with brain.create_context_manager() as ctx:
    brain.initialize()
    for step in range(500):
        brain.step(step, step * 0.001)
    
    # Save model
    save_result = ctx.save_to_json("simulation_model.json")
    print(f"Model saved: {save_result}")

# Load model
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
load_result = ctx.load_from_json("simulation_model.json")
print(f"Model loaded: {load_result}")

# Verify loaded model
print(f"Loaded neurons: {brain2.getTotalNeuronCount()}")
print(f"Loaded synapses: {brain2.getTotalSynapseCount()}")
```

### 4.4 Performance Monitoring
```python
import pynlm
import time

# Create brain for performance testing
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Performance monitoring
class PerformanceMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.timings = []
        self.memory_usage = []
    
    def start_monitoring(self):
        self.start_time = time.time()
        self.timings = []
        self.memory_usage = []
    
    def step(self, step_number, timestep):
        step_start = time.time()
        
        self.brain.step(step_number, timestep)
        
        step_end = time.time()
        self.timings.append(step_end - step_start)
        
        # Memory monitoring (simplified)
        import psutil
        process = psutil.Process()
        self.memory_usage.append(process.memory_info().rss / 1024 / 1024)
    
    def stop_monitoring(self):
        end_time = time.time()
        
        results = {
            'total_time': end_time - self.start_time,
            'average_step_time': sum(self.timings) / len(self.timings),
            'steps_per_second': len(self.timings) / (end_time - self.start_time),
            'peak_memory': max(self.memory_usage),
            'average_memory': sum(self.memory_usage) / len(self.memory_usage),
            'memory_efficiency': len(self.timings) / max(self.memory_usage) if max(self.memory_usage) > 0 else 0
        }
        
        return results

monitor = PerformanceMonitor(brain)

with brain.create_context_manager() as ctx:
    brain.initialize()
    monitor.start_monitoring()
    
    for step in range(100):
        brain.step(step, step * 0.001)
        monitor.step(step, step * 0.001)
    
    performance_results = monitor.stop_monitoring()
    print("Performance Results:")
    print(f"Steps per second: {performance_results['steps_per_second']:.2f}")
    print(f"Average step time: {performance_results['average_step_time']*1000:.2f}ms")
    print(f"Memory efficiency: {performance_results['memory_efficiency']:.2f} steps/MB")
```

## 5. Best Practices

### 5.1 Resource Management
```python
import pynlm

# Best practice: Use context managers for resource management
def simulate_with_context(config, steps):
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Use context managers for automatic cleanup
    with brain.create_context_manager() as brain_ctx:
        world.configure(20, 20, 8, 8)
        world.reset()
        
        with agent.create_context_manager(world) as agent_ctx:
            for step in range(steps):
                agent_ctx.step(step, step * 0.1)
                
                # Optional: Save progress periodically
                if step % 100 == 0:
                    brain.save(f"checkpoint_{step}.nld")
    
    # Resources automatically cleaned up
    return brain

# Use it
config = pynlm.createDefaultConfig()
simulation_result = simulate_with_context(config, 1000)
```

### 5.2 Error Handling
```python
import pynlm

# Best practice: Comprehensive error handling
def safe_simulation(config, steps):
    try:
        brain = pynlm.createBrain(config)
        
        with brain.create_context_manager() as ctx:
            if not brain.initialize():
                raise pynlm.BrainError("Failed to initialize brain")
            
            for step in range(steps):
                ctx.step(step, step * 0.1)
                
    except pynlm.ConfigError as e:
        print(f"Configuration error: {e}")
        return None
    except pynlm.BrainError as e:
        print(f"Brain error: {e}")
        return None
    except Exception as e:
        print(f"Unexpected error: {e}")
        return None
    
    return brain

# Use it with error handling
simulation_result = safe_simulation(pynlm.createDefaultConfig(), 100)
if simulation_result:
    print("Simulation completed successfully")
else:
    print("Simulation failed")
```

### 5.3 Configuration Management
```python
import pynlm

# Best practice: Configuration validation
def validate_and_fix_config(config):
    # Validate configuration
    validation_results = config.validate()
    
    if validation_results.get('valid', False):
        print("Configuration is valid")
        return config
    
    # Fix common issues
    fixes_applied = []
    
    if not config.has('neuron_count') or config.get('neuron_count') <= 0:
        config.set('neuron_count', 100)
        fixes_applied.append("Set default neuron_count: 100")
    
    if not config.has('region_count') or config.get('region_count') <= 0:
        config.set('region_count', 1)
        fixes_applied.append("Set default region_count: 1")
    
    if config.has('connection_probability'):
        prob = config.get('connection_probability')
        if prob < 0.0 or prob > 1.0:
            config.set('connection_probability', 0.1)
            fixes_applied.append("Set default connection_probability: 0.1")
    
    if fixes_applied:
        print(f"Applied fixes: {', '.join(fixes_applied)}")
    
    return config

# Use it
config = pynlm.createDefaultConfig()
config.set("invalid_value", "test")  # Add invalid config
fixed_config = validate_and_fix_config(config)
simulation = pynlm.createBrain(fixed_config)
```

### 5.4 Performance Optimization
```python
import pynlm
import time

# Best practice: Performance optimization
class OptimizedSimulation:
    def __init__(self, config):
        self.config = config
        self.brain = None
        self.performance_data = []
    
    def run_optimized(self, steps):
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        
        # Warm-up steps to stabilize simulation
        with self.brain.create_context_manager() as ctx:
            self.brain.initialize()
            
            # Warm-up
            for step in range(10):
                self.brain.step(step, step * 0.001)
            
            # Main simulation
            for step in range(steps):
                step_start = time.time()
                
                # Inject input if enabled
                if self.config.get('enable_learning', False):
                    self.brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)
                
                # Execute step
                ctx.step(step, step * 0.001)
                
                # Record performance
                step_time = time.time() - step_start
                self.performance_data.append(step_time)
                
                # Optional: Adaptive timestep based on performance
                if step % 100 == 0:
                    self._adjust_performance(step_time)
        
        return self._get_performance_summary()
    
    def _adjust_performance(self, step_time):
        # Adaptive performance adjustment
        if step_time > 0.1:  # If step takes too long
            # Reduce complexity
            pass
    
    def _get_performance_summary(self):
        if not self.performance_data:
            return None
        
        avg_step_time = sum(self.performance_data) / len(self.performance_data)
        steps_per_second = len(self.performance_data) / sum(self.performance_data)
        
        return {
            'total_steps': len(self.performance_data),
            'average_step_time': avg_step_time,
            'steps_per_second': steps_per_second,
            'performance_score': min(100, steps_per_second / 100)  # Normalize to 0-100
        }

# Use it
optimized_sim = OptimizedSimulation(pynlm.createDefaultConfig())
performance = optimized_sim.run_optimized(1000)
print(f"Performance: {performance}")
```

## 6. Troubleshooting

### 6.1 Common Issues

#### Issue 1: Brain Initialization Fails
**Symptoms:** Brain initialization returns False or throws exception

**Solutions:**
```python
# Check configuration
config = pynlm.createDefaultConfig()
print(f"Neuron count: {config.getOr('neuron_count', 0)}")
print(f"Region count: {config.getOr('region_count', 0)}")

# Ensure required configuration is set
if config.getOr('neuron_count', 0) <= 0:
    config.set('neuron_count', 100)

# Try initialization again
brain = pynlm.createBrain(config)
if not brain.initialize():
    print("Brain initialization failed - check configuration")
```

#### Issue 2: Simulation Crashes
**Symptoms:** Simulation crashes with segmentation fault or runtime error

**Solutions:**
```python
# Enable detailed error reporting
import logging
logging.basicConfig(level=logging.DEBUG)

# Use context managers for safe resource management
try:
    brain = pynlm.createBrain(config)
    with brain.create_context_manager() as ctx:
        ctx.initialize()
        for step in range(100):
            ctx.step(step, step * 0.001)
except Exception as e:
    print(f"Simulation error: {e}")
    import traceback
    traceback.print_exc()
```

### 6.2 Debugging Techniques

#### Debug Simulation Step
```python
import pynlm

# Enable tracing
brain = pynlm.createBrain(pynlm.createDefaultConfig())
with brain.create_context_manager() as ctx:
    ctx.initialize()
    
    # Run a few steps with tracing
    for step in range(5):
        print(f"\n=== Step {step} ===")
        print(f"Before step - Neurons: {brain.getTotalNeuronCount()}, Spikes: {brain.getTotalSpikeCount()}")
        
        ctx.step(step, step * 0.001)
        
        print(f"After step - Neurons: {brain.getTotalNeuronCount()}, Spikes: {brain.getTotalSpikeCount()}")
        print(f"Active neurons: {brain.getActiveNeuronCount()}")
        print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

#### Memory Monitoring
```python
import pynlm
import psutil
import os

# Monitor memory usage
def monitor_memory(brain, steps):
    process = psutil.Process(os.getpid())
    memory_samples = []
    
    with brain.create_context_manager() as ctx:
        brain.initialize()
        
        for step in range(steps):
            ctx.step(step, step * 0.001)
            
            # Record memory usage
            memory_mb = process.memory_info().rss / 1024 / 1024
            memory_samples.append(memory_mb)
            
            if step % 100 == 0:
                print(f"Step {step}: {memory_mb:.2f} MB")
    
    # Analyze memory usage
    if memory_samples:
        print(f"\nMemory Usage Summary:")
        print(f"Max memory: {max(memory_samples):.2f} MB")
        print(f"Min memory: {min(memory_samples):.2f} MB")
        print(f"Average memory: {sum(memory_samples) / len(memory_samples):.2f} MB")
        print(f"Memory efficiency: {steps / max(memory_samples):.2f} steps/MB")

monitor_memory(pynlm.createBrain(pynlm.createDefaultConfig()), 500)
```

## 7. Migration Guide

### 7.1 From Version 1.0 to 2.0

This guide helps migrate from NLM 1.0 to 2.0 with enhanced Python bindings.

#### API Changes

**1.0 to 2.0 Changes:**
- **Removed:** Direct access to private attributes
- **Added:** Context managers for resource management
- **Added:** Enhanced error handling with custom exceptions
- **Added:** Advanced analysis tools
- **Added:** Performance monitoring capabilities

#### Migration Examples

```python
# Version 1.0 code
brain = pynlm.createBrain(config)
brain.initialize()
for step in range(100):
    brain.step(step)
stats = brain.getStatistics()

# Version 2.0 equivalent
brain = pynlm.createBrain(config)
with brain.create_context_manager() as ctx:
    ctx.initialize()
    for step in range(100):
        ctx.step(step)
    stats = ctx.getStatistics()
```

### 7.2 Migration Checklist

- [ ] Review custom exceptions and error handling
- [ ] Replace direct attribute access with methods
- [ ] Add context managers for resource management
- [ ] Update error handling in existing code
- [ ] Test advanced features integration
- [ ] Update documentation and examples
- [ ] Performance testing after migration

## 8. API Reference

### 8.1 Core Classes

#### Config
```python
class Config:
    def __init__(self)
    def loadFromFile(filepath)
    def loadFromArgs(argc, argv)
    def saveToFile(filepath)
    def has(key)
    def get(key, default=None)
    def set(key, value, source=ConfigSource.Default)
    def remove(key)
    def getKeys()
    def clear()
    def summary()
    def validate()
```

#### Brain
```python
class Brain:
    def __init__(config)
    def initialize()
    def step(step, time)
    def reset()
    def save(filepath)
    def load(filepath)
    def getTotalNeuronCount()
    def getTotalSynapseCount()
    def getAverageFiringRate()
    def getTotalSpikeCount()
    def getActiveNeuronCount()
    def getFiringNeuronCount()
    def getExcitationInhibitionRatio()
    def getDevelopmentalStage()
    def create_context_manager()
```

#### AgentBrain
```python
class AgentBrain:
    def __init__(brain)
    def initialize(world)
    def processSensoryInput(percept)
    def decodeMotorCommand()
    def applyRewardModulation(reward, predicted_reward)
    def updateDevelopment(timestep)
    def getDevelopmentalStage()
    def getNeuromodulationLevel()
    def getCuriosityLevel()
    def getNoveltyLevel()
    def getPredictionError()
    def create_context_manager(world)
```

#### SimpleWorld
```python
class SimpleWorld:
    def __init__()
    def configure(width, height, visionWidth, visionHeight)
    def reset()
    def setAgentStart(x, y)
    def update(timestep)
    def applyMotorCommand(cmd, time)
    def getSensoryPercept()
    def getAgentBody()
    def addObject(obj)
    def removeObject(x, y)
    def isValidPosition(x, y)
```

### 8.2 Exception Classes

#### Custom Exceptions
```python
class NLMException(Exception)
class ConfigError(NLMException)
class BrainError(NLMException)
class WorldError(NLMException)
class AgentError(NLMException)
```

### 8.3 Utility Functions

#### Analysis Utilities
```python
# connectivity matrix analysis
connectivity = analysis.computeConnectivityMatrix(brain)

# activity pattern analysis
patterns = analysis.computeActivityPatterns(brain, start_step, end_step)

# performance profiling
profile = analysis.performanceProfile(brain, steps)

# learning analysis
learning_results = analysis.analyzeLearning(brain)
```

#### Serialization Utilities
```python
# JSON serialization
save_success = serialization.saveBrainToJSON(brain, "brain.json")
load_success = serialization.loadBrainFromJSON(brain, "brain.json")
```

### 8.4 Context Manager Classes

#### BrainContextManager
```python
class BrainContextManager:
    def __init__(brain)
    def initialize()
    def getBrain()
    def step(step, time)
    def getStatistics()
    def getStatus()
    def disableReset()
```

#### AgentBrainContextManager
```python
class AgentBrainContextManager:
    def __init__(agent, world)
    def initialize()
    def getAgent()
    def step(step, time)
    def getSimulationState()
```

#### SimpleWorldContextManager
```python
class SimpleWorldContextManager:
    def __init__(world)
    def configure(width, height, visionWidth, visionHeight)
    def getWorld()
    def reset()
```

## Conclusion

This documentation provides comprehensive guidance for using the NLM framework with the enhanced Python bindings and command-line interface. By following these best practices and examples, users can:

- **Get started quickly** with basic simulations
- **Build complex simulations** with advanced features
- **Analyze neural activity** with powerful analysis tools
- **Manage models efficiently** with persistence and configuration management
- **Optimize performance** with profiling and monitoring tools
- **Debug issues** with comprehensive error handling and tracing

The NLM framework is designed to be flexible and extensible, allowing users to customize their neural simulations for specific research needs while maintaining ease of use for beginners.

For additional support, please refer to the CLI documentation, examples directory, or contact the NLM development team.