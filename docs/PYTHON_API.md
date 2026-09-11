// NLM Python API Documentation
# Python Bindings for NLM (Neural Learning Machine)

## Overview

The NLM project provides Python bindings that expose the complete C++ neural simulation framework through the `pynlm` module. These bindings enable researchers and developers to leverage Phase 6's integrated artificial brain capabilities in Python for rapid prototyping, research, and experimentation.

## Installation

### From Source (Recommended)

```bash
# Install Python bindings
pip install -e .

# Or install dependencies and build
pip install scikit-build-core pybind11 numpy pytest
pip install .
```

### Prerequisites

- Python 3.8+
- C++ compiler with C++20 support
- CMake 3.16+
- scikit-build-core, pybind11

## Quick Start

```python
import pynlm

# Create a default brain configuration
config = pynlm.createDefaultConfig()

# Initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

## Core Brain Classes

### Brain Class

The `Brain` class represents the complete integrated artificial brain with all Phase 6 systems.

#### Creation and Configuration

```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Configure brain parameters
config.set("neuron_count", 2000)
config.set("connection_probability", 0.15)
brain = pynlm.createBrain(config)

# Initialize (required before simulation)
brain.initialize()
```

#### Simulation Control

```python
# Run simulation steps
for step in range(1000):
    brain.step(step)

# Run with timing
for step in range(1000):
    brain.step(step, step * 0.001)

# Reset brain state
brain.reset()

# Save/load brain state
brain.save("checkpoint.bin")
brain.load("checkpoint.bin")
```

#### Brain Statistics

```python
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

### Config Class

Configuration management for brain parameters.

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Query existing parameters
if config.has("neuron_count"):
    print(f"Neuron count: {config.get('neuron_count')}")

# Get all configuration keys
keys = config.getKeys()

# Modify parameters
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("neuromod.dopamine.scale", 1.0)

# Load/save configuration from/to file
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

### AgentBrain Class

Connects NLM brain to environments through sensory processing and motor decoding.

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

# Configure world
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent with world
agent.initialize(world)

# Run complete agent simulation
def run_agent_simulation(num_steps=1000):
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

run_agent_simulation(1000)
```

### SimpleWorld Class

2D simulation world with vision, collision detection, and basic physics.

```python
import pynlm

# Create simple world
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

# Run world update
world.update(timestep=0.1)

# Get sensory percept for agent
percept = world.getSensoryPercept()

# Get agent body state
body = world.getAgentBody()

# Add objects to world
world.addObject(pynlm.WorldObject(x=5.0, y=5.0, 
                                   type=pynlm.WorldObjectType.Resource, 
                                   value=10.0))
```

## Advanced Python Features

### Memory Systems Integration

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access memory systems
working_mem = brain.getWorkingMemory()
episodic_mem = brain.getEpisodicMemory()
associative_mem = brain.getAssociativeMemory()

# Working memory - transient information storage
print(f"Working memory capacity: {working_mem.getCapacity()}")
print(f"Active traces: {working_mem.getActiveTraces()}")

# Episodic memory - experience storage
print(f"Max episodes: {episodic_mem.getMaxEpisodes()}")
print(f"Current episodes: {episodic_mem.getEpisodeCount()}")

# Associative memory - pattern associations
print(f"Associative memory size: {associative_mem.getSize()}")
```

### Neuromodulation and Learning

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Enable advanced learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation with learning
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = calculate_reward()  # Your reward function
    agent.applyRewardModulation(reward, agent.getExpectedReward())
    
    # Update development
    agent.updateDevelopment(0.1)
```

### Development and Aging

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Monitor developmental progress
for step in range(5000):
    brain.step(step)
    
    stage = brain.getDevelopmentalStage()
    print(f"Step {step}: {stage}")
    
    if stage == pynlm.DevelopmentalStage.Adult:
        print("Brain has reached adulthood!")
        break
```

### Prediction and Planning

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access prediction system
prediction = brain.getPredictionSystem()
if prediction:
    print("Prediction system available for forward modeling")

# Access cognitive systems
planner = brain.getPlanner()
concept = brain.getConceptFormation()
attention = brain.getAttention()

if planner:
    print("Planning system available for action selection")
if concept:
    print("Concept formation available for pattern discovery")
if attention:
    print("Attention system available for selective focus")
```

## Enumerations

### Neuron Types

```python
import pynlm

