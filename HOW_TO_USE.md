# NLM (Neural Learning Machine) - HOW TO USE

## Table of Contents

1. [Overview](#overview)
2. [Part 1: Building the C++ Project from Source](#part-1-building-the-c-project-from-source)
   - [Prerequisites](#prerequisites)
   - [Build Steps](#build-steps)
   - [Build Outputs](#build-outputs)
   - [Build Options](#build-options)
3. [Part 2: Installing the Python Library](#part-2-installing-the-python-library)
   - [From Source (Recommended for Development)](#from-source-recommended-for-development)
   - [Using pyproject.toml (Modern Python Packaging)](#using-pyprojecttoml-modern-python-packaging)
   - [Verifying Installation](#verifying-installation)
4. [Part 3: Using the Python Library](#part-3-using-the-python-library)
   - [Basic Usage](#basic-usage)
     - [Creating a Brain](#creating-a-brain)
     - [Running a Simulation](#running-a-simulation)
     - [Creating a Simple World](#creating-a-simple-world)
   - [Complete Agent Example](#complete-agent-example)
   - [Key Classes and Methods](#key-classes-and-methods)
     - [Brain Class](#brain-class)
     - [Config Class](#config-class)
     - [AgentBrain Class](#agentbrain-class)
     - [SimpleWorld Class](#simpleworld-class)
   - [Enumerations](#enumerations)
5. [Part 4: Configuration Options](#part-4-configuration-options)
   - [Brain Configuration](#brain-configuration)
   - [Plasticity Configuration](#plasticity-configuration)
   - [Neuromodulation Configuration](#neuromodulation-configuration)
6. [Part 5: Example Scripts](#part-5-example-scripts)
   - [Minimal Example](#minimal-example)
   - [Environment Interaction](#environment-interaction)
7. [Part 6: Advanced Usage](#part-6-advanced-usage)
   - [Advanced Configuration](#advanced-configuration)
   - [Performance Optimization](#performance-optimization)
   - [Real-World Applications](#real-world-applications)
8. [Part 7: Troubleshooting](#part-7-troubleshooting)
9. [Part 8: Further Reading](#part-8-further-reading)

---

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API
4. Advanced usage patterns and optimization
5. Troubleshooting and best practices

---

<details>
<summary><strong>Part 1: Building the C++ Project from Source</strong></summary>

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

</details>

---

<details>
<summary><strong>Part 2: Installing the Python Library</strong></summary>

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
brain.initialize()
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

</details>

---

<details>
<summary><strong>Part 3: Using the Python Library</strong></summary>

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

</details>

---

<details>
<summary><strong>Part 4: Configuration Options</strong></summary>

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

</details>

---

<details>
<summary><strong>Part 5: Example Scripts</strong></summary>

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

</details>

---

<details>
<summary><strong>Part 6: Advanced Usage</strong></summary>

### Advanced Configuration

#### Custom Brain Architectures

```python
def create_advanced_brain(config):
    """Create brain with specific region configuration."""
    
    # Add different neuron types
    config.set("brain.neuron_count", 2000)
    config.set("brain.excitatory_ratio", 0.8)
    config.set("brain.inhibitory_ratio", 0.2)
    
    # Configure plasticity
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", False)
    
    # Create and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return brain
```

#### Performance Monitoring

```python
import time

def run_monitored_simulation(brain, world, agent, num_steps):
    """Run simulation with performance metrics."""
    
    start_time = time.time()
    step_times = []
    
    for step in range(num_steps):
        step_start = time.time()
        
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Apply motor command
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record timing
        step_time = time.time() - step_start
        step_times.append(step_time)
        
        # Log every 100 steps
        if step % 100 == 0:
            avg_time = sum(step_times[-100:]) / 100
            print(f"Step {step}: {avg_time*1000:.2f}ms avg, "
                  f"{brain.getFiringNeuronCount()} firing neurons")
    
    total_time = time.time() - start_time
    print(f"\nPerformance Summary:")
    print(f"Total time: {total_time:.2f}s")
    print(f"Steps per second: {num_steps/total_time:.1f}")
    print(f"Average step time: {total_time/num_steps*1000:.2f}ms")
    
    return step_times
```

#### Memory Management

```python
import pynlm
import gc

def advanced_memory_example():
    """Advanced memory management example."""
    
    # Enable garbage collection
    gc.enable()
    
    # Create brain with specific configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with all subsystems
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world) if 'world' in locals() else None
    
    # Enable all learning mechanisms
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Create world with monitoring
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Run simulation with checkpoints
    for step in range(2000):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Save checkpoints periodically
        if step % 500 == 0:
            checkpoint_file = f"checkpoint_{step}.bin"
            brain.save(checkpoint_file)
            print(f"Saved checkpoint: {checkpoint_file}")
        
        # Load previous checkpoint if available
        if step > 1000:
            prev_checkpoint = f"checkpoint_1000.bin"
            try:
                brain.load(prev_checkpoint)
                print(f"Loaded checkpoint: {prev_checkpoint}")
            except FileNotFoundError:
                pass
```

### Performance Optimization

#### Speed Tips

```python
import pynlm

def optimized_simulation(brain, world, agent, num_steps):
    """Optimized simulation with performance tips."""
    
    # Optimize configuration
    config = brain.getConfig() if hasattr(brain, 'getConfig') else None
    if config:
        # Use fewer neurons for speed
        current_neuron_count = config.get("brain.neuron_count", 1000)
        if current_neuron_count > 500:
            config.set("brain.neuron_count", 500)
            brain = pynlm.createBrain(config)
            brain.initialize()
        
        # Disable unused features
        if config.get("plasticity.structural.enable", True):
            config.set("plasticity.structural.enable", False)
        
        # Reduce synapse density
        if config.get("brain.synapse_density", 0.1) > 0.05:
            config.set("brain.synapse_density", 0.05)
    
    # Batch processing for efficiency
    batch_size = 10
    for step in range(num_steps):
        batch_start = step // batch_size * batch_size
        
        for batch_step in range(batch_size):
            current_step = batch_start + batch_step
            
            if current_step >= num_steps:
                break
                
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(current_step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

#### Memory Pools and Efficiency

```python
import pynlm
import numpy as np

def efficient_memory_simulation(brain, world, agent, num_steps):
    """Simulation with memory pools for efficiency."""
    
    # Pre-allocate arrays for efficiency
    percept_cache = []
    action_cache = []
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get and cache percept
        percept = world.getSensoryPercept()
        percept_cache.append(percept)
        
        # Reuse percept from cache (every 5 steps)
        if step % 5 == 0 and step > 0:
            cached_percept = percept_cache[-10]
            agent.processSensoryInput(cached_percept)
        else:
            agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Cache and reuse actions
        action = agent.decodeMotorCommand()
        action_cache.append(action)
        
        # Use action from cache if recent
        if step % 3 == 0 and step > 0:
            cached_action = action_cache[-5]
            world.applyMotorCommand(cached_action, world.getSimulationTime())
        else:
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Clear old caches periodically
        if step % 100 == 0:
            percept_cache = percept_cache[-20:]
            action_cache = action_cache[-20:]
```

### Real-World Applications

#### Example 1: Maze Navigation

```python
def maze_navigation_example():
    """Brain learns to navigate a maze."""
    
    # Create world with walls
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=5, visionHeight=5)
    
    # Add maze walls (example pattern)
    for x in range(0, 50, 5):
        world.addObject(pynlm.WorldObject(x, 25, pynlm.WorldObjectType.Wall, value=1.0))
    
    # Create learning brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.curiosity.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run episode
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Check if found goal
        if world.getSensoryPercept().getInternal() and world.getSensoryPercept().getInternal()[0] > 0.8:
            print(f"Goal found at step {step}!")
            break
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    return brain.getAverageFiringRate()
```

#### Example 2: Environment Exploration

```python
def environment_exploration_example():
    """Brain explores and learns about environment."""
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=7, visionHeight=7)
    world.reset()
    
    # Add resources
    resources = [
        (5, 5), (10, 10), (15, 15), (20, 20), (25, 25)
    ]
    
    for x, y in resources:
        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
    
    # Create curious brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)
    config.set("neuromod.curiosity.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable exploration features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Explore environment
    visited_positions = set()
    
    for step in range(1000):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Track visited positions
        x, y = world.getAgentBody().getPosition()
        visited_positions.add((round(x, 1), round(y, 1)))
        
        # Try to find new resources
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}: Explored {len(visited_positions)} positions")
            print(f"Curiosity level: {agent.getCuriosityLevel():.2f}")
    
    print(f"Total unique positions visited: {len(visited_positions)}")
    return len(visited_positions)
```

#### Example 3: Adaptive Behavior

```python
def adaptive_behavior_example():
    """Brain adapts its behavior based on experience."""
    
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=6, visionHeight=6)
    world.reset()
    
    # Add dynamic elements
    import random
    random.seed(42)
    
    # Moving hazard
    hazard_pos = (20, 20)
    hazard_direction = 0.1  # Move right
    
    # Resource that regenerates
    resource_pos = (30, 30)
    resource_regeneration = False
    
    # Create adaptive brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 400)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable adaptive learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    
    # Adaptive behavior loop
    hazard_avoidance = False
    resource_seeking = False
    
    for step in range(1000):
        world.update(0.1)
        
        # Update dynamic elements
        hazard_pos = (hazard_pos[0] + hazard_direction, hazard_pos[1])
        if hazard_pos[0] > 40 or hazard_pos[0] < 0:
            hazard_direction = -hazard_direction
        
        if not resource_regeneration and step % 300 == 0:
            resource_regeneration = True
            resource_pos = (random.randint(10, 35), random.randint(10, 35))
        
        # Process and adapt
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Adaptive decision making
        action = agent.decodeMotorCommand()
        
        # Modify behavior based on experience
        if step % 200 == 0:
            # Learn from experience
            curiosity = agent.getCuriosityLevel()
            if curiosity > 0.7:
                # High curiosity = exploration
                resource_seeking = True
                hazard_avoidance = False
            elif curiosity < 0.3:
                # Low curiosity = routine
                resource_seeking = False
                hazard_avoidance = True
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 200 == 0:
            print(f"Step {step}:")
            print(f"  Curiosity: {agent.getCuriosityLevel():.2f}")
            print(f"  Behavior - Seeking: {resource_seeking}, Avoiding: {hazard_avoidance}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    
    return agent.getPredictionError()
```

</details>

---

<details>
<summary><strong>Part 7: Troubleshooting</strong></summary>

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
- Check for memory corruption with large neuron counts

**Common Configuration Problems:**
- Invalid parameter values
- Missing required parameters
- Incorrect data types

### Debugging Tips

**Memory Issues:**
```python
import tracemalloc
tracemalloc.start()

# Run simulation
brain.initialize()
for step in range(1000):
    brain.step(step)

# Check memory usage
snapshot = tracemalloc.take_snapshot()
top_stats = snapshot.statistics('lineno')
for stat in top_stats[:10]:
    print(stat)
```

**Performance Monitoring:**
```python
import time
import pynlm

def profile_simulation(brain, world, agent, num_steps):
    start_time = time.time()
    
    for step in range(num_steps):
        step_start = time.time()
        
        # Run simulation step
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Log performance
        step_time = time.time() - step_start
        if step % 100 == 0:
            avg_time = (time.time() - start_time) / (step + 1)
            print(f"Step {step}: {step_time*1000:.2f}ms, Avg: {avg_time*1000:.2f}ms")
    
    total_time = time.time() - start_time
    print(f"\nTotal time: {total_time:.2f}s")
    print(f"Steps per second: {num_steps/total_time:.1f}")
```

</details>

---

<details>
<summary><strong>Part 8: Further Reading</strong></summary>

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [ROADMAP.md](docs/ROADMAP.md) - Development roadmap
- [LIMITATIONS.md](docs/LIMITATIONS.md) - Current limitations

### Getting Help

**Online Resources:**
- GitHub Repository: [NLM GitHub](https://github.com/your-username/nlm)
- Discussion Forum: [Community](https://community.example.com)
- Issue Tracker: [GitHub Issues](https://github.com/your-username/nlm/issues)

**Quick Start for New Users:**
1. Read `easy_usage.md` for beginner-friendly introduction
2. Try the examples in `HOW_TO_USE.md`
3. Consult `docs/EXPERIMENTS.md` for real results
4. Check `docs/ARCHITECTURE.md` for technical understanding

### Citation

If you use NLM in your research, please cite:

```bibtex
@article{nlm2026,
    title={NLM: An Experimental Artificial Developmental Brain},
    author={NLM Team},
    year={2026},
    note={Phase 6: Final Integration}
}
```

### Contributing

NLM is an open research project. Contributions are welcome:

- Report bugs and issues
- Share experiment results
- Suggest improvements
- Help document new features
- Test on different platforms

See the [CONTRIBUTING.md](CONTRIBUTING.md) file for detailed guidelines.

</details>

---