# NLM - Comprehensive Python User Guide

## Overview

NLM (熙然) is an experimental artificial developmental brain simulation framework. This guide provides comprehensive examples for users at all skill levels, from complete beginners to advanced developers.

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [Core Concepts](#core-concepts)
3. [Beginner Examples](#beginner-examples)
4. [Intermediate Examples](#intermediate-examples)
5. [Advanced Examples](#advanced-examples)
6. [Configuration Guide](#configuration-guide)
7. [Troubleshooting](#troubleshooting)
8. [API Reference](#api-reference)

---

## Quick Start

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm as nlm
import numpy as np

# Create a brain with default configuration
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Run some computation
for i in range(100):
    brain.step(i)

print(f"Simulation completed: {brain.getTotalSpikeCount()} spikes")
```

---

## Core Concepts

### The Three Main Components

NLM works with three core components:

1. **Brain**: The neural network that processes information
2. **World**: The environment that the brain interacts with
3. **Agent**: The interface that connects brain to world

```python
# Create all three components
brain = nlm.createBrain(nlm.createDefaultConfig())
world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)

# Initialize the agent with the world
agent.initialize(world)
```

### The Brain Loop

The basic cycle of interaction:

1. **Sensory Input**: The world provides sensory information
2. **Processing**: The brain processes this input
3. **Motor Output**: The brain produces an action
4. **World Update**: The world executes the action
5. **Feedback**: The world provides feedback to the brain

```python
# Complete cycle
percept = world.getSensoryPercept()  # Step 1
agent.processSensoryInput(percept)   # Step 2
brain.step(step)                      # Step 3
action = agent.decodeMotorCommand()   # Step 4
world.applyMotorCommand(action, time) # Step 5
```

---

## Beginner Examples

### Example 1: Silent Brain

A brain that just processes without external input:

```python
import pynlm as nlm

# Create and initialize brain
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Run the brain
for i in range(10):
    brain.step(i)

print(f"Brain has {brain.getTotalNeuronCount()} neurons")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Example 2: Simple World Observation

A brain that observes a world:

```python
import pynlm as nlm

# Setup components
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()
world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Observe for 30 steps
for i in range(30):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(i)

print("Observation complete")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
```

### Example 3: Complete Agent

A fully functional agent with learning capabilities:

```python
import pynlm as nlm

# Setup with learning enabled
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processing
    brain.step(step)
    
    # Decision and action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Log progress
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

---

## Intermediate Examples

### Example 1: Memory and Learning

Demonstrating memory formation and learning:

```python
import pynlm as nlm
import time

def create_learning_agent(world_size=10, steps=200):
    """Create an agent that learns patterns through experience."""
    
    # Configure for learning
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 300)  # More neurons for learning
    config.set("connection_probability", 0.2)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    world = nlm.createSimpleWorld()
    world.configure(width=world_size, height=world_size, 
                   visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Store initial state for comparison
    initial_spikes = brain.getTotalSpikeCount()
    initial_neurons = brain.getFiringNeuronCount()
    
    # Run learning episode
    for step in range(steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learning feedback simulation
        agent.applyRewardModulation(1.0, 0.8)  # Positive feedback
    
    # Analysis
    final_spikes = brain.getTotalSpikeCount() - initial_spikes
    final_neurons = brain.getFiringNeuronCount()
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'learning_metrics': {
            'spike_increase': final_spikes,
            'firing_neuron_increase': final_neurons - initial_neurons,
            'final_spike_count': brain.getTotalSpikeCount(),
            'development_stage': brain.getDevelopmentalStage()
        }
    }

# Run learning experiment
results = create_learning_agent(world_size=15, steps=300)
print("Learning experiment completed!")
print(f"Spike increase: {results['learning_metrics']['spike_increase']}")
print(f"Firing neurons increase: {results['learning_metrics']['firing_neuron_increase']}")
```

### Example 2: Different Sensorimotor Configurations

Testing various sensor and motor setups:

```python
import pynlm as nlm
from pynlm import SensoryInput, Vision, Audio, ActionType

def test_sensorimotor_setup():
    """Test different sensor and motor configurations."""
    
    # Test 1: High-resolution vision
    config1 = nlm.createDefaultConfig()
    config1.set("neuron_count", 500)
    brain1 = nlm.createBrain(config1)
    brain1.initialize()
    
    world1 = nlm.createSimpleWorld()
    world1.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world1.reset()
    
    agent1 = nlm.createAgentBrain(brain1)
    agent1.initialize(world1)
    
    # Test 2: Audio-based environment
    config2 = nlm.createDefaultConfig()
    config2.set("neuron_count", 400)
    brain2 = nlm.createBrain(config2)
    brain2.initialize()
    
    world2 = nlm.createSimpleWorld()
    world2.configure(width=25, height=25, visionWidth=8, visionHeight=8)
    world2.reset()
    
    agent2 = nlm.createAgentBrain(brain2)
    agent2.initialize(world2)
    
    # Run both test scenarios
    for brain, agent, world in [(brain1, agent1, world1), (brain2, agent2, world2)]:
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
    
    print("Sensorimotor test scenarios completed")
    return brain1, brain2

brain1, brain2 = test_sensorimotor_setup()
```

### Example 3: Developmental Brain

Simulating brain development over time:

```python
import pynlm as nlm
from pynlm import DevelopmentalStage

def simulate_development():
    """Simulate brain development across different stages."""
    
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 600)  # More neurons for development
    config.set("developmental_stage", "CriticalPeriod")
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableDevelopment(True)
    
    # Track development over time
    developmental_stages = []
    firing_rates = []
    
    for step in range(500):
        world.update(0.1)
        
        # Update development (changes brain properties)
        agent.updateDevelopment(0.1)
        
        # Log development stage
        stage = brain.getDevelopmentalStage()
        developmental_stages.append(stage)
        firing_rates.append(brain.getAverageFiringRate())
        
        # Process sensory input and act
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Simulate environmental challenges
        if step % 100 == 0:
            agent.applyRewardModulation(0.5, 0.3)  # Moderate challenge
    
    return {
        'developmental_stages': list(set(developmental_stages)),
        'firing_rates': firing_rates,
        'brain': brain,
        'world': world,
        'agent': agent
    }

results = simulate_development()
print(f"Development stages observed: {results['developmental_stages']}")
print(f"Final firing rate: {results['firing_rates'][-1]:.2f}")
```

---

## Advanced Examples

### Example 1: Custom Neural Network Topology

Creating specialized neural architectures:

```python
import pynlm as nlm
from pynlm import NeuronType, ActionType

def create_specialized_brain():
    """Create a brain with specialized neural populations."""
    
    # Custom configuration
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    config.set("connection_probability", 0.3)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create regions with different functions
    # In a full implementation, you'd add regions here
    # This demonstrates the pattern:
    
    # Sensory region for input processing
    # Cognitive region for decision making
    # Motor region for action execution
    
    return brain

def create_reinforcement_learner():
    """Create a brain optimized for reinforcement learning."""
    
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 800)
    config.set("connection_probability", 0.25)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    world = nlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Configure for reinforcement learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    
    return brain, world, agent

# Create specialized brains
specialized_brain = create_specialized_brain()
rl_brain, rl_world, rl_agent = create_reinforcement_learner()
```

### Example 2: Advanced State Management

Working with brain states and persistence:

```python
import pynlm as nlm
import os
import json

def save_brain_state(brain, filepath):
    """Save brain state to file."""
    brain.save(filepath)
    print(f"Brain state saved to {filepath}")

def load_brain_state(config, filepath):
    """Load brain state from file."""
    if os.path.exists(filepath):
        brain = nlm.createBrain(config)
        if brain.load(filepath):
            print(f"Brain state loaded from {filepath}")
            return brain
        else:
            print(f"Failed to load brain state from {filepath}")
            return None
    else:
        print(f"File {filepath} does not exist")
        return None

def demonstrate_state_management():
    """Demonstrate saving and loading brain states."""
    
    # Create and train a brain
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 500)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    # Train the brain
    for step in range(200):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Save the trained brain
    save_brain_state(brain, "trained_brain.nbm")
    
    # Create a new brain and load the saved state
    new_brain = nlm.createBrain(config)
    new_brain.initialize()
    
    loaded_brain = load_brain_state(config, "trained_brain.nbm")
    if loaded_brain:
        print("Successfully loaded trained brain")
        print(f"Loaded spike count: {loaded_brain.getTotalSpikeCount()}")

# Demonstrate state management
demonstrate_state_management()
```

### Example 3: Advanced Visualization and Monitoring

Monitoring brain activity and creating visualizations:

```python
import pynlm as nlm
import matplotlib.pyplot as plt

def monitor_brain_activity(brain, duration=100):
    """Monitor and visualize brain activity over time."""
    
    time_steps = []
    spike_counts = []
    firing_rates = []
    neuron_counts = []
    
    for step in range(duration):
        # Record metrics
        time_steps.append(step)
        spike_counts.append(brain.getTotalSpikeCount())
        firing_rates.append(brain.getAverageFiringRate())
        neuron_counts.append(brain.getFiringNeuronCount())
        
        # Simple step (in real usage, would have world and agent)
        # brain.step(step)
    
    # Create visualization
    fig, axes = plt.subplots(3, 1, figsize=(12, 10))
    
    # Spike count over time
    axes[0].plot(time_steps, spike_counts, 'b-', linewidth=2)
    axes[0].set_title('Total Spikes Over Time')
    axes[0].set_xlabel('Time Step')
    axes[0].set_ylabel('Spike Count')
    axes[0].grid(True, alpha=0.3)
    
    # Firing rate over time
    axes[1].plot(time_steps, firing_rates, 'r-', linewidth=2)
    axes[1].set_title('Average Firing Rate Over Time')
    axes[1].set_xlabel('Time Step')
    axes[1].set_ylabel('Firing Rate (Hz)')
    axes[1].grid(True, alpha=0.3)
    
    # Active neurons over time
    axes[2].plot(time_steps, neuron_counts, 'g-', linewidth=2)
    axes[2].set_title('Active Neurons Over Time')
    axes[2].set_xlabel('Time Step')
    axes[2].set_ylabel('Active Neurons')
    axes[2].grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('brain_activity.png')
    plt.close()
    
    return {
        'time_steps': time_steps,
        'spike_counts': spike_counts,
        'firing_rates': firing_rates,
        'neuron_counts': neuron_counts
    }

# Create a world for realistic monitoring
config = nlm.createDefaultConfig()
config.set("neuron_count", 400)

brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Monitor brain activity
monitoring_data = monitor_brain_activity(brain, duration=50)
print("Brain monitoring completed and saved to 'brain_activity.png'")
```

---

## Configuration Guide

### Basic Configuration

The `Config` class manages all brain parameters:

```python
import pynlm as nlm

# Create default configuration
config = nlm.createDefaultConfig()

# Access configuration values
print(f"Neuron count: {config.getInt('neuron_count')}")
print(f"Simulation timestep: {config.getFloat('simulation_timestep')}")
print(f"Random seed: {config.getInt('random_seed')}")
```

### Creating Custom Configurations

```python
import pynlm as nlm

def create_research_config():
    """Create a configuration optimized for research."""
    
    config = nlm.createDefaultConfig()
    
    # Basic parameters
    config.set("neuron_count", 1000)
    config.set("simulation_timestep", 0.001)
    
    # Plasticity parameters
    config.set("stdp_ltp_weight", 0.05)
    config.set("stdp_ltd_weight", 0.025)
    config.set("synaptogenesis_rate", 0.001)
    config.set("pruning_rate", 0.0005)
    
    # Development parameters
    config.set("developmental_stage", "Maturation")
    config.set("critical_period_end", 100.0)
    
    # Memory parameters
    config.set("working_memory_capacity", 100)
    config.set("episodic_memory_size", 50)
    
    return config

research_config = create_research_config()
```

### Configuration Validation

```python
import pynlm as nlm

def validate_configuration(config):
    """Validate configuration parameters."""
    
    errors = []
    warnings = []
    
    # Check neuron count
    neuron_count = config.getInt("neuron_count")
    if neuron_count < 10:
        errors.append("Neuron count must be at least 10")
    elif neuron_count > 10000:
        warnings.append("Neuron count is very large, may impact performance")
    
    # Check timestep
    timestep = config.getFloat("simulation_timestep")
    if timestep <= 0 or timestep > 1.0:
        errors.append("Simulation timestep must be between 0 and 1.0")
    
    # Check developmental stage consistency
    dev_stage = config.getString("developmental_stage")
    if dev_stage == "CriticalPeriod" and config.getInt("random_seed") == 0:
        warnings.append("Critical period with random_seed=0 may not produce good results")
    
    if errors:
        raise ValueError(f"Configuration errors: {', '.join(errors)}")
    
    if warnings:
        print(f"Warnings: {', '.join(warnings)}")
    
    return True

# Validate a configuration
validate_configuration(research_config)
```

---

## Troubleshooting

### Common Issues and Solutions

#### "The brain isn't doing anything"

**Problem**: The brain simulation appears to be inactive.

**Solutions**:

```python
# 1. Check initialization
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()  # Always required!

# 2. Run enough steps
for i in range(100):  # Try more steps
    brain.step(i)

# 3. Check configuration
if config.getInt("neuron_count") < 10:
    print("Warning: Low neuron count may result in minimal activity")

# 4. Add input
if using_world:
    # Ensure sensory input is being processed
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
```

#### "The agent isn't moving"

**Problem**: Actions are not being executed in the world.

**Solutions**:

```python
# 1. Check motor decoding
if action.type == ActionType.Wait:
    print("Agent decided to wait - check neural connections")
else:
    print(f"Agent chose action: {action.type}")

# 2. Verify world update cycle
for step in range(100):
    world.update(0.1)                    # Update world
    agent.processSensoryInput(percept)   # Process input
    brain.step(step)                      # Think
    action = agent.decodeMotorCommand()   # Decide
    world.applyMotorCommand(action, time) # Execute

# 3. Check world validity
if not world.isValidPosition(action.x, action.y):
    print("Action position is invalid")
```

#### "Everything is 0 or undefined"

**Problem**: Most metrics show zero values.

**Solutions**:

```python
# 1. Check simulation duration
if brain.getTotalSpikeCount() == 0:
    print("Brain may need time to 'warm up'")
    print(f"Current spikes: {brain.getTotalSpikeCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")

# 2. Check configuration
print("Current configuration:")
for key in config.getKeys():
    print(f"  {key}: {config.getOrDefault(key, 'N/A')}")

# 3. Verify neural connectivity
region_count = brain.getRegionCount()
print(f"Brain regions: {region_count}")
if region_count == 0:
    print("Warning: No brain regions created")

# 4. Try with a simpler setup
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()
```

### Debugging Tips

```python
import pynlm as nlm

def debug_brain(brain, world=None, agent=None):
    """Debug brain state and print diagnostics."""
    
    print("=== BRAIN DEBUG INFO ===")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    
    if world and agent:
        print("\n=== WORLD/AGENT INFO ===")
        percept = world.getSensoryPercept()
        print(f"World dimensions: {world.getWidth()} x {world.getHeight()}")
        print(f"Agent body position: ({agent.getAgentBody().x}, {agent.getAgentBody().y})")
        print(f"Has vision input: {agent.hasVision()}")
        print(f"Has touch input: {agent.hasTouch()}")
        print(f"Has internal signals: {agent.hasInternal()}")
    
    print("\n=== CONFIGURATION ===")
    for key in brain.getConfig().getKeys():
        value = brain.getConfig().get(key)
        print(f"  {key}: {value}")
    
    return True

# Use for debugging
debug_brain(brain, world, agent)
```

---

## API Reference

### Core Classes

#### Brain
```python
class Brain:
    def __init__(self, config: Config)
    def initialize(self) -> None
    def step(self, step: SimulationStep, time: Timestamp = 0.0) -> None
    def receiveSensoryInput(self, input: SensoryInput) -> None
    def injectCurrent(self, neuron: NeuronId, current: float) -> None
    def produceAction(self) -> Action
    def reset(self) -> None
    def save(self, filepath: str) -> bool
    def load(self, filepath: str) -> bool
    def getRegionCount(self) -> int
    def getTotalNeuronCount(self) -> int
    def getTotalSynapseCount(self) -> int
    def getTotalSpikeCount(self) -> int
    def getFiringNeuronCount(self) -> int
    def getAverageFiringRate(self) -> float
    def getDevelopmentalStage(self) -> str
    def setDevelopmentalStage(self, stage: str) -> None
```

#### Config
```python
class Config:
    def __init__(self)
    def loadFromFile(self, filepath: str) -> bool
    def loadFromArgs(self, argc: int, argv: List[str]) -> bool
    def saveToFile(self, filepath: str) -> bool
    def has(self, key: str) -> bool
    def getKeys(self) -> List[str]
    def clear(self) -> None
    def getOr(self, key: str, default: Any) -> Any
    def set(self, key: str, value: Any, source: ConfigSource = ConfigSource.User) -> None
    def summary(self) -> str
```

#### AgentBrain
```python
class AgentBrain:
    def __init__(self, brain: Brain)
    def initialize(self, world: SimpleWorld) -> None
    def processSensoryInput(self, percept: SensoryPercept) -> None
    def decodeMotorCommand(self) -> Action
    def applyRewardModulation(self, reward: float, predicted_reward: float) -> None
    def updateDevelopment(self, timestep: float) -> None
    def enableRewardModulation(self, enable: bool) -> None
    def enableCuriosity(self, enable: bool) -> None
    def enableDevelopment(self, enable: bool) -> None
```

#### SimpleWorld
```python
class SimpleWorld:
    def __init__(self)
    def configure(self, width: int, height: int, visionWidth: int, visionHeight: int) -> None
    def reset(self) -> None
    def setAgentStart(self, x: float, y: float) -> None
    def update(self, timestep: float) -> None
    def applyMotorCommand(self, action: Action, time: float) -> None
    def getSensoryPercept(self) -> SensoryPercept
    def getAgentBody(self) -> AgentBody
    def addObject(self, obj: WorldObject) -> None
    def isValidPosition(self, x: float, y: float) -> bool
```

### Factory Functions

```python
# Core factory functions
createBrain(config: Config) -> Brain
createSimpleWorld() -> SimpleWorld
createAgentBrain(brain: Brain) -> AgentBrain
createDefaultConfig() -> Config

# Convenience functions
runSimpleExperiment(config: Config, world: SimpleWorld, steps: int) -> Tuple[AgentBrain, SimpleWorld]
```

### Constants

```python
# Module constants
VERSION: str = "0.1.0"
PHASE: str = "6 - Final Integration"
MAX_SUPPORTED_NEURONS: int = 100000
DEFAULT_SIMULATION_STEPS: int = 1000

# Invalid IDs (use these to indicate "no valid ID")
INVALID_NEURON_ID: NeuronId
INVALID_SYNAPSE_ID: SynapseId
INVALID_REGION_ID: RegionId
INVALID_POPULATION_ID: PopulationId
```

### Enums

- `NeuronType`: Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal
- `SynapseType`: Excitatory, Inhibitory, Modulatory, Electrical, GapJunction
- `DevelopmentalStage`: Initial, CriticalPeriod, Maturation, Adult, Aging
- `FiringState`: Resting, Active, Refractory, Inhibited
- `ActionType`: MoveForward, MoveBackward, MoveLeft, MoveRight, TurnLeft, TurnRight, etc.
- `WorldObjectType`: Empty, Resource, Hazard, Wall, Marker

---

## Conclusion

This guide covers the essentials of working with NLM. Remember that NLM is an experimental framework for research and education. The most important principle is to:

1. **Start simple**: Begin with basic examples and gradually add complexity
2. **Understand the components**: Brain, World, and Agent each have specific roles
3. **Iterate and experiment**: Try different configurations and observe the results
4. **Learn from the code**: The NLM repository contains many example implementations

For advanced usage and research applications, refer to the technical documentation and the main NLM documentation in the `docs/` directory.

---

*Last updated: Phase 6, Final Integration*
*Version: 0.1.0*