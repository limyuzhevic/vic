# NLM (Neural Learning Machine) - Documentation
# Comprehensive Guide for Users and Developers

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project designed to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

This document provides comprehensive guidance for using, developing, and extending the NLM framework.

## Quick Start Guide

### 1. Installation

#### Build from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

#### Install Python Bindings

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install Python package
pip install .

# Or install in development mode
pip install -e .
```

### 2. Basic Usage

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    
print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

### 3. Advanced Usage

```python
import pynlm

# Complete agent simulation
def run_agent_simulation(num_steps=1000):
    # 1. Setup brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # 2. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    # 3. Run simulation loop
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Optional: Apply reward and update development
        reward = extract_reward_from_world(world)
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Log progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

# Run simulation
brain, agent, world = run_agent_simulation(1000)
```

## Architecture Overview

### Core Components

The NLM framework consists of several interconnected systems:

```
┌─────────────────────────────────────────────────────────┐
│                      NLM Brain                          │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐     │
│  │   Sensory   │───▶│    Brain    │───▶│    Motor    │     │
│  │   Input    │    │   Core      │    │    Output   │     │
│  └─────────────┘    └─────────────┘    └─────────────┘     │
│        │                  │                  │        │
│        ▼                  ▼                  ▼        │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐     │
│  │  Sensory    │    │  Memory     │    │   Action    │     │
│  │  Processing │    │  Systems   │    │  Selection  │     │
│  └─────────────┘    └─────────────┘    └─────────────┘     │
│                            │                                │
│                            ▼                                │
│                    ┌─────────────┐                          │
│                    │  Prediction │                          │
│                    │   System    │                          │
│                    └─────────────┘                          │
│                            │                                │
│                            ▼                                │
│                    ┌─────────────┐                          │
│                    │Neuromodul. │                          │
│                    │  Signals    │                          │
│                    └─────────────┘                          │
│                            │                                │
│                            ▼                                │
│                    ┌─────────────┐                          │
│                    │ Development │                          │
│                    │   System    │                          │
│                    └─────────────┘                          │
└─────────────────────────────────────────────────────────┘
```

### Phase-Based Development

The NLM framework was developed through 6 phases:

1. **Phase 1**: Project skeleton, core types, configuration, neural interfaces
2. **Phase 2**: Real neural computation (LIF neurons, event-driven spikes, STDP)
3. **Phase 3**: Basic functionality and simple experiments
4. **Phase 4**: Memory, prediction, and cognition systems
5. **Phase 5**: Performance optimizations
6. **Phase 6**: Final integration (all systems working together)

## Python API Reference

### Core Classes

#### Brain Class
```python
# Creation and initialization
brain = pynlm.createBrain(config)
brain.initialize()

# Core simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()           # Get motor action

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

#### Config Class
```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### AgentBrain Class
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

#### SimpleWorld Class
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

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

### Enumerations

#### Neuron Types
```python
pynlm.NeuronType.Excitatory    # Standard excitatory neuron
pynlm.NeuronType.Inhibitory    # Inhibitory neuron
pynlm.NeuronType.Sensory       # Sensory processing neuron
pynlm.NeuronType.Motor         # Motor output neuron
pynlm.NeuronType.Modulatory    # Neuromodulatory neuron
pynlm.NeuronType.Internal     # Internal processing neuron
```

#### Developmental Stages
```python
pynlm.DevelopmentalStage.Initial         # Initial developmental state
pynlm.DevelopmentalStage.CriticalPeriod  # Critical learning period
pynlm.DevelopmentalStage.Maturation      # Physical and functional maturation
pynlm.DevelopmentalStage.Adult           # Full adult capabilities
pynlm.DevelopmentalStage.Aging           # Age-related decline
```

#### Action Types
```python
pynlm.ActionType.MoveForward    # Move forward
pynlm.ActionType.MoveBackward   # Move backward
pynlm.ActionType.MoveLeft       # Move left
pynlm.ActionType.MoveRight      # Move right
pynlm.ActionType.TurnLeft       # Turn left
pynlm.ActionType.TurnRight      # Turn right
pynlm.ActionType.Look           # Look in current direction
pynlm.ActionType.Interact       # Interact with object
pynlm.ActionType.Eat            # Consume food/drink
pynlm.ActionType.Rest           # Stay stationary
pynlm.ActionType.Wait           # Wait without action
```

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

## Example Scripts

### 1. Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### 2. Environment Interaction

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

### 3. Complete Agent with Learning

```python
import pynlm

