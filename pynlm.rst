# NLM Python API Documentation

## Overview

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Python API. NLM is a brain-inspired spiking neural network simulator written in C++ with Python bindings via pybind11.

## Quick Start

### Minimal Example

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simulation
for i in range(1000):
    brain.step(i)

print(f"Simulation complete! Neurons: {brain.getTotalNeuronCount()}, Spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent with World

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent interface
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Initialize agent with world
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run complete simulation loop
for step in range(1000):
    # Update world
    world.update(timestep=0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward from world
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    
    # Apply reward modulation
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print progress every 100 steps
    if step % 100 == 0:
        print(f"Step {step}: Neurons firing={brain.getFiringNeuronCount()}, "
              f"Curiosity={agent.getCuriosityLevel():.3f}, "
              f"Novelty={agent.getNoveltyLevel():.3f}, "
              f"Dev Stage={brain.getDevelopmentalStage()}")

print("Agent simulation complete!")
```

## API Reference

### Core Classes

#### Config
Configuration class for NLM system parameters.

**Methods:**
- `loadFromFile(filepath)` - Load configuration from JSON file
- `loadFromArgs(argc, argv)` - Load configuration from command line
- `saveToFile(filepath)` - Save configuration to JSON file
- `has(key)` - Check if key exists
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration
- `summary()` - Get configuration summary
- `set(key, value)` - Set configuration value
- `get(key)` - Get configuration value

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

brain = pynlm.createBrain(config)
brain.initialize()
```

#### Brain
Central neural simulation brain class with all integrated systems.

**Methods:**
- `initialize()` - Initialize brain with configuration
- `step(step_number, time=None)` - Perform simulation step
- `receiveSensoryInput(input)` - Inject sensory data
- `injectCurrent(neuron, current)` - Inject current to specific neuron
- `injectCurrentToNeurons(neuron_type, current)` - Inject current to neuron type
- `produceAction()` - Produce motor action
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state
- `addRegion(name)` - Add neural region
- `getRegion(id)` - Get region by ID
- `getRegions()` - Get all regions

**Statistics:**
- `getTotalNeuronCount()` - Total neurons
- `getTotalSynapseCount()` - Total synapses
- `getActiveNeuronCount()` - Active neurons
- `getFiringNeuronCount()` - Currently firing neurons
- `getAverageFiringRate()` - Average firing rate
- `getExcitationInhibitionRatio()` - E/I balance
- `getTotalSpikeCount()` - Total spike count
- `getPendingSpikeEventCount()` - Pending spike events
- `getDevelopmentalStage()` - Current developmental stage

**System Access:**
- `getSpikeSystem()` - Access spike system
- `getSTDP()` - Access STDP plasticity
- `getHebbian()` - Access Hebbian plasticity
- `getStructuralPlasticity()` - Access structural plasticity
- `getWorkingMemory()` - Access working memory
- `getEpisodicMemory()` - Access episodic memory
- `getAssociativeMemory()` - Access associative memory
- `getPredictionSystem()` - Access prediction system
- `getPlanner()` - Access neural planner
- `getConceptFormation()` - Access concept formation
- `getAttention()` - Access attentional selection
- `getDevelopmentSystem()` - Access development system
- `getDopamine()` - Access dopamine neuromodulator
- `getCuriosity()` - Access curiosity neuromodulator
- `getNovelty()` - Access novelty detection
- `getPredictionErrorSignal()` - Access prediction error

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get brain statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Firing: {brain.getFiringNeuronCount()}")
print(f"Avg rate: {brain.getAverageFiringRate():.2f} Hz")
```

#### AgentBrain
Bridge between NLM brain and world for sensory-motor integration.

**Methods:**
- `initialize(world)` - Initialize with world
- `getSensoryInputSize()` - Get expected sensory input size
- `getMotorOutputSize()` - Get expected motor output size
- `processSensoryInput(percept)` - Process sensory percept
- `decodeMotorCommand()` - Decode brain activity to motor command
- `applyRewardModulation(reward, predicted_reward)` - Apply reward-based learning
- `updateDevelopment(timestep)` - Update development system
- `reset()` - Reset for new episode
- `getBrain()` - Get underlying brain

**Statistics:**
- `getDevelopmentalStage()` - Current developmental stage
- `getNeuromodulationLevel()` - Current neuromodulation level
- `getCuriosityLevel()` - Current curiosity level
- `getNoveltyLevel()` - Current novelty level
- `getPredictionError()` - Current prediction error

**Configuration:**
- `enableRewardModulation(enable)` - Enable/disable reward modulation
- `enableStructuralPlasticity(enable)` - Enable/disable structural plasticity
- `enableDevelopment(enable)` - Enable/disable development
- `enableCuriosity(enable)` - Enable/disable curiosity
- `isRewardModulationEnabled()` - Check if reward modulation enabled
- `isStructuralPlasticityEnabled()` - Check if structural plasticity enabled
- `isDevelopmentEnabled()` - Check if development enabled
- `isCuriosityEnabled()` - Check if curiosity enabled

**Example:**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()

agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
```

#### SimpleWorld
2D simulation world for agent-environment interaction.

**Methods:**
- `configure(width, height, vision_width, vision_height)` - Configure world dimensions
- `reset()` - Reset to initial state
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world state
- `applyMotorCommand(cmd, current_time)` - Apply motor command
- `getSensoryPercept()` - Get current sensory percept
- `getAgentBody()` - Get agent body state
- `addObject(obj)` - Add world object
- `removeObject(x, y)` - Remove object at position
- `isValidPosition(x, y)` - Check if position valid
- `getObjectAt(x, y)` - Get object at position

**Properties:**
- `getWidth()`, `getHeight()` - World dimensions
- `getMaxEnergy()` - Maximum energy
- `setMaxEnergy(e)` - Set maximum energy
- `getEnergyDecayRate()` - Energy decay rate
- `setEnergyDecayRate(r)` - Set energy decay rate
- `getSimulationTime()` - Current simulation time
- `getRandomSeed()` - Get random seed
- `setRandomSeed(seed)` - Set random seed

**Example:**
```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

for _ in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    # Process percept...
```

### Enumerations

#### NeuronType
- `Excitatory` - Excitatory neurons
- `Inhibitory` - Inhibitory neurons
- `Modulatory` - Modulatory neurons
- `Sensory` - Sensory neurons
- `Motor` - Motor neurons
- `Internal` - Internal neurons

#### SynapseType
- `Excitatory` - Excitatory synapses
- `Inhibitory` - Inhibitory synapses
- `Modulatory` - Modulatory synapses
- `Electrical` - Electrical synapses
- `GapJunction` - Gap junctions

#### DevelopmentalStage
- `Initial` - Initial developmental stage
- `CriticalPeriod` - Critical period
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

#### ActionType
- `MoveForward`, `MoveBackward` - Movement actions
- `TurnLeft`, `TurnRight` - Rotation actions
- `Look`, `LookUp`, `LookDown` - Looking actions
- `Interact` - Interaction action
- `Eat`, `Drink` - Consumption actions
- `Rest`, `Wait` - Rest actions
- `Custom` - Custom action

#### MotorCommand
- `MoveForward`, `MoveBackward` - Movement commands
- `TurnLeft`, `TurnRight` - Rotation commands
- `LookLeft`, `LookRight` - Look commands
- `Interact` - Interaction command
- `Wait` - Wait command

#### WorldObjectType
- `Empty` - Empty space
- `Resource` - Resource object
- `Hazard` - Hazard object
- `Wall` - Wall object
- `Marker` - Marker object

### Factory Functions

#### pynlm
Module-level factory functions for creating NLM objects.

**Functions:**
- `createDefaultConfig()` - Create default configuration
- `createBrain(config)` - Create brain with configuration
- `createSimpleWorld()` - Create simple world
- `createAgentBrain(brain)` - Create agent brain interface

### Advanced Usage Patterns

#### Pattern 1: Custom Configuration

```python
import pynlm
import json

# Load configuration from JSON
config = pynlm.createDefaultConfig()
config.loadFromFile("my_config.json")

# Modify configuration
config.set("brain.neuron_count", 500)
config.set("brain.connection_probability", 0.05)

# Save custom configuration
config.saveToFile("custom_config.json")

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Pattern 2: Save and Load Brain State

```python
import pynlm

# Create and train brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Train for some time
for i in range(1000):
    brain.step(i)

# Save trained brain
brain.save("trained_brain.bin")
print("Brain saved!")

# Later, load trained brain
new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
new_brain.load("trained_brain.bin")
print("Brain loaded!")
```

#### Pattern 3: Multi-Agent Simulation

```python
import pynlm

# Create shared configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

# Create multiple agents with shared brain or separate brains
agents = []
worlds = []

for i in range(3):
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(i * 10, i * 10)
    
    agents.append(agent)
    worlds.append(world)

# Run multi-agent simulation
for step in range(500):
    # Update all worlds
    for world in worlds:
        world.update(0.1)
    
    # Process sensory input for each agent
    for i, agent in enumerate(agents):
        percept = worlds[i].getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinking
        agent.getBrain().step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        worlds[i].applyMotorCommand(action, worlds[i].getSimulationTime())
```

#### Pattern 4: Performance Monitoring

```python
import pynlm
import time

# Create brain with monitoring enabled
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)

brain = pynlm.createBrain(config)
brain.initialize()

# Performance monitoring
start_time = time.time()
step_times = []

for step in range(1000):
    step_start = time.time()
    
    brain.step(step)
    
    step_end = time.time()
    step_times.append(step_end - step_start)
    
    if step % 100 == 0:
        avg_firing = brain.getAverageFiringRate()
        firing = brain.getFiringNeuronCount()
        print(f"Step {step}: {avg_firing:.1f} Hz, {firing} firing, "
              f"step time: {step_times[-1]*1000:.2f} ms")

end_time = time.time()
total_time = end_time - start_time
avg_step_time = sum(step_times) / len(step_times) if step_times else 0

print(f"\nPerformance Summary:")
print(f"Total time: {total_time:.2f}s")
print(f"Average step time: {avg_step_time*1000:.2f}ms")
print(f"Steps per second: {1/avg_step_time:.1f}")
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

#### Pattern 5: Experiment Reproduction

```python
import pynlm

def run_experiment(config_params, num_steps=1000):
    """Run a reproducible experiment with given parameters."""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Apply experiment parameters
    for key, value in config_params.items():
        if isinstance(value, int):
            config.set(key, value)
        elif isinstance(value, float):
            config.set(key, value)
        elif isinstance(value, bool):
            config.set(key, value)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run experiment
    for step in range(num_steps):
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Decode and apply action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and apply learning
        reward = world.getSensoryPercept().getInternal()[0] or 0.0
        agent.applyRewardModulation(reward, 0.0)
    
    return {
        'final_reward': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'neurons_firing': brain.getFiringNeuronCount(),
        'development_stage': brain.getDevelopmentalStage()
    }

# Run experiment with specific parameters
experiment_config = {
    "brain.neuron_count": 500,
    "brain.synapse_density": 0.1,
    "neuromod.dopamine.scale": 1.0,
    "neuromod.curiosity.enable": True,
    "neuromod.novelty.enable": True
}

results = run_experiment(experiment_config, num_steps=500)
print(f"Experiment results: {results}")
```

## Advanced Topics

### 1. Developmental Programming

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create agent
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()
agent.initialize(world)

# Enable development
agent.enableDevelopment(True)

# Simulate development over time
for step in range(2000):
    agent.updateDevelopment(0.1)
    
    # Monitor development stage
    stage = agent.getDevelopmentalStage()
    
    if stage.value >= pynlm.DevelopmentalStage.Maturation.value:
        print(f"Stage reached: {stage}")
        break
```

### 2. Curiosity-Driven Exploration

```python
import pynlm

# Create agent with curiosity
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableCuriosity(True)
agent.enableRewardModulation(True)

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run curiosity-driven exploration
for step in range(1000):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processing
    brain.step(step)
    
    # Decode action (curiosity influences selection)
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Monitor curiosity level
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    
    if step % 100 == 0:
        print(f"Step {step}: Curiosity={curiosity:.3f}, Novelty={novelty:.3f}")
```

### 3. Memory Integration

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access memory systems
working_mem = brain.getWorkingMemory()
episodic_mem = brain.getEpisodicMemory()
associative_mem = brain.getAssociativeMemory()

# Use memory systems
for step in range(1000):
    brain.step(step)
    
    # Working memory - store recent activity
    if working_mem:
        working_mem.store("current_step", step)
    
    # Episodic memory - record experience
    if episodic_mem:
        episodic_mem.recordExperience(f"Experience at step {step}")
    
    # Check memory usage
    if working_mem and working_mem.getUsage() > 0.8:
        print(f"Working memory usage: {working_mem.getUsage():.2%}")
```

### 4. Prediction and Planning

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access prediction system
prediction_sys = brain.getPredictionSystem()
planner = brain.getPlanner()

for step in range(1000):
    brain.step(step)
    
    if prediction_sys:
        # Get prediction error
        error = prediction_sys.getPredictionError()
        
        # Access confidence
        confidence = prediction_sys.getConfidence()
    
    if planner:
        # Plan actions
        action_plan = planner.planActions()
        # Execute planned actions
    
    # Monitor prediction error
    prediction_error = brain.getPredictionErrorSignal() or 0.0
    if step % 100 == 0:
        print(f"Step {step}: Prediction error: {prediction_error:.3f}")
```

## Troubleshooting

### Common Issues

#### "My brain isn't doing anything"
```python
# Ensure initialization
brain = pynlm.createBrain(config)
brain.initialize()  # Always call this!

# Check step count
for i in range(1000):
    brain.step(i)
```

#### "The agent isn't moving"
```python
# Ensure world update happens before motor command
world.update(0.1)
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

#### "Everything is 0"
```python
# Brains need time to warm up
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: Firing neurons: {brain.getFiringNeuronCount()}")
```

#### "ImportError: No module named 'pynlm'"
```bash
# Install or reinstall
pip install --force-reinstall .
```

#### "Segmentation fault"
```python
# Ensure reset is called between simulations
brain.reset()  # Always call this before re-running!
```

## Performance Tips

1. **Use larger simulation steps** for better performance
2. **Monitor memory usage** during long simulations
3. **Use checkpoints** to save progress and resume later
4. **Consider SIMD optimizations** for large networks
5. **Profile your code** to identify bottlenecks

## Examples

The examples directory contains additional scripts demonstrating:
- Complete agent training
- Memory system usage
- Developmental processes
- Multi-agent scenarios
- Performance monitoring

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [docs/PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 integration audit