print("Neuron types:")
print(f"  Excitatory: {pynlm.NeuronType.Excitatory}")
print(f"  Inhibitory: {pynlm.NeuronType.Inhibitory}")
print(f"  Sensory: {pynlm.NeuronType.Sensory}")
print(f"  Motor: {pynlm.NeuronType.Motor}")
print(f"  Modulatory: {pynlm.NeuronType.Modulatory}")
print(f"  Internal: {pynlm.NeuronType.Internal}")
```

### Developmental Stages

```python
import pynlm

print("Developmental stages:")
for stage in pynlm.DevelopmentalStage:
    print(f"  {stage}")
```

### Motor Commands

```python
import pynlm

print("Motor commands:")
for cmd in pynlm.MotorCommand:
    print(f"  {cmd}")
```

### World Object Types

```python
import pynlm

print("World object types:")
for obj_type in pynlm.WorldObjectType:
    print(f"  {obj_type}")
```

## Example Scripts

### Example 1: Simple Brain Simulation

File: `examples/simple_brain.py`

```python
import pynlm
import time

print("NLM Python Example: Simple Brain Simulation")
print("=" * 50)

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Record start time
start_time = time.time()

# Run simulation
for step in range(1000):
    brain.step(step)

# Print results
end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Example 2: Complete Agent Simulation

File: `examples/agent_simulation.py`

```python
import pynlm

def run_complete_agent_simulation():
    """Complete agent simulation with world interaction"""
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run episode
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        if result.success:
            agent.applyRewardModulation(result.reward, agent.getExpectedReward())
        
        agent.updateDevelopment(0.1)
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"curiosity: {agent.getCuriosityLevel():.3f}")
    
    print("Agent simulation complete!")
    return brain

# Run the simulation
brain = run_complete_agent_simulation()
```

### Example 3: Learning Experiment

File: `examples/learning_experiment.py`

```python
import pynlm
import numpy as np

print("NLM Python Example: Learning Experiment")
print("=" * 40)

# Create brain for learning experiment
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
brain = pynlm.createBrain(config)
brain.initialize()

# Enable plasticity
if auto* stdp = brain.getSTDP():
    stdp->configure(0.01f, 0.012f, 20.0f)

# Create initial weight map
initial_weights = []
if auto* region = brain.getRegion(pynlm.RegionId(1)):
    for syn : region->getSynapses():
        initial_weights.push_back(syn->getWeight())

print(f"Initial average weight: {np.mean(initial_weights):.4f}")

# Apply learning stimulus
for step in range(1000):
    # Inject correlated pre-post activity for LTP
    brain.injectCurrentToNeurons(pynlm.NeuronType::Excitatory, 30.0f)
    brain.step(step)
    
    if step % 100 == 0:
        # Record weight changes
        final_weights = []
        if auto* region = brain.getRegion(pynlm.RegionId(1)):
            for syn : region->getSynapses():
                final_weights.push_back(syn->getWeight())
        
        avg_weight = np.mean(final_weights)
        initial_avg = np.mean(initial_weights)
        print(f"Step {step}: Weight changed from {initial_avg:.4f} to {avg_weight:.4f} "
              f"(Δ={avg_weight - initial_avg:.4f})")

print("Learning experiment complete!")
```

## Configuration Options

The Python API supports configuration through the `createDefaultConfig()` function or by creating custom configurations.

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int | 1000 | Total number of neurons |
| `connection_probability` | float | 0.1 | Probability of synapse formation |
| `region_count` | int | 1 | Number of neural regions |
| `simulation_timestep` | float | 0.001 | Time step for simulation |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.time_constant` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Performance Tips

1. **Reuse Objects**: Reuse Brain, AgentBrain, and World objects across simulations to reduce allocation overhead.

2. **Batch Operations**: For large simulations, consider batching operations or using compiled extensions.

3. **Memory Management**: Monitor memory usage in long-running simulations. Use checkpoints to save/restart state.

4. **Multiprocessing**: For parallel simulations, run multiple processes with separate brain instances.

5. **Configuration Caching**: Load configurations once and reuse them across multiple brain instances.

## Limitations

- Python bindings have some overhead compared to direct C++ usage
- Some internal data structures may not be fully exposed through Python
- Certain advanced features may require additional C++ extensions
- Performance-critical paths should remain in C++ where possible

## Future Enhancements

- Add more Python-specific convenience functions
- Expand JSON configuration support
- Add visualization tools for Python-based plotting
- Include machine learning integration interfaces

## References

- NLM Project Documentation: https://nlm.readthedocs.io
- C++ API Reference: See source code in `src/`
- Research Papers: Available in `docs/SCIENCE.md`

© 2026 NLM Project - MIT License