def run_complete_agent_simulation(num_steps=1000):
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
    agent.enableStructuralPlasticity(True)
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
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_complete_agent_simulation(1000)
```

## Performance Tips

### 1. Configuration Optimization

```python
# Custom configuration for specific use cases
config = pynlm.createDefaultConfig()

# Reduce memory usage for large simulations
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.05)

# Increase learning speed
config.set("plasticity.stdp.learning_rate", 0.01)
```

### 2. Simulation Control

```python
# Use variable timesteps for different phases
if brain.getDevelopmentalStage() == pynlm.DevelopmentalStage.CriticalPeriod:
    timestep = 0.01  # Faster learning
else:
    timestep = 0.1   # Normal operation

for step in range(1000):
    world.update(timestep)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step, step * timestep)
```

### 3. Memory Management

```python
# Save checkpoint periodically for long simulations
checkpoint_interval = 1000
for step in range(total_steps):
    # ... simulation loop ...
    
    if step % checkpoint_interval == 0:
        brain.save(f"checkpoint_step_{step}.bin")
        print(f"Checkpoint saved at step {step}")
```

## Troubleshooting

### Common Issues

#### "My brain isn't doing anything"

**Cause**: Brain not properly initialized

**Solution**:
```python
# Ensure proper initialization
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()  # CRITICAL: Always call this!
```

#### "The agent isn't moving"

**Cause**: Motor command not applied

**Solution**:
```python
# Ensure motor command is applied
world.applyMotorCommand(motor_cmd, world.getSimulationTime())
```

#### "Everything is 0"

**Cause**: Brain needs time to "warm up"

**Solution**:
```python
# Run more steps for proper development
for step in range(1000):  # More steps
    brain.step(step)
```

### Build Issues

#### "CMake cannot find Python:"

```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

#### "Compilation errors:"

```bash
# Ensure requirements are met
- C++20 compiler support
- CMake version >= 3.16
- pybind11 version >= 2.11.0
- All source files present
```

#### "ImportError: No module named 'pynlm':"

```bash
pip install --force-reinstall .
```

#### "Segmentation faults:"

```bash
# Check initialization
- Ensure initialize() is called before step()
- Ensure reset() is called before re-running simulation
- Check configuration parameters
```

## Further Reading

### Technical Documentation

1. **ARCHITECTURE.md** - System architecture overview
2. **SCIENCE.md** - Scientific background and biological inspiration
3. **EXPERIMENTS.md** - Experiment descriptions and protocols
4. **PHASE6_FINAL_AUDIT.md** - Phase 6 integration audit report
5. **PERFORMANCE.md** - Performance benchmarks and optimization

### Research Papers

- "Neural Learning Machine: An Experimental Artificial Developmental Brain"
- "Event-Driven Spiking Neural Networks with Structural Plasticity"
- "Multi-Modal Sensory Processing in Artificial Neural Systems"

### Related Projects

- **OpenNN** - Neural networks library
- **Nengo** - Large-scale neural simulations
- **Brian** - Neuronal network simulators
- **Neupy** - Neural networks in Python

## Contributing

### Code Quality Guidelines

1. **Follow existing patterns** - Maintain consistency with existing codebase
2. **Add comprehensive documentation** - Document all public APIs
3. **Write unit tests** - Ensure code coverage for new functionality
4. **Follow Python style** - PEP 8 for Python code, Google style for C++

### Development Workflow

1. Create feature branch from `main`
2. Write tests for new functionality
3. Implement changes with clear commit messages
4. Run existing test suite
5. Update documentation
6. Create pull request with description

### Testing Standards

```python
# Unit test example
def test_brain_initialization():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    assert brain.initialize()
    assert brain.getTotalNeuronCount() > 0
    
def test_brain_step():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    initial_spikes = brain.getTotalSpikeCount()
    brain.step(0)
    
    assert brain.getTotalSpikeCount() >= initial_spikes
```

## License

MIT License - See LICENSE file for details.

## Copyright

Copyright (c) 2026 NLM Project - Research project for artificial developmental brains.

---

*This documentation is continuously updated. For the latest version, visit the project repository.*
