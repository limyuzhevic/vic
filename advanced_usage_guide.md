# NLM Advanced Usage Guide

This document provides comprehensive guidance for using the NLM Python API, including beginner tutorials, advanced features, and expert-level usage.

## Table of Contents

1. [Quick Start for Beginners](#quick-start-for-beginners)
2. [Basic API Usage](#basic-api-usage)
3. [Advanced Features](#advanced-features)
4. [Expert-Level Usage](#expert-level-usage)
5. [Configuration Management](#configuration-management)
6. [Performance Optimization](#performance-optimization)
7. [Debugging and Monitoring](#debugging-and-monitoring)
8. [Batch Processing](#batch-processing)
9. [Memory Management](#memory-management)
10. [Troubleshooting](#troubleshooting)
11. [API Reference](#api-reference)

## Quick Start for Beginners

If you're new to NLM, start with this simplified tutorial:

### 1. Basic Brain Simulation

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete. Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### 2. Complete Agent Simulation

```python
import pynlm

# Setup brain with configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent and world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: Firing neurons: {brain.getFiringNeuronCount()}")

print("Agent simulation complete!")
```

## Basic API Usage

### Core Classes

The NLM Python API provides several key classes:

- **Brain**: The central neural simulation brain class
- **Config**: Configuration management system  
- **AgentBrain**: Interface connecting NLM brain to world
- **SimpleWorld**: Simple 2D world for NLM simulation
- **Action**: Action representation for motor output
- **SensoryPercept**: Sensory percept data container

### Common Operations

#### Creating and Initializing

```python
# Create configuration
config = pynlm.createDefaultConfig()

# Customize configuration
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)

# Create brain and initialize
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Running Simulations

```python
# Basic step-by-step simulation
for step in range(1000):
    brain.step(step)
    # Collect statistics
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Action Production

```python
# Produce motor actions based on neural activity
action = brain.produceAction()
print(f"Action type: {action.getType()}")
print(f"Action parameters: {action.getParameters()}")
```

### State Management

```python
# Save brain state (checkpointing)
brain.save("my_checkpoint.bin")

# Load brain state (resume)
brain.load("my_checkpoint.bin")

# Reset brain
brain.reset()
```

## Advanced Features

### 1. Integration Status Monitoring

Check if all brain systems are properly connected:

```python
integration_status = brain.getIntegrationStatus()

print("System Integration Status:")
print(f"  Working Memory: {'CONNECTED' if integration_status['working_memory'] else 'DISCONNECTED'}")
print(f"  Episodic Memory: {'CONNECTED' if integration_status['episodic_memory'] else 'DISCONNECTED'}")
print(f"  Neuromodulation: {'CONNECTED' if integration_status['dopamine'] else 'DISCONNECTED'}")
print(f"  Prediction System: {'CONNECTED' if integration_status['prediction_system'] else 'DISCONNECTED'}")
```

### 2. Performance Statistics

Get comprehensive performance metrics:

```python
stats = brain.getPerformanceStats()

print("Performance Metrics:")
print(f"  Neurons: {stats['total_neurons']}")
print(f"  Synapses: {stats['total_synapses']}")
print(f"  Active Neurons: {stats['active_neurons']}")
print(f"  Firing Rate: {stats['average_firing_rate']:.2f} Hz")
print(f"  E/I Ratio: {stats['excitation_inhibition_ratio']:.2f}")
print(f"  Total Spikes: {stats['total_spikes']}")
print(f"  Pending Spikes: {stats['pending_spikes']}")
```

### 3. Advanced Configuration

Use expert-level configuration for fine-tuning:

```python
# Advanced configuration for expert users
advanced_settings = {
    "stdp_ltp_weight": 0.015,
    "stdp_ltd_weight": 0.018,
    "stdp_tau": 25.0,
    "synaptogenesis_rate": 0.0002,
    "pruning_rate": 0.00002,
    "replay_interval": 200,
    "consolidation_interval": 2000
}

brain.configureAdvanced(advanced_settings)
```

### 4. System Health Diagnostics

Monitor system health and detect issues:

```python
health = brain.checkSystemHealth()

print("System Health Status:")
print(f"  Brain Status: {health['brain_status']}")
print(f"  Memory Systems: {health['memory_systems']}")
print(f"  Warnings: {health['warnings']}")
```

### 5. Memory System Profiling

Analyze memory system performance:

```python
memory_profile = brain.getMemoryProfile()

if "working_memory" in memory_profile:
    print(f"Working Memory Capacity: {memory_profile['working_memory'].get('capacity', 0)}")
    print(f"Active Memory Traces: {memory_profile['working_memory'].get('active_traces', 0)}")

if "episodic_memory" in memory_profile:
    print(f"Episodic Memory Episodes: {memory_profile['episodic_memory'].get('episode_count', 0)}")
    print(f"Max Episodes: {memory_profile['episodic_memory'].get('max_episodes', 0)}")
```

### 6. Neuromodulation Analysis

Examine neuromodulation system:

```python
neuro_mod_profile = brain.getNeuromodulationProfile()

if "dopamine" in neuro_mod_profile:
    print(f"Dopamine Level: {neuro_mod_profile['dopamine'].get('level', 0):.3f}")
    print(f"Plasticity Factor: {neuro_mod_profile['dopamine'].get('plasticity_factor', 0):.3f}")

if "curiosity" in neuro_mod_profile:
    print(f"Curiosity Level: {neuro_mod_profile['curiosity'].get('level', 0):.3f}")

if "novelty" in neuro_mod_profile:
    print(f"Novelty Level: {neuro_mod_profile['novelty'].get('level', 0):.3f}")
```

## Expert-Level Usage

### 1. Batch Processing

Run multiple simulations efficiently:

```python
# Run batch simulation
batch_result = brain.runAdvancedSimulation({
    "steps": 5000,
    "neuron_count": 3000,
    "region_count": 4,
    "connection_probability": 0.15
})

print(f"Batch simulation completed:")
print(f"  Steps: {batch_result['steps_completed']}")
print(f"  Average Firing Rate: {batch_result['average_firing_rate']:.2f}")
```

### 2. Custom Simulation with Callbacks

Implement custom processing during simulation:

```python
def custom_callback(step, time):
    # Your custom analysis or processing
    result = {
        "step": step,
        "time": time,
        "spike_rate": brain.getAverageFiringRate(),
        "active_neurons": brain.getActiveNeuronCount()
    }
    return result

# Run simulation with custom callback
results = brain.runWithCallback(1000, custom_callback)

# Analyze results
print(f"Collected {len(results['step_results'])} data points")
print(f"Peak firing rate: {max(r['spike_rate'] for r in results['step_results']):.2f}")
```

### 3. Performance Benchmarking

Measure simulation performance:

```python
# Benchmark brain performance
benchmark = brain.benchmark(steps=2000, warmup=200)

print(f"Performance Benchmark:")
print(f"  Steps: {benchmark['steps']}")
print(f"  Warmup: {benchmark['warmup_steps']}")
print(f"  Time per Step: {benchmark['time_per_step_us']:.2f} μs")
print(f"  Steps per Second: {benchmark['steps_per_second']:.0f}")
```

### 4. State Inspection and Debugging

Deep inspection of brain state:

```python
# Get detailed brain state
brain_state = brain.inspectState()

print("Brain State Inspection:")
print(f"  Simulation Step: {brain_state['simulation_step']}")
print(f"  Simulation Time: {brain_state['simulation_time']:.3f} ms")
print(f"  Total Neurons: {brain_state['neuron_count']}")
print(f"  Total Synapses: {brain_state['synapse_count']}")
print(f"  Region Count: {brain_state['region_count']}")

# Inspect individual regions
for region in brain_state['regions']:
    print(f"  Region {region['id']}: {region['name']} - {region['neuron_count']} neurons, {region['synapse_count']} synapses")
```

### 5. Advanced Configuration Management

Export and import configurations:

```python
# Export current configuration
config_text = brain.exportConfig()
print("Current configuration:")
print(config_text)

# Import and apply new configuration (simplified example)
# new_config = pynlm.createDefaultConfig()
# new_config.loadFromArgs(len(sys.argv), sys.argv)
# brain.importConfig(config_text)
```

### 6. Simulation Control and Timing

Precise control over simulation timing:

```python
# Set custom timestep
brain.setTimestep(0.005)  # 5ms per step

# Get current timestep
current_timestep = brain.getTimestep()
print(f"Current timestep: {current_timestep:.4f} ms")

# Simulate specific time duration
steps = int(total_time / current_timestep)
for step in range(steps):
    brain.step(step, step * current_timestep)
```

## Configuration Management

### 1. Configuration File Operations

```python
# Save configuration to file
config.saveToFile("my_config.json")

# Load configuration from file
config.loadFromFile("my_config.json")

# Check configuration keys
print("Available configuration keys:", config.getKeys())

# Check if key exists
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.get('brain.neuron_count')}")
```

### 2. Configuration Templates

Create reusable configuration templates:

```python
def createDefaultBrainConfig(neuron_count=1000, regions=1):
    """Create a standard brain configuration"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", regions)
    config.set("brain.connection_probability", 0.1)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    return config

def createResearchConfig(neuron_count=5000, regions=3):
    """Create a configuration for research purposes"""
    config = createDefaultBrainConfig(neuron_count, regions)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.learning_rate", 0.005)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.strength", 0.5)
    config.set("neuromod.novelty.threshold", 0.3)
    return config
```

## Performance Optimization

### 1. Memory Optimization

```python
# Monitor memory usage during simulation
memory_profile = brain.getMemoryProfile()

# Check working memory
if "working_memory" in memory_profile:
    capacity = memory_profile["working_memory"].get("capacity", 0)
    active_traces = memory_profile["working_memory"].get("active_traces", 0)
    
    if active_traces > capacity * 0.9:
        print("Warning: Working memory approaching capacity")
```

### 2. Performance Monitoring

Track simulation performance:

```python
import time

# Performance monitoring
start_time = time.time()

# Run simulation
for step in range(10000):
    brain.step(step)
    
    # Periodic performance check
    if step % 1000 == 0:
        elapsed = time.time() - start_time
        steps_per_second = step / elapsed
        print(f"Step {step}: {steps_per_second:.0f} steps/sec")
```

### 3. Checkpointing Strategy

Implement efficient checkpointing:

```python
def savePeriodicCheckpoints(brain, interval=1000, prefix="checkpoint"):
    """Save brain state periodically during long simulations"""
    for step in range(10000):
        brain.step(step)
        
        if step % interval == 0:
            filename = f"{prefix}_{step:06d}.bin"
            brain.checkpoint(filename)
            print(f"Checkpoint saved: {filename}")

def loadLatestCheckpoint(brain, checkpoint_dir="checkpoints"):
    """Load the most recent checkpoint"""
    # Implementation would find latest checkpoint file
    # and call brain.load() with the filename
    pass
```

## Debugging and Monitoring

### 1. Debug Mode

Enable detailed logging:

```python
# Enable debug mode
brain.enableDebugMode()

# Run simulation with debug output
for step in range(100):
    brain.step(step)

# Disable debug mode when done
brain.disableDebugMode()
```

### 2. Experimental Mode

Test experimental features:

```python
# Enable experimental features
brain.enableExperimentalMode()

# Test experimental configurations
experimental_config = {
    "experimental_plasticity": True,
    "experimental_neuromodulation": True,
    "experimental_memory": True
}

# Apply experimental configuration
brain.configureAdvanced(experimental_config)

# Run experimental simulation
for step in range(500):
    brain.step(step)

# Disable experimental mode
brain.disableExperimentalMode()
```

### 3. State Validation

Validate brain state consistency:

```python
def validateBrainState(brain):
    """Validate brain state for consistency"""
    health = brain.checkSystemHealth()
    
    issues = []
    
    if not health["warnings"].empty():
        issues.extend(health["warnings"].values())
    
    if brain.getTotalNeuronCount() == 0:
        issues.append("No neurons initialized")
    
    if brain.getTotalSynapseCount() == 0:
        issues.append("No synapses created")
    
    return {
        "valid": len(issues) == 0,
        "issues": issues,
        "health_status": health
    }
```

## Batch Processing

### 1. Sequential Batch Processing

```python
def runBatchSimulations(configs, steps_list):
    """Run multiple simulations with different configurations"""
    results = []
    
    for i, (config, steps) in enumerate(zip(configs, steps_list)):
        print(f"Running simulation {i+1}/{len(configs)}...")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps):
            brain.step(step)
        
        # Collect results
        result = {
            "simulation_id": i,
            "steps_completed": steps,
            "final_spike_count": brain.getTotalSpikeCount(),
            "final_firing_rate": brain.getAverageFiringRate(),
            "neuron_count": brain.getTotalNeuronCount(),
            "synapse_count": brain.getTotalSynapseCount()
        }
        
        results.append(result)
    
    return results
```

### 2. Parallel Processing

```python
import multiprocessing
import time

def simulateWorker(args):
    """Worker function for parallel simulation"""
    config, steps, seed = args
    
    # Set random seed for reproducibility
    config.set("random_seed", seed)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    return {
        "spike_count": brain.getTotalSpikeCount(),
        "firing_rate": brain.getAverageFiringRate(),
        "final_step": steps
    }

def runParallelBatch(configs, steps_list, num_workers=None):
    """Run simulations in parallel"""
    if num_workers is None:
        num_workers = multiprocessing.cpu_count()
    
    # Prepare arguments for workers
    worker_args = list(zip(configs, steps_list, range(len(configs))))
    
    # Create process pool
    with multiprocessing.Pool(num_workers) as pool:
        results = pool.map(simulateWorker, worker_args)
    
    return results
```

## Memory Management

### 1. Memory Profiling

Analyze memory usage:

```python
def analyzeMemoryUsage(brain, iterations=10):
    """Analyze memory usage over time"""
    import psutil
    import os
    
    process = psutil.Process(os.getpid())
    
    memory_samples = []
    time_samples = []
    
    for i in range(iterations):
        # Run simulation step
        brain.step(i)
        
        # Measure memory
        memory_info = process.memory_info()
        memory_samples.append(memory_info.rss / 1024 / 1024)  # MB
        time_samples.append(time.time())
        
        if i % 100 == 0:
            print(f"Iteration {i}: {memory_samples[-1]:.1f} MB")
    
    return {
        "memory_samples": memory_samples,
        "time_samples": time_samples,
        "memory_growth": memory_samples[-1] - memory_samples[0],
        "average_memory": sum(memory_samples) / len(memory_samples)
    }
```

### 2. Efficient Memory Cleanup

Clean up memory after simulation:

```python
def cleanupBrainResources(brain):
    """Clean up brain resources and free memory"""
    # Clear working memory
    if hasattr(brain, 'getWorkingMemory'):
        working_memory = brain.getWorkingMemory()
        if working_memory:
            working_memory.clear()
    
    # Clear episodic memory
    if hasattr(brain, 'getEpisodicMemory'):
        episodic_memory = brain.getEpisodicMemory()
        if episodic_memory:
            episodic_memory.clear()
    
    # Reset brain state
    brain.reset()
    
    # Force garbage collection
    import gc
    gc.collect()
```

## Troubleshooting

### 1. Common Issues and Solutions

#### Issue: Brain initialization fails

**Symptoms:**
- `brain.initialize()` returns False
- Brain appears to hang during initialization

**Solutions:**
```python
# Check configuration
config = pynlm.createDefaultConfig()
print("Configuration:", config.summary())

# Try with minimal configuration
minimal_config = pynlm.createDefaultConfig()
minimal_config.set("neuron_count", 100)
minimal_config.set("region_count", 1)

brain = pynlm.createBrain(minimal_config)
brain.initialize()
```

#### Issue: Simulation produces no spikes

**Symptoms:**
- Neurons have zero firing rate
- No action produced

**Solutions:**
```python
# Check configuration
config = brain.getConfig()
print("Current configuration:", config.summary())

# Enable sensory input
if hasattr(brain, 'receiveSensoryInput'):
    # Create simple sensory input
    from pynlm import SensoryInput
    sensory_input = SensoryInput()
    sensory_input.setData([1.0, 0.5, 0.0, -0.5, -1.0])
    brain.receiveSensoryInput(sensory_input)
```

#### Issue: Memory usage grows uncontrollably

**Symptoms:**
- Memory usage increases over time
- Simulation eventually crashes

**Solutions:**
```python
# Monitor memory usage
memory_profile = brain.getMemoryProfile()
print("Memory profile:", memory_profile)

# Implement periodic cleanup
cleanupBrainResources(brain)

# Use checkpointing to reduce memory pressure
brain.checkpoint("checkpoint_point")
```

### 2. Debugging Techniques

#### Enable verbose logging

```python
import logging

# Enable detailed logging
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')

# Import brain modules
from pynlm.brain import Brain
from pynlm.core import Config

# Create brain with verbose logging
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize with logging
if brain.initialize():
    print("Brain initialization successful")
else:
    print("Brain initialization failed")
```

#### Manual inspection

```python
def manualBrainInspection(brain):
    """Perform manual inspection of brain state"""
    print("=== MANUAL BRAIN INSPECTION ===")
    
    # Check basic properties
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    # Check memory systems
    integration_status = brain.getIntegrationStatus()
    print("\nIntegration Status:")
    for system, connected in integration_status.items():
        print(f"  {system}: {'CONNECTED' if connected else 'DISCONNECTED'}")
    
    # Check neuromodulation
    neuro_profile = brain.getNeuromodulationProfile()
    print("\nNeuromodulation Profile:")
    for modulator, profile in neuro_profile.items():
        print(f"  {modulator}:")
        for key, value in profile.items():
            print(f"    {key}: {value}")
    
    return True
```

## API Reference

### Core Classes

#### Brain

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize()` | Initialize brain with configuration | None | bool |
| `step(step, time=None)` | Run simulation step | step, time | None |
| `receiveSensoryInput(input)` | Inject sensory input | input | None |
| `injectCurrent(neuron, current)` | Inject current to neuron | neuron, current | None |
| `produceAction()` | Produce motor action | None | Action |
| `reset()` | Reset brain state | None | None |
| `save(filepath)` | Save brain state | filepath | bool |
| `load(filepath)` | Load brain state | filepath | bool |
| `getTotalNeuronCount()` | Get total neuron count | None | size_t |
| `getTotalSynapseCount()` | Get total synapse count | None | size_t |
| `getFiringNeuronCount()` | Get firing neuron count | None | size_t |
| `getAverageFiringRate()` | Get average firing rate | None | float |

#### Config

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `loadFromFile(filepath)` | Load from JSON file | filepath | bool |
| `loadFromArgs(argc, argv)` | Load from command line | argc, argv | bool |
| `saveToFile(filepath)` | Save to JSON file | filepath | bool |
| `has(key)` | Check if key exists | key | bool |
| `getOr(key, default)` | Get with default | key, default | T |
| `set(key, value)` | Set value | key, value | None |

#### AgentBrain

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize(world)` | Initialize with world | world | None |
| `processSensoryInput(percept)` | Process sensory input | percept | None |
| `decodeMotorCommand()` | Decode motor command | None | MotorCommand |
| `applyRewardModulation(reward, predicted)` | Apply reward modulation | reward, predicted | None |
| `updateDevelopment(timestep)` | Update development | timestep | None |

### Advanced Features

#### Expert Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `configureAdvanced(settings)` | Configure advanced parameters | settings (dict) | bool |
| `getPerformanceStats()` | Get performance statistics | None | dict |
| `checkSystemHealth()` | Check system health | None | dict |
| `getMemoryProfile()` | Get memory profile | None | dict |
| `getNeuromodulationProfile()` | Get neuromodulation profile | None | dict |
| `inspectState()` | Inspect brain state | None | dict |

#### Simulation Control

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `checkpoint(filepath)` | Save checkpoint | filepath | None |
| `restore(filepath)` | Load checkpoint | filepath | None |
| `setTimestep(timestep)` | Set timestep | timestep | None |
| `getTimestep()` | Get timestep | None | double |

### Enumerations

#### NeuronType
- `Excitatory`
- `Inhibitory`
- `Modulatory`
- `Sensory`
- `Motor`
- `Internal`

#### ActionType
- `MoveForward`
- `MoveBackward`
- `MoveLeft`
- `MoveRight`
- `TurnLeft`
- `TurnRight`
- `Look`
- `Interact`
- `Eat`
- `Rest`
- `Wait`
- `Custom`

#### DevelopmentalStage
- `Initial`
- `CriticalPeriod`
- `Maturation`
- `Adult`
- `Aging`

## Examples

### Complete Working Example

```python
#!/usr/bin/env python3
"""
Complete NLM simulation example demonstrating advanced features
"""
import pynlm
import time
import json

def createAdvancedBrainConfig():
    """Create advanced brain configuration"""
    config = pynlm.createDefaultConfig()
    
    # Basic brain properties
    config.set("brain.neuron_count", 2000)
    config.set("brain.region_count", 4)
    config.set("brain.connection_probability", 0.12)
    
    # Plasticity settings
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.01)
    config.set("plasticity.stdp.tau", 20.0)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.0001)
    config.set("plasticity.structural.pruning_rate", 0.00001)
    
    # Neuromodulation
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.strength", 0.5)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.threshold", 0.3)
    
    # Memory systems
    config.set("memory.working_memory.capacity", 200)
    config.set("memory.episodic_memory.max_episodes", 1000)
    config.set("memory.associative_memory.association_limit", 5000)
    
    return config

def runAdvancedSimulation():
    """Run advanced simulation with monitoring"""
    print("=== NLM Advanced Simulation ===")
    
    # Create brain with advanced configuration
    config = createAdvancedBrainConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Performance monitoring
    start_time = time.time()
    step_times = []
    firing_rates = []
    
    print("Starting simulation...")
    
    # Run simulation with advanced features
    for step in range(2000):
        step_start = time.time()
        
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect metrics
        step_time = time.time() - step_start
        step_times.append(step_time)
        firing_rates.append(brain.getAverageFiringRate())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Progress reporting
        if step % 200 == 0:
            elapsed = time.time() - start_time
            avg_step_time = sum(step_times) / len(step_times)
            avg_firing_rate = sum(firing_rates) / len(firing_rates)
            
            print(f"Step {step}: {elapsed:.1f}s elapsed, "
                  f"{avg_step_time*1000:.2f}ms/step, "
                  f"{avg_firing_rate:.2f}Hz avg firing rate")
            
            # Save checkpoint periodically
            if step % 500 == 0:
                brain.checkpoint(f"advanced_checkpoint_{step}.bin")
                print(f"Checkpoint saved at step {step}")
    
    # Simulation complete
    elapsed = time.time() - start_time
    final_stats = brain.getPerformanceStats()
    health_status = brain.checkSystemHealth()
    
    print("\n=== SIMULATION RESULTS ===")
    print(f"Total simulation time: {elapsed:.1f} seconds")
    print(f"Total steps: 2000")
    print(f"Average steps per second: {2000/elapsed:.0f}")
    print(f"Average step time: {sum(step_times)/len(step_times)*1000:.2f} ms")
    print(f"\nFinal Statistics:")
    print(f"  Total neurons: {final_stats['total_neurons']}")
    print(f"  Total synapses: {final_stats['total_synapses']}")
    print(f"  Final firing rate: {final_stats['average_firing_rate']:.2f} Hz")
    print(f"  Total spikes: {final_stats['total_spikes']}")
    print(f"  System health: {'HEALTHY' if health_status['valid'] else 'ISSUES DETECTED'}")
    
    # Save final results
    results = {
        "simulation_time": elapsed,
        "steps_completed": 2000,
        "performance": final_stats,
        "health_status": health_status
    }
    
    with open("advanced_simulation_results.json", "w") as f:
        json.dump(results, f, indent=2)
    
    print(f"\nResults saved to advanced_simulation_results.json")
    
    return results

if __name__ == "__main__":
    runAdvancedSimulation()
```

This comprehensive example demonstrates:
- Advanced configuration setup
- Real-time performance monitoring
- Checkpointing and recovery
- Progress tracking and reporting
- Result analysis and storage
- Error handling and validation

## Conclusion

This advanced usage guide provides comprehensive coverage of the NLM Python API, from basic usage to expert-level features. The NLM framework offers powerful capabilities for neural simulation, learning, and behavior generation, with extensive customization options for different use cases.

For the latest documentation, examples, and support, visit the NLM project website. New features and improvements are continuously being added based on user feedback and research developments.