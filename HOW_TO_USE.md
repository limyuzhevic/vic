# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

---

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verifying Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

---

## Part 3: Using the Python Library

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running a Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Creating a Simple World

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

# Run world update
world.update(timestep=0.1)
```

### Advanced Features

The NLM Python API includes advanced features for complex simulations and experiments:

#### Context Managers

Automatic resource management prevents memory leaks and ensures clean shutdown:

```python
# Create agent with automatic cleanup
with pynlm.createSimpleAgent(width=100, height=100) as (brain, world, agent):
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
# Agent is automatically reset when exiting the context
```

Brain context manager for automatic reset:

```python
# Brain context manager - useful for simulation loops
with pynlm.BrainContextManager(brain) as brain_ctx:
    for step in range(100):
        brain_ctx.step(step)
```

World context manager:

```python
# World context manager for world management
with pynlm.SimpleWorldContextManager(world) as world_ctx:
    for step in range(100):
        world_ctx.update(0.1)
```

#### Factory Functions

One-line agent creation with pre-configured settings:

```python
import pynlm

# Method 1: Simple agent with custom dimensions
brain, world, agent = pynlm.createSimpleAgent(
    width=100, 
    height=100, 
    vision_width=20, 
    vision_height=20
)

# Method 2: Default settings (pre-configured)
brain, world, agent = pynlm.createDefaultAgent()

# Method 3: Training agent (advanced features enabled)
brain, world, agent = pynlm.createTrainingAgent(width=200, height=200)

# Method 4: Challenge agent (obstacles and hazards)
brain, world, agent = pynlm.createChallengeAgent(width=300, height=300)

# Method 5: Experiment agent (custom configuration)
brain, world, agent = pynlm.createExperimentAgent(
    width=400, height=400, 
    config_file="my_config.json"
)
```

#### Error Handling and Validation

Robust simulation with comprehensive error handling:

```python
import pynlm

