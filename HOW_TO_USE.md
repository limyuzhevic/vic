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

### Complete Agent Example

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

# Run the simulation
run_agent_simulation(1000)
```

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

## Enhanced Features (New in Version 0.1.0)

The enhanced version of NLM now includes several new features for improved usability and functionality:

### Enhanced Python Interface

The enhanced NLM Python interface now provides two main ways to use the library:

1. **Simple/Classic API** (backward compatible) - Use `pynlm.createBrain()`, `pynlm.createDefaultConfig()`
2. **Enhanced/High-level API** (new features and utilities) - Use `NLM` class, `ConfigBuilder`, `create_educational_simulation()`, etc.

#### Enhanced Interface Example

```python
import pynlm
from pynlm import ConfigBuilder, create_educational_simulation, run_episode, NLM

# Method 1: Using ConfigBuilder for advanced configuration
config = (ConfigBuilder()
         .set_neuron_count(1000)
         .set_region_count(2)
         .set_connection_probability(0.15)
         .enable_stdp(True, ltp_weight=0.01, ltd_weight=0.012, tau=20.0)
         .enable_curiosity(True)
         .enable_structural_plasticity(True)
         .build())

# Method 2: Using high-level NLM class
nlm = NLM()
nlm.setup(world_width=30.0, world_height=30.0, enable_learning=True)

# Run simulation
stats = nlm.run(1000, verbose=True)
```

### New Classes and Utilities

#### ConfigBuilder Class (New)

The `ConfigBuilder` class provides a fluent API for creating NLM configurations with validation:

```python
# Create custom configuration
config = (ConfigBuilder()
         .set_neuron_count(500)  # Will raise ValueError if negative
         .set_connection_probability(0.15)  # Will raise ValueError if out of range
         .enable_stdp(True)
         .enable_curiosity(True)
         .set_simulation_timestep(0.001)
         .set_random_seed(42)
         .build())

# Load from file
config = (ConfigBuilder()
         .load_from_file("my_config.cfg")
         .set_neuron_count(500)
         .build())
```

#### NLM Class (New)

The `NLM` class provides a high-level interface for common simulation scenarios:

```python
# Create and run simulation
nlm = NLM()
nlm.setup(world_width=20.0, world_height=20.0, enable_learning=True)

stats = nlm.run(1000, verbose=True)
print(f"Efficiency score: {stats['efficiency_score']:.4f}")
```

#### PerformanceMonitor Class (New)

The `PerformanceMonitor` class tracks simulation performance for optimization:

```python
from pynlm import create_performance_monitor

monitor = create_performance_monitor()

for step in range(100):
    monitor.start_step()
    # Run simulation step
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    monitor.end_step(brain, world)

# Get performance summary
perf_stats = monitor.get_summary()
print(f"Average step time: {perf_stats['avg_step_time_ms']:.1f} ms")
print(f"Efficiency score: {perf_stats['efficiency_score']:.4f}")
```

#### Context Managers (New)

New context managers provide automatic resource management:

```python
from pynlm import brain_context, simulation_episode

# Automatic brain lifecycle management
with brain_context() as brain:
    for step in range(1000):
        brain.step(step)

# Complete episode management with automatic cleanup
with simulation_episode(brain, world, agent, 500) as stats:
    # Simulation runs
    pass
```

#### Configuration Presets (New)

Predefined configuration presets for common scenarios:

```python
# Research-grade configuration (large, detailed)
research_config = pynlm.create_research_config()

# Educational configuration (medium complexity)
educational_config = pynlm.create_educational_config()

# Simple configuration (quick tests)
simple_config = pynlm.create_simple_config()
```

## Enhanced Factory Functions

### create_educational_simulation()

Creates a complete educational simulation with a learning agent:

```python
brain, world, agent = pynlm.create_educational_simulation()

# Enable learning
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)

# Run simulation
stats = pynlm.run_episode(brain, world, agent, 500, verbose=True)
```

### run_episode() (Enhanced)

Runs a complete simulation episode with comprehensive statistics:

```python
stats = pynlm.run_episode(
    brain, world, agent, 
    num_steps=1000, 
    verbose=True
)

