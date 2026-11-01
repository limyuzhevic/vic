# Advanced NLM User Guide

## Table of Contents

1. [Quick Start](#quick-start)
2. [Core Concepts](#core-concepts)
3. [Creating Brains](#creating-brains)
4. [Working with Worlds](#working-with-worlds)
5. [Using Agent Brains](#using-agent-brains)
6. [Learning and Reward Systems](#learning-and-reward-systems)
7. [Advanced Features](#advanced-features)
8. [Troubleshooting](#troubleshooting)
9. [API Reference](#api-reference)

---

## Quick Start

NLM (Neural Learning Machine) is a brain-inspired spiking neural network simulator. Think of it as creating virtual brains that can learn, remember things, and make decisions.

### What You'll Need

1. **Brain** - The virtual brain that thinks and learns
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

### Your First Brain

```python
import pynlm

# Create and initialize a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Make it think for 100 steps
for i in range(100):
    brain.step(i)

print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
```

### Complete Agent Example

```python
import pynlm

# 1. Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# 2. Create agent
agent = pynlm.createAgentBrain(brain)

# 3. Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# 4. Initialize agent with world
agent.initialize(world)

# 5. Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# 6. Run simulation
for step in range(1000):
    world.update(0.1)                    # Update world
    agent.processSensoryInput(world.getSensoryPercept())  # Brain sees world
    brain.step(step)                     # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Act

print("Simulation complete!")
```

---

## Core Concepts

### The Brain-Loop

```
World → Sensory Input → Brain → Action → World → Reward → Brain (Learning)
```

1. **Sensory Input**: Brain receives information about the world
2. **Brain Processing**: Neural computation, memory access, decision making
3. **Action Selection**: Brain chooses what to do
4. **World Update**: Action affects the world
5. **Reward**: Feedback on action outcome
6. **Learning**: Brain updates based on reward

### Key Components

- **Neurons**: Simulate biological brain cells that fire and communicate
- **Synapses**: Connections between neurons that strengthen/ weaken with learning
- **Memory Systems**: Working memory, episodic memory, concept formation
- **Neuromodulation**: Dopamine (reward), Curiosity (exploration), Novelty detection
- **Plasticity**: Learning mechanisms (STDP, Hebbian, structural)

### Learning Principles

- **Rewards**: Positive outcomes reinforce certain behaviors
- **Prediction Error**: Discrepancy between expected and actual outcomes
- **Novelty**: New experiences trigger curiosity and exploration
- **Development**: Brain matures over time, affecting learning rates

---

## Creating Brains

### Basic Brain Creation

```python
import pynlm

# Create with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Modify configuration
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
```

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection |
| `brain.initial_weight_mean` | float | 0.5 | Initial synaptic weight |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |

### Advanced Brain Configuration

```python
config = pynlm.createDefaultConfig()

# Large brain for complex tasks
config.set("brain.neuron_count", 5000)
config.set("brain.region_count", 2)
config.set("brain.connection_probability", 0.15)

# Learning parameters
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Neuromodulation
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Brain Operations

```python
# Simulation
brain.step(step_number)           # Run one step
brain.step(step_number, time)     # Run with timestamp
brain.reset()                      # Reset state
brain.save("checkpoint.bin")        # Save state
brain.load("checkpoint.bin")        # Load state

# Input/Output
brain.receiveSensoryInput(sensory_input)
action = brain.produceAction()

# Statistics
brain.getTotalNeuronCount()
brain.getTotalSynapseCount()
brain.getFiringNeuronCount()
brain.getAverageFiringRate()
brain.getExcitationInhibitionRatio()
```

---

## Working with Worlds

### Creating Worlds

```python
import pynlm

# Create a simple 2D world
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

### World Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `width` | float | 20.0 | World width |
| `height` | float | 20.0 | World height |
| `visionWidth` | int | 16 | Vision sensor width |
| `height` | int | 16 | Vision sensor height |
| `maxEnergy` | float | 100.0 | Maximum agent energy |
| `energyDecayRate` | float | 0.01 | Energy decay per step |

### World Operations

```python
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
world.isValidPosition(x, y)
```

### Sensory Processing

The world provides sensory inputs to the brain:

- **Vision**: Ray-casted observations of the environment
- **Touch**: Proximity to objects in different directions
- **Internal**: Agent state (energy, health, movement)
- **Proprioception**: Body position and movement

### Reward System in World

The world provides rewards based on agent actions:

- **Resource Collection**: +10 to +30 reward
- **Hazard Avoidance**: Avoid -5 to -15 penalties
- **Movement Cost**: -0.01 to -0.05 per action
- **Novelty Detection**: Small curiosity bonuses

---

## Using Agent Brains

### Agent Brain Creation

```python
import pynlm

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)
```

### Key Agent Operations

```python
# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
action = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
```

### Subsystem Control

```python
# Enable learning systems
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# Check subsystem status
agent.isRewardModulationEnabled()
agent.isCuriosityEnabled()
```

### Agent Statistics

```python
# Neuromodulation levels
agent.getNeuromodulationLevel()
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()

# Brain integration
agent.getBrain()                           # Get underlying brain
agent.getDevelopmentalStage()             # Current development stage
agent.getSelfModelConfidence()             # Self-model confidence
```

### Complete Agent Pattern

```python
def run_agent_simulation(config, world_config, num_steps=1000):
    """Complete agent simulation with learning."""
    
    # 1. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Configure world
    world = pynlm.createSimpleWorld()
    world.configure(**world_config)
    world.reset()
    
    # 4. Initialize agent
    agent.initialize(world)
    
    # 5. Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 6. Run simulation
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain computation
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record metrics
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Reward: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    
    return brain, agent, world
```

---

## Learning and Reward Systems

### Reward-Based Learning

The brain learns through reward prediction and neuromodulation:

1. **Reward Signals**: Real-world outcomes (+10 for resources, -15 for hazards)
2. **Prediction Error**: Difference between expected and actual rewards
3. **Dopamine Modulation**: Affects synaptic plasticity based on reward
4. **Curiosity**: Drives exploration of novel situations
5. **Novelty Detection**: Identifies new patterns in sensory input

### Concept Formation

The brain forms concepts from experience patterns:

```python
# Concepts emerge from pattern/reward associations
# A bright scene with +10 becomes one concept
# A dark scene with -15 becomes another concept

# Concept bias influences actions
if concept.reward > 0.1:
    interactRate += 8.0  # Approach bright objects
else if concept.reward < -0.1:
    interactRate -= 8.0  # Avoid dark objects
```

### Development Stages

The brain matures over time:

- **Initial**: High plasticity, rapid learning
- **CriticalPeriod**: Modulated plasticity
- **Maturation**: Stable learning rates
- **Adult**: Low plasticity, refined behavior

### Learning Metrics

Track learning progress:

```python
def track_learning_progress(brain, agent, steps):
    """Monitor learning effectiveness."""
    
    metrics = {
        'reward_evolution': [],
        'curiosity_levels': [],
        'concept_count': [],
        'action_distribution': {},
        'novelty_detection': []
    }
    
    for step in range(steps):
        # Record metrics
        metrics['reward_evolution'].append(agent.getNeuromodulationLevel())
        metrics['curiosity_levels'].append(agent.getCuriosityLevel())
        metrics['concept_count'].append(brain.getConceptFormation().getConceptCount())
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Track action distribution
        action = agent.decodeMotorCommand()
        # Update action distribution tracking
    
    return metrics
```

---

## Advanced Features

### Batch Processing

Process multiple steps efficiently:

```python
# Batch simulation
brain.batchStep(start_step, end_step, timestep)

# Performance monitoring
metrics = brain.getPerformanceMetrics(start_time)
```

### Checkpointing and Persistence

```python
# Save brain state
brain.save("my_brain.bin")

# Load brain state
if brain.load("my_brain.bin"):
    print("Brain loaded successfully")

# Checkpoint management
brain.listCheckpoints()
brain.pruneOldCheckpoints(5)
```

### Custom World Objects

```python
# Create custom world objects
resource = pynlm.WorldObject(
    x=5.0, y=5.0,
    type=pynlm.WorldObjectType.Resource,
    value=20.0,
    radius=0.5
)
world.addObject(resource)
```

### Advanced Agent Features

```python
# Thought monitoring
thought = agent.getThoughtTrace()
print(f"Agent thought: {thought}")

# Self-model
confidence = agent.getSelfModelConfidence()
print(f"Self-model confidence: {confidence}")

# Social learning
agent.observeOtherAgent(
    observed_action, before_vision, after_vision
)
```

---

## Troubleshooting

### Common Issues and Solutions

#### "My brain isn't doing anything"

**Problem**: Brain appears inactive or not learning.

**Solutions**:
1. Ensure `brain.initialize()` is called before `step()`
2. Increase simulation steps for observation
3. Check that sensory input is properly connected
4. Verify that learning systems are enabled

**Debug Code**:
```python
# Enable detailed logging
import logging
logging.basicConfig(level=logging.DEBUG)

# Test with simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Check neuron activity
for i in range(10):
    brain.step(i)
    print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons")
```

#### "The agent isn't moving"

**Problem**: Agent appears stuck or not taking actions.

**Solutions**:
1. Check that `world.applyMotorCommand()` is being called
2. Ensure `world.update()` is called in the simulation loop
3. Verify agent has motor neurons
4. Check action decoding thresholds

**Debug Code**:
```python
# Debug agent movement
for i in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"Step {i}: Action={action}, Reward={result.reward}, Success={result.success}")
```

#### "Everything is 0"

**Problem**: All metrics appear zero or constant.

**Solutions**:
1. Brains need time to "warm up" - try more steps
2. Some neurons need input to fire - ensure sensory connection
3. Check that learning systems are properly configured
4. Verify that initialization happens before use

**Debug Code**:
```python
# Test initialization sequence
print("Testing NLM initialization sequence...")

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

brain.initialize()
print("Brain initialized")

for i in range(50):
    brain.step(i)
    if i % 10 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Build Issues

**"CMake cannot find Python:"**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**"ImportError: No module named 'pynlm':"**
```bash
pip install --force-reinstall .
```

### Performance Optimization

**Tips for faster simulations**:

1. Use larger timesteps for quick testing
2. Reduce neuron count for simple tasks
3. Disable unused subsystems
4. Use batch processing for multiple steps

**Example Optimized Simulation**:
```python
def optimized_simulation(config, num_steps):
    """Optimized simulation for speed."""
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Use larger timestep for faster simulation
    timestep = 0.1
    
    # Batch process when possible
    for step in range(0, num_steps, 10):
        end_step = min(step + 10, num_steps)
        brain.batchStep(step, end_step, timestep)
        
        if step % 100 == 0:
            print(f"Progress: {step}/{num_steps} steps")
    
    return brain
```

---

## API Reference

### Core Classes

#### Brain
| Method | Description |
|--------|-------------|
| `initialize()` | Initialize brain with configuration |
| `step(step, time=None)` | Run one simulation step |
| `reset()` | Reset brain to initial state |
| `save(filepath)` | Save brain state to file |
| `load(filepath)` | Load brain state from file |

#### Config
| Method | Description |
|--------|-------------|
| `set(key, value)` | Set configuration value |
| `getOr(key, default)` | Get value with default |
| `has(key)` | Check if key exists |

#### AgentBrain
| Method | Description |
|--------|-------------|
| `initialize(world)` | Initialize with world interface |
| `processSensoryInput(percept)` | Process sensory input |
| `decodeMotorCommand()` | Get motor command |
| `applyRewardModulation(reward, predicted)` | Apply reward-based learning |

#### SimpleWorld
| Method | Description |
|--------|-------------|
| `configure(width, height, visionWidth, visionHeight)` | Configure world dimensions |
| `reset()` | Reset world to initial state |
| `update(timestep)` | Update world physics |
| `applyMotorCommand(action, time)` | Apply agent action to world |

### Enumerations

#### NeuronType
- `Excitatory`: Excitatory neurons
- `Inhibitory`: Inhibitory neurons
- `Sensory`: Sensory neurons
- `Motor`: Motor neurons
- `Modulatory`: Modulatory neurons
- `Internal`: Internal neurons

#### ActionType
- `MoveForward`: Move forward
- `MoveBackward`: Move backward
- `TurnLeft`: Turn left
- `TurnRight`: Turn right
- `LookLeft`: Look left
- `LookRight`: Look right
- `Interact`: Interact with object
- `Eat`: Eat object
- `Wait`: Do nothing

#### DevelopmentalStage
- `Initial`: High plasticity, rapid learning
- `CriticalPeriod`: Modulated plasticity
- `Maturation`: Stable learning
- `Adult`: Refined behavior

#### WorldObjectType
- `Empty`: Empty space
- `Resource`: Resource object
- `Hazard`: Hazard object
- `Wall`: Wall object
- `Marker`: Marker object

### Factory Functions

| Function | Returns |
|----------|---------|
| `createDefaultConfig()` | Default configuration |
| `createBrain(config)` | Brain instance |
| `createSimpleWorld()` | World instance |
| `createAgentBrain(brain)` | Agent brain interface |

### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `DEFAULT_TIMESTEP` | 0.001 | Default simulation timestep |
| `DEFAULT_NEURON_COUNT` | 500 | Default neuron count |
| `DEFAULT_MAX_STEPS` | 2000 | Default max steps |

---

## Mini Projects

### Project 1: Silent Brain

Test basic brain functionality:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
```

### Project 2: Brain Watching a World

Observe agent interaction with environment:

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

# Watch the world
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print(f"Firing rate: {brain.getAverageFiringRate()}")
```

### Project 3: Complete Agent Learning

Full reinforcement learning agent:

```python
import pynlm

def train_agent(config, world_config, num_episodes=5):
    """Train an agent through multiple episodes."""
    
    results = []
    
    for episode in range(num_episodes):
        print(f"\n=== Episode {episode} ===")
        
        # Setup
        brain = pynlm.createBrain(config)
        brain.initialize()
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(**world_config)
        world.reset()
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run episode
        episode_reward = 0.0
        for step in range(200):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            episode_reward += agent.getNeuromodulationLevel()
        
        # Record results
        results.append({
            'episode': episode,
            'total_reward': episode_reward,
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel(),
            'concept_count': brain.getConceptCount()
        })
        
        print(f"Episode {episode}: Reward={episode_reward:.2f}, "
              f"Curiosity={agent.getCuriosityLevel():.2f}, "
              f"Concepts={brain.getConceptCount()}")
    
    return results

# Train agent
train_agent(
    config=pynlm.createDefaultConfig(),
    world_config={'width': 15, 'height': 15, 'visionWidth': 8, 'visionHeight': 8},
    num_episodes=3
)
```

---

## Next Steps

1. **Read the Architecture Documentation**: `docs/ARCHITECTURE.md` explains how all components work together
2. **Explore Experiments**: Look at `docs/EXPERIMENTS.md` for more complex examples
3. **Scientific Background**: `docs/SCIENCE.md` explains the neuroscience principles behind NLM
4. **Try Different Configurations**: Experiment with different brain and world settings
5. **Join the Community**: Share your experiments and learn from others!

---

This guide provides comprehensive coverage of NLM usage, from basic examples to advanced features. Start with the Quick Start, then explore the detailed sections based on your interests and needs.

Ready to create your own intelligent agent? Begin with the simplest example and gradually build up to more complex simulations!
