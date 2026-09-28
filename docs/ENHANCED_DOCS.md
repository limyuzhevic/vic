# Enhanced Documentation and Examples
# NLM (熙然) - Neural Learning Machine Documentation Improvements
# Created: 2026-09-28

## Overview

This document contains enhanced documentation and examples for the NLM project, designed to serve users from beginners to expert researchers.

## Table of Contents

1. [Quick Start Guide for Beginners](#quick-start-guide-for-beginners)
2. [Comprehensive Tutorial](#comprehensive-tutorial)
3. [Expert-Level Examples](#expert-level-examples)
4. [Configuration and Customization](#configuration-and-customization)
5. [Python API Reference](#python-api-reference)
6. [Advanced Features](#advanced-features)
7. [Performance Guidelines](#performance-guidelines)
8. [Troubleshooting](#troubleshooting)
9. [Research Applications](#research-applications)

## Quick Start Guide for Beginners

### What is NLM?

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. It simulates how brains learn, remember, and make decisions through neural computation.

### Core Concepts

- **Brain**: Virtual neural network that processes information
- **World**: Environment where the brain interacts
- **Agent**: Interface connecting brain to world

### Installation and Setup

```bash
# Build the NLM framework
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run the Phase 6 integration test
./nlm_phase6_demo
```

### First Steps with Python

The easiest way to get started is with the Python bindings:

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Make the brain think
for i in range(100):
    brain.step(i)

print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Step-by-Step Tutorial

#### 1. Create a Brain
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)  # Adjust as needed

# Create brain
brain = pynlm.createBrain(config)
```

#### 2. Initialize the Brain
```python
brain.initialize()
print("Brain initialized successfully!")
```

#### 3. Simulate Thinking
```python
# Run for 1000 simulation steps
for step in range(1000):
    brain.step(step)
```

#### 4. Explore Brain Statistics
```python
print(f"Neurons firing: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Development stage: {brain.getDevelopmentalStage()}")
```

## Comprehensive Tutorial

### Part 1: Basic Brain Simulation

This section teaches the fundamentals of brain simulation using NLM.

#### Example 1: Silent Brain
```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print("Silent brain test completed!")
```

#### Example 2: Brain in a Simple World
```python
import pynlm

# Setup brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Setup world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# Setup agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Part 2: Learning and Development

#### Example 3: Learning Brain
```python
import pynlm

# Create learning-enabled brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable learning features
config = brain.getConfig()
config.set("stdp_ltp_weight", 0.02f)
config.set("stdp_ltd_weight", 0.015f)

# Run learning simulation
for step in range(1000):
    # Inject input to stimulate learning
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Learning simulation completed!")
print(f"Final spike count: {brain.getTotalSpikeCount()}")
```

#### Example 4: Developmental Brain
```python
import pynlm

# Create brain with development
config = pynlm.createDefaultConfig()
config.set("development_enabled", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Simulate development over time
for step in range(5000):  # 5 seconds at 0.001 timestep
    brain.step(step)
    
    if step % 1000 == 0:
        stage = brain.getDevelopmentalStage()
        print(f"Time {step/1000:.1f}s: Development stage {stage}")

print("Development simulation completed!")
```

### Part 3: Advanced Applications

#### Example 5: Complete Agent System
```python
import pynlm

# Setup complete agent system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run complete agent simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

## Expert-Level Examples

### Expert Example 1: Network Analysis
```python
import pynlm
import numpy as np

# Create expert-level analysis
config = pynlm.createBenchmarkConfig(epochCount=10)
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(2000):
    brain.step(step)

# Export network data for visualization
import json
network_data = brain.exportForVisualization()

# Analyze network properties
stats = brain.computeNetworkStats()
print("Network Statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")

# Save analysis
with open("network_analysis.json", "w") as f:
    json.dump(network_data, f)
```

### Expert Example 2: Custom Learning Patterns
```python
import pynlm

# Create custom learning brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Configure specific learning patterns
brain.getSTDP()->setLTPWeight(0.05f)  # Strong potentiation
brain.getSTDP()->setLTDWeight(0.03f)  # Moderate depression

# Create custom input patterns
def generateBurstPattern(neuron_ids, intensity, duration):
    for step in range(duration):
        for neuron_id in neuron_ids:
            brain.injectCurrent(neuron_id, intensity)
        brain.step(step)

# Create burst pattern for Hebbian learning
burst_neurons = [0, 1, 2, 3, 4]  # First 5 neurons
generateBurstPattern(burst_neurons, 50.0f, 100)

print("Custom learning pattern completed!")
```

### Expert Example 3: Performance Optimization
```python
import pynlm
import time

# Create performance-optimized brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Optimize for performance
brain.optimizeForPerformance()

# Profile performance
start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

print(f"Performance: {1000/(end_time-start_time):.0f} steps/second")
print(f"Efficiency: {brain.getAverageFiringRate():.2f} Hz average firing rate")
```

### Expert Example 4: Research Applications
```python
import pynlm
import matplotlib.pyplot as plt

# Setup research-grade simulation
config = pynlm.createBenchmarkConfig(epochCount=50)
brain = pynlm.createBrain(config)
brain.initialize()

# Enable all advanced features
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)

# Run research simulation
for step in range(5000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 500 == 0:
        print(f"Research step {step}: {brain.getTotalSpikeCount()} total spikes")

# Analyze results
stats = brain.computeNetworkStats()
print("Research Results:")
for key, value in stats.items():
    print(f"  {key}: {value:.4f}")

# Create visualization
viz_data = brain.exportForVisualization()
plt.figure(figsize=(12, 8))
# Add visualization code here
plt.savefig("research_results.png")
```

## Configuration and Customization

### Basic Configuration

NLM uses a flexible configuration system. Here's how to customize your simulation:

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Set basic parameters
config.set("simulation_timestep", 0.001)
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.1)

# Set learning parameters
config.set("stdp_ltp_weight", 0.02f)
config.set("stdp_ltd_weight", 0.015f)
config.set("stdp_tau", 20.0f)

# Set development parameters
config.set("synaptogenesis_rate", 0.0001)
config.set("pruning_rate", 0.00001)
```

### Advanced Configuration

```python
# Advanced settings for research
config.set("replay_interval", 100)        # Memory replay frequency
config.set("consolidation_interval", 1000) # Memory consolidation frequency
config.set("checkpoint_dir", "./checkpoints")  # Save directory
config.set("random_seed", 42)              # Reproducibility

# Neuromodulation settings
config.set("dopamine_level", 0.5f)
config.set("curiosity_threshold", 0.3f)
```

## Python API Reference

### Core Classes

#### Config
```python
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
config.loadFromFile("config.json")
config.saveToFile("my_config.json")
```

#### Brain
```python
brain = pynlm.createBrain(config)
brain.initialize()

# Simulation
brain.step(step)
brain.receiveSensoryInput(sensory_input)

# Control
brain.reset()
brain.save("brain_state")
brain.load("brain_state")
```

#### AgentBrain
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Processing
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
action = agent.decodeMotorCommand()

# Learning
agent.applyRewardModulation(reward, predicted_reward)
agent.updateDevelopment(timestep)
```

#### SimpleWorld
```python
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)

# World operations
world.update(timestep)
percept = world.getSensoryPercept()
action_result = world.applyMotorCommand(action, time)
```

### Expert API

#### Network Analysis
```python
# Get comprehensive statistics
stats = brain.computeNetworkStats()
print(f"Network density: {stats['network_density']:.4f}")
print(f"Modularity: {stats['modularity']:.4f}")

# Export for visualization
viz_data = brain.exportForVisualization()
```

#### Learning Configuration
```python
# Configure learning modes
agent.setExplorationMode(True)
agent.setLearningMode(hebbian=True, stdp=True, structural=False)
agent.setDevelopmentRate(0.5)
```

#### Performance Optimization
```python
# Optimize for different scenarios
if memory_constrained:
    brain.optimizeMemoryUsage()
else:
    brain.optimizeForPerformance()

# Profile performance
metrics = agent.profilePerformance(duration_steps=1000)
```

## Advanced Features

### 1. Memory Management

NLM includes sophisticated memory systems:

```python
# Working memory
working_memory = brain.getWorkingMemory()
working_memory.storeToNeuron(neuron_id, activation)
working_memory.update(timestep)

# Episodic memory
episodic_memory = brain.getEpisodicMemory()
episodic_memory.storeEpisode(episode_data)

# Semantic memory
semantic_memory = brain.getSemanticMemory()
semantic_memory.associate(concept1, concept2)
```

### 2. Neuromodulation

Advanced neuromodulation system:

```python
# Reward processing
agent.applyRewardModulation(reward, predicted_reward)

# Curiosity-driven exploration
curiosity_level = agent.getCuriosityLevel()
if curiosity_level > threshold:
    action = agent.selectWithCuriosity(default_action)

# Prediction error
dopamine_level = agent.getNeuromodulationLevel()
```

### 3. Development

Developmental stages and plasticity:

```python
# Development stages
stage = brain.getDevelopmentalStage()
if stage == pynlm.DevelopmentalStage.CriticalPeriod:
    # Sensitive learning period
    pass

# Structural plasticity
agent.enableStructuralPlasticity(True)
brain.develop()
```

### 4. Prediction and Planning

Cognitive prediction systems:

```python
# Prediction system
prediction_system = brain.getPredictionSystem()
# Make predictions about future states

# Neural planning
planner = brain.getPlanner()
planner.setPlanningDepth(5)
```

## Performance Guidelines

### Performance Optimization

1. **For Real-time Applications**
   ```python
   config.set("simulation_timestep", 0.01)
   config.set("neuron_count", 100)
   brain.optimizeForPerformance()
   ```

2. **For Research Accuracy**
   ```python
   config.set("simulation_timestep", 0.001)
   config.set("neuron_count", 2000)
   brain.optimizeForAccuracy()
   ```

3. **For Memory Efficiency**
   ```python
   brain.optimizeMemoryUsage()
   ```

### Scaling Guidelines

- **Small Scale**: < 1000 neurons (real-time applications)
- **Medium Scale**: 1000-5000 neurons (research)
- **Large Scale**: 5000-20000 neurons (advanced research)
- **Massive Scale**: > 20000 neurons ( supercomputer)

## Troubleshooting

### Common Issues

#### "My brain isn't doing anything"
**Solution**: 
```python
# Check if brain was initialized
brain.initialize()

# Try with more steps
for step in range(1000):  # More steps
    brain.step(step)
```

#### "The agent isn't moving"
**Solution**:
```python
# Check world updates
world.update(0.1)

# Check action decoding
action = agent.decodeMotorCommand()
if action == pynlm.MotorCommand.Wait:
    # Check neural activity
    print(f"Motor activity: {brain.getFiringNeuronCount()}")
```

#### "Everything is 0"
**Solution**:
```python
# Brains need time to "warm up"
for step in range(100):  # More steps
    brain.step(step)

# Ensure sensory input is connected
agent.processSensoryInput(percept)
```

#### "High memory usage"
**Solution**:
```python
# Use memory optimization
brain.optimizeMemoryUsage()

# Or reduce network size
config.set("neuron_count", 100)
```

#### "Performance issues"
**Solution**:
```python
# Profile performance
metrics = agent.profilePerformance(1000)
print(f"Performance: {metrics['steps_per_second']:.0f} steps/second")

# Optimize
brain.optimizeForPerformance()
```

## Research Applications

### Neuroscience Research

NLM enables neuroscience research through:

1. **Neural Network Analysis**
   - Study connectivity patterns
   - Analyze spike timing
   - Investigate plasticity rules

2. **Learning Mechanisms**
   - Test different learning algorithms
   - Study reward-based learning
   - Investigate curiosity-driven exploration

3. **Development Studies**
   - Model neural development
   - Study critical periods
   - Investigate maturation processes

### Artificial Intelligence

NLM provides AI research capabilities:

1. **Autonomous Agents**
   - Create intelligent agents
   - Study decision-making processes
   - Implement adaptive behavior

2. **Cognitive Architecture**
   - Build cognitive systems
   - Implement memory systems
   - Create planning algorithms

3. **Machine Learning**
   - Use spiking neural networks
   - Implement biological learning rules
   - Study emergent behaviors

### Computational Neuroscience

Applications in computational neuroscience:

1. **Brain Modeling**
   - Model neural circuits
   - Study information processing
   - Investigate network dynamics

2. **Neuropsychology**
   - Model cognitive functions
   - Study decision-making
   - Investigate memory processes

3. **Neural Engineering**
   - Design neural interfaces
   - Create brain-computer interfaces
   - Develop neuroprosthetics

## Conclusion

NLM provides a comprehensive platform for neural simulation, learning, and development research. The enhanced documentation and examples make it accessible to beginners while providing powerful tools for expert researchers.

### Next Steps

1. **For Beginners**: Start with the Quick Start Guide
2. **For Students**: Follow the Comprehensive Tutorial
3. **For Researchers**: Explore Expert Examples
4. **For Developers**: Study the Python API Reference
5. **For All Users**: Join the NLM community for support and collaboration

The NLM project continues to evolve with new features, improved documentation, and enhanced capabilities. Check the documentation regularly for updates.
