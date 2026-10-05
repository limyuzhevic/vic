# NLM (熙然) - Neural Learning Machine Documentation

**An Experimental Artificial Developmental Brain**

## Overview

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

---

## Quick Start

### Installation

```bash
# Install from source
pip install .

# Or install in development mode
pip install -e .
```

### Basic Usage

```python
import pynlm

# Create a default brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
```

---

## Table of Contents

1. [Getting Started](#getting-started)
2. [Core Concepts](#core-concepts)
3. [Python API Reference](#python-api-reference)
   - [Config Class](#config-class)
   - [Brain Class](#brain-class)
   - [AgentBrain Class](#agentbrain-class)
   - [SimpleWorld Class](#simpleworld-class)
4. [Examples](#examples)
5. [Advanced Features](#advanced-features)
6. [Configuration Options](#configuration-options)
7. [Troubleshooting](#troubleshooting)
8. [Architecture](#architecture)
9. [References](#references)

---

## Getting Started

### What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

### Installation

NLM comes with Python bindings that make it easy to use. Install via pip:

```bash
pip install pynlm
```

For the latest development version:

```bash
pip install -e .
```

### Your First Brain

Here's the simplest possible example:

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

### Running a Complete Agent Simulation

For a more complete example with environment interaction:

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
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

---

## Core Concepts

### The NLM Architecture

NLM implements a brain-inspired neural network with the following components:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

### Neural Components

- **Neurons**: Leaky Integrate-and-Fire (LIF) units with membrane potential dynamics
- **Synapses**: Connections between neurons with Hebbian and STDP plasticity
- **Populations**: Groups of neurons with shared properties
- **Regions**: Functional subdivisions of the brain

### Learning Mechanisms

- **STDP (Spike-Timing-Dependent Plasticity)**: Learning based on precise spike timing
- **Hebbian Learning**: Co-activation strengthens connections
- **Structural Plasticity**: Creation and elimination of synapses
- **Neuromodulation**: Dopamine, curiosity, and novelty signals
- **Development**: Critical periods and maturation processes

### Memory Systems

- **Working Memory**: Transient active information
- **Episodic Memory**: Storage of experiences
- **Associative Memory**: Pattern associations
- **Procedural Memory**: Motor skills and habits

### Cognitive Abilities

- **Prediction**: Anticipating future states
- **Planning**: Selecting optimal action sequences
- **Concept Formation**: Discovering abstract patterns
- **Attention**: Selective focus through competition
- **Self-Model**: Understanding body schema
- **Social Learning**: Imitating others
- **Continual Learning**: Adapting to new tasks

---

## Python API Reference

### Config Class

Configuration management for NLM systems:

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Query configuration
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify configuration
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

**Key Configuration Parameters:**

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Brain Class

The central neural simulation brain class:

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

### AgentBrain Class

Connects NLM brain to the world:

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

### SimpleWorld Class

Virtual 2D world for NLM simulation:

```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)
```

### Key Classes and Methods

#### Sensory Inputs

| Class | Purpose | Example |
|-------|---------|---------|
| `Vision` | Visual input (grayscale) | `Vision(width=8, height=8)` |
| `Audio` | Audio input | `Audio(sampleRate=16000, numSamples=160)` |
| `InternalSignals` | Internal homeostatic signals | `InternalSignals()` |
| `SensoryInput` | Base class for all sensory inputs | `SensoryInput()` |

#### Enumerations

| Enumeration | Values | Description |
|-------------|--------|-------------|
| `NeuronType` | Excitatory, Inhibitory, Sensory, Motor, etc. | Types of neurons |
| `DevelopmentalStage` | Initial, CriticalPeriod, Maturation, Adult, Aging | Brain development stages |
| `ActionType` | MoveForward, TurnLeft, Interact, etc. | Motor action types |
| `MotorCommand` | MoveForward, MoveBackward, TurnLeft, TurnRight, etc. | Low-level motor commands |
| `WorldObjectType` | Empty, Resource, Hazard, Wall, Marker | Object types in world |

---

## Examples

### Example 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Example 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Example 3: Complete Agent with Learning

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
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
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
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
        
        # Optional: Apply reward modulation
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
    
    return brain, world, agent

# Run the simulation
run_agent_simulation(1000)
```

---

## Advanced Features

### Neural Network Architectures

NLM supports various network architectures:

```python
# Multiple regions for different functions
region_cortex = brain.addRegion("cortex")
region_hippocampus = brain.addRegion("hippocampus")
region_amygdala = brain.addRegion("amygdala")

# Inter-region connections for communication
brain.addInterRegionConnection(region_cortex, region_hippocampus, weight=0.5)
brain.addInterRegionConnection(region_hippocampus, region_cortex, weight=0.3)
```

### Advanced Configuration

```python
# Complex configuration with multiple parameters
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.05)
config.set("brain.connection_probability", 0.1)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("brain.development.enabled", True)
config.set("brain.development.critical_period_length", 1000)
```

### Brain Loading and Saving

```python
# Save brain state for later resumption
brain.save("my_brain_checkpoint.bin")

# Load brain state (useful for continuing training)
new_brain = pynlm.createBrain(config)
new_brain.load("my_brain_checkpoint.bin")
new_brain.initialize()
```

### Agent Brain Features

```python
# Create agent with specific configuration
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable specific learning subsystems
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# Check current status
print("Reward modulation:", agent.isRewardModulationEnabled())
print("Structural plasticity:", agent.isStructuralPlasticityEnabled())
print("Development:", agent.isDevelopmentEnabled())
print("Curiosity:", agent.isCuriosityEnabled())
```

### World Configuration

```python
# Create world with specific parameters
world = pynlm.createSimpleWorld()
world.configure(
    width=30,                    # World width
    height=30,                   # World height
    visionWidth=16,              # Agent vision width
    visionHeight=16,             # Agent vision height
)

# Set world properties
world.setMaxEnergy(100.0)       # Maximum energy for agent
world.setEnergyDecayRate(0.01)  # Energy decay per timestep
world.setRandomSeed(42)         # Reproducible simulations

# Add objects to world
world.addObject(pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Resource, value=1.0))
world.addObject(pynlm.WorldObject(20.0, 20.0, pynlm.WorldObjectType.Hazard, value=-0.5))
```

---

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.development.enabled` | bool | true | Enable brain development |
| `brain.development.critical_period_length` | int | 1000 | Length of critical period |
| `brain.development.maturation_rate` | float | 0.001 | Rate of maturation |
| `brain.development.plasticity_modifier` | float | 1.0 | Modifies plasticity with age |

---

## Troubleshooting

### Common Issues and Solutions

**"My brain isn't doing anything"**

- **Did you call `brain.initialize()`?** Always call `initialize()` before using the brain!
- **Try increasing the number of steps** Your brain needs time to "warm up"
- **Make sure sensory input is connected** Some neurons need input to fire

**"The agent isn't moving"**

- **Did you call `world.applyMotorCommand()`?** This is what actually moves the agent
- **Check that `world.update()` is being called** The world needs to be updated each timestep
- **Verify agent has motor neurons** The agent needs motor output to move

**"Everything is 0"**

- **Brains need time to "warm up"** - try more steps
- **Some neurons need input to fire** - make sure sensory input is connected
- **Check configuration parameters** - some may be too conservative

**"ImportError: No module named 'pynlm'"**

```bash
pip install --force-reinstall .
```

**"Segmentation faults"**

- **Check that `initialize()` is called before `step()`**
- **Ensure `reset()` is called before re-running simulation**
- **Verify all required libraries are installed** (especially C++ dependencies)

**Build Issues**

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

---

## Architecture

### NLM System Architecture

NLM implements a closed-loop brain-in-world architecture:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

### Components

#### Core Neural System
- **Neurons**: Leaky Integrate-and-Fire (LIF) units
- **Synapses**: Event-driven with delays and plasticity
- **Populations**: Functional groups of neurons
- **Regions**: Anatomical subdivisions

#### Memory Systems
- **Working Memory**: Transient information storage
- **Episodic Memory**: Experience recording
- **Associative Memory**: Pattern associations
- **Semantic Memory**: Abstract knowledge
- **Procedural Memory**: Skills and habits

#### Neuromodulation
- **Dopamine**: Reward prediction error
- **Curiosity**: Exploration motivation
- **Novelty**: Change detection
- **Prediction Error**: Surprise signals

#### Cognitive Systems
- **Neural Prediction**: Temporal and spatial expectations
- **Neural Planning**: Action sequence evaluation
- **Concept Formation**: Pattern discovery
- **Attention**: Competitive selection
- **Self-Model**: Body schema

#### Development System
- **Critical Periods**: Sensitive learning windows
- **Maturation**: Age-dependent changes
- **Structural Plasticity**: Growing and pruning connections
- **Homeostatic Regulation**: Maintaining stability

### Integration Architecture

Phase 6 achieved full integration of all subsystems:

1. **Memory Integration**: All memory systems connected to neural processing
2. **Neuromodulation Integration**: Neuromodulators affect plasticity and dynamics
3. **Prediction Integration**: Prediction as central organizing principle
4. **Development Integration**: Development stages modulate plasticity
5. **Persistence**: Checkpoint save/load working
6. **Replay and Consolidation**: Memory replay during rest

---

## References

1. **NLM Project Documentation**: https://nlm.readthedocs.io
2. **NLM GitHub Repository**: https://github.com/nlm-project/nlm
3. **Phase 6 Final Audit**: docs/PHASE6_FINAL_AUDIT.md
4. **Architecture Overview**: docs/ARCHITECTURE.md
5. **Scientific Background**: docs/SCIENCE.md

### Citing NLM

If you use NLM in your research, please cite:

```bibtex
@misc{nlm2024manual,
  title={NLM (熙然) - Neural Learning Machine Manual},
  author={NLM Authors},
  year={2024},
  note={Phase 6: Final Integration},
  url={https://github.com/nlm-project/nlm}
}
```

---

*This documentation is continuously updated. For the latest information, visit the NLM documentation portal.*

---

## Quick Reference

### Essential Functions

| Function | Purpose | Example |
|----------|---------|---------|
| `createDefaultConfig()` | Create default configuration | `config = pynlm.createDefaultConfig()` |
| `createBrain(config)` | Create neural brain | `brain = pynlm.createBrain(config)` |
| `brain.initialize()` | Initialize brain | `brain.initialize()` |
| `brain.step(step)` | Run one simulation step | `brain.step(i)` |
| `createSimpleWorld()` | Create simulation world | `world = pynlm.createSimpleWorld()` |
| `createAgentBrain(brain)` | Create agent interface | `agent = pynlm.createAgentBrain(brain)` |
| `agent.initialize(world)` | Initialize agent with world | `agent.initialize(world)` |
| `agent.processSensoryInput(percept)` | Process sensory input | `agent.processSensoryInput(percept)` |
| `agent.decodeMotorCommand()` | Get motor command | `action = agent.decodeMotorCommand()` |

### Common Parameters

| Parameter | Type | Typical Values |
|-----------|------|----------------|
| `neuron_count` | int | 100-10000 |
| `connection_probability` | float | 0.05-0.2 |
| `simulation_timestep` | float | 0.001-0.01 |
| `plasticity.learning_rate` | float | 0.001-0.01 |
| `world.width` | int | 10-100 |
| `world.height` | int | 10-100 |

---

*NLM Project - Phase 6 Final Integration Complete*  🌟