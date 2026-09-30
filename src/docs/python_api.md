# NLM Python API Reference

## Overview

The NLM (Neural Learning Machine) Python API provides bindings to the C++ neural simulation framework. This documentation provides complete class-by-class reference with method signatures, return values, exceptions, and performance characteristics.

## Table of Contents

- [Core Classes](#core-classes)
- [Configuration System](#configuration-system)
- [Memory System](#memory-system)
- [Prediction System](#prediction-system)
- [Neuromodulation](#neuromodulation)
- [Development System](#development-system)
- [Error Handling](#error-handling)
- [Performance Considerations](#performance-considerations)

## Core Classes

### Brain

The primary simulation class representing the neural network.

#### Creation and Initialization

```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize the brain
brain.initialize()

# Create with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("learning.rate", 0.01)
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Main Methods

| Method | Description | Parameters | Returns | Exceptions |
|--------|-------------|------------|---------|------------|
| `initialize()` | Initialize brain neurons and connections | None | None | RuntimeError |
| `step(timestep)` | Process one simulation timestep | `timestep` (int) | None | RuntimeError |
| `getTotalNeuronCount()` | Get total neuron count | None | int | None |
| `getFiringNeuronCount()` | Get number of currently firing neurons | None | int | None |
| `getTotalSpikeCount()` | Get total spike count since initialization | None | int | None |
| `getAverageFiringRate()` | Get average firing rate | None | float | None |
| `getDevelopmentalStage()` | Get current developmental stage | None | DevelopmentalStage | None |

#### Properties

```python
# Access brain properties
brain = pynlm.createBrain(config)
brain.initialize()

neuron_count = brain.getTotalNeuronCount()  # 1000 (default)
firing_count = brain.getFiringNeuronCount()  # Current firing neurons
spike_count = brain.getTotalSpikeCount()     # Total spikes so far
rate = brain.getAverageFiringRate()          # Average firing rate
stage = brain.getDevelopmentalStage()       # Current developmental stage
```

#### Performance Characteristics

- **Time complexity**: `O(N)` where N is neuron count for each timestep
- **Memory usage**: Approximately 100 bytes per neuron
- **Scalability**: Can handle 10,000+ neurons on modern hardware
- **Optimizations**: Uses event-driven spike propagation, SIMD vectorization

### AgentBrain

Manages agent-specific functionality including sensory processing, motor output, and neuromodulation.

#### Creation and Initialization

```python
import pynlm

# Create brain and agent brain
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Create world and initialize agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
agent.initialize(world)
```

#### Main Methods

| Method | Description | Parameters | Returns | Exceptions |
|--------|-------------|------------|---------|------------|
| `initialize(world)` | Initialize agent with world | `world` (SimpleWorld) | None | RuntimeError |
| `processSensoryInput(percept)` | Process sensory input from world | `percept` (SensoryPercept) | None | RuntimeError |
| `decodeMotorCommand()` | Decode motor command from neural activity | None | Action | None |
| `applyRewardModulation(reward, predicted)` | Apply reward-based learning | `reward` (float), `predicted` (float) | None | RuntimeError |
| `updateDevelopment(timestep)` | Update developmental stage | `timestep` (float) | None | RuntimeError |

#### Neuromodulation Methods

```python
# Enable neuromodulation features
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableCuriosity(True)             # Drive exploration
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain maturation

# Get neuromodulation levels
curiosity = agent.getCuriosityLevel()      # Current curiosity level
novelty = agent.getNoveltyLevel()          # Novelty detection level
prediction_error = agent.getPredictionError()  # Prediction accuracy
neuromodulation = agent.getNeuromodulationLevel()  # Overall neuromodulation
```

### SimpleWorld

Virtual environment simulation with sensory input and motor output capabilities.

#### Creation and Configuration

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=20,                    # World width
    height=20,                   # World height
    visionWidth=8,              # Agent's visual field width
    visionHeight=8,             # Agent's visual field height
    maxEnergy=100.0,             # Maximum energy
    timeStep=0.1                 # Simulation timestep
)

# Reset world state
world.reset()
```

#### Main Methods

| Method | Description | Parameters | Returns | Exceptions |
|--------|-------------|------------|---------|------------|
| `configure(width, height, visionWidth, visionHeight, maxEnergy, timeStep)` | Configure world parameters | Varies | None | RuntimeError |
| `update(dt)` | Advance simulation by dt | `dt` (float) | None | RuntimeError |
| `reset()` | Reset world to initial state | None | None | RuntimeError |
| `getSensoryPercept()` | Get current sensory perception | None | SensoryPercept | None |
| `applyMotorCommand(action, time)` | Apply agent action | `action` (Action), `time` (float) | None | RuntimeError |
| `setAgentStart(x, y)` | Set agent starting position | `x`, `y` (int) | None | RuntimeError |

#### State Management

```python
# World state access
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)

# Sensory input
percept = world.getSensoryPercept()

# Agent position and body
body = world.getAgentBody()
pos_x, pos_y = body.getPosition()
energy = body.getEnergy()

# World objects
objects = world.getObjectsAt(x, y)
resource_value = world.getResourceValueAt(x, y)

# World properties
width = world.getWidth()
height = world.getHeight()
simulation_time = world.getSimulationTime()
max_energy = world.getMaxEnergy()
```

### Configuration

Configuration management system for brain parameters.

#### Creation and Access

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Access configuration values
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify configuration values
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### Configuration Parameters

| Category | Parameter | Type | Default | Description |
|----------|-----------|------|---------|-------------|
| `brain` | `neuron_count` | int | 1000 | Total number of neurons |
| | `synapse_density` | float | 0.1 | Synapse density (0.0-1.0) |
| | `learning.rate` | float | 0.01 | Learning rate |
| | `learning.stdp_strength` | float | 1.0 | STDP learning strength |
| | `brain.excitability` | float | 1.0 | Neuron excitability |
| `simulation` | `time_step` | float | 0.1 | Simulation timestep |
| | `max_spike_count` | int | 1000000 | Maximum spike count |
| `memory` | `working_memory_size` | int | 100 | Working memory capacity |
| | `episodic_capacity` | int | 1000 | Episodic memory capacity |
| `neuromodulation` | `dopamine_strength` | float | 1.0 | Dopamine neuromodulation strength |
| | `curiosity_strength` | float | 1.0 | Curiosity drive strength |

### NeuronId, SynapseId, RegionId, PopulationId

Unique identifier types for NLM components.

```python
import pynlm

# Create and use identifiers
neuron_id = pynlm.NeuronId(42)
synapse_id = pynlm.SynapseId(100)
region_id = pynlm.RegionId(1)
population_id = pynlm.PopulationId(10)

# Access identifier properties
print(f"Neuron ID value: {neuron_id.value}")
print(f"Neuron ID index: {neuron_id.index()}")
print(f"Synapse ID: {synapse_id}")

# Equality and hashing
id1 = pynlm.NeuronId(42)
id2 = pynlm.NeuronId(42)
print(f"IDs equal: {id1 == id2}")  # True
```

### Enumerations

#### NeuronType

```python
import pynlm

# Neuron type constants
neuron_type = pynlm.NeuronType.Excitatory

# Available types
print(f"Excitatory: {pynlm.NeuronType.Excitatory}")
print(f"Inhibitory: {pynlm.NeuronType.Inhibitory}")
print(f"Sensory: {pynlm.NeuronType.Sensory}")
print(f"Motor: {pynlm.NeuronType.Motor}")
print(f"Modulatory: {pynlm.NeuronType.Modulatory}")
print(f"Internal: {pynlm.NeuronType.Internal}")
```

#### DevelopmentalStage

```python
import pynlm

# Developmental stage constants
stage = pynlm.DevelopmentalStage.Maturation

# Available stages
stages = [
    pynlm.DevelopmentalStage.Initial,
    pynlm.DevelopmentalStage.CriticalPeriod,
    pynlm.DevelopmentalStage.Maturation,
    pynlm.DevelopmentalStage.Adult,
    pynlm.DevelopmentalStage.Aging
]
```

## Configuration System

The configuration system allows dynamic parameter adjustment and persistence.

### Configuration Operations

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 2000)
config.set("learning.rate", 0.005)
config.set("brain.excitability", 1.2)

# Get configuration values
neuron_count = config.getInt("brain.neuron_count")
learning_rate = config.getFloat("learning.rate")
excitability = config.getFloat("brain.excitability")

# Check if parameter exists
if config.has("brain.some_parameter"):
    value = config.getString("brain.some_parameter")

# List all configuration keys
keys = config.getKeys()
for key in keys:
    print(f"Key: {key}")

# Save and load configuration
config.saveToFile("my_config.json")
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")
```

### Configuration Categories

#### Brain Configuration
- `brain.neuron_count`: Number of neurons
- `brain.synapse_density`: Synapse density
- `learning.rate`: Learning rate
- `learning.stdp_strength`: STDP learning strength
- `brain.excitability`: Neural excitability

#### Simulation Configuration
- `simulation.time_step`: Simulation timestep
- `simulation.max_spike_count`: Maximum spike count
- `simulation.checkpoint_interval`: Checkpoint frequency

#### Memory Configuration
- `memory.working_memory_size`: Working memory capacity
- `memory.episodic_capacity`: Episodic memory capacity
- `memory.associative_capacity`: Associative memory capacity

#### Neuromodulation Configuration
- `neuromodulation.dopamine_strength`: Dopamine effect strength
- `neuromodulation.curiosity_strength`: Curiosity drive strength
- `neuromodulation.novelty_threshold`: Novelty detection threshold

## Memory System

Working memory, episodic memory, and associative memory systems.

### Working Memory

```python
import pynlm

# Access working memory
brain = pynlm.createBrain(config)
brain.initialize()

# Working memory operations
working_memory = brain.getWorkingMemory()
working_memory.addPattern(pattern_data)
pattern = working_memory.getPattern(index)
working_memory.clear()

# Working memory properties
capacity = working_memory.getCapacity()
size = working_memory.getSize()
is_full = working_memory.isFull()
```

### Episodic Memory

```python
import pynlm

# Episodic memory operations
episodic_memory = brain.getEpisodicMemory()

# Store and retrieve experiences
episodic_memory.storeExperience(experience_data, reward, context)
experience = episodic_memory.getExperience(index)
episodic_memory.replay(index, speed=1.0)

# Episodic memory properties
capacity = episodic_memory.getCapacity()
usage = episodic_memory.getUsage()
```

### Associative Memory

```python
import pynlm

# Associative memory operations
associative_memory = brain.getAssociativeMemory()

# Pattern association
associative_memory.associate(pattern_a, pattern_b, strength=1.0)
retrieved = associative_memory.retrieve(pattern_a)
associative_memory.remove(pattern_a)

# Statistical properties
strength = associative_memory.getStrength(pattern_a, pattern_b)
```

## Prediction System

Forward and inverse models for action planning and prediction.

### Prediction Error

```python
import pynlm

# Prediction error operations
prediction_error = brain.getPredictionError()

# Prediction error calculation
error = prediction_error.calculate(prediction, actual, reward)
error_signal = prediction_error.getErrorSignal()
prediction_accuracy = prediction_error.getAccuracy()

# Error properties
error_value = prediction_error.getValue()
error_type = prediction_error.getType()
```

### Forward Model

```python
import pynlm

# Forward model operations
forward_model = brain.getForwardModel()

# Prediction
predicted_state = forward_model.predict(current_state, action)
prediction_confidence = forward_model.getConfidence()

# Model properties
model_accuracy = forward_model.getAccuracy()
model_complexity = forward_model.getComplexity()
```

### Inverse Model

```python
import pynlm

# Inverse model operations
inverse_model = brain.getInverseModel()

# Action planning
planned_action = inverse_model.plan(desired_state, current_state)
planning_confidence = inverse_model.getConfidence()

# Planning properties
plan_quality = inverse_model.getPlanQuality()
planning_time = inverse_model.getPlanningTime()
```

## Neuromodulation

Dopamine, curiosity, novelty, and other neuromodulators.

### Dopamine System

```python
import pynlm

# Dopamine system operations
dopamine = agent.getDopamineSystem()

# Dopamine effects
dopamine.addSignal(reward, prediction_error)
dopamine_level = dopamine.getLevel()
dopamine_influence = dopamine.getInfluence()

# Dopamine properties
modulation_strength = dopamine.getModulationStrength()
adapting = dopamine.isAdapting()
```

### Curiosity System

```python
import pynlm

# Curiosity system operations
curiosity = agent.getCuriositySystem()

# Curiosity operations
curiosity_explore = curiosity.shouldExplore()
curiosity_update = curiosity.update(novelty, prediction_error)
curiosity_level = curiosity.getLevel()

# Curiosity properties
exploration_drive = curiosity.getExplorationDrive()
novelty_sensitivity = curiosity.getNoveltySensitivity()
```

### Novelty Detection

```python
import pynlm

# Novelty detection operations
novelty = agent.getNoveltySystem()

# Novelty calculation
is_novel = novelty.isNovel(pattern)
novelty_score = novelty.calculate(pattern)
novelty_threshold = novelty.getThreshold()

# Novelty properties
detection_sensitivity = novelty.getSensitivity()
adaptation_rate = novelty.getAdaptationRate()
```

## Development System

Developmental stages affecting plasticity and learning.

### Development Stages

```python
import pynlm

# Development stage operations
development = agent.getDevelopmentSystem()

# Stage operations
current_stage = development.getCurrentStage()
stage_duration = development.getStageDuration(current_stage)
stage_progress = development.getStageProgress()
next_stage = development.getNextStage()

# Stage properties
is_critical_period = development.isCriticalPeriod()
plasticity_rate = development.getPlasticityRate()
development_age = development.getAge()
```

### Structural Plasticity

```python
import pynlm

# Structural plasticity operations
structural_plasticity = agent.getStructuralPlasticitySystem()

# Plasticity operations
grow_new_connection(source_id, target_id, strength)
prune_connection(connection_id)
remodel_connection(connection_id, new_strength)
connection_count = structural_plasticity.getConnectionCount()

# Plasticity properties
growth_rate = structural_plasticity.getGrowthRate()
pruning_rate = structural_plasticity.getPruningRate()
connectivity = structural_plasticity.getConnectivity()
```

## Error Handling

Comprehensive error handling for robust simulations.

### Exception Types

```python
import pynlm

try:
    # Simulation code
    brain = pynlm.createBrain(config)
    brain.initialize()
    for step in range(1000):
        brain.step(step)
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Error Checking

```python
import pynlm

# Check system state
brain = pynlm.createBrain(config)

# Configuration errors
if not config.isValid():
    errors = config.getValidationErrors()
    for error in errors:
        print(f"Config error: {error}")

# State errors
if brain.isInitialized():
    print("Brain is initialized")
else:
    print("Brain not initialized")

# Resource errors
if brain.isMemoryAvailable():
    print("Memory available")
else:
    print("Memory exhausted")

# Performance errors
if brain.isOverloaded():
    print("Brain performance degraded")
else:
    print("Brain performance OK")
```

### Safe Simulation Practices

```python
import pynlm

def safe_simulation_run(brain, world, agent, max_steps):
    """Safely run simulation with error handling and checkpointing"""
    checkpoint_interval = 100
    
    try:
        # Initialize simulation
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        for step in range(max_steps):
            # Check for errors
            if brain.isOverloaded():
                print(f"Step {step}: Brain overloaded")
                break
                
            if step % checkpoint_interval == 0:
                # Save checkpoint
                brain.saveCheckpoint(f"checkpoint_{step}.bin")
                
            # Run simulation step
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Print progress
            if step % 100 == 0:
                firing_rate = brain.getAverageFiringRate()
                print(f"Step {step}: Firing rate = {firing_rate:.2f}")
                
    except pynlm.RuntimeError as e:
        print(f"Simulation error at step {step}: {e}")
        # Restore from checkpoint if available
        if step > checkpoint_interval:
            brain.loadCheckpoint(f"checkpoint_{step - checkpoint_interval}.bin")
    except Exception as e:
        print(f"Unexpected error: {e}")
        raise

# Usage example
if __name__ == "__main__":
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    agent = pynlm.createAgentBrain(brain)
    
    safe_simulation_run(brain, world, agent, 1000)
```

## Performance Considerations

### Memory Optimization

```python
import pynlm

# Optimize memory usage
config = pynlm.createDefaultConfig()

# Use memory-efficient configurations
config.set("brain.neuron_count", 500)      # Reduce neuron count
config.set("memory.working_memory_size", 50)    # Limit working memory
config.set("memory.episodic_capacity", 500)    # Limit episodic memory

# Monitor memory usage
brain = pynlm.createBrain(config)
brain.initialize()

while True:
    if brain.isMemoryAvailable():
        # Continue simulation
        brain.step(step)
    else:
        # Handle memory pressure
        print("Memory pressure - reducing simulation load")
        break
```

### Time Optimization

```python
import pynlm

# Optimize simulation speed
config = pynlm.createDefaultConfig()

# Configure for speed
config.set("simulation.time_step", 0.05)      # Smaller timesteps
config.set("brain.excitability", 1.0)        # Optimize neuron dynamics

# Use performance monitoring
brain = pynlm.createBrain(config)
brain.initialize()

import time
start_time = time.time()
steps_completed = 0

for step in range(1000):
    step_start = time.time()
    
    brain.step(step)
    
    step_time = time.time() - step_start
    steps_completed += 1
    
    # Check performance
    if step_time > 0.1:  # Step taking too long
        print(f"Step {step}: Slow performance ({step_time:.3f}s)")

end_time = time.time()
total_time = end_time - start_time
average_step_time = total_time / steps_completed

print(f"Total time: {total_time:.2f}s")
print(f"Average step time: {average_step_time:.4f}s")
print(f"Steps per second: {steps_completed/total_time:.1f}")
```

### Scalability Strategies

```python
import pynlm

# Implement scalability strategies
class ScalableSimulation:
    def __init__(self, max_neurons=10000):
        self.max_neurons = max_neurons
        self.current_neurons = 0
        self.batch_size = 1000
        
    def run_large_simulation(self, total_steps):
        """Run large simulation with batching"""
        brain = pynlm.createBrain(self.createConfig())
        brain.initialize()
        
        for step in range(total_steps):
            # Process in batches for large simulations
            if self.current_neurons > self.max_neurons:
                print(f"Step {step}: Approaching neuron limit")
                self.reduce_neuron_count(brain)
                
            brain.step(step)
            
    def createConfig(self):
        """Create optimized configuration for large simulations"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", min(5000, self.max_neurons))
        config.set("memory.working_memory_size", 200)
        config.set("simulation.time_step", 0.05)
        return config
        
    def reduce_neuron_count(self, brain):
        """Reduce neuron count to manage memory"""
        config = brain.getConfig()
        current_count = config.getInt("brain.neuron_count")
        if current_count > 1000:
            config.set("brain.neuron_count", current_count // 2)
            # Recreate brain with new config
            new_brain = pynlm.createBrain(config)
            new_brain.initialize()
            # Transfer state if needed
            brain = new_brain
```

## Complete Example: Full Integration

```python
import pynlm
import time

def run_advanced_simulation(config_overrides=None, enable_learning=True):
    """Advanced simulation with all features"""
    
    # 1. Configure simulation
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("learning.rate", 0.01)
    config.set("memory.working_memory_size", 200)
    
    if config_overrides:
        for key, value in config_overrides.items():
            config.set(key, value)
            
    # 2. Create and initialize components
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    agent = pynlm.createAgentBrain(brain)
    
    agent.initialize(world)
    
    # 3. Enable all neuromodulation features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    # 4. Setup monitoring and checkpointing
    performance_monitor = PerformanceMonitor()
    checkpoint_interval = 500
    
    # 5. Run simulation
    print("Starting advanced simulation...")
    start_time = time.time()
    
    for step in range(10000):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learning
        if enable_learning and step % 10 == 0:
            reward = calculate_reward(world, agent)
            predicted = agent.getPrediction()
            agent.applyRewardModulation(reward, predicted)
        
        # Development updates
        if step % 50 == 0:
            agent.updateDevelopment(step * 0.1)
        
        # Checkpointing
        if step % checkpoint_interval == 0:
            brain.saveCheckpoint(f"checkpoint_{step}.bin")
            print(f"Checkpoint saved at step {step}")
        
        # Performance monitoring
        if step % 100 == 0:
            stats = performance_monitor.getStats()
            print(f"Step {step}: {stats}")
    
    end_time = time.time()
    
    # 6. Final analysis
    final_stats = {
        "total_time": end_time - start_time,
        "total_spikes": brain.getTotalSpikeCount(),
        "average_firing_rate": brain.getAverageFiringRate(),
        "development_stage": brain.getDevelopmentalStage(),
        "curiosity_level": agent.getCuriosityLevel(),
        "novelty_level": agent.getNoveltyLevel(),
        "prediction_error": agent.getPredictionError(),
        "neuromodulation_level": agent.getNeuromodulationLevel()
    }
    
    print("Simulation completed successfully!")
    print(f"Final stats: {final_stats}")
    
    return brain, world, agent, final_stats

# Performance monitor class
class PerformanceMonitor:
    def __init__(self):
        self.step_times = []
        self.memory_usage = []
        self.cpu_usage = []
        
    def record_step(self, step_time, memory_usage, cpu_usage):
        self.step_times.append(step_time)
        self.memory_usage.append(memory_usage)
        self.cpu_usage.append(cpu_usage)
        
    def getStats(self):
        if not self.step_times:
            return {}
            
        avg_step_time = sum(self.step_times) / len(self.step_times)
        avg_memory = sum(self.memory_usage) / len(self.memory_usage)
        avg_cpu = sum(self.cpu_usage) / len(self.cpu_usage)
        
        return {
            "avg_step_time": avg_step_time,
            "avg_memory_mb": avg_memory,
            "avg_cpu_percent": avg_cpu,
            "min_step_time": min(self.step_times),
            "max_step_time": max(self.step_times),
            "throughput_steps_per_sec": len(self.step_times) / (sum(self.step_times) if self.step_times else 1)
        }

# Helper function for reward calculation
def calculate_reward(world, agent):
    """Calculate reward based on world state and agent behavior"""
    body = world.getAgentBody()
    energy = body.getEnergy()
    
    # Simple reward function
    reward = 0.0
    
    # Energy reward
    if energy > 50:
        reward += 0.1
        
    # Curiosity reward
    curiosity = agent.getCuriosityLevel()
    reward += curiosity * 0.05
    
    # Novelty reward
    novelty = agent.getNoveltyLevel()
    reward += novelty * 0.05
    
    return reward

# Usage example
if __name__ == "__main__":
    # Custom configuration for specific task
    custom_config = {
        "brain.neuron_count": 3000,
        "learning.rate": 0.005,
        "memory.working_memory_size": 300,
        "neuromodulation.dopamine_strength": 1.5,
        "neuromodulation.curiosity_strength": 1.2
    }
    
    brain, world, agent, stats = run_advanced_simulation(
        config_overrides=custom_config,
        enable_learning=True
    )
```

This comprehensive API reference provides complete documentation for all NLM Python classes, methods, and functionality. It includes creation examples, performance characteristics, error handling, and advanced usage patterns to help developers build robust neural simulations.

## See Also

- [Easy Usage Guide](../easy_usage.md)
- [Comprehensive Usage Guide](../HOW_TO_USE.md)
- [Architecture Documentation](../docs/ARCHITECTURE.md)
- [Performance Documentation](../docs/PERFORMANCE.md)
- [Phase 6 Final Audit](../docs/PHASE6_FINAL_AUDIT.md)
