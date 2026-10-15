# NLM Enhanced Python API - Comprehensive User Guide

## Overview

This document provides a comprehensive guide to the enhanced NLM (Neural Learning Machine) Python API. The improvements include:

- **Enhanced Python Bindings**: Better documentation, type hints, and Pythonic interface
- **Improved Configuration System**: Robust validation and error handling
- **Advanced Examples**: Complex learning scenarios with detailed explanations
- **Comprehensive Testing**: Extensive test suite covering all functionality
- **Better Error Handling**: Improved robustness and validation

## Table of Contents

1. [Quick Start](#quick-start)
2. [Enhanced Python API](#enhanced-python-api)
3. [Configuration Management](#configuration-management)
4. [Advanced Learning Examples](#advanced-learning-examples)
5. [Error Handling and Validation](#error-handling-and-validation)
6. [Performance Optimization](#performance-optimization)
7. [Testing and Validation](#testing-and-validation)
8. [Migration Guide](#migration-guide)
9. [Troubleshooting](#troubleshooting)

## Quick Start

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Advanced Agent with Learning

```python
import pynlm

# Create advanced agent with all learning systems
brain = create_advanced_agent_brain(1000)
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Enable advanced learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run learning simulation
results = run_advanced_learning_simulation(brain, agent, world, 500)
```

## Enhanced Python API

### Core Classes

#### Brain Class

The enhanced Brain class provides a Pythonic interface to neural simulation:

```python
# Basic operations
brain = pynlm.createBrain(config)
brain.initialize()

# Step simulation
brain.step(step_number)                    # Basic step
brain.step(step_number, timestamp)          # Step with time

# Neural operations
brain.receiveSensoryInput(percept)          # Inject sensory input
brain.injectCurrent(neuron_id, current)     # Inject to specific neuron
brain.produceAction()                       # Get motor action

# Statistics and analysis
total_neurons = brain.getTotalNeuronCount()
active_neurons = brain.getActiveNeuronCount()
spike_count = brain.getTotalSpikeCount()
avg_firing_rate = brain.getAverageFiringRate()
```

#### AgentBrain Class

Enhanced agent interface with learning systems:

```python
# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Learning system control
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Sensory and motor processing
agent.processSensoryInput(percept)
action = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
stage = agent.getDevelopmentalStage()
```

#### Config Class

Enhanced configuration management:

```python
# Type-specific configuration
config = pynlm.createDefaultConfig()
config.setInt("neuron_count", 1000)
config.setDouble("connection_probability", 0.1)
config.setBool("plasticity_enable", True)
config.set("custom_string", "value")

# Get values with type safety
neuron_count = config.get("neuron_count")
string_value = config.get("custom_string")
default_value = config.getOr("missing_key", "default")

# Configuration validation
is_valid, message = pynlm.validateConfiguration(config)

# File operations
config.saveToFile("brain.cfg")
config.loadFromFile("brain.cfg")
```

#### SimpleWorld Class

Enhanced world simulation:

```python
# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# World control
world.update(timestep)
percept = world.getSensoryPercept()
body = world.getAgentBody()

# Object management
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=10.0))
world.isValidPosition(x, y)
```

### Convenience Functions

```python
# Direct creation functions
brain = pynlm.createBrain()                    # Uses default config
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Benchmarking
neuron_count, synapse_count, duration = pynlm.benchmarkBrainCreation(500)

# Network creation
brain = pynlm.createRandomNetwork(200, 0.1)

# Validation
is_valid, errors = pynlm.validateConfiguration(config)
```

## Configuration Management

### Default Configuration

The enhanced configuration system includes:

```python
# Default configuration values
{
    "brain.neuron_count": 1000,
    "brain.synapse_density": 0.1,
    "brain.connection_probability": 0.05,
    "brain.v_thresh": -50.0,
    "brain.v_rest": -70.0,
    "plasticity.stdp.enable": true,
    "plasticity.hebbian.enable": true,
    "plasticity.structural.enable": true,
    "neuromod.dopamine.scale": 1.0,
    "neuromod.curiosity.enable": true,
    "neuromod.novelty.enable": true,
    "reward_discount_factor": 0.99
}
```

### Configuration Validation

The enhanced Config class includes validation:

```python
config = pynlm.createDefaultConfig()
config.setInt("neuron_count", 500)
config.setDouble("connection_probability", 0.05)

# Validate configuration
is_valid, message = pynlm.validateConfiguration(config)
assert is_valid == True

# File operations with error handling
try:
    config.saveToFile("brain.cfg")
    config.loadFromFile("brain.cfg")
except Exception as e:
    print(f"Configuration error: {e}")
```

### Advanced Configuration

```python
# Advanced neural configuration
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.3)
config.set("brain.initial_weight_mean", 0.7)
config.set("brain.initial_weight_std", 0.15)
config.set("brain.v_thresh", -55.0)
config.set("brain.tau_mem", 30.0)
config.set("brain.tau_ref", 3.0)

# Plasticity configuration
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Neuromodulation configuration
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
```

## Advanced Learning Examples

### Example 1: Curiosity-Driven Learning

```python
def curiosity_driven_learning():
    # Create agent with curiosity system
    brain = create_advanced_agent_brain(500)
    agent = pynlm.createAgentBrain(brain)
    world = create_learning_world()
    agent.initialize(world)
    
    # Enable curiosity system
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    
    # Run curiosity-driven exploration
    for step in range(1000):
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes information
        brain.step(step)
        
        # Get action (curiosity drives exploration)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = calculate_reward(percept, action)
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Monitor curiosity level
        if step % 100 == 0:
            curiosity = agent.getCuriosityLevel()
            print(f"Step {step}: Curiosity level = {curiosity:.3f}")
    
    return brain, agent, world
```

### Example 2: Developmental Learning

```python
def developmental_learning_simulation():
    # Create brain with developmental potential
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    config.set("development_synaptogenesis_rate", 0.002)
    config.set("development_pruning_rate", 0.0005)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    world = create_learning_world()
    agent.initialize(world)
    
    # Enable all developmental systems
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Track developmental stages
    stages = []
    for step in range(800):
        world.update(0.1)
        
        # Process experience
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record developmental stage
        current_stage = brain.getDevelopmentalStage()
        stages.append(current_stage)
        
        # Check for stage transitions
        if step % 100 == 0:
            print(f"Step {step}: Developmental stage = {current_stage}")
    
    return brain, agent, world, stages
```

### Example 3: Reward-Based Learning

```python
def reward_based_learning():
    # Create agent optimized for reinforcement learning
    brain = create_advanced_agent_brain(1000)
    agent = pynlm.createAgentBrain(brain)
    world = create_learning_world(enable_resources=True)
    agent.initialize(world)
    
    # Enable reward systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Learning episode
    episode_rewards = []
    prediction_errors = []
    
    for step in range(500):
        world.update(0.1)
        
        # Get current state
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Select action
        action = agent.decodeMotorCommand()
        
        # Execute action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward
        reward = calculate_step_reward(percept, action)
        prediction_error = calculate_prediction_error(reward)
        
        # Apply reinforcement learning
        agent.applyRewardModulation(reward, prediction_error)
        
        # Collect statistics
        episode_rewards.append(reward)
        prediction_errors.append(prediction_error)
        
        if step % 100 == 0:
            avg_reward = sum(episode_rewards) / len(episode_rewards)
            avg_prediction_error = sum(prediction_errors) / len(prediction_errors)
            print(f"Step {step}: Avg Reward = {avg_reward:.3f}, "
                  f"Avg Prediction Error = {avg_prediction_error:.3f}")
    
    return {
        'total_reward': sum(episode_rewards),
        'average_reward': sum(episode_rewards) / len(episode_rewards),
        'final_curiosity': agent.getCuriosityLevel(),
        'final_prediction_error': agent.getPredictionError()
    }
```

## Error Handling and Validation

### Configuration Validation

The enhanced Config class includes comprehensive validation:

```python
def validate_config_safety(config):
    """Validate configuration for safe operation."""
    errors = []
    
    # Validate neuron count
    neuron_count = config.getOr("brain.neuron_count", 1000)
    if neuron_count <= 0:
        errors.append("neuron_count must be positive")
    elif neuron_count > 10000:
        errors.append("neuron_count too large (max: 10000)")
    
    # Validate connection probability
    connection_prob = config.getOr("brain.connection_probability", 0.05)
    if connection_prob < 0.0 or connection_prob > 1.0:
        errors.append("connection_probability must be between 0.0 and 1.0")
    
    # Validate plasticity settings
    if config.getOr("plasticity.stdp.enable", True):
        stdp_rate = config.getOr("plasticity.stdp.learning_rate", 0.001)
        if stdp_rate <= 0.0:
            errors.append("STDP learning_rate must be positive")
    
    return len(errors) == 0, errors
```

### Runtime Error Handling

```python
def safe_brain_operation(brain, operation, *args, **kwargs):
    """Safe brain operation with error handling."""
    try:
        if operation == 'step':
            step = args[0]
            brain.step(step)
        elif operation == 'initialize':
            brain.initialize()
        elif operation == 'save':
            filepath = args[0]
            return brain.save(filepath)
        elif operation == 'load':
            filepath = args[0]
            return brain.load(filepath)
        else:
            raise ValueError(f"Unknown operation: {operation}")
        
        return True
    except Exception as e:
        print(f"Error in {operation}: {str(e)}")
        return False
```

### Graceful Degradation

```python
def robust_simulation(brain, world, agent, steps):
    """Robust simulation with graceful error handling."""
    successful_steps = 0
    failed_steps = 0
    
    for step in range(steps):
        try:
            # Update world
            world.update(0.1)
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            successful_steps += 1
            
        except Exception as e:
            print(f"Step {step} failed: {str(e)}")
            failed_steps += 1
            
            # Try to recover
            try:
                brain.reset()
                agent.reset()
                world.reset()
                successful_steps += 1  # Count recovery as success
            except:
                print(f"Failed to recover from error at step {step}")
                break
    
    return successful_steps, failed_steps
```

## Performance Optimization

### Brain Creation Optimization

```python
def optimized_brain_creation(neuron_count=1000, region_count=2):
    """Optimized brain creation using benchmarking."""
    
    # Use benchmarking API for optimal configuration
    neuron_count_opt, synapse_count, duration_ms = pynlm.benchmarkBrainCreation(
        neuron_count, region_count
    )
    
    # Create brain with validated configuration
    config = pynlm.createDefaultConfig()
    config.setInt("brain.neuron_count", neuron_count_opt)
    config.setInt("brain.region_count", region_count)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return brain, duration_ms

# Use optimized creation
brain, creation_time = optimized_brain_creation(500, 1)
print(f"Created brain in {creation_time:.2f} ms")
```

### Memory-Efficient Simulation

```python
import gc

def memory_efficient_simulation(brain, agent, world, steps, batch_size=100):
    """Memory-efficient simulation with garbage collection."""
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect garbage periodically
        if step % batch_size == 0:
            gc.collect()
            
            # Log memory usage
            import psutil
            process = psutil.Process()
            memory_mb = process.memory_info().rss / 1024 / 1024
            print(f"Step {step}: Memory usage: {memory_mb:.1f} MB")
```

### Parallel Processing

```python
def parallel_brain_simulation(brains, agent_configs, steps):
    """Parallel brain simulation for comparison."""
    import concurrent.futures
    import threading
    
    results = []
    
    def simulate_brain(brain, agent_config, step_range):
        """Simulate a single brain."""
        for step in step_range:
            brain.step(step)
        return brain.getTotalSpikeCount()
    
    # Create thread pool
    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
        # Submit simulation tasks
        futures = []
        steps_per_brain = steps // len(brains)
        
        for i, brain in enumerate(brains):
            step_range = range(i * steps_per_brain, (i + 1) * steps_per_brain)
            future = executor.submit(simulate_brain, brain, {}, step_range)
            futures.append(future)
        
        # Collect results
        for future in concurrent.futures.as_completed(futures):
            try:
                spike_count = future.result()
                results.append(spike_count)
            except Exception as e:
                print(f"Simulation failed: {str(e)}")
                results.append(None)
    
    return results
```

## Testing and Validation

### Comprehensive Test Suite

The test suite includes:

```bash
# Run all tests
python test_enhanced_pynlm.py

# Run specific test categories
python -m pytest tests/test_config_api.py -v
python -m pytest tests/test_advanced_examples.py -v
python -m pytest tests/test_error_handling.py -v
```

### Test Categories

1. **Basic Functionality Tests**
   - Brain creation and initialization
   - Sensory and motor processing
   - World simulation

2. **Advanced API Tests**
   - Configuration validation
   - Learning system integration
   - Performance benchmarking

3. **Error Handling Tests**
   - Invalid configuration
   - Runtime errors
   - Graceful degradation

4. **Integration Tests**
   - Complete learning simulations
   - Advanced agent behavior
   - Performance benchmarks

### Performance Testing

```python
def performance_benchmark():
    """Comprehensive performance benchmark."""
    import time
    import statistics
    
    # Benchmark brain creation
    creation_times = []
    for _ in range(10):
        start = time.time()
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        end = time.time()
        creation_times.append(end - start)
    
    avg_creation_time = statistics.mean(creation_times)
    print(f"Average brain creation time: {avg_creation_time:.3f} seconds")
    
    # Benchmark simulation
    sim_times = []
    for _ in range(5):
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        start = time.time()
        for step in range(100):
            brain.step(step)
        end = time.time()
        
        sim_times.append(end - start)
    
    avg_sim_time = statistics.mean(sim_times)
    print(f"Average simulation time (100 steps): {avg_sim_time:.3f} seconds")
```

## Migration Guide

### From Basic API to Enhanced API

#### Changes in Brain Class

**Before:**
```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step)
```

**After:**
```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step)  # Same API, enhanced functionality
```

#### Changes in Config Class

**Before:**
```python
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

**After:**
```python
config = pynlm.createDefaultConfig()
config.loadFromFile("config.cfg")
config.saveToFile("config.cfg")

# Enhanced type-specific setters
config.setInt("neuron_count", 1000)
config.setDouble("connection_probability", 0.1)
config.setBool("enable_learning", True)
```

#### New Features Added

1. **Enhanced Error Handling**: Better validation and exception handling
2. **Pythonic Interface**: More intuitive method names and return values
3. **Advanced Configuration**: Type-specific configuration methods
4. **Learning System Integration**: Better support for learning algorithms
5. **Performance Benchmarking**: Built-in performance testing tools
6. **Validation Tools**: Comprehensive configuration validation

### Migration Examples

```python
def migrate_basic_to_enhanced():
    """Migrate from basic to enhanced API."""
    
    # OLD: Basic API
    print("OLD API:")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.step(0)
    print(f"Neurons: {brain.getTotalNeuronCount()}")
    
    # NEW: Enhanced API
    print("\nNEW API:")
    config = pynlm.createDefaultConfig()
    config.setInt("neuron_count", 1000)
    config.setDouble("connection_probability", 0.1)
    config.setBool("enable_structural_plasticity", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enhanced stepping with error handling
    if brain.step(0):
        print(f"Simulation successful: {brain.getTotalSpikeCount()} spikes")
    
    # Enhanced configuration validation
    is_valid, message = pynlm.validateConfiguration(config)
    print(f"Configuration valid: {is_valid}")
    if not is_valid:
        print(f"Validation message: {message}")
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: Brain Initialization Fails

**Problem:** Brain fails to initialize with "invalid configuration" error.

**Solution:**
```python
# Check configuration
config = pynlm.createDefaultConfig()
config.setInt("neuron_count", 0)  # Invalid: zero neurons
config.setDouble("connection_probability", 1.5)  # Invalid: > 1.0

# Fix configuration
config.setInt("neuron_count", 1000)
config.setDouble("connection_probability", 0.1)

brain = pynlm.createBrain(config)
brain.initialize()  # Should now work
```

#### Issue 2: Memory Usage Too High

**Problem:** Simulation consumes excessive memory.

**Solution:**
```python
# Use memory-efficient simulation
import gc

def memory_efficient_simulation(brain, agent, world, steps):
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Force garbage collection
        if step % 100 == 0:
            gc.collect()
```

#### Issue 3: Learning System Not Working

**Problem:** Reward modulation or curiosity systems not responding.

**Solution:**
```python
# Check if systems are enabled
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Verify systems are enabled
print(f"Reward Modulation: {agent.isRewardModulationEnabled()}")
print(f"Curiosity: {agent.isCuriosityEnabled()}")

# Apply reward and check response
agent.applyRewardModulation(1.0, 0.5)
neuromod_level = agent.getNeuromodulationLevel()
print(f"Neuromodulation level: {neuromod_level}")
```

#### Issue 4: Configuration File Not Loading

**Problem:** Configuration file loading fails.

**Solution:**
```python
# Check file exists and is readable
import os
config_file = "brain.cfg"

if os.path.exists(config_file):
    config = pynlm.createDefaultConfig()
    success = config.loadFromFile(config_file)
    if success:
        print("Configuration loaded successfully")
    else:
        print("Failed to load configuration")
else:
    print(f"Configuration file not found: {config_file}")
```

### Error Messages and Solutions

| Error Message | Cause | Solution |
|---------------|-------|----------|
| "Configuration key cannot be empty" | Empty configuration key | Use valid key like "brain.neuron_count" |
| "Invalid character in configuration key" | Invalid characters in key | Use only alphanumeric, _, -, . |
| "Failed to load configuration" | File not found or corrupted | Check file path and permissions |
| "Brain initialization failed" | Invalid configuration | Validate configuration before initialization |
| "Step operation failed" | Invalid step number | Use non-negative integer for step |

### Getting Help

For additional support:

1. **Documentation**: See this guide for comprehensive API documentation
2. **Examples**: Run example scripts to see advanced usage
3. **Testing**: Use test suite to identify issues
4. **Community**: Join NLM community for support

## Conclusion

The enhanced NLM Python API provides:

- **Improved Developer Experience**: Better documentation and Pythonic interface
- **Enhanced Reliability**: Comprehensive error handling and validation
- **Advanced Features**: Learning systems, development, and curiosity
- **Performance Tools**: Benchmarking and optimization utilities
- **Robust Testing**: Comprehensive test suite covering all functionality

This guide provides everything needed to get started with NLM and build sophisticated neural simulation applications.

---

*Last updated: October 15, 2026*
*Version: 0.1.0*
*NLM Authors*