print(f"Episode completed in {stats['steps_completed']} steps")
print(f"Total spikes: {stats['total_spikes']}")
print(f"Efficiency: {stats['efficiency_score']:.4f}")
print(f"Action distribution: {stats['action_counts']}")
```

## Enhanced Exception Handling

The enhanced interface provides better error messages and recovery suggestions:

```python
try:
    brain = pynlm.create_brain(config)
    brain.initialize()
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
    print("Try checking your config parameters")
except pynlm.InitializationError as e:
    print(f"Initialization error: {e}")
    print("Ensure all required subsystems are enabled")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
    print("Check that brain.initialize() was called before brain.step()")
```

## Advanced Examples

### Learning with Performance Monitoring

```python
import pynlm

# Create educational simulation
brain, world, agent = pynlm.create_educational_simulation()

# Setup performance monitoring
from pynlm import create_performance_monitor
monitor = create_performance_monitor()

# Run simulation with monitoring
for step in range(500):
    monitor.start_step()
    
    # Single simulation step
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # End step and record metrics
    monitor.end_step(brain, world)

# Get performance summary
perf_stats = monitor.get_summary()
print(f"Performance summary:")
print(f"  Steps completed: {perf_stats['steps_completed']}")
print(f"  Average step time: {perf_stats['avg_step_time_ms']:.1f} ms")
print(f"  Efficiency score: {perf_stats['efficiency_score']:.4f}")
```

### Custom Development Configuration

```python
import pynlm
from pynlm import ConfigBuilder

# Create a development-focused configuration
config = (ConfigBuilder()
         .set_neuron_count(2000)  # Larger network for development
         .set_region_count(4)     # Multiple regions
         .set_connection_probability(0.2)
         .enable_stdp(True, ltp_weight=0.015, ltd_weight=0.013, tau=25.0)
         .enable_hebbian(True)
         .enable_structural_plasticity(True)
         .enable_curiosity(True)      # Drive exploration
         .enable_novelty(True)       # Detect new experiences
         .enable_reward_modulation(True)
         .set_replay_interval(50)     # Frequent memory replay
         .set_development_interval(500)  # Longer development periods
         .build())

# Use the configuration
brain = pynlm.create_brain(config)
brain.initialize()
print(f"Development brain created with {brain.getTotalNeuronCount()} neurons")
```

## Backward Compatibility

The enhanced interface maintains full backward compatibility with existing code:

```python
# Old code still works (classic/simple API)
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)

# Enhanced code (new features)
import pynlm
from pynlm import NLM, ConfigBuilder

# High-level control
nlm = NLM()
nlm.setup(enable_learning=True)
stats = nlm.run(1000)
```

## Performance Optimization Tips

1. **Use simple configurations for testing**: `create_simple_config()`
2. **Disable learning for speed**: `enable_reward_modulation(False)`
3. **Monitor performance**: Use `PerformanceMonitor()`
4. **Use context managers**: `with simulation_episode(...)`
5. **Leverage presets**: Use predefined configurations

## Summary of New Features

| Feature | Description | When to Use |
|---------|-------------|-------------|
| `ConfigBuilder` | Fluent configuration API with validation | Custom configurations |
| `NLM` class | High-level simulation control | Common use cases |
| `PerformanceMonitor` | Performance tracking and optimization | Benchmarking |
| `brain_context` | Automatic brain lifecycle management | Reusable simulations |
| `simulation_episode` | Complete episode management | Complex scenarios |
| `create_educational_simulation()` | Ready-to-use educational setup | Teaching/learning |
| `create_performance_monitor()` | Factory for performance monitoring | Any simulation |
| Enhanced exceptions | Better error messages and recovery | Debugging |

The enhanced NLM interface provides a complete solution for neural network simulation, from simple demonstrations to complex research scenarios, with tools for optimization, monitoring, and easy configuration management.
