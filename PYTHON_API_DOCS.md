# NLM Python API - Comprehensive Documentation

## Overview

NLM (熙然) is a brain-inspired spiking neural network simulator written in C++20. The Python bindings (`pynlm`) provide access to the complete NLM Phase 6 artificial brain system, including:

- **Real spiking neural computation** with Leaky Integrate-and-Fire (LIF) neurons
- **Event-driven spike propagation** with synaptic delays
- **Multiple plasticity mechanisms** including STDP, Hebbian learning, and structural plasticity
- **Complete memory systems**: working memory, episodic memory, semantic memory, procedural memory, associative memory
- **Integrated neuromodulation**: dopamine, curiosity, novelty, prediction error
- **Cognitive systems**: neural planner, concept formation, attentional selection
- **Development system**: staged plasticity modulation and maturation
- **Prediction system**: sensory prediction and error computation
- **Checkpoint/save-load functionality**: persistence of brain states
- **Memory replay and consolidation**: during rest and periodically

This documentation covers the complete Python API with examples for beginners and advanced usage patterns.

## Table of Contents

1. [Getting Started](#getting-started)
2. [Core Classes](#core-classes)
3. [Brain Class](#brain-class)
4. [AgentBrain Class](#agentbrain-class)
5. [SimpleWorld Class](#simpleworld-class)
6. [Memory Systems](#memory-systems)
7. [Neuromodulation](#neuromodulation)
8. [Advanced Usage](#advanced-usage)
9. [Performance Tips](#performance-tips)
10. [Troubleshooting](#troubleshooting)
11. [API Reference](#api-reference)

---

# Getting Started

## Installation

```bash
# Install from source (recommended for development)
pip install -e .

# Or install from package
pip install pynlm
```

## Basic Import

```python
import pynlm

# Check version
print(f"NLM version: {pynlm.__version__}")
```

## Creating Your First Brain

This example creates a simple brain with 100 neurons and runs a basic simulation:

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)

# Print statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Spikes generated: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

## Creating a Complete Agent Environment

This example shows how to create a complete agent system with brain, world, and agent:

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

---

# Core Classes

## NeuronId

Unique identifier for a neuron. Used throughout the API to reference specific neurons.

```python
import pynlm

# Create neuron IDs
id1 = pynlm.NeuronId(1)
id2 = pynlm.NeuronId(2)

# Check equality
print(f"id1 == id2: {id1 == id2}")  # False
print(f"id1 == id1: {id1 == id1}")  # True

# Get index
print(f"id1.index(): {id1.index()}")  # 1
```

## SynapseId, RegionId, PopulationId

Similar to NeuronId, these are used for synapses, regions, and populations respectively.

## NeuronType (Enum)

Neuron types for classification:

```python
import pynlm

# Neuron type constants
print(f"Excitatory: {pynlm.NeuronType.Excitatory}")
print(f"Inhibitory: {pynlm.NeuronType.Inhibitory}")
print(f"Sensory: {pynlm.NeuronType.Sensory}")
print(f"Motor: {pynlm.NeuronType.Motor}")
```

## SynapseType (Enum)

Synapse connection types:

```python
import pynlm

print(f"Excitatory: {pynlm.SynapseType.Excitatory}")
print(f"Inhibitory: {pynlm.SynapseType.Inhibitory}")
print(f"Electrical: {pynlm.SynapseType.Electrical}")
```

## DevelopmentalStage (Enum)

Developmental stages for brain maturation:

```python
import pynlm

stages = [
    pynlm.DevelopmentalStage.Initial,
    pynlm.DevelopmentalStage.CriticalPeriod,
    pynlm.DevelopmentalStage.Maturation,
    pynlm.DevelopmentalStage.Adult,
    pynlm.DevelopmentalStage.Aging
]

for stage in stages:
    print(f"Stage: {stage}")
```

## FiringState (Enum)

Neuron firing states:

```python
import pynlm

print(f"Resting: {pynlm.FiringState.Resting}")
print(f"Active: {pynlm.FiringState.Active}")
print(f"Refractory: {pynlm.FiringState.Refractory}")
print(f"Inhibited: {pynlm.FiringState.Inhibited}")
```

---

# Brain Class

The `Brain` class is the central component of the NLM system. It contains all neural computation, memory systems, and cognitive functions.

## Creation and Initialization

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)

# Initialize brain (always call this before using)
brain.initialize()
```

## Basic Brain Operations

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

## Memory Systems Access

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

if working_memory:
    print(f"Working memory traces: {working_memory.getActiveTraces()}")

if episodic_memory:
    print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount()}")
```

## Sensory and Motor Operations

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create sensory input
vision = pynlm.Vision(width=8, height=8, channels=3)
vision_data = [0.5, 0.3, 0.8, 0.2, 0.6, 0.4, 0.9, 0.1] * 3  # 24 values for 8x8x3
vision.setData(vision_data)

# Inject sensory input into brain
brain.receiveSensoryInput(vision)

# Get motor output
action = brain.produceAction()
if action:
    print(f"Action type: {action.getType()}")
    print(f"Action parameters: {action.getParameters()}")
```

## Checkpoint Operations

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Save brain state
brain.save("my_brain_checkpoint.bin")

# Create new brain and load checkpoint
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()
brain2.load("my_brain_checkpoint.bin")
```

## Development and Neuromodulation

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Set developmental stage
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)

# Get current stage
stage = brain.getDevelopmentalStage()
print(f"Developmental stage: {stage}")

# Access neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionErrorSignal()

if dopamine:
    print(f"Dopamine level: {dopamine.getLevel():.3f}")

if curiosity:
    print(f"Curiosity level: {curiosity.getLevel():.3f}")
```

## Prediction System

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

prediction_system = brain.getPredictionSystem()
if prediction_system:
    print("Prediction system integrated")
    # The prediction system works with sensory input
    # and provides prediction error signals for learning
```

## Cognitive Systems

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access cognitive systems
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
attention = brain.getAttention()

if planner:
    print("Planner system available")
if concept_formation:
    print("Concept formation system available")
if attention:
    print("Attention system available")
```

---

# AgentBrain Class

The `AgentBrain` class provides an interface between the brain and the world environment, handling sensory processing and motor decoding.

## Creation and Initialization

```python
import pynlm

# Create brain first
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent with world
agent.initialize(world)
```

## Sensory Processing

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run sensory processing loop
for step in range(100):
    world.update(0.1)
    
    # Get what the agent sees
    percept = world.getSensoryPercept()
    
    # Tell the brain what it sees
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action from brain
    motor_cmd = agent.decodeMotorCommand()
    
    # Do action in world
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
```

## Neuromodulation

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Enable neuromodulation subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Apply reward modulation
reward = 1.0  # Positive reward
predicted_reward = 0.5
agent.applyRewardModulation(reward, predicted_reward)

# Get neuromodulation levels
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
```

## Development

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Update development
for step in range(0, 5000, 100):
    agent.updateDevelopment(0.1)
    
    if step % 1000 == 0:
        print(f"Step {step}, Dev stage: {agent.getDevelopmentalStage()}")
```

---

# SimpleWorld Class

The `SimpleWorld` class provides a 2D environment for the agent to interact with.

## Creation and Configuration

```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)
```

## World Operations

```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(5.0, 5.0)

# Set random seed for reproducible experiments
world.setRandomSeed(42)

# Update world state
world.update(timestep=0.1)

# Get current state
percept = world.getSensoryPercept()
body = world.getAgentBody()

print(f"Agent position: ({body.x}, {body.y})")
print(f"Agent orientation: {body.orientation} radians")
print(f"Agent energy: {body.energy}")
print(f"Agent health: {body.health}")
```

## Object Management

```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Add objects to world
resource = pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Resource, value=1.0)
world.addObject(resource)

hazard = pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Hazard, value=0.5, radius=1.0)
world.addObject(hazard)

# Check if position is valid
is_valid = world.isValidPosition(5.0, 5.0)
print(f"Position (5,5) is valid: {is_valid}")

# Remove object
world.removeObject(15.0, 15.0)
```

## World Properties

```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

print(f"World width: {world.getWidth()}")
print(f"World height: {world.getHeight()}")
print(f"World simulation time: {world.getSimulationTime():.2f}")
print(f"World max energy: {world.getMaxEnergy():.2f}")

# Set properties
world.setMaxEnergy(100.0)
world.setEnergyDecayRate(0.01)
```

---

# Memory Systems

NLM includes three main memory systems:

## Working Memory

Transient memory that maintains active information for immediate processing.

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

working_memory = brain.getWorkingMemory()
if working_memory:
    # Set capacity
    working_memory.setCapacity(100)
    
    # Store neuron activity
    working_memory.storeToNeuron(pynlm.NeuronId(1), 0.5)
    
    # Get active traces
    traces = working_memory.getActiveTraces()
    print(f"Working memory traces: {traces}")
```

## Episodic Memory

Storage of experiences as episodes with context and outcomes.

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    # Set maximum episodes
    episodic_memory.setMaxEpisodes(1000)
    
    # Run simulation to generate episodes
    for i in range(100):
        brain.step(i)
    
    # Get episode count
    episode_count = episodic_memory.getEpisodeCount()
    print(f"Episodic memory episodes: {episode_count}")
    
    # Get episodes for replay (used during rest/consolidation)
    episodes = episodic_memory.getEpisodesForReplay(3)
    print(f"Retrieved {len(episodes)} episodes for replay")
```

## Associative Memory

Pattern association and pattern completion capabilities.

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

associative_memory = brain.getAssociativeMemory()
if associative_memory:
    print("Associative memory system available")
    # Forms associations between patterns and experiences
    # Supports pattern completion for recall
```

---

# Neuromodulation

Neuromodulation systems provide dynamic regulation of neural processing based on environmental context and learning.

## Dopamine

Reward prediction and reinforcement learning:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

dopamine = brain.getDopamine()
if dopamine:
    print(f"Dopamine level: {dopamine.getLevel():.3f}")
    print(f"Dopamine plasticity factor: {dopamine.getPlasticityFactor():.3f}")
```

## Curiosity

Exploration motivation and intrinsic reward:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

curiosity = brain.getCuriosity()
if curiosity:
    print(f"Curiosity level: {curiosity.getLevel():.3f}")
    print(f"Novelty detection enabled: {curiosity.isNoveltyDetectionEnabled()}")
```

## Novelty
n
Novelty detection and surprise signals:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

novelty = brain.getNovelty()
if novelty:
    print(f"Novelty level: {novelty.getLevel():.3f}")
    print(f"Novelty threshold: {novelty.getThreshold():.3f}")
```

## Prediction Error

Mismatch between expected and actual outcomes:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

prediction_error = brain.getPredictionErrorSignal()
if prediction_error:
    print(f"Prediction error level: {prediction_error.getLevel():.3f}")
```

---

# Advanced Usage

## Complete Agent Training

This example shows how to train an agent using all available subsystems:

```python
import pynlm
import time

def train_agent(num_steps=10000):
    """Train an agent using all NLM Phase 6 subsystems."""
    
    # Initialize system
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print("Starting agent training...")
    print("Step | Reward | Firing Rate | Curiosity | Novelty | Dev Stage")
    print("-----|--------|-------------|-----------|---------|----------")
    
    start_time = time.time()
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Get reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress
        if step % 100 == 0:
            print(f"{step:4d} | {reward:6.3f} | {brain.getAverageFiringRate():11.2f} | "
                  f"{agent.getCuriosityLevel():9.3f} | {agent.getNoveltyLevel():6.3f} | "
                  f"{agent.getDevelopmentalStage():10}")
    
    end_time = time.time()
    
    print(f"\nTraining complete! ({end_time - start_time:.1f}s)")
    print(f"Final statistics:")
    print(f"  Total reward: {sum(agent.applyRewardModulation(0, 0) for _ in range(1)):.2f}")  # Note: this is a placeholder
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Working memory traces: {brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0}")
    print(f"  Episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")

# Run training
train_agent(10000)
```

## Configuration Customization

```python
import pynlm
import json

# Create configuration and set custom parameters
config = pynlm.createDefaultConfig()

# Modify brain parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.2)
config.set("brain.v_thresh", -50.0)

# Modify plasticity parameters
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Modify neuromodulation
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Save configuration
config.saveToFile("custom_config.cfg")

# Load configuration from file
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("custom_config.cfg")

# Create brain with custom config
brain = pynlm.createBrain(config2)
brain.initialize()
```

## Performance Monitoring

```python
import pynlm
import time

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Monitor performance over time
print("Performance monitoring...")
print("Step | Neurons | Synapses | Spikes/sec | Time (ms)")
print("-----|---------|----------|------------|----------")

for step in range(0, 5000, 100):
    start_time = time.time()
    
    for i in range(100):  # Run 100 substeps
        brain.step(step * 100 + i)
    
    end_time = time.time()
    
    elapsed = (end_time - start_time) * 1000  # Convert to milliseconds
    spikes_per_sec = brain.getTotalSpikeCount() / (elapsed / 1000)
    
    print(f"{step:4d} | {brain.getTotalNeuronCount():7d} | {brain.getTotalSynapseCount():8d} | "
          f"{spikes_per_sec:10.1f} | {elapsed:8.2f}")
```

---

# Performance Tips

## Optimization Strategies

1. **Batch processing**: Run multiple simulation steps in quick succession to reduce overhead
2. **Memory management**: Clear spike history periodically to prevent memory bloat
3. **Configuration tuning**: Adjust plasticity and neuromodulation parameters for your specific task
4. **Parallel processing**: Use multiple regions for parallel neural processing

## Common Performance Issues

1. **Memory leaks**: Ensure you clear spike history and other buffers periodically
2. **Inefficient plasticity**: Disable unused plasticity rules to reduce computation
3. **Poor connectivity**: Use appropriate connection probabilities for your network size
4. **Integration overhead**: Consider reducing neuromodulation complexity for real-time applications

## Profiling Recommendations

1. **Time critical sections**: Use timing to identify bottlenecks in your simulation loop
2. **Memory usage**: Monitor memory usage for long-running simulations
3. **Spike processing**: Profile spike event handling, as it's often a performance bottleneck
4. **Plasticity computation**: Optimize STDP and Hebbian plasticity calculations

---

# Troubleshooting

## Common Issues and Solutions

### "Brain initialization failed"

**Cause**: Configuration errors or missing files.

**Solution**:
```python
import pynlm

config = pynlm.createDefaultConfig()
print(config.summary())  # Check configuration

brain = pynlm.createBrain(config)
if (!brain.initialize()) {
    print("Brain initialization failed")
    // Check logs for details
}
```

### "No spikes generated"

**Cause**: Incorrect neuron parameters or insufficient input.

**Solution**:
```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
config.set("brain.v_thresh", -55.0)  // Lower threshold
config.set("brain.v_rest", -70.0)    // Standard resting potential

brain = pynlm.createBrain(config)
brain.initialize()

// Inject current to stimulate neurons
brain.injectCurrentToNeurons(pynlm.NeuronType::Sensory, 30.0f)
```

### "Memory systems not working"

**Cause**: Memory systems are only accessible if properly initialized.

**Solution**:
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

working_memory = brain.getWorkingMemory()
if working_memory:
    print("Working memory is available")
else:
    print("Working memory not available - check brain initialization")
```

### Checkpoint save/load failures

**Cause**: File system permissions or disk space.

**Solution**:
```python
import pynlm
import os

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

// Use absolute path and check permissions
checkpoint_path = "/tmp/test_checkpoint.bin"

// Ensure directory exists
os.makedirs(os.path.dirname(checkpoint_path), exist_ok=True)

if brain.save(checkpoint_path) {
    print("Checkpoint saved successfully")
} else {
    print("Checkpoint save failed - check file permissions and disk space")
}
```

## Error Messages

### Log Level Settings

```python
import pynlm

// Different log levels for debugging
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

// Set different log levels
logger.setLevel(pynlm.LogLevel::Debug)    // Most verbose
logger.setLevel(pynlm.LogLevel::Info)     // Default
logger.setLevel(pynlm.LogLevel::Warning)  // Only warnings and errors
logger.setLevel(pynlm.LogLevel::Error)    // Only errors
```

---

# API Reference

This section provides detailed documentation of all Python classes and methods.

## Config Class

### `createDefaultConfig()`

```python
config = pynlm.createDefaultConfig()
```

### `loadFromFile(filepath)`

Load configuration from a file.

**Parameters:**
- `filepath` (str): Path to configuration file

**Returns:** bool - True if successful, False otherwise

### `saveToFile(filepath)`

Save configuration to a file.

**Parameters:**
- `filepath` (str): Path to save configuration

**Returns:** bool - True if successful, False otherwise

### `has(key)`

Check if configuration key exists.

**Parameters:**
- `key` (str): Configuration key

**Returns:** bool - True if key exists, False otherwise

### `getKeys()`

Get all configuration keys.

**Returns:** List[str] - List of all configuration keys

## Brain Class

### `createBrain(config)`

Create a new brain with configuration.

**Parameters:**
- `config` (Config): Configuration for brain

**Returns:** Brain - New brain instance

### `initialize()`

Initialize the brain with configuration.

**Returns:** bool - True if successful, False otherwise

### `step(currentStep[, currentTime])`

Perform simulation step(s).

**Parameters:**
- `currentStep` (int): Current simulation step
- `currentTime` (float, optional): Current simulation time in seconds

### `receiveSensoryInput(input)`

Inject sensory input into the brain.

**Parameters:**
- `input` (SensoryInput): Sensory input data

### `save(filepath)`

Save brain state to file.

**Parameters:**
- `filepath` (str): Path to save brain state

**Returns:** bool - True if successful, False otherwise

### `load(filepath)`

Load brain state from file.

**Parameters:**
- `filepath` (str): Path to load brain state from

**Returns:** bool - True if successful, False otherwise

## AgentBrain Class

### `createAgentBrain(brain)`

Create agent brain interface.

**Parameters:**
- `brain` (Brain): Brain instance to interface with

**Returns:** AgentBrain - New agent brain instance

### `initialize(world)`

Initialize agent with world.

**Parameters:**
- `world` (SimpleWorld): World instance to interact with

### `processSensoryInput(percept)`

Process sensory percept and inject into brain.

**Parameters:**
- `percept` (SensoryPercept): Sensory percept data

### `decodeMotorCommand()`

Decode brain motor activity into motor command.

**Returns:** MotorCommand - Decoded motor command

### `applyRewardModulation(reward, predictedReward)`

Apply reward-based neuromodulation.

**Parameters:**
- `reward` (float): Actual reward received
- `predicted_reward` (float): Predicted reward

### `updateDevelopment(timestep)`

Update development system.

**Parameters:**
- `timestep` (float): Time step duration

## SimpleWorld Class

### `createSimpleWorld()`

Create a new simple world.

**Returns:** SimpleWorld - New world instance

### `configure(width, height, visionWidth, visionHeight)`

Configure world dimensions.

**Parameters:**
- `width` (int): World width
- `height` (int): World height
- `visionWidth` (int): Vision width
- `visionHeight` (int): Vision height

### `reset()`

Reset world to initial state.

### `setAgentStart(x, y)`

Set agent starting position.

**Parameters:**
- `x` (float): X coordinate
- `y` (float): Y coordinate

### `update(timestep)`

Update world state.

**Parameters:**
- `timestep` (float): Time step duration

### `applyMotorCommand(cmd, currentTime)`

Apply motor command to world.

**Parameters:**
- `cmd` (MotorCommand): Motor command to apply
- `currentTime` (float): Current simulation time

### `getSensoryPercept()`

Get sensory percept from world.

**Returns:** SensoryPercept - Current sensory percept

### `getAgentBody()`

Get agent body state.

**Returns:** AgentBody - Current agent body state

## Enumerations

### NeuronType
- `Excitatory`
- `Inhibitory`
- `Modulatory`
- `Sensory`
- `Motor`
- `Internal`

### SynapseType
- `Excitatory`
- `Inhibitory`
- `Modulatory`
- `Electrical`
- `GapJunction`

### DevelopmentalStage
- `Initial`
- `CriticalPeriod`
- `Maturation`
- `Adult`
- `Aging`

### FiringState
- `Resting`
- `Active`
- `Refractory`
- `Inhibited`

### ActionType
- `MoveForward`
- `MoveBackward`
- `MoveLeft`
- `MoveRight`
- `TurnLeft`
- `TurnRight`
- `Look`
- `LookUp`
- `LookDown`
- `Interact`
- `Eat`
- `Drink`
- `Rest`
- `Wait`
- `Custom`

### MotorCommand
- `MoveForward`
- `MoveBackward`
- `TurnLeft`
- `TurnRight`
- `LookLeft`
- `LookRight`
- `Interact`
- `Wait`

### WorldObjectType
- `Empty`
- `Resource`
- `Hazard`
- `Wall`
- `Marker`

---

This documentation provides comprehensive coverage of the NLM Python API. For the most up-to-date information, refer to the source code and release notes.

For additional support, please visit the NLM project documentation or GitHub repository.