# NLM (Neural Learning Machine) - Comprehensive Python API Reference

**Version**: 0.1.0
**Last Updated**: October 25, 2026

## Overview

NLM (熙然) is an experimental artificial developmental brain simulator. This document provides comprehensive Python API documentation for the NLM Python bindings.

## Installation

### Prerequisites

```bash
pip install pynlm
# or build from source
pip install -e .
```

### Building from Source

```bash
# Clone the repository
cd /path/to/nlm

# Build with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install .
```

## Module Structure

The NLM Python module provides the following main components:

- **Core Types**: NeuronId, SynapseId, RegionId, PopulationId
- **Enumerations**: NeuronType, SynapseType, DevelopmentalStage, FiringState
- **Enumerations**: ActionType, MotorCommand, WorldObjectType
- **Configuration**: Config class for system settings
- **Neural Core**: Brain class for neural simulation
- **Agent System**: AgentBrain class for world interaction
- **Environment**: SimpleWorld for simulation world
- **Sensory/Motor**: Vision, Audio, InternalSignals, Action
- **State**: WorldObject, AgentBody, SensoryPercept, ActionResult
- **Utilities**: Factory functions for quick setup

## Core Classes

### Config

Configuration management for NLM system.

#### Methods

**Constructor**
```python
config = Config()
```

**Get/Set Configuration**
```python
# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)
config.set("brain.connection_probability", 0.1)

# Get configuration values
neuron_count = config.get("brain.neuron_count")
# Returns None if key doesn't exist

# Get with default value
neuron_count = config.getOr("brain.neuron_count", 1000)
```

**File Operations**
```python
# Load from JSON file
config.loadFromFile("config.json")

# Save to JSON file
config.saveToFile("config.json")

# Get all configuration keys
keys = config.getKeys()
```

**Utility Methods**
```python
# Check if key exists
if config.has("brain.neuron_count"):
    pass

# Clear all configuration
config.clear()

# Get configuration summary
print(config)  # Uses __str__ method
```

#### Factory Functions

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Create from file
config = pynlm.createConfigFromFile("config.json")

# Create with preset
config = pynlm.createPresetConfig("standard")
```

### Brain

Central neural simulation brain class.

#### Constructor
```python
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
```

#### Core Methods

**Initialization**
```python
brain.initialize()  # Required before use
```

**Simulation**
```python
# Basic step
for step in range(100):
    brain.step(step)

# Step with timestamp
for step in range(100):
    brain.step(step, step * 0.001)  # Add time component
```

**Sensory/Motor Interface**
```python
# Inject sensory input
percept = world.getSensoryPercept()
brain.receiveSensoryInput(percept)

# Inject current into neurons
brain.injectCurrent(neuron_id, 5.0)
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)

# Get motor output
action = brain.produceAction()
```

**Memory Systems**
```python
# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()
```

**Neuromodulation Systems**
```python
# Access neuromodulators
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionErrorSignal()
```

**Cognitive Systems**
```python
# Access cognitive systems
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
attention = brain.getAttention()
```

**Development**
```python
# Get and set developmental stage
stage = brain.getDevelopmentalStage()
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
```

**State Management**
```python
# Save/load brain state
brain.save("checkpoint.bin")
brain.load("checkpoint.bin")

# Reset brain
brain.reset()
```

**Statistics**
```python
# Get brain statistics
neuron_count = brain.getTotalNeuronCount()
synapse_count = brain.getTotalSynapseCount()
firing_count = brain.getFiringNeuronCount()
avg_firing_rate = brain.getAverageFiringRate()
exc_inh_ratio = brain.getExcitationInhibitionRatio()
total_spikes = brain.getTotalSpikeCount()
active_neurons = brain.getActiveNeuronCount()
```

### AgentBrain

Agent interface connecting brain to world.

#### Constructor
```python
agent = pynlm.createAgentBrain(brain)
```

#### Methods

**Initialization**
```python
agent.initialize(world)  # Required before use
```

**Sensory Processing**
```python
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
```

**Motor Decoding**
```python
action = agent.decodeMotorCommand()
```

**Neuromodulation**
```python
agent.applyRewardModulation(reward, predicted_reward)
```

**Development**
```python
agent.updateDevelopment(timestep)
stage = agent.getDevelopmentalStage()
```

**Neuromodulation Levels**
```python
dopamine_level = agent.getNeuromodulationLevel()
curiosity_level = agent.getCuriosityLevel()
novelty_level = agent.getNoveltyLevel()
prediction_error_val = agent.getPredictionError()
```

**Subsystem Control**
```python
# Enable/disable subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Check subsystem status
if agent.isRewardModulationEnabled():
    pass
