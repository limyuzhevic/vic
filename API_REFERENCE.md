# NLM Python API Reference

## Overview
This document provides comprehensive API documentation for the NLM (Neural Learning Machine) Python bindings. It covers all available functions, classes, and examples for working with neural simulation, learning systems, and cognitive architectures.

## Version
1.0.0

## Table of Contents

1. [Introduction](#introduction)
2. [Core Module: pynlm](#core-module-pynlm)
   - [Functions](#functions)
   - [Classes](#classes)
3. [Advanced Examples](#advanced-examples)
4. [Getting Started Guide](#getting-started-guide)
5. [Error Handling](#error-handling)
6. [Performance Considerations](#performance-considerations)
7. [Migration Guide](#migration-guide)

## Introduction

NLM is a comprehensive neural simulation framework that enables the creation of virtual brains capable of learning, memory, decision-making, and cognitive processing. The Python bindings provide a convenient interface to all NLM capabilities.

### Key Features

- **Neural Computation**: Spiking neural networks with event-driven dynamics
- **Memory Systems**: Working memory, episodic memory, semantic memory, and procedural memory
- **Neuromodulation**: Dopamine, acetylcholine, norepinephrine, serotonin, curiosity, and novelty systems
- **Cognitive Architecture**: Planning, concept formation, attention, and developmental systems
- **Prediction Systems**: Forward models and prediction error computation
- **Learning Mechanisms**: Hebbian learning, STDP, structural plasticity, and reward modulation
- **Simulation Control**: World modeling, agent-environment interaction, and behavioral analysis

## Core Module: pynlm

### Functions

#### createBrain(config)
Creates a new neural brain instance with the specified configuration.

**Parameters:**
- `config` (Config): Brain configuration object

**Returns:**
- `Brain`: New brain instance

**Example:**
```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
```

#### createSimpleWorld()
Creates a simple virtual world for agent-environment interaction.

**Parameters:**
- None

**Returns:**
- `SimpleWorld`: New world instance

**Example:**
```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
```

#### createAgentBrain(brain)
Creates an agent brain that connects a neural brain to a virtual world.

**Parameters:**
- `brain` (Brain): Brain instance to connect

**Returns:**
- `AgentBrain`: New agent brain instance

**Example:**
```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

#### createDefaultConfig()
Creates a default brain configuration with standard parameters.

**Parameters:**
- None

**Returns:**
- `Config`: Default configuration object

**Example:**
```python
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
```

### Classes

#### Brain
Implements the main neural brain class with spiking neural computation and integrated cognitive systems.

**Methods:**

- **initialize()** – Initialize brain with configuration
  ```python
  brain.initialize()
  ```

- **step(step_number)** – Perform one simulation step
  ```python
  brain.step(step_number)
  ```

- **receiveSensoryInput(input)** – Inject sensory input into brain
  ```python
  brain.receiveSensoryInput(sensoryInput)
  ```

- **injectCurrent(neuron_id, current)** – Inject current into specific neuron
  ```python
  brain.injectCurrent(neuronId, membranePotential)
  ```

- **getWorkingMemory()** – Get working memory system
  ```python
  workingMemory = brain.getWorkingMemory()
  ```

- **getEpisodicMemory()** – Get episodic memory system
  ```python
  episodicMemory = brain.getEpisodicMemory()
  ```

- **getAssociativeMemory()** – Get associative memory system
  ```python
  associativeMemory = brain.getAssociativeMemory()
  ```

- **getPredictionSystem()** – Get prediction system
  ```python
  predictionSystem = brain.getPredictionSystem()
  ```

- **getPlanner()** – Get neural planner
  ```python
  planner = brain.getPlanner()
  ```

- **getConceptFormation()** – Get concept formation system
  ```python
  conceptFormation = brain.getConceptFormation()
  ```

- **getAttention()** – Get attentional selection system
  ```python
  attention = brain.getAttention()
  ```

- **getDevelopmentSystem()** – Get development system
  ```python
  developmentSystem = brain.getDevelopmentSystem()
  ```

- **getDopamine()** – Get dopamine neuromodulator
  ```python
  dopamine = brain.getDopamine()
  ```

- **getAcetylcholine()** – Get acetylcholine neuromodulator
  ```python
  acetylcholine = brain.getAcetylcholine()
  ```

- **getNorepinephrine()** – Get norepinephrine neuromodulator
  ```python
  norepinephrine = brain.getNorepinephrine()
  ```

- **getSerotonin()** – Get serotonin neuromodulator
  ```python
  serotonin = brain.getSerotonin()
  ```

- **getCuriosity()** – Get curiosity neuromodulator
  ```python
  curiosity = brain.getCuriosity()
  ```

- **getNovelty()** – Get novelty detection system
  ```python
  novelty = brain.getNovelty()
  ```

- **getPredictionErrorSignal()** – Get prediction error signal
  ```python
  predErrorSignal = brain.getPredictionErrorSignal()
  ```

- **getTotalNeuronCount()** – Get total neuron count
  ```python
  neuronCount = brain.getTotalNeuronCount()
  ```

- **getFiringNeuronCount()** – Get firing neuron count
  ```python
  firingCount = brain.getFiringNeuronCount()
  ```

- **getAverageFiringRate()** – Get average firing rate
  ```python
  firingRate = brain.getAverageFiringRate()
  ```

- **getTotalSpikeCount()** – Get total spike count
  ```python
  spikeCount = brain.getTotalSpikeCount()
  ```

- **getPredictionError()** – Get prediction error
  ```python
  error = brain.getPredictionError()
  ```

- **getDevelopmentalStage()** – Get developmental stage
  ```python
  stage = brain.getDevelopmentalStage()
  ```

#### AgentBrain
Connects NLM brain to the world, handling sensory transduction and motor decoding.

**Methods:**

- **initialize(world)** – Initialize agent with world
  ```python
  agent.initialize(world)
  ```

- **processSensoryInput(percept)** – Process sensory percept from world
  ```python
  agent.processSensoryInput(percept)
  ```

- **decodeMotorCommand()** – Decode motor command from brain
  ```python
  action = agent.decodeMotorCommand()
  ```

- **applyRewardModulation(reward, predicted_reward)** – Apply reward-based neuromodulation
  ```python
  agent.applyRewardModulation(reward, predictedReward)
  ```

- **updateDevelopment(double timestep)** – Update development system
  ```python
  agent.updateDevelopment(timestep)
  ```

- **getNeuromodulationLevel()** – Get neuromodulation level
  ```python
  neuromodLevel = agent.getNeuromodulationLevel()
  ```

- **getCuriosityLevel()** – Get curiosity level
  ```python
  curiosityLevel = agent.getCuriosityLevel()
  ```

- **getNoveltyLevel()** – Get novelty level
  ```python
  noveltyLevel = agent.getNoveltyLevel()
  ```

- **getPredictionSystem()** – Get prediction system
  ```python
  predictionSystem = agent.getPredictionSystem()
  ```

- **getPredictionError()** – Get prediction error
  ```python
  predictionError = agent.getPredictionError()
  ```

- **enableRewardModulation(bool enable)** – Enable reward modulation
  ```python
  agent.enableRewardModulation(True)
  ```

- **enableCuriosity(bool enable)** – Enable curiosity-based learning
  ```python
  agent.enableCuriosity(True)
  ```

- **enableDevelopment(bool enable)** – Enable developmental learning
  ```python
  agent.enableDevelopment(True)
  ```

- **enableStructuralPlasticity(bool enable)** – Enable structural plasticity
  ```python
  agent.enableStructuralPlasticity(True)
  ```

#### SimpleWorld
Simple virtual world for agent-environment interaction.

**Methods:**

- **configure(width, height, visionWidth, visionHeight)** – Configure world dimensions and sensor properties
  ```python
  world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
  ```

- **reset()** – Reset world to initial state
  ```python
  world.reset()
  ```

- **update(time)** – Update world by specified time step
  ```python
  world.update(0.1)
  ```

- **getSensoryPercept()** – Get current sensory percept
  ```python
  percept = world.getSensoryPercept()
  ```

- **applyMotorCommand(action, time)** – Apply motor command to world
  ```python
  world.applyMotorCommand(action, time)
  ```

- **getSimulationTime()** – Get current simulation time
  ```python
  time = world.getSimulationTime()
  ```

- **getWidth()** – Get world width
  ```python
  width = world.getWidth()
  ```

- **getHeight()** – Get world height
  ```python
  height = world.getHeight()
  ```

## Advanced Examples

### File: examples/advanced_usage.py

Demonstrates comprehensive usage patterns and examples for NLM that showcase all new functionality.

**Topics Covered:**

- Basic memory systems (working, episodic, semantic, procedural, associative)
- Neuromodulation basics (dopamine, acetylcholine, norepinephrine, serotonin, curiosity, novelty)
- Prediction system basics and forward models
- Cognitive system basics (planning, concept formation, attention)
- Development system basics and maturation

**Advanced Examples:**

- Advanced memory usage with pattern completion and working memory optimization
- Neuromodulation integration with coordinated signals for exploration-exploitation
- Predictive planning with action consequence learning
- Cognitive architecture with self-model, planning, and learning
- Developmental learning across lifespan with age-dependent plasticity
- Multi-system coordination integration

### File: examples/custom_components.py

Demonstrates custom component creation including brain regions, plasticity rules, neuromodulators, memory systems, and plugin architecture.

**Topics Covered:**

- Custom brain region creation with specialized functions
- Custom plasticity rule implementation (Hebbian, Oja, Bayesian)
- Custom neuromodulator creation (AttentionGating, Stress, Curiosity)
- Custom memory system implementation (Temporal, Emotional, Implicit)
- Plugin architecture for dynamic component loading and integration

**Plugin Architecture Features:**

- Plugin Manager for centralized plugin lifecycle management
- BrainRegionPlugin, PlasticityRulePlugin, NeuromodulatorPlugin, MemorySystemPlugin
- Dynamic plugin registration and loading
- Configuration management for plugins

### File: examples/performance_optimization.py

Demonstrates performance optimization techniques including memory management, monitoring, parallel processing, and simulation scaling.

**Topics Covered:**

- Memory pool management and optimization
- Real-time performance monitoring and metrics collection
- Parallel processing with thread pools and SIMD optimization
- Large-scale brain simulation and scaling strategies
- Performance bottleneck identification and resolution

**Performance Monitoring Features:**

- PerformanceProfiler with function call tracking
- CPUProfiler with hotspot identification
- MemoryUsageTracker with leak detection
- Real-time metrics collection and analysis

### File: examples/debugging_troubleshooting.py

Demonstrates debugging and troubleshooting techniques including memory leak detection, signal validation, simulation inspection, and performance analysis.

**Topics Covered:**

- Memory debugging with allocation tracking and leak detection
- Neuromodulation signal validation and anomaly detection
- Simulation state inspection and step-by-step analysis
- Performance profiling and CPU hotspot identification
- Comprehensive error handling and troubleshooting

### File: examples/integration_framework.py

Demonstrates integration frameworks for research, agent training, behavioral analysis, and real-world applications.

**Topics Covered:**

- Research framework for scientific experimentation
- Agent training framework for reinforcement learning
- Behavioral analysis tools for neural activity patterns
- Real-world application setup and integration
- Application lifecycle management and monitoring

## Getting Started Guide

### Prerequisites

- Python 3.8 or higher
- NLM Python bindings installed (`pip install pynlm`)
- Basic understanding of neural networks and brain simulation concepts

### Installation

```bash
pip install pynlm
```

### Basic Setup

1. **Create a brain instance**
   ```python
   import pynlm
   
   # Create brain with default configuration
   brain = pynlm.createBrain(pynlm.createDefaultConfig())
   ```

2. **Initialize the brain**
   ```python
   brain.initialize()
   ```

3. **Create a world**
   ```python
   world = pynlm.createSimpleWorld()
   world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
   world.reset()
   ```

4. **Create an agent**
   ```python
   agent = pynlm.createAgentBrain(brain)
   agent.initialize(world)
   ```

5. **Enable learning systems**
   ```python
   agent.enableRewardModulation(True)
   agent.enableCuriosity(True)
   agent.enableDevelopment(True)
   ```

6. **Run simulation**
   ```python
   for step in range(100):
       # Update world
       world.update(0.1)
       
       # Get sensory input
       percept = world.getSensoryPercept()
       
       # Process sensory input
       agent.processSensoryInput(percept)
       
       # Brain processes
       brain.step(step)
       
       # Get action
       action = agent.decodeMotorCommand()
       
       # Apply action
       world.applyMotorCommand(action, world.getSimulationTime())
       
       # Optional: log metrics
       if step % 10 == 0:
           print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
   ```

### Basic Monitoring

```python
# Monitor brain performance
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Prediction error: {brain.getPredictionError():.3f}")

# Monitor neuromodulation levels
dopamine = brain.getDopamine()
acetylcholine = brain.getAcetylcholine()
curiosity = agent.getCuriosityLevel()

print(f"Dopamine level: {dopamine.getLevel():.3f}")
print(f"Acetylcholine level: {acetylcholine.getLevel():.3f}")
print(f"Curiosity level: {curiosity:.3f}")
```

### Example: Simple Learning Task

```python
import pynlm
import time

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run learning task
for step in range(200):
    # Update environment
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process input
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Select action
    action = agent.decodeMotorCommand()
    
    # Execute action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Provide occasional reward
    if step % 20 == 0:
        agent.applyRewardModulation(1.0, 0.0)
    
    # Log progress
    if step % 50 == 0:
        print(f"Step {step}: "
              f"neurons={brain.getFiringNeuronCount()}, "
              f"rate={brain.getAverageFiringRate():.3f}, "
              f"dopamine={brain.getDopamine().getLevel():.3f}")

print("Learning task completed!")
```

## Error Handling

### Common Error Conditions

1. **Brain not initialized**
   ```python
   # Error: Brain must be initialized before use
   brain.initialize()  # Always call this first
   ```

2. **Invalid neuron ID**
   ```python
   # Error: Invalid neuron ID
   brain.injectCurrent(invalidNeuronId, 0.5)
   ```

3. **Memory system access**
   ```python
   # Error: Memory system not available
   workingMemory = brain.getWorkingMemory()
   # Check if workingMemory exists before use
   if workingMemory:
       workingMemory.store(pattern, 1.0)
   ```

4. **Neuromodulator access**
   ```python
   # Error: Neuromodulator not available
   dopamine = brain.getDopamine()
   # Check if dopamine exists
   if dopamine:
       dopamine.signalReward(1.0)
   ```

### Error Handling Best Practices

```python
def safe_neuron_operation(brain, neuron_id, current):
    try:
        # Check if brain is initialized
        if not hasattr(brain, 'getTotalNeuronCount'):
            print("Error: Brain not properly initialized")
            return False
            
        # Check neuron validity
        if neuron_id >= brain.getTotalNeuronCount():
            print(f"Error: Invalid neuron ID {neuron_id}")
            return False
            
        # Perform operation
        brain.injectCurrent(neuron_id, current)
        return True
        
    except Exception as e:
        print(f"Error in neuron operation: {e}")
        return False
```

## Performance Considerations

### Memory Management

- **Use memory pools for frequent allocations**: Avoid repeated `malloc`/`free` cycles
- **Implement object pooling**: Reuse objects instead of creating new ones
- **Consider SIMD optimizations**: Use vectorized operations for neural computations
- **Profile memory usage**: Use tools like Valgrind or memory profilers

### CPU Optimization

- **Parallelize independent operations**: Use thread pools for neural processing
- **Cache frequently accessed data**: Reduce memory access latency
- **Optimize data structures**: Use contiguous memory layouts for better cache locality
- **Profile CPU hotspots**: Identify and optimize slow functions

### Simulation Scaling

- **Decompose large simulations**: Split across multiple processes/threads
- **Use asynchronous processing**: Overlap computation with I/O operations
- **Implement checkpointing**: Save and restore simulation state
- **Optimize memory bandwidth**: Use efficient data structures and algorithms

## Migration Guide

### From Version 0.x to 1.0.0

#### API Changes

1. **Brain initialization**
   - **Before**: `brain.initialize(config)`
   - **After**: `brain.initialize()`

2. **Memory system access**
   - **Before**: `brain.getMemorySystem("working")`
   - **After**: `brain.getWorkingMemory()`

3. **Neuromodulator access**
   - **Before**: `brain.getNeuromodulator("dopamine")`
   - **After**: `brain.getDopamine()`

4. **Error handling**
   - **Before**: Exceptions raised with minimal information
   - **After**: Detailed error messages with validation details

#### New Features Added

1. **Enhanced neuromodulation systems**: Added acetylcholine, norepinephrine, serotonin
2. **Advanced memory systems**: Temporal, emotional, implicit memory systems
3. **Plugin architecture**: Dynamic component loading and extension
4. **Performance monitoring**: Comprehensive profiling and bottleneck detection
5. **Debugging tools**: Memory leak detection, signal validation, state inspection

#### Migration Examples

```python
# Old API
brain = pynlm.createBrain(config)
brain.initialize()
dopamine = brain.getNeuromodulator("dopamine")

# New API
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
dopamine = brain.getDopamine()
```

```python
# Old API
memory = brain.getMemorySystem("working")
memory.store(pattern)

# New API
workingMemory = brain.getWorkingMemory()
workingMemory.store(pattern)
```

## Appendix: Type Definitions

### NeuronId
64-bit identifier for neurons

### SynapseId
64-bit identifier for synapses

### RegionId
64-bit identifier for brain regions

### PopulationId
64-bit identifier for neuron populations

### SimulationStep
Integer representing simulation step number

### Timestamp
Double representing simulation time in seconds

### MembranePotential
Float representing neuron membrane potential

### TimestepDuration
Float representing time duration for simulation steps

### DevelopmentalStage
Enumeration representing brain development stage
- INFANCY
- CHILDHOOD
- ADOLESCENCE
- YOUNG_ADULT
- MIDDLE_AGED
- SENIOR

## License
MIT License

© 2026 NLM Project

---

*For more information, visit [https://github.com/nlm-project/nlm](https://github.com/nlm-project/nlm)*