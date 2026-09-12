# NLM (Neural Learning Machine) Documentation

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20 with Python bindings. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document provides a complete guide for using NLM, from building to usage.

## Quick Start

### Basic Usage

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent Example

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

## Building

### Prerequisites

- C++ Compiler: GCC 10+, Clang 12+, or MSVC 2019+
- CMake: Version 3.16 or higher
- Python: 3.8+
- pybind11: Version 2.11.0+

### Build Steps

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Build Options

```bash
# Debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Installation

### From Source

```bash
# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install Python bindings
pip install -e .
```

### Verification

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

## Key Classes

### Brain Class

#### Creation and Initialization
```python
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Simulation
```python
brain.step(step_number)           # Run one step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state
```

#### Input/Output
```python
brain.receiveSensoryInput(input)  # Inject sensory data
action = brain.produceAction()    # Get motor action
```

#### Statistics
```python
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance
brain.getTotalSpikeCount()       # Total spikes
brain.getDevelopmentalStage()    # Current developmental stage
```

#### State Management
```python
brain.save("checkpoint.bin")      # Save state
brain.load("checkpoint.bin")      # Load state
```

### Config Class

#### Basic Operations
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)  # Modify value
value = config.getOr("brain.neuron_count", 1000)  # Get with default
exists = config.has("brain.neuron_count")  # Check existence
keys = config.getKeys()  # Get all keys
```

#### File I/O
```python
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

### AgentBrain Class

#### Creation and Initialization
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

#### Core Operations
```python
agent.processSensoryInput(percept)    # Process sensory input
motor_cmd = agent.decodeMotorCommand()  # Decode motor command
agent.applyRewardModulation(reward, predicted_reward)  # Apply reward modulation
agent.updateDevelopment(timestep)      # Update development
```

#### Statistics
```python
agent.getCuriosityLevel()       # Current curiosity level
agent.getNoveltyLevel()         # Current novelty level
agent.getPredictionError()      # Prediction error
agent.getNeuromodulationLevel() # Current neuromodulation level
agent.getDevelopmentalStage()  # Current developmental stage
```

#### Subsystem Control
```python
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)
```

### SimpleWorld Class

#### Creation and Configuration
```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)
```

#### Control
```python
world.update(timestep)
world.applyMotorCommand(motor_cmd, current_time)
world.setRandomSeed(42)
```

#### State Access
```python
percept = world.getSensoryPercept()
body = world.getAgentBody()
world.isValidPosition(x, y)
```

#### Properties
```python
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

### Enumerations

#### Neuron Types
```python
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal
```

#### Developmental Stages
```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

#### Action Types
```python
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest
pynlm.ActionType.Wait
pynlm.ActionType.Custom
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

## Examples

### Example 1: Simple Brain Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

### Example 2: Complete Agent System

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
world.reset()
world.setAgentStart(15.0, 15.0)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(2000):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process input and think
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action and execute
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning
    reward = 0.0  # Calculate reward based on action result
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 200 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
```

### Example 3: Custom Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Configure for high-performance simulation
config.set("brain.neuron_count", 5000)           # More neurons
config.set("brain.synapse_density", 0.2)         # More connections
config.set("plasticity.stdp.enable", True)      # Enable STDP
config.set("plasticity.stdp.learning_rate", 0.01)  # Faster learning
config.set("neuromod.dopamine.scale", 2.0)      # Stronger reward signals

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

## Performance Tips

1. **Start Small**: Begin with 100-500 neurons for testing
2. **Monitor Stats**: Use brain statistics to debug issues
3. **Use Checkpoints**: Save and reload brain states for long simulations
4. **Enable Features**: Gradually enable complex features like curiosity and development
5. **Optimize Configuration**: Adjust neuron count and synapse density based on requirements

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions