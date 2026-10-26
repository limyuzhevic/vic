# NLM (熙然) - Neural Learning Machine
# Comprehensive Documentation

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project that implements a neural system capable of learning, memory, prediction, and development through interaction with an environment. This document provides comprehensive guidance for using NLM for research, experimentation, and development.

## Table of Contents

1. [Getting Started](#getting-started)
2. [System Architecture](#system-architecture)
3. [Python API Guide](#python-api-guide)
4. [Command Line Interface](#command-line-interface)
5. [Advanced Usage Examples](#advanced-usage-examples)
6. [Configuration Reference](#configuration-reference)
7. [Error Handling and Troubleshooting](#error-handling-and-troubleshooting)
8. [Performance Optimization](#performance-optimization)
9. [Research Applications](#research-applications)
10. [Future Development](#future-development)

---

## Getting Started

### Quick Installation

The NLM project includes both Python bindings (pynlm) and native C++ components. For most users, the Python API provides the easiest way to start experimenting with neural simulation.

#### Prerequisites

- Python 3.8 or higher
- pip (Python package installer)
- C++ compiler (for native components)

#### Installation Steps

```bash
# Option 1: Install from source (recommended for development)
pip install -e .

# Option 2: Install from PyPI (if available)
pip install pynlm

# Verify installation
import pynlm
print(pynlm.__version__)
```

### Basic Usage Pattern

```python
import pynlm

# 1. Create configuration
config = pynlm.createDefaultConfig()

# 2. Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# 3. Create agent and world interface
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# 4. Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# 5. Run simulation
for step in range(1000):
    world.update(0.1)  # Update world state
    agent.processSensoryInput(world.getSensoryPercept())  # Process sensory input
    brain.step(step)  # Brain processes and learns
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Execute action

# 6. Analyze results
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Development stage: {brain.getDevelopmentalStage()}")
```

## System Architecture

NLM implements a comprehensive brain-inspired neural system with the following key components:

### Core Components

1. **Neural Processing**
   - Leaky Integrate-and-Fire (LIF) neurons
   - Event-driven spike propagation with delays
   - Multiple neuron types (sensory, motor, internal, modulatory)

2. **Memory Systems**
   - **Working Memory**: Transient active information storage
   - **Episodic Memory**: Experience storage and replay
   - **Associative Memory**: Pattern associations and recall

3. **Plasticity Mechanisms**
   - **STDP (Spike-Timing-Dependent Plasticity)**: Activity-dependent synaptic modification
   - **Hebbian Learning**: Co-activation based strengthening
   - **Structural Plasticity**: Synaptogenesis and pruning

4. **Neuromodulation**
   - **Dopamine**: Reward prediction error and reinforcement
   - **Curiosity**: Exploration motivation and novelty detection
   - **Novelty**: Unpredictability detection

5. **Prediction and Cognition**
   - **Prediction System**: Forward models and error signals
   - **Neural Planner**: Action planning and evaluation
   - **Concept Formation**: Pattern discovery and abstraction
   - **Attentional Selection**: Competitive selection of representations

6. **Development**
   - **Developmental Stages**: Initial, critical period, maturation, adult, aging
   - **Plasticity Modulation**: Age-dependent learning rates
   - **Experience-Dependent Maturation**

### System Flow

```
WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE
    ↓                                               ↓
  ACTION ← MOTORS ← BRAIN ACTIVITY ← MEMORY/PREDICTION
    ↓                                               ↓
 WORLD CONSEQUENCE → REWARD/SURPRISE → PLASTICITY
    ↓                                               ↓
MEMORY/DEVELOPMENT → CHANGED BRAIN → CHANGED FUTURE BEHAVIOR
```

## Python API Guide

### Core Classes

#### Brain Class

The `Brain` class is the central component representing the artificial neural system.

**Creation and Initialization**

```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create brain with specific configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.05)
brain = pynlm.createBrain(config)
brain.initialize()
```

**Simulation Methods**

```python
# Run simulation steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Reset brain for new episode
brain.reset()

# Save and load brain state
brain.save("checkpoint.bin")
# ... later ...
brain.load("checkpoint.bin")
```

**Region Management**

```python
# Add and access brain regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)

# Get all regions
regions = brain.getRegions()
for region_id in brain.getRegionIds():
    region = brain.getRegion(region_id)
    # ... process region ...
```

**Statistics and Monitoring**

```python
# Neural activity statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()} Hz")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")

# State information
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
print(f"Working memory traces: {brain.getWorkingMemory()->getActiveTraces()}")
print(f"Episodic memory episodes: {brain.getEpisodicMemory()->getEpisodeCount()}")
```

#### Config Class

The `Config` class manages brain and system configuration parameters.

**Configuration Operations**

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Query configuration values
if config.has("brain.neuron_count"):
    neurons = config.get("brain.neuron_count")
    if neurons:
        print(f"Neuron count: {neurons.value()}")

# Get configuration with defaults
neurons = config.getOr("brain.neuron_count", 1000)
print(f"Neuron count (default 1000): {neurons}")

# Modify configuration
config.set("brain.neuron_count", 5000)
config.set("brain.connection_probability", 0.05)
config.set("plasticity.stdp.enable", True)

# Save and load configuration
config.saveToFile("my_config.txt")
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.txt")
```

**Available Configuration Categories**

```python
# Brain configuration
brain.neuron_count                    # Total number of neurons
brain.synapse_density                 # Synapse connectivity density
brain.connection_probability         # Connection probability
brain.initial_weight_mean            # Initial synaptic weight mean
brain.initial_weight_std             # Initial synaptic weight std
brain.v_thresh                      # Neuron threshold potential
brain.v_rest                        # Resting potential
brain.v_reset                       # Reset potential after spike
brain.tau_mem                       # Membrane time constant
brain.tau_ref                       # Refractory period

# Plasticity configuration
plasticity.stdp.enable              # Enable/disable STDP
plasticity.stdp.learning_rate       # STDP learning rate
plasticity.stdp.tau_plus           # STDP time constant (potentiation)
plasticity.stdp.tau_minus          # STDP time constant (depression)
plasticity.hebbian.enable          # Enable/disable Hebbian learning
plasticity.hebbian.learning_rate   # Hebbian learning rate
plasticity.structural.enable        # Enable/disable structural plasticity

# Neuromodulation configuration
neuromod.dopamine.scale             # Dopamine modulation scale
neuromod.curiosity.enable           # Enable/disable curiosity
neuromod.curiosity.exploration_rate # Curiosity exploration rate
neuromod.novelty.enable             # Enable/disable novelty detection

# Development configuration
development.enable                   # Enable/disable development
development.initial_plasticity      # Initial plasticity level

development.plasticity_decay_rate   # Plasticity decay rate

development.critical_period_end     # Critical period end

development.adult_plasticity        # Adult plasticity level

# Memory configuration
memory.working.enable              # Enable/disable working memory
memory.working.trace_decay_rate    # Working memory trace decay
memory.episodic.enable             # Enable/disable episodic memory

memory.episodic.max_episodes       # Maximum episodic memory episodes

memory.associative.enable          # Enable/disable associative memory

memory.associative.association_strength # Association strength

# Simulation configuration
simulation.timestep                 # Simulation time step
simulation.max_steps                # Maximum simulation steps
simulation.random_seed              # Random seed for reproducibility
```

#### AgentBrain Class

The `AgentBrain` class bridges the brain with the world environment, handling sensory processing and motor output.

**Basic Usage**

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent with world
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
```

**Sensory and Motor Processing**

```python
# Complete agent simulation loop
for step in range(1000):
    # Update world state
    world.update(0.1)
    
    # Get sensory input from world
    percept = world.getSensoryPercept()
    
    # Process sensory input in brain
    agent.processSensoryInput(percept)
    
    # Run brain simulation
    brain.step(step)
    
    # Decode motor command from brain activity
    motor_cmd = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Log progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
```

**AgentBrain Methods**

```python
# AgentBrain convenience methods
agent.setLearningParameters(True, True, True, True)  # Enable all learning
reward, structural, development, curiosity = agent.getLearningParameters()

# Agent state and metrics
metrics = agent.getAgentState()  # Dictionary with agent state
status = agent.getAgentStatus()  # Formatted status report
metrics = agent.getMetrics()    # Metric structure

# Performance monitoring
steps = agent.getStepCount()
time = agent.getTotalSimulationTime()

# Error handling
if agent.hasErrors():
    print(f"Errors: {agent.getErrors()}")
    agent.clearErrors()
```

#### SimpleWorld Class

The `SimpleWorld` class represents the environment the agent interacts with.

**World Configuration**

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# World control
world.reset()
world.update(timestep=0.1)

# Object manipulation
obj = pynlm.WorldObject(x=5.0, y=5.0, type=pynlm.WorldObjectType.Resource, value=1.0)
world.addObject(obj)
world.removeObject(5.0, 5.0)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()
world.isValidPosition(5.0, 5.0)

# Properties
world.getWidth()
world.getHeight()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

#### SensoryInput and Action Classes

These classes represent sensory data and motor actions respectively.

```python
import pynlm

# Vision sensory input
vision = pynlm.Vision(width=8, height=8, channels=1)
vision.setData([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8,
                0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9,
                0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.1,
                0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.1, 0.2,
                0.5, 0.6, 0.7, 0.8, 0.9, 0.1, 0.2, 0.3,
                0.6, 0.7, 0.8, 0.9, 0.1, 0.2, 0.3, 0.4,
                0.7, 0.8, 0.9, 0.1, 0.2, 0.3, 0.4, 0.5,
                0.8, 0.9, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6])

# Action
move_forward = pynlm.Action(pynlm.ActionType.MoveForward)
action_with_params = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.3, 0.8])
```

### Enumerations

#### NeuronType

```python
# Neuron types
pynlm.NeuronType.Excitatory   # Excitatory neurons
pynlm.NeuronType.Inhibitory   # Inhibitory neurons
pynlm.NeuronType.Modulatory   # Modulatory neurons
pynlm.NeuronType.Sensory      # Sensory neurons
pynlm.NeuronType.Motor        # Motor neurons
pynlm.NeuronType.Internal    # Internal neurons
```

#### DevelopmentalStage

```python
# Developmental stages
pynlm.DevelopmentalStage.Initial        # Initial development
pynlm.DevelopmentalStage.CriticalPeriod # Critical period
pynlm.DevelopmentalStage.Maturation      # Maturation phase
pynlm.DevelopmentalStage.Adult          # Adult stage
pynlm.DevelopmentalStage.Aging         # Aging phase
```

#### ActionType

```python
# Action types
pynlm.ActionType.MoveForward  # Move forward
pynlm.ActionType.MoveBackward # Move backward
pynlm.ActionType.TurnLeft     # Turn left
pynlm.ActionType.TurnRight    # Turn right
pynlm.ActionType.Look         # Look (pan sensors)
pynlm.ActionType.Interact    # Interact with objects
pynlm.ActionType.Eat          # Eat/consume
pynlm.ActionType.Drink        # Drink
pynlm.ActionType.Rest         # Rest/idle
pynlm.ActionType.Wait         # Wait (do nothing)
```

#### WorldObjectType

```python
# World object types
pynlm.WorldObjectType.Empty    # Empty space
pynlm.WorldObjectType.Resource # Resource (positive reward)
pynlm.WorldObjectType.Hazard  # Hazard (negative reward)
pynlm.WorldObjectType.Wall    # Wall/boundary
pynlm.WorldObjectType.Marker  # Navigation aid
```

## Command Line Interface

The NLM project includes a command-line interface for easier experimentation and setup.

### Basic Usage

```bash
# Show help
nlm --help

# Run demonstration mode (simple example)
nlm --demo

# Run interactive mode (with keyboard controls)
nlm --interactive

# Run with specific configuration
nlm --brain 1000 --steps 500 --world-size 10 10

# Enable learning features
nlm --curiosity --development --reward --episodes 10

# Load checkpoint
nlm --load my_checkpoint.bin

# Save checkpoint
nlm --checkpoint my_checkpoint.bin
```

### Command Line Options

#### Brain Configuration

```bash
--brain NEURONS
    Set number of neurons (default: 1000)

--steps STEPS
    Set number of simulation steps (default: 1000)

--episodes EPISODES
    Set number of episodes (default: 1)
```

#### World Configuration

```bash
--world-size WIDTH HEIGHT
    Set world dimensions (default: 20 20)

--vision-size WV WV
    Set vision dimensions (default: 8 8)
```

#### Feature Options

```bash
--curiosity
    Enable curiosity-driven exploration

--development
    Enable developmental plasticity

--reward
    Enable reward modulation

--checkpoint PATH
    Save checkpoint to PATH

--load PATH
    Load checkpoint from PATH
```

#### Modes

```bash
--demo
    Run demonstration mode (simple example)

--interactive
    Interactive mode (press 'q' to quit)

--benchmark
    Run benchmark mode (performance testing)
```

### Interactive Mode Controls

In interactive mode, use the following keyboard commands:

| Key | Action |
|-----|--------|
| `i` | Inject current into sensory neurons |
| `m` | Modify motor neuron activity |
| `r` | Reset brain |
| `p` | Print brain statistics |
| `q` | Quit interactive mode |

### Configuration File Format

Configuration files use a simple key=value format:

```
# Example configuration file
brain.neuron_count = 1000
brain.connection_probability = 0.05
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
neuromod.curiosity.enable = true
development.enable = true
simulation.timestep = 0.001
simulation.max_steps = 1000
```

## Advanced Usage Examples

### Example 1: Curious Agent in a Resource-Rich Environment

```python
import pynlm
import random

def createResourceWorld(width, height):
    """Create a world with random resources."""
    world = pynlm.createSimpleWorld()
    world.configure(width=width, height=height, visionWidth=8, visionHeight=8)
    
    # Add resources at random positions
    for _ in range(10):
        x = random.uniform(1, width-1)
        y = random.uniform(1, height-1)
        obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
        world.addObject(obj)
    
    # Add hazards at random positions
    for _ in range(5):
        x = random.uniform(1, width-1)
        y = random.uniform(1, height-1)
        obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=-0.5)
        world.addObject(obj)
    
    return world

def runCuriousAgent(num_steps=1000):
    """Run a curious agent in a resource-rich environment."""
    # Create brain with enhanced curiosity
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.exploration_rate", 0.5)
    config.set("development.enable", True)
    config.set("development.initial_plasticity", 1.2)  # Higher initial plasticity
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create curious agent
    agent = pynlm.createAgentBrain(brain)
    world = createResourceWorld(20, 20)
    world.reset()
    agent.initialize(world)
    
    # Enable all learning features for curious exploration
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Run simulation
    print("Running curious agent in resource-rich environment...")
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Compute reward (resources = positive, hazards = negative)
        percept = world.getSensoryPercept()
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Log progress
        if step % 200 == 0:
            print(f"Step {step}: Reward {reward:.2f}, "
                  f"Curiosity {agent.getCuriosityLevel():.3f}, "
                  f"Development {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

# Run the example
brain, agent, world = runCuriousAgent(1000)
```

### Example 2: Developmental Learning Agent

```python
import pynlm

def runDevelopmentalLearning():
    """Run an agent that undergoes developmental learning stages."""
    # Create configuration that emphasizes development
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("development.enable", True)
    config.set("development.initial_plasticity", 1.5)  # High initial plasticity
    config.set("development.critical_period_end", 200.0)  # Longer critical period
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.002)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world with challenges
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print("Running developmental learning agent...")
    print("Expected: High initial plasticity, learning through experience")
    
    # Track development stages
    stages = []
    for step in range(1500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Compute reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Record developmental stage
        stage = brain.getDevelopmentalStage()
        if stage not in stages:
            stages.append(stage)
            print(f"New developmental stage reached: {stage} at step {step}")
        
        if step % 300 == 0:
            print(f"Step {step}: Stage {stage}, "
                  f"Plasticity {agent.getPlasticityModifier():.3f}, "
                  f"Curiosity {agent.getCuriosityLevel():.3f}")
    
    return brain, agent, world, stages

# Run developmental learning example
brain, agent, world, stages = runDevelopmentalLearning()
```

### Example 3: Multiple Episode Learning

```python
import pynlm

def runMultipleEpisodes(num_episodes=5):
    """Run multiple episodes of learning to track progress over time."""
    all_brains = []
    all_agents = []
    all_worlds = []
    
    for episode in range(num_episodes):
        print(f"\n=== Episode {episode + 1}/{num_episodes} ===")
        
        # Create fresh brain for each episode (or load from checkpoint)
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("simulation.max_steps", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent with learning capabilities
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        world.reset()
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run episode
        print(f"Running episode {episode + 1}...")
        for step in range(500):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Reward based on agent performance
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            if step % 100 == 0:
                print(f"  Step {step}: Reward {reward:.2f}, "
                      f"Curiosity {agent.getCuriosityLevel():.3f}")
        
        # Save episode results
        all_brains.append(brain)
        all_agents.append(agent)
        all_worlds.append(world)
        
        print(f"Episode {episode + 1} completed")
        print(f"  Total reward: {brain.getTotalSpikeCount()} spikes")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    return all_brains, all_agents, all_worlds

# Run multiple episodes example
brains, agents, worlds = runMultipleEpisodes(3)
```

### Example 4: Brain Checkpointing and Resumption

```python
import pynlm
import os

def runCheckpointingExample():
    """Demonstrate brain checkpointing and resumption."""
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    
    # Run some simulation steps
    print("Running initial simulation...")
    for step in range(200):
        brain1.step(step)
        if step % 50 == 0:
            print(f"  Step {step}: {brain1.getFiringNeuronCount()} firing neurons")
    
    # Save checkpoint
    checkpoint_file = "nlm_checkpoint.bin"
    if brain1.save(checkpoint_file):
        print(f"Checkpoint saved to {checkpoint_file}")
    else:
        print("Failed to save checkpoint")
        return
    
    # Simulate some time passing (create new brain)
    print("Creating new brain from scratch...")
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    
    # Load checkpoint into new brain
    if brain2.load(checkpoint_file):
        print("Checkpoint loaded successfully!")
        print("Brain state restored to previous simulation state")
    else:
        print("Failed to load checkpoint")
        return
    
    # Continue simulation from checkpoint
    print("Continuing simulation from checkpoint...")
    for step in range(200, 400):
        brain2.step(step)
        if step % 50 == 0:
            print(f"  Step {step}: {brain2.getFiringNeuronCount()} firing neurons")
    
    # Compare results
    print(f"\nComparison:")
    print(f"  Initial brain spikes: {brain1.getTotalSpikeCount()}")
    print(f"  Loaded brain spikes: {brain2.getTotalSpikeCount()}")
    print(f"  Spike difference: {brain2.getTotalSpikeCount() - brain1.getTotalSpikeCount()}")
    
    # Cleanup
    if os.path.exists(checkpoint_file):
        os.remove(checkpoint_file)
        print(f"Cleaned up checkpoint file")
    
    return brain1, brain2

# Run checkpointing example
brain1, brain2 = runCheckpointingExample()
```

## Configuration Reference

### Default Configuration Parameters

| Category | Parameter | Type | Default | Description |
|----------|-----------|------|---------|-------------|
| **Brain** | `brain.neuron_count` | int | 1000 | Total number of neurons |
| | `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| | `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| | `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| | `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| | `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| | `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| | `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| | `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| | `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (potentiation) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (depression) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.001 | Hebbian learning rate |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.synaptogenesis_rate` | float | 0.0001 | Synaptogenesis rate |
| `plasticity.structural.pruning_rate` | float | 0.00001 | Pruning rate |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.exploration_rate` | float | 0.3 | Curiosity exploration rate |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.sensitivity` | float | 1.0 | Novelty detection sensitivity |

### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.enable` | bool | true | Enable developmental processes |
| `development.initial_plasticity` | float | 1.0 | Initial plasticity level |
| `development.plasticity_decay_rate` | float | 0.95 | Plasticity decay rate |
| `development.critical_period_end` | float | 300.0 | Critical period end time |
| `development.adult_plasticity` | float | 0.2 | Adult plasticity level |

### Memory Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working.enable` | bool | true | Enable working memory |
| `memory.working.trace_decay_rate` | float | 0.99 | Working memory trace decay rate |
| `memory.episodic.enable` | bool | true | Enable episodic memory |
| `memory.episodic.max_episodes` | int | 100 | Maximum number of episodic memory episodes |
| `memory.associative.enable` | bool | true | Enable associative memory |
| `memory.associative.association_strength` | float | 0.5 | Association strength |

### Simulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation.timestep` | float | 0.001 | Simulation time step (s) |
| `simulation.max_steps` | int | 1000 | Maximum simulation steps |
| `simulation.random_seed` | int | 42 | Random seed for reproducibility |

## Error Handling and Troubleshooting

### Common Issues and Solutions

#### Issue 1: Brain Initialization Fails

**Symptoms:**
- "Failed to initialize brain!"
- Segmentation fault during initialization
- No neurons or synapses created

**Possible Causes:**
- Invalid configuration parameters
- Memory allocation failure
- Circular dependencies in brain components

**Solutions:**
```python
# Check configuration validity
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 10)  # Start small
brain = pynlm.createBrain(config)

if (!brain->initialize()) {
    // Handle initialization failure
    return 1;
}
```

#### Issue 2: Memory Usage Too High

**Symptoms:**
- High memory consumption
- Slow performance
- System freezes

**Possible Causes:**
- Too many neurons or synapses
- Memory pools not properly managed
- Inefficient data structures

**Solutions:**
```python
# Use performance-optimized configuration
config = pynlm.createPerformanceConfig();

# Limit brain size
config.set("brain.neuron_count", 500);
config.set("memory.episodic.max_episodes", 50);

# Disable expensive features
config.set("plasticity.structural.enable", false);
config.set("memory.associative.enable", false);
```

#### Issue 3: Sensory Input Processing Fails

**Symptoms:**
- No sensory input received
- Agent doesn't respond to environment
- All actions are "Wait"

**Possible Causes:**
- World not properly initialized
- Agent not connected to world
- Sensory configuration incorrect

**Solutions:**
```python
# Ensure proper initialization
world = pynlm.createSimpleWorld();
world.configure(width=10, height=10, visionWidth=4, visionHeight=4);
world.reset();

agent = pynlm.createAgentBrain(brain);
agent.initialize(world);

# Verify sensory input size
input_size = agent.getSensoryInputSize();
percept = world.getSensoryPercept();
print(f"Input size: {input_size}");
print(f"Vision size: {percept.getVisionWidth()}x{percept.getVisionHeight()}");
```

#### Issue 4: Learning Doesn't Occur

**Symptoms:**
- Synaptic weights don't change
- No learning over time
- Agent behavior doesn't improve

**Possible Causes:**
- Plasticity disabled
- Learning rate too low
- No sufficient stimulation
- Reward system not properly connected

**Solutions:**
```python
# Enable learning features
agent.enableRewardModulation(true);
agent.enableCuriosity(true);
agent.enableDevelopment(true);
agent.enableStructuralPlasticity(true);

# Increase learning rate
config.set("plasticity.stdp.learning_rate", 0.01);
config.set("plasticity.hebbian.learning_rate", 0.01);

# Provide more stimulation
brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
```

### Error Handling Best Practices

1. **Always Check Return Values**

```python
if (!brain->initialize()) {
    // Handle initialization failure
    return 1;
}
```

2. **Use Configuration Validation**

```python
Config config = pynlm.createDefaultConfig();
if (!config.validate()) {
    std::cerr << "Configuration validation failed: " << config.getValidationErrors() << std::endl;
    return 1;
}
```

3. **Implement Error Recovery**

```python
// Check for errors and attempt recovery
if (agent.hasErrors()) {
    std::string errors = agent.getErrors();
    // Attempt recovery actions
    agent.clearErrors();
    agent.reset();
    agent.initialize(world);
}
```

4. **Log Important Events**

```python
NLM_LOG_INFO("Starting simulation");
NLM_LOG_INFO("Brain initialized successfully");
NLM_LOG_INFO("Agent ready for interaction");
```

### Debug Information

NLM provides extensive logging and debug information. To enable debug logging:

```python
#include "core/Logger/Logger.hpp"
nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Debug);
```

Or via Python API:

```python
import pynlm
# Enable debug logging through configuration
config = pynlm.createDefaultConfig()
config.set("core.log_level", "Debug")
```

### Performance Monitoring

Monitor system performance using built-in metrics:

```python
// Brain metrics
std::cout << "Brain performance:" << std::endl;
std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;

// Agent metrics
std::cout << "\nAgent performance:" << std::endl;
std::cout << "  Curiosity level: " << agent.getCuriosityLevel() << std::endl;
std::cout << "  Novelty level: " << agent.getNoveltyLevel() << std::endl;
std::cout << "  Development stage: " << brain->getDevelopmentalStage() << std::endl;
std::cout << "  Step count: " << agent.getStepCount() << std::endl;
```

## Performance Optimization

### Optimizing Configuration

1. **Choose Appropriate Brain Size**

```python
// For real-time applications, use smaller brains
config.set("brain.neuron_count", 200);
config.set("simulation.max_steps", 500);

// For high-fidelity simulations, use larger brains
config.set("brain.neuron_count", 5000);
config.set("simulation.max_steps", 5000);
```

2. **Enable Performance Features**

```python
// Use performance-optimized configuration
config = pynlm.createPerformanceConfig();

// Disable computationally expensive features
config.set("plasticity.structural.enable", false);
config.set("memory.associative.enable", false);
config.set("memory.episodic.max_episodes", 50);
```

3. **Adjust Plasticity Parameters**

```python
// Reduce plasticity computational cost
config.set("plasticity.stdp.learning_rate", 0.0001);  // Slower learning
config.set("plasticity.hebbian.learning_rate", 0.0001);
```

### Memory Management

1. **Use Appropriate Memory Pools**

```python
// Configure memory systems for performance
config.set("memory.working.trace_decay_rate", 0.999);  // Faster decay
config.set("memory.episodic.max_episodes", 20);        // Limit episodes
```

2. **Monitor Memory Usage**

```python
// Track memory usage during simulation
for (step in range(simulation_steps)) {
    // Check memory usage periodically
    if (step % 1000 == 0) {
        print("Memory usage check...");
        // Implement memory monitoring
    }
    
    brain.step(step);
}
```

### Computational Efficiency

1. **Use SIMD Optimizations**

The NLM codebase includes SIMD optimizations for neural computations. Ensure your configuration enables these optimizations.

2. **Parallel Processing**

For large-scale simulations, consider using parallel processing:

```python
// Configure for parallel processing
config.set("core.use_parallel_processing", true);
config.set("core.num_threads", 4);
```

3. **Batch Processing**

Process multiple steps in batches when possible:

```python
// Batch simulation steps
for (batch_start = 0; batch_start < max_steps; batch_start += batch_size) {
    int batch_end = std::min(batch_start + batch_size, max_steps);
    
    for (int step = batch_start; step < batch_end; ++step) {
        brain.step(step);
    }
    
    // Process batch results
    processBatchResults(batch_start, batch_end);
}
```

### Profiling and Benchmarking

1. **Built-in Benchmarking**

Use the command-line benchmark mode:

```bash
nlm --benchmark --brain 1000 --steps 1000
```

2. **Custom Profiling**

Implement custom profiling for your specific use case:

```cpp
#include <chrono>

// Time critical sections
auto start_time = std::chrono::high_resolution_clock::now();

// Run simulation
for (int step = 0; step < 1000; ++step) {
    brain.step(step);
}

auto end_time = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

std::cout << "Simulation time: " << duration.count() << " ms" << std::endl;
std::cout << "Average time per step: " << (duration.count() / 1000.0) << " ms" << std::endl;
```

## Research Applications

### Experiment 1: Studying Neural Development

**Objective:** Investigate how developmental stages affect learning capabilities.

**Experimental Setup:**

```python
import pynlm

def developmentalLearningExperiment():
    """Study the effects of developmental stages on learning."""
    // Create brain with developmental emphasis
    config = pynlm.createDefaultConfig();
    config.set("development.enable", true);
    config.set("development.initial_plasticity", 1.5);
    config.set("development.critical_period_end", 150.0);
    
    brain = pynlm.createBrain(config);
    brain.initialize();
    
    // Create challenging environment
    world = pynlm.createSimpleWorld();
    world.configure(width=12, height=12, visionWidth=8, visionHeight=8);
    world.reset();
    
    agent = pynlm.createAgentBrain(brain);
    agent.initialize(world);
    
    // Enable all learning features
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    agent.enableDevelopment(true);
    agent.enableStructuralPlasticity(true);
    
    // Track developmental stages
    developmental_stages = [];
    learning_rates = [];
    
    for step in range(1000):
        world.update(0.1);
        agent.processSensoryInput(world.getSensoryPercept());
        brain.step(step);
        action = agent.decodeMotorCommand();
        world.applyMotorCommand(action, world.getSimulationTime());
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0;
        agent.applyRewardModulation(reward, 0.0);
        agent.updateDevelopment(0.1);
        
        // Record data
        developmental_stages.append(brain.getDevelopmentalStage());
        learning_rates.append(calculateLearningRate(brain, agent));
        
        if step % 100 == 0:
            print(f"Step {step}: Stage {brain.getDevelopmentalStage()}, "
                  f"Learning {calculateLearningRate(brain, agent):.4f}");
    
    return developmental_stages, learning_rates;
```

### Experiment 2: Multi-Agent Competition

**Objective:** Study competitive behaviors in multi-agent systems.

**Experimental Setup:**

```python
import pynlm

def multiAgentCompetitionExperiment(num_agents=3):
    """Run competition between multiple agents."""
    // Create shared environment
    world = pynlm.createSimpleWorld();
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8);
    
    // Add resources and hazards
    for _ in range(15):
        x = random.uniform(2, 18);
        y = random.uniform(2, 18);
        obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0);
        world.addObject(obj);
    
    // Create competing agents
    agents = [];
    for i in range(num_agents):
        config = pynlm.createDefaultConfig();
        config.set("brain.neuron_count", 300);
        config.set("neuromod.curiosity.exploration_rate", random.uniform(0.1, 0.5));
        
        brain = pynlm.createBrain(config);
        brain.initialize();
        
        agent = pynlm.createAgentBrain(brain);
        agent.initialize(world);
        
        agent.enableRewardModulation(true);
        agent.enableCuriosity(true);  // Each agent has different curiosity levels
        agent.enableDevelopment(true);
        
        agents.append(agent);
    
    // Run competition
    print(f"Competition between {num_agents} agents...");
    for step in range(500):
        world.update(0.1);
        
        // Each agent processes sensory input and acts
        for agent in agents:
            agent.processSensoryInput(world.getSensoryPercept());
            brain.step(step);
            action = agent.decodeMotorCommand();
            world.applyMotorCommand(action, world.getSimulationTime());
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0;
            agent.applyRewardModulation(reward, 0.0);
            agent.updateDevelopment(0.1);
        
        // Check for resources/hazards collision
        // Update world state
        
        if step % 100 == 0:
            // Print competition status
            for i, agent in enumerate(agents):
                brain = agent.getBrain();
                print(f"Agent {i}: {brain.getTotalSpikeCount()} spikes, "
                      f"Stage {brain.getDevelopmentalStage()}");
    
    return agents;
```

### Experiment 3: Memory Consolidation

**Objective:** Study memory consolidation and replay mechanisms.

**Experimental Setup:**

```python
import pynlm

def memoryConsolidationExperiment():
    """Study memory consolidation and replay."""
    // Create brain with episodic memory
    config = pynlm.createDefaultConfig();
    config.set("memory.episodic.enable", true);
    config.set("memory.episodic.max_episodes", 50);
    config.set("simulation.max_steps", 800);
    
    brain = pynlm.createBrain(config);
    brain.initialize();
    
    // Create world with memorable events
    world = pynlm.createSimpleWorld();
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6);
    world.reset();
    
    // Add memorable objects (resources that create strong associations)
    for i in range(10):
        x = 5 + i * 0.8;
        y = 7.5;
        obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=2.0);
        world.addObject(obj);
    
    agent = pynlm.createAgentBrain(brain);
    agent.initialize(world);
    
    // Enable all memory systems
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    agent.enableDevelopment(true);
    
    // Track memory consolidation
    memory_metrics = [];
    
    for step in range(800):
        world.update(0.1);
        agent.processSensoryInput(world.getSensoryPercept());
        brain.step(step);
        action = agent.decodeMotorCommand();
        world.applyMotorCommand(action, world.getSimulationTime());
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0;
        agent.applyRewardModulation(reward, 0.0);
        agent.updateDevelopment(0.1);
        
        // Record memory metrics
        auto* episodic_mem = brain.getEpisodicMemory();
        if (episodic_mem) {
            memory_metrics.append({
                "step": step,
                "episode_count": episodic_mem.getEpisodeCount(),
                "working_memory_traces": brain.getWorkingMemory()->getActiveTraces(),
                "firing_rate": brain.getAverageFiringRate(),
                "curiosity": agent.getCuriosityLevel()
            });
        }
        
        if step % 200 == 0:
            print(f"Step {step}: {brain.getTotalSpikeCount()} spikes, "
                  f"{brain.getEpisodicMemory()->getEpisodeCount()} episodes, "
                  f"Curiosity {agent.getCuriosityLevel():.3f}");
    
    return brain, agent, world, memory_metrics;
```

## Future Development

### Planned Enhancements

1. **Advanced Neural Architectures**
   - Integration with transformer-like architectures
   - Attention mechanisms for cognitive control
   - Hierarchical neural processing

2. **Enhanced Learning Mechanisms**
   - Meta-learning and few-shot learning
   - Transfer learning across tasks
   - Lifelong learning capabilities

3. **Expanded Environment Support**
   - 3D environments
   - Continuous action spaces
   - Multi-agent social learning

4. **Improved Visualization**
   - Real-time neural activity visualization
   - Memory trace visualization
   - Development trajectory plotting

5. **Distributed Computing**
   - Multi-GPU support
   - Cloud-based simulation
   - Edge computing capabilities

### Research Collaboration

NLM is designed for research collaboration. Key features for collaborative research:

1. **Modular Architecture**: Easy to extend with new neural components
2. **Flexible Configuration**: Simple to modify experimental parameters
3. **Comprehensive Logging**: Detailed recording of all neural activity
4. **Checkpointing Support**: Easy sharing of experimental states
5. **Performance Monitoring**: Built-in benchmarking tools

### Community Resources

For additional resources and community support:

1. **Documentation**: Visit the NLM documentation website
2. **Source Code**: Available on GitHub with contribution guidelines
3. **Examples**: Comprehensive example gallery
4. **Tutorials**: Step-by-step tutorials for different use cases
5. **Support**: Community forums and issue tracking

### Citation

When using NLM in research publications, please cite:

```
@misc{nlm2026project,
    title={NLM (熙然) - Neural Learning Machine},
    author={NLM Research Team},
    year={2026},
    note={Experimental artificial developmental brain project},
    url={https://nlm-project.github.io}
}
```

### License

NLM is released under the MIT License. See the LICENSE file for details.

---

## Conclusion

NLM (熙然) provides a comprehensive platform for studying artificial developmental brains and neural systems. This documentation covers the main features, usage patterns, and advanced applications of the NLM project. Whether you're a researcher, educator, or developer, NLM offers powerful tools for exploring the principles of neural computation, learning, and development.

The combination of a rich Python API, command-line interface, and native C++ performance makes NLM suitable for a wide range of applications, from educational demonstrations to cutting-edge research.

We hope you find NLM useful for your neural computation research and development endeavors!

---

*Document version: 2.0.0*  
*Last updated: October 26, 2026*  
*© 2026 NLM Research Team*