```

### SimpleWorld

Simple 2D world for NLM simulation.

#### Constructor
```python
world = pynlm.createSimpleWorld()
```

#### Configuration
```python
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
```

#### Control
```python
world.reset()  # Reset to initial state
world.update(0.1)  # Update by timestep
world.setAgentStart(10.0, 10.0)  # Set starting position
world.applyMotorCommand(action, current_time)  # Apply motor command
```

#### State Access
```python
percept = world.getSensoryPercept()
body = world.getAgentBody()
world.addObject(world_object)
world.removeObject(x, y)
```

#### Properties
```python
width = world.getWidth()
height = world.getHeight()
simulation_time = world.getSimulationTime()
```

### Vision

Visual sensory input.

```python
# Create vision
vision = pynlm.Vision(8, 8, 3)  # width, height, channels

# Set data from numpy array
import numpy as np
vision_data = np.random.rand(8, 8, 3).astype(np.float32)
vision.setData(vision_data)

# Get vision data
vision_array = vision.getData()  # Returns numpy array
```

### Action

Motor action representation.

```python
# Create action
action = pynlm.Action(pynlm.ActionType.MoveForward)

# Create with parameters
action = pynlm.Action(
    pynlm.ActionType.Custom,
    [0.5, 0.3, 0.8]  # Custom parameters
)

# Get action name
name = action.getName()  # "MoveForward", "TurnLeft", etc.
```

### SensoryPercept

Complete sensory information from world.

```python
percept = world.getSensoryPercept()

# Access different modalities
vision = percept.getVision()
touch = percept.getTouch()
internal = percept.getInternal()
proprioception = percept.getProprioception()
audio = percept.getAudio()
```

## Factory Functions and Utilities

### Quick Setup Functions

```python
# Create mini brain for testing
brain, firing_rate = pynlm.create_mini_brain(neuron_count=500, steps=100)

# Create complete agent simulation
brain, agent, world, stats = pynlm.create_agent_simulation(
    width=20, height=20,
    vision_width=8, vision_height=8,
    steps=500, enable_learning=True
)

# Benchmark performance
results = pynlm.benchmark(
    neuron_counts=[100, 1000, 5000],
    steps=1000
)
```

### Advanced Features

```python
# Run simulation utility
pynlm.runSimulation(brain, 1000)

# Run complete episode
pynlm.runEpisode(brain, agent, world, 500)

# Validate brain integration
is_valid, issues = pynlm.validateBrainIntegration(brain)
```

### Preset Configurations

```python
# Get common configuration presets
minimal_config = pynlm.createPresetConfig("minimal")  # Small network for testing
standard_config = pynlm.createPresetConfig("standard")  # Balanced setup
high_performance_config = pynlm.createPresetConfig("high_performance")  # Optimized for speed
research_config = pynlm.createPresetConfig("research")  # Comprehensive setup
```

### Phase 6 Integration

```python
# Phase 6 integration experiment
experiment = pynlm.Phase6IntegratedExperiment()
result = experiment.run(pynlm.Phase6Config())

# Simple integration verification
is_integrated = experiment.verifyIntegration()

# Test individual systems
experiment.testMemoryIntegration()
experiment.testNeuromodulationIntegration()
experiment.testCheckpointing()
experiment.testReplay()
```

## Advanced Features

### Performance Optimization

```python
# Check performance configuration
info = pynlm.getPerformanceInfo()
print(f"SIMD width: {info['simd_width']}")
print(f"Thread count: {info['thread_count']}")

# Enable optimizations (requires rebuild with -DNLM_PERFORMANCE_ENABLED)
pynlm.enableOptimizations()
```

### Configuration Management

```python
# Export brain state
pynlm.exportBrainState(brain, "brain_state.json")

# Import brain state
imported_brain = pynlm.importBrainState("brain_state.json")
```

### Simulation Control

```python
# Configure advanced simulation settings
settings = {
    "timestep": 0.001,
    "plasticity_enabled": True,
    "neuromodulation_enabled": True,
    "development_enabled": True
}
pynlm.configureSimulation(brain, settings)
```

## Error Handling

NLM provides comprehensive error handling with custom exception types:

```python
import pynlm

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.step(0)
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except pynlm.ValueError as e:
    print(f"Invalid value: {e}")
except pynlm.IndexError as e:
    print(f"Index out of range: {e}")
```

## Performance Considerations

### Memory Usage

NLM uses different memory configurations based on build settings:

```python
# Performance info shows current configuration
info = pynlm.getPerformanceInfo()
if info["soa_enabled"]:
    print("Using Structure-of-Arrays (SoA) for better cache performance")
if info["simd_enabled"]:
    print(f"Using SIMD with width {info['simd_width']}")
if info["multithreading_enabled"]:
    print(f"Using {info['thread_count']} threads for parallel processing")
```

### Scaling Guidelines

- **Small networks** (100-1000 neurons): Good for development and testing
- **Medium networks** (1000-10,000 neurons): Suitable for most applications
- **Large networks** (10,000+ neurons): Requires performance optimizations

## Troubleshooting

### Common Issues and Solutions

**"My brain isn't doing anything"
```python
# Solution: Ensure brain is initialized
brain.initialize()
# Make sure you're calling step() in a loop
for step in range(100):
    brain.step(step)
```

**"The agent isn't moving"
```python
# Solution: Ensure world update is called
for step in range(100):
    world.update(0.1)  # Update world first
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

**"Everything is 0"
```python
# Solution: Brains need time to "warm up"
for step in range(1000):  # Use more steps
    brain.step(step)
# Also ensure sensory input is connected
```

### Build Issues

**"CMake cannot find Python"**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**"ImportError: No module named 'pynlm'"**
```bash
pip install --force-reinstall .
```

**"Segmentation faults"**
```python
# Ensure proper initialization order
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()  # MUST call this before step()
brain.step(0)
```

## Examples and Tutorials

### Basic Usage

```python
import pynlm

# Example 1: Silent brain test
print("=== Silent Brain Test ===")
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print(f"Silent brain test done! Neurons: {brain.getTotalNeuronCount()}")
```

```python
# Example 2: Brain watching a world
print("=== World Watching Test ===")
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

print(f"Watched world for 30 steps")
print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
```

```python
# Example 3: Complete agent with learning
print("=== Complete Agent Test ===")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
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
print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
print(f"Final novelty: {agent.getNoveltyLevel():.3f}")
```

## API Reference Summary

| Category | Classes | Key Methods |
|----------|---------|-------------|
| Core Types | NeuronId, SynapseId, RegionId | get/set, __eq__, __repr__ |
| Neural Core | Brain | step(), initialize(), produceAction() |
| Agent System | AgentBrain | decodeMotorCommand(), applyRewardModulation() |
| Environment | SimpleWorld | update(), getSensoryPercept() |
| Sensory | Vision, Audio, InternalSignals | setData(), getData() |
| Motor | Action | getType(), getParameters() |
| State | SensoryPercept, ActionResult | get/set methods |

## Getting Help

1. **Documentation**: Check the NLM documentation folder for detailed guides
2. **Examples**: Run the example scripts in the repository
3. **Community**: Join the NLM community for support and discussion
4. **Issues**: Report bugs and feature requests on GitHub

## License

MIT License. See LICENSE file for details.

---

For more detailed documentation and examples, visit the NLM documentation website.
