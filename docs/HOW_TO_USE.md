# NLM (Neural Learning Machine) - Complete User Guide

NLM (熙然, meaning "serene flow") is an experimental computational brain project that creates a neural system beginning in a primitive developmental state and acquiring increasingly complex abilities through interaction with an environment.

This document provides comprehensive guidance for using NLM, from basic setup to advanced usage.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Building and Installation](#building-and-installation)
3. [Core Concepts](#core-concepts)
4. [Python API Reference](#python-api-reference)
5. [Configuration Options](#configuration-options)
6. [Advanced Usage](#advanced-usage)
7. [Troubleshooting](#troubleshooting)
8. [Further Reading](#further-reading)

---

## Quick Start

### What is NLM?

NLM simulates brain-like computation using spiking neural networks. It includes:

- **Realistic neurons** using Leaky Integrate-and-Fire (LIF) dynamics
- **Multiple memory systems** (working, episodic, associative)
- **Neuromodulation** (dopamine, curiosity, novelty detection)
- **Cognition systems** (planning, concept formation, attention)
- **Developmental stages** affecting plasticity and learning

### The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks (neural computation)
2. **World** - The environment the brain lives in (simulation context)
3. **Agent** - The bridge connecting brain to world (I/O interface)

### Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
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

---

## Building and Installation

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Building the C++ Project

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

### Installing the Python Library

#### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

#### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Building Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
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

## Core Concepts

### Brain System Components

NLM implements a complete brain-inspired architecture with interconnected systems:

1. **Neural Core**: Spiking neurons with LIF dynamics and event-driven propagation
2. **Memory Systems**: Working memory (short-term), episodic memory (experiences), associative memory (pattern associations)
3. **Neuromodulation**: Dopamine (reward), curiosity (exploration), novelty (surprise detection)
4. **Cognition**: Planning (action sequences), concept formation (pattern discovery), attention (competitive selection)
5. **Development**: Plasticity changes over time, structural modifications, maturation stages
6. **Prediction**: Forward models and prediction error computation

### Developmental Stages

NLM brains develop through distinct stages:

- **Initial**: High plasticity, basic capabilities
- **CriticalPeriod**: Enhanced learning, sensory refinement
- **Maturation**: Balanced plasticity, stable behavior
- **Adult**: Low plasticity, stable expertise
- **Aging**: Declining plasticity, accumulated knowledge

### Key Principles

- **Closed-loop processing**: Sensor → neural processing → action → world consequences
- **Emergent behavior**: Complex cognition arises from simple neural interactions
- **Experience-dependent learning**: Brain changes through interaction with environment
- **Hierarchical organization**: Multiple levels of processing from neurons to cognition

---

## Python API Reference

### Brain Class

#### Creation and Initialization

```python
# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Core Operations

```python
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

#### Configuration

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

### AgentBrain Class

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

### SimpleWorld Class

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

### SensoryInput Class

```python
# Vision input (camera-like sensor)
vision = pynlm.Vision(width, height, channels=3)
vision.setData(pixel_data_list)

# Audio input (sound sensor)
audio = pynlm.Audio(sample_rate, num_samples)
audio.setData(audio_samples)
audio.setSampleRate(sample_rate)

# Internal signals (for modeling internal states)
internal = pynlm.InternalSignals()
internal.addSignal(value)
internal.clearSignals()
```

### Action and MotorCommand

```python
# Action representation (output of brain)
action = pynlm.Action(pynlm.ActionType.MoveForward)
# Or with parameters
action = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.3, 1.2])

# Motor commands (higher-level actions)
motor_cmd = pynlm.MotorCommand.MoveForward
motor_cmd = pynlm.MotorCommand.Interact
motor_cmd = pynlm.MotorCommand.Wait
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
pynlm.ActionType.Drink
pynlm.ActionType.Rest
pynlm.ActionType.Wait
pynlm.ActionType.Custom

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker

# Motor commands
pynlm.MotorCommand.MoveForward
pynlm.MotorCommand.TurnLeft
pynlm.MotorCommand.Interact
pynlm.MotorCommand.Wait
```

---

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
| `brain.tau_ref` | float | 2.0 | Refractory period (steps) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP potentiation time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP depression time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity (synaptogenesis/pruning) |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.synaptogenesis_rate` | float | 0.001 | Rate of new synapse formation |
| `development.pruning_rate` | float | 0.0001 | Rate of synapse elimination |
| `development.critical_period_start` | float | 0.0 | Start of critical period (developmental stage) |
| `development.critical_period_end` | float | 100.0 | End of critical period (developmental stage) |

### Environment Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `environment.name` | string | GridWorld | World simulation type |
| `environment.width` | int | 10 | World width (cells) |
| `environment.height` | int | 10 | World height (cells) |
| `environment.vision_width` | int | 8 | Agent sensing range (cells) |
| `environment.vision_height` | int | 8 | Agent sensing height (cells) |
| `environment.max_energy` | float | 100.0 | Maximum agent energy |
| `environment.energy_decay_rate` | float | 0.01 | Energy loss per timestep |

### Logging Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `logging.level` | string | INFO | Log verbosity level (DEBUG, INFO, WARNING, ERROR) |
| `logging.to_file` | bool | false | Write logs to file |
| `logging.filename` | string | nlm.log | Log file name |
| `logging.flush_interval` | float | 1.0 | Seconds between log flushes |

### Simulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation.timestep` | float | 0.001 | Simulation timestep (seconds) |
| `simulation.max_steps` | int | 10000 | Maximum simulation steps |
| `simulation.time_limit` | float | 0.0 | Maximum simulation time (seconds, 0 = unlimited) |
| `simulation.checkpoint_interval` | int | 1000 | Steps between checkpoints |
| `simulation.save_checkpoints` | bool | true | Save checkpoint files |

### Visualization Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `visualization.enabled` | bool | false | Enable visualization output |
| `visualization.update_rate` | float | 30.0 | Visualization update frequency (Hz) |
| `visualization.output_file` | string | nlm_viz.html | Output file for web visualization |

---

## Advanced Usage

### Custom Configuration File

Create a custom configuration file `my_config.json`:

```json
{
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.15,
        "v_thresh": -45.0,
        "v_rest": -65.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.002,
            "tau_plus": 25.0,
            "tau_minus": 25.0
        },
        "hebbian": {
            "enable": true
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.2
        },
        "curiosity": {
            "enable": true
        }
    },
    "environment": {
        "width": 15,
        "height": 15,
        "vision_width": 10,
        "vision_height": 10
    },
    "logging": {
        "level": "DEBUG",
        "to_file": true,
        "filename": "my_simulation.log"
    }
}
```

Load it in Python:

```python
import pynlm

config = pynlm.createDefaultConfig()
config.loadFromFile("my_config.json")

brain = pynlm.createBrain(config)
brain.initialize()
```

### Command Line Arguments

Use command-line arguments to configure the C++ executable `nlm`:

```bash
# Basic usage
./nlm

# Load configuration file
./nlm --config my_config.cfg

# Override parameters
./nlm --brain.neuron_count 3000 --plasticity.stdp.learning_rate 0.005

# Debug mode
./nlm --log_level DEBUG --save_checkpoints true

# Run specific experiment
./nlm --experiment phase6_integration --steps 5000
```

### Checkpointing and Saving

Save brain state for later resumption:

```python
import pynlm

# Create and run simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run for a while
for step in range(1000):
    brain.step(step)

# Save brain state
brain.save("my_brain_checkpoint.bin")

# Later, load and continue
brain2 = pynlm.createBrain(config)
brain2.load("my_brain_checkpoint.bin")
print(f"Loaded brain with {brain2.getTotalNeuronCount()} neurons")

# Continue simulation from saved state
for step in range(1000, 2000):
    brain2.step(step)
```

### Performance Monitoring

Track simulation performance and statistics:

```python
import time
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

start_time = time.time()
steps_completed = 0

for step in range(10000):
    brain.step(step)
    steps_completed += 1
    
    # Log statistics every 1000 steps
    if step % 1000 == 0:
        elapsed = time.time() - start_time
        steps_per_second = steps_completed / elapsed if elapsed > 0 else 0
        
        print(f"Step {step}:")
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
        print(f"  Synapses: {brain.getTotalSynapseCount()}")
        print(f"  Firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  Performance: {steps_per_second:.0f} steps/sec")
        print(f"  Memory: {brain.getFiringNeuronCount()} firing neurons")
        
        # Check for anomalies
        firing_rate = brain.getAverageFiringRate()
        if firing_rate > 100.0:
            print("  WARNING: High firing rate detected!")
        elif firing_rate < 0.1:
            print("  INFO: Low firing rate - brain may be inactive")

elapsed = time.time() - start_time
print(f"Simulation complete: {steps_completed} steps in {elapsed:.1f}s")
print(f"Average performance: {steps_completed/elapsed:.0f} steps/sec")
```

### Multi-Agent Scenarios

Create multiple agents sharing the same world:

```python
import pynlm

# Create shared world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()

# Create multiple brains
config = pynlm.createDefaultConfig()
brain1 = pynlm.createBrain(config)
brain2 = pynlm.createBrain(config)  # Different brain, same config

brain1.initialize()
brain2.initialize()

# Create agent interfaces
agent1 = pynlm.createAgentBrain(brain1)
agent2 = pynlm.createAgentBrain(brain2)

# Initialize agents with world
agent1.initialize(world)
agent2.initialize(world)

# Place agents at different positions
world.setAgentStart(5.0, 5.0)  # Agent 1
agent1.initialize(world)

world.setAgentStart(25.0, 25.0)  # Agent 2
agent2.initialize(world)

# Enable learning for both agents
agent1.enableRewardModulation(True)
agent1.enableCuriosity(True)
agent2.enableRewardModulation(True)
agent2.enableCuriosity(True)

# Run multi-agent simulation
for step in range(2000):
    world.update(0.1)
    
    # Each agent gets its own percept
    percept1 = world.getSensoryPercept()
    percept2 = world.getSensoryPercept()
    
    # Agents process independently
    agent1.processSensoryInput(percept1)
    agent2.processSensoryInput(percept2)
    
    # Brains think
    brain1.step(step)
    brain2.step(step)
    
    # Each agent acts
    action1 = agent1.decodeMotorCommand()
    action2 = agent2.decodeMotorCommand()
    
    # World applies actions
    world.applyMotorCommand(action1, world.getSimulationTime())
    world.applyMotorCommand(action2, world.getSimulationTime())
    
    if step % 200 == 0:
        print(f"Step {step}:")
        print(f"  Agent 1 - Firing: {brain1.getFiringNeuronCount()}, Curiosity: {agent1.getCuriosityLevel():.3f}")
        print(f"  Agent 2 - Firing: {brain2.getFiringNeuronCount()}, Curiosity: {agent2.getCuriosityLevel():.3f}")
        print(f"  World size: {world.getWidth()}x{world.getHeight()}")
```

### Custom Sensory and Motor Systems

Extend the basic system with custom components:

```python
import pynlm
import numpy as np

class CustomSensory(pynlm.SensoryInput):
    def __init__(self, size):
        super().__init__()
        self.size = size
    
    def getData(self):
        # Return custom sensory data
        return np.random.randn(self.size).tolist()

class CustomAction(pynlm.Action):
    def __init__(self, move_x, move_y, force):
        super().__init__(pynlm.ActionType.Custom)
        self.parameters = [move_x, move_y, force]
    
    def getParameters(self):
        return self.parameters

# Use custom components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create custom world with enhanced sensory input
class EnhancedWorld(pynlm.SimpleWorld):
    def getEnhancedPercept(self):
        vision = self.getSensoryPercept().getVision()
        # Add custom features to standard vision data
        custom_data = vision.getData()
        custom_data.append(sum(custom_data) / len(custom_data))  # Add mean feature
        return custom_data

# The custom systems integrate seamlessly with the existing framework
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

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify configuration parameters are valid

### Runtime Issues

**Memory usage too high:**
- Reduce neuron count in configuration
- Disable visualization if enabled
- Use smaller world dimensions

**Simulation too slow:**
- Increase simulation timestep
- Disable unnecessary subsystems
- Use Release build instead of Debug

**Learning not occurring:**
- Enable plasticity subsystems (`enableStructuralPlasticity`, `enableRewardModulation`)
- Check that neurons are receiving input
- Verify configuration includes plasticity parameters

**Agent not moving:**
- Ensure motor neurons are properly connected
- Check that `applyMotorCommand()` is being called
- Verify world boundaries allow movement

### Common Configuration Errors

**Invalid parameter types:**
```python
# Wrong type causes issues
config.set("brain.neuron_count", "1000")  # String instead of int
```

**Missing required parameters:**
```python
# Some systems require minimum values
config.set("brain.neuron_count", 10)  # Too few neurons
config.set("environment.width", 1)   # Too small world
```

**Conflicting parameters:**
```python
# High firing rate settings with low threshold
config.set("brain.v_thresh", -20.0)  # Very low threshold
config.set("brain.v_rest", -60.0)   # Reasonable resting potential
```

### Debugging Tips

**Enable verbose logging:**
```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("logging.level", "DEBUG")
config.set("logging.to_file", True)
config.set("logging.filename", "debug.log")

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with debug logging
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step} completed")
```

**Check system state:**
```python
print("=== Brain Status ===")
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"Development stage: {brain.getDevelopmentalStage()}")

if brain.getWorkingMemory():
    print(f"Working memory traces: {brain.getWorkingMemory().getActiveTraces()}")

if brain.getEpisodicMemory():
    print(f"Episodic episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
```

**Profile simulation performance:**
```python
import time
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Time the simulation
start = time.perf_counter()
for step in range(1000):
    brain.step(step)
end = time.perf_counter()

print(f"Simulation completed in {end-start:.3f} seconds")
print(f"Average step time: {(end-start)/1000*1000:.2f} ms")
```

---

## Further Reading

### Documentation

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background and principles
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions and results
- [ROADMAP.md](docs/ROADMAP.md) - Development roadmap and future directions
- [PERFORMANCE.md](docs/PERFORMANCE.md) - Performance optimization and scaling
- [LIMITATIONS.md](docs/LIMITATIONS.md) - Current limitations and areas for improvement

### Research Papers

- NLM Project Papers - Available on GitHub
- Neuronal Dynamics and Learning in Artificial Brains
- Developmental Principles in Artificial Neural Systems
- Neuromodulation and Learning in Spiking Neural Networks

### Related Projects

- **BrainSim** - Similar spiking neural network frameworks
- **Nengo** - Large-scale neural engineering software
- **Neuromorphic Computing** - Hardware acceleration for neural networks
- **Deep Learning** - Complementary approaches to AI

### Web Resources

- [NLM GitHub Repository](https://github.com/nlm-project/nlm)
- [Documentation Website](https://nlm.readthedocs.io)
- [Discussion Forum](https://forum.nlm-project.org)
- [Project Wiki](https://github.com/nlm-project/nlm/wiki)

---

## License

MIT License. See LICENSE file for details.

## Authors

NLM Project Contributors

For questions, issues, or contributions:
- GitHub Issues: [Report bugs and feature requests](https://github.com/nlm-project/nlm/issues)
- Documentation: [GitHub Wiki](https://github.com/nlm-project/nlm/wiki)
- Support: [Project Forum](https://forum.nlm-project.org)

---

*This documentation is continuously updated as NLM develops. Check the GitHub repository for the latest version.*