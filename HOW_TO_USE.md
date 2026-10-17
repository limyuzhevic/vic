# NLM (Neural Learning Machine) - How To Use

## Quick Start Guide

This document provides comprehensive guidance for using the NLM (Neural Learning Machine) project. Start with the **Quick Start** section if you're new, or use the **Table of Contents** to navigate directly to specific topics.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Building the C++ Project](#building-the-c-project)
3. [Python Installation](#python-installation)
4. [Python API Overview](#python-api-overview)
5. [Configuration Reference](#configuration-reference)
6. [Example Scripts](#example-scripts)
7. [Troubleshooting](#troubleshooting)
8. [Advanced Topics](#advanced-topics)
9. [References](#references)

---

# Quick Start

## Installation

NLM supports both C++ and Python interfaces. Choose the option that's most relevant for your needs.

### Option 1: Python (Recommended for most users)

```bash
# Install dependencies
pip install pybind11 scikit-build-core pytest numpy

# Install NLM in development mode (recommended)
pip install -e .

# Or install from source
pip install .
```

### Option 2: C++ Development

```bash
# Clone the repository
# Build with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## First NLM Program

### Python Example

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"and {brain.getTotalSynapseCount()} synapses")

# Run simulation
for step in range(100):
    brain.step(step)

print("Simulation complete!")
```

### C++ Example

```cpp
#include <pynlm.h>
#include <iostream>

int main() {
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(config);
    if (brain->initialize()) {
        std::cout << "Brain created with " 
                 << brain->getTotalNeuronCount() << " neurons" << std::endl;
        
        for (int step = 0; step < 100; ++step) {
            brain->step(step);
        }
        
        std::cout << "Simulation complete!" << std::endl;
    }
    
    return 0;
}
```

## Further Reading

- [Python API Overview](#python-api-overview) - Learn the Python interface
- [Configuration Reference](#configuration-reference) - Customize brain parameters
- [Example Scripts](#example-scripts) - See more complex examples

---

# Building the C++ Project

## Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

## Build Steps

### Standard Build

```bash
# Navigate to project directory
cd /path/to/nlm

# Create build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build Options

#### Debug Build (for development)

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

#### Custom Compiler

```bash
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc
```

#### Custom Installation

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Build Outputs

After successful compilation, you'll have these targets:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

## Common Build Issues

### CMake cannot find Python

```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

### Compilation errors

- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

---

# Python Installation

## Modern Python Packaging

NLM uses `pyproject.toml` with scikit-build-core for modern Python packaging.

### Installation Methods

#### Method 1: pip install (Recommended)

```bash
# Install with automatic build
pip install .

# Install in development mode
pip install -e .
```

#### Method 2: Build from source

```bash
# Install build tools
pip install build

# Build distribution
python -m build

# Install from wheel
pip install dist/*.whl
```

#### Method 3: Manual installation

```bash
# Install dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .
```

## Verification

### Version Check

```python
import pynlm
print(pynlm.__version__)
```

### Basic Functionality Test

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print("✓ NLM is working correctly!")
```

## Python Module Structure

The Python interface provides several main modules:

- **pynlm**: Core NLM functionality
- **pynlm.brain**: Brain-specific operations
- **pynlm.agent**: Agent interface
- **pynlm.world**: World simulation

## Common Python Issues

### ImportError: No module named 'pynlm'

```bash
# Force reinstall
pip install --force-reinstall .
```

### AttributeError

```bash
# Ensure you're using the correct Python environment
pip list | grep pynlm
```

---

# Python API Overview

## Key Classes

### Brain Class

The main interface for neural computation.

#### Creation

```python
brain = pynlm.createBrain(config)
```

#### Initialization

```python
brain.initialize()
```

#### Core Operations

```python
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state
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
```

#### State Management

```python
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state
```

### Config Class

Configuration management.

#### Creation

```python
config = pynlm.createDefaultConfig()
```

#### Query and Modify

```python
if config.has("brain.neuron_count"):
    keys = config.getKeys()

config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)
```

#### File I/O

```python
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

### AgentBrain Class

Connects brain to the world.

#### Creation

```python
agent = pynlm.createAgentBrain(brain)
```

#### Initialization

```python
agent.initialize(world)
```

#### Sensory/Motor Interface

```python
agent.processSensoryInput(percept)
motor_cmd = agent.decodeMotorCommand()
```

#### Neuromodulation

```python
agent.applyRewardModulation(reward, predicted_reward)
```

#### Development

```python
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()
```

### SimpleWorld Class

Environment simulation.

#### Creation

```python
world = pynlm.createSimpleWorld()
```

#### Configuration

```python
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
```

#### Control

```python
world.reset()
world.update(timestep)
world.setRandomSeed(42)
```

#### Agent Interface

```python
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)
```

#### State Access

```python
percept = world.getSensoryPercept()
body = world.getAgentBody()
```

## Core Types

### Neuron Types

```python
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal
```

### Developmental Stages

```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

### Action Types

```python
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
# ... and more
```

### World Object Types

```python
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

---

# Configuration Reference

## Configuration System

Configuration parameters use hierarchical key names. Common patterns:

- `brain.neuron_count`: Brain-level parameters
- `plasticity.stdp.learning_rate`: Plasticity mechanism parameters
- `neuromod.dopamine.scale`: Neuromodulation parameters

## Configuration Categories

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

## Configuration Usage

### Creating Configuration

```python
# Create empty configuration
config = pynlm.Config()

# Set values
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.01f)

# Use preset configurations
config = pynlm.createDefaultConfig()
config = pynlm.createSmallConfig()
config = pynlm.createLargeConfig()
```

### Accessing Configuration

```python
# Check if parameter exists
if config.has("brain.neuron_count"):
    value = config.get("brain.neuron_count")

# Get all keys
keys = config.getKeys()

# Modify values
config.set("brain.neuron_count", 1500)
```

### File I/O

```python
# Save configuration
config.saveToFile("config.json")

# Load configuration
config.loadFromFile("config.json")
```

## Preset Configurations

### createDefaultConfig()

Balanced configuration for general-purpose simulations.

### createSmallConfig()

Minimal configuration for rapid testing and prototyping.

### createLargeConfig()

Expanded configuration for complex simulations and research.

---

# Example Scripts

## Minimal Example

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

## Environment Interaction

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

## Complete Agent Example

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
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
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

# Troubleshooting

## Build Issues

### CMake cannot find Python

```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

### Compilation errors

- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

## Runtime Issues

### ImportError: No module named 'pynlm'

```bash
pip install --force-reinstall .
```

### Segmentation faults

- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify configuration parameters are valid

### No sensory input detected

```bash
# Ensure world is properly configured and reset
world.reset()
world.update(0.1)
percept = world.getSensoryPercept()
```

### Brain not learning

```bash
# Check plasticity is enabled
config.set("plasticity.stdp.enable", true)
config.set("plasticity.hebbian.enable", true)

# Check neuromodulation is enabled
agent.enableRewardModulation(true)
```

---

# Advanced Topics

## Multi-Agent Systems

For complex behaviors, consider implementing multiple interacting agents:

```python
# Create multiple agents
agents = []
for i in range(3):
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    agents.append(agent)

# Agents interact with shared world
shared_world = pynlm.createSimpleWorld()
for agent in agents:
    agent.initialize(shared_world)
```

## Memory Systems

Explore advanced memory capabilities:

```python
# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Working memory for active information
working_memory.storeToNeuron(neuron_id, activation)
activation = working_memory.getActivation(neuron_id)

# Episodic memory for experience storage
episodic_memory.storeEpisode(episode)
```

## Prediction and Cognition

Implement cognitive functions:

```python
# Access prediction system
prediction_system = brain.getPredictionSystem()

# Access cognition systems
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
attention = brain.getAttention()
```

## Development and Aging

Study developmental processes:

```python
# Monitor development
for step in range(1000):
    agent.updateDevelopment(0.1)
    stage = agent.getDevelopmentalStage()
    
    # Adapt behavior based on developmental stage
    if stage == pynlm.DevelopmentalStage.Adult:
        # Stable, learned behavior
        pass
    elif stage == pynlm.DevelopmentalStage.Initial:
        # High plasticity, exploration
        pass
```

---

# References

## Documentation

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PHASE6_FINAL_RESULTS.md](docs/PHASE6_FINAL_RESULTS.md) - Final experimental results

## Research Papers

- "Neural Learning Machine: An Artificial Developmental Brain"
- "Integrated Memory Systems in Spiking Neural Networks"
- "Neuromodulation and Learning in Artificial Brains"
- "Event-Driven Spiking Neural Computation"

## Online Resources

- [NLM GitHub Repository](https://github.com/yourusername/nlm)
- [Documentation Wiki](https://wiki.github.com/yourusername/nlm)
- [Discussion Forum](https://github.com/yourusername/nlm/discussions)
- [Bug Tracker](https://github.com/yourusername/nlm/issues)

## Technical Support

For technical questions or support:

1. **GitHub Issues**: Report bugs and feature requests
2. **Stack Overflow**: Use with `[nlm]` tag
3. **Email**: support@neural-learning-machine.org
4. **Slack/Discord**: Join official NLM community

---

This documentation provides comprehensive guidance for using the NLM (Neural Learning Machine) project. Start with the Quick Start for basic usage, or explore the detailed sections for advanced functionality and troubleshooting.