def run_agent_simulation(width=100, height=100, num_steps=1000):
    """Run a simulation with proper error handling."""
    brain = world = agent = None
    
    try:
        # Create agent using factory function
        brain, world, agent = pynlm.createSimpleAgent(width, height)
        
        # Run simulation loop
        for step in range(num_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Error checking
            if brain.getFiringNeuronCount() == 0 and step > 100:
                print("Warning: No neurons firing - possible initialization issue")
                
    except pynlm.NLMException as e:
        print(f"NLM error: {e}")
        raise
    except Exception as e:
        print(f"Unexpected error: {e}")
        raise
    finally:
        # Cleanup
        if agent:
            agent.reset()
        if world:
            world.reset()
        print("Simulation completed or terminated")

# Run robust simulation
try:
    run_agent_simulation()
except Exception as e:
    print(f"Simulation failed: {e}")
```

#### Advanced Configuration

Create and use complex configuration files:

```python
import pynlm
import json

# Create custom configuration
config = pynlm.createDefaultConfig()

# Customize brain parameters
config.set("brain.neuron_count", 2000)           # More neurons
config.set("brain.v_thresh", -45.0)              # Different threshold
config.set("plasticity.stdp.enable", True)       # Enable STDP
config.set("plasticity.stdp.learning_rate", 0.001)  # Learning rate
config.set("neuromodulation.reward_modulation", True)  # Enable reward

# Customize world parameters
config.set("world.max_energy", 200.0)
config.set("world.width", 300.0)
config.set("world.energy_decay_rate", 0.01)

# Save configuration to file
config.saveToFile("advanced_config.json")

# Load configuration from file
new_config = pynlm.Config()
if new_config.loadFromFile("advanced_config.json"):
    brain = pynlm.createBrain(new_config)
    brain.initialize()
else:
    print("Failed to load configuration")
```

#### Custom Experiments

Building complex experiment setups:

```python
import pynlm
from typing import Tuple

def create_custom_experiment(
    config_file: str = None,
    world_width: float = 200,
    world_height: float = 200,
    vision_size: int = 30,
    enable_all_features: bool = True
) -> Tuple[pynlm.Brain, pynlm.SimpleWorld, pynlm.AgentBrain]:
    """Create a custom experiment with advanced setup."""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    if config_file:
        if not config.loadFromFile(config_file):
            raise ValueError(f"Failed to load config file: {config_file}")
    
    if enable_all_features:
        # Override with advanced settings
        config.set("brain.neuron_count", 3000)
        config.set("brain.region_count", 10)
        config.set("world.max_energy", 200.0)
        config.set("world.energy_decay_rate", 0.01)
        config.set("development.enabled", True)
        config.set("neuromodulation.reward_modulation", True)
        config.set("neuromodulation.curiosity", True)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    if not brain.initialize():
        raise RuntimeError("Failed to initialize brain")
    
    # Create world with configuration
    world = pynlm.createSimpleWorld()
    world.configure(world_width, world_height, vision_size, vision_size)
    world.setAgentStart(world_width/2, world_height/2)
    
    # Enable advanced environment features
    if enable_all_features:
        world.setMaxEnergy(200.0)
        world.setEnergyDecayRate(0.01)
        
        # Add challenge environment objects
        world.addObject(pynlm.WorldObject.create_resource(50, 50, 10.0))
        world.addObject(pynlm.WorldObject.create_resource(150, 150, 5.0))
        world.addObject(pynlm.WorldObject.create_hazard(100, 100, 3.0))
        world.addObject(pynlm.WorldObject.create_marker(180, 180))
    
    # Create and initialize agent brain
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    if enable_all_features:
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
    
    return brain, world, agent

def run_experiment(brain, world, agent, num_steps=1000):
    """Run a complete experiment with detailed monitoring."""
    for step in range(num_steps):
        # Update world state
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain simulation
        brain.step(step)
        
        # Get and apply action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print()
```

### Learning Path Integration

The advanced features build on the basic concepts from `easy_usage.md` and provide a progression:

1. **Start with factory functions** (`createSimpleAgent()`) - combines basic patterns
2. **Add context managers** - ensures proper resource management
3. **Implement error handling** - robust production code
4. **Use advanced configuration** - custom experiments
5. **Create complex experiments** - full research setups

### Quick Reference Guide

| Advanced Feature | When to Use | One-line Method |
|------------------|-------------|-----------------|
| Context Manager | Need automatic cleanup | `with pynlm.createSimpleAgent():` |
| Factory Function | Want pre-configured agent | `pynlm.createSimpleAgent()` |
| Error Handling | Production code | Wrap in try/except/finally |
| Config Files | Persistent settings | `config.saveToFile()` |
| Custom Experiments | Research setups | `create_custom_experiment()` |

### Error Codes

| Exception Type | When Raised | How to Handle |
|----------------|-------------|---------------|
| `NLMException` | NLM-specific errors | Check configuration and state |
| `ValueError` | Invalid parameters | Validate input types/ranges |
| `KeyError` | Missing config keys | Use `config.has()` to check |
| `RuntimeError` | Initialization failure | Check system resources |

---

## Part 5: Example Scripts

### Minimal Example (using factory function)

```python
import pynlm

# Simple one-liner using factory function
brain, world, agent = pynlm.createDefaultAgent()

for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation complete!")
```

### Environment Interaction with Error Handling

```python
import pynlm

def run_robust_episode(width=10, height=10, num_steps=500):
    """Run agent in world with error handling."""
    try:
        brain, world, agent = pynlm.createSimpleAgent(width, height)
        
        for step in range(num_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
            
    except Exception as e:
        print(f"Episode failed: {e}")
        raise
    finally:
        # Cleanup
        if 'agent' in locals():
            agent.reset()
        if 'world' in locals():
            world.reset()

# Run robust episodes
for episode in range(10):
    print(f"Episode {episode + 1}")
    run_robust_episode()
    print("Episode completed")
    print()
```

---

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
- Use context managers for automatic cleanup

**Memory leaks:**
- Always use context managers: `with pynlm.createSimpleAgent():`
- Or manually call `reset()` on components

**Configuration errors:**
- Check key names: `config.set("brain.neuron_count", 1000)`
- Use `config.has("key")` to check existence
- Save/load config: `config.saveToFile("config.json")`

---

## Further Reading

- [easy_usage.md](easy_usage.md) - Quick start guide with new API
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [docs/SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [docs/EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [docs/PROGRESSION_TUTORIAL.md](docs/PROGRESSION_TUTORIAL.md) - Complete learning path guide

## Quick Start Summary

For beginners:
1. Use `easy_usage.md` for quick start
2. Try `pynlm.createSimpleAgent()` factory function
3. Use context managers: `with pynlm.createSimpleAgent():`

For intermediates:
1. Use factory functions: `createTrainingAgent()`, `createChallengeAgent()`
2. Add error handling with try/except/finally
3. Use advanced configuration files

For advanced users:
1. Build custom experiments with `create_custom_experiment()`
2. Implement complex learning loops
3. Create research-grade simulations

### Key Classes and Methods

#### Brain Class

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

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
# ... and more

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

---

## Part 4: Configuration Options

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

---

## Part 5: Example Scripts

### Minimal Example

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

### Environment Interaction

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

---

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

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
