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

### Quick Start (matches easy_usage.md)

This section shows examples that match the `easy_usage.md` guide, with enhanced features:

#### Simplest Possible Example

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

#### Example: Simplest Brain

```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

#### Example: Silent Brain (with new factory methods)

```python
import pynlm

# Using new factory methods
brain = pynlm.simpleBrain()  # One-liner using new API
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

#### Example: Brain Watching a World (using new builder pattern)

```python
import pynlm

# Setup using new builder pattern
brain = pynlm.createLearningBrain()  # Factory method
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
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

#### Example: Complete Agent (enhanced with new APIs)

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning using new API
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
```

### Enhanced Python API (New Features)

The enhanced Python bindings now include:

#### Factory Methods

```python
import pynlm

# Create different types of brains using presets
brain = pynlm.createSimpleBrain()      # Quick simulation
brain = pynlm.createLearningBrain()    # Advanced learning
brain = pynlm.createDevelopmentBrain()  # Developmental learning
brain = pynlm.createExplorationBrain() # Exploration-focused
```

#### Agent System Factory

```python
# Create complete agent systems
brain, world, agent = pynlm.createAgentSystem("learning")  # String preset
# or using builder pattern
from pynlm import BrainConfigBuilder
config = BrainConfigBuilder().presetLearning().build()
```

#### Quick Start Helpers

```python
# One-line examples for quick prototyping
brain = pynlm.simpleBrain()
brain, world, agent = pynlm.worldInteraction(50)
brain, world, agent = pynlm.completeAgent(100)
```

#### Builder Pattern

```python
from pynlm import BrainConfigBuilder

# Build custom configurations
config = (BrainConfigBuilder()
          .withNeuronCount(500)
          .withSTDP(0.02f, 0.018f, 30.0f)
          .withNeuromodulation(True, True, True)
          .withEnvironment(20, 20, 10, 10)
          .presetLearning())  # Apply common preset

brain = pynlm.createBrain(config)
brain.initialize()
```

#### Validation and Utilities

```python
# Validate configurations
is_valid = pynlm.validateConfig(config)

# Get configuration report
report = pynlm.generateConfigReport(config)
print(report)
```

### Enhanced AgentBrain Features

The enhanced Python bindings include improved AgentBrain methods:

```python
agent = pynlm.createAgentBrain(brain)

# Enable features using clear method names
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Check feature status
print("Reward modulation enabled:", agent.isRewardModulationEnabled())
print("Development enabled:", agent.isDevelopmentEnabled())

# Get enhanced statistics
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
print("Prediction error:", agent.getPredictionError())
```

### Simulation Helpers

```python
import pynlm

# Run complete simulation loop
pynlm.run_simulation(brain, world, agent, 1000)

# Get statistics
stats = pynlm.get_brain_stats(brain)
print(stats)
```

### Integration Examples

#### Complete NLM System

```python
import pynlm

def create_complete_nlm_system(preset="learning"):
    """Create a complete NLM system using factory methods."""
    # Create agent system using factory
    brain, world, agent = pynlm.createAgentSystem(preset)
    
    # Configure based on preset
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(preset != "quick")
    agent.enableCuriosity(preset == "exploration" or preset == "learning")
    
    return brain, world, agent

def run_complete_simulation(num_steps=1000, preset="learning"):
    """Run a complete simulation with brain, world, and agent."""
    # Create system
    brain, world, agent = create_complete_nlm_system(preset)
    
    # Run simulation
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get sensory percept
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Decode motor command
        action = agent.decodeMotorCommand()
        
        # 6. Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Apply reward modulation
        energy = world.getAgentBody().energy
        agent.applyRewardModulation(energy, 0.0)
        
        # 8. Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    return brain, world, agent

# Run example simulation
brain, world, agent = run_complete_simulation(1000, "learning")
print("Simulation complete!")
```

#### Configuration Management

```python
import pynlm

# Create and save custom configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)

# Validate configuration
if pynlm.validateConfig(config):
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Configuration valid - brain created successfully")
else:
    print("Configuration invalid - please check")

# Get configuration report
report = pynlm.generateConfigReport(config)
print("\nConfiguration Report:")
print(report)

# Save configuration
config.saveToFile("my_config.json")
```

### Presets and Templates

The NLM library provides several preset configurations:

```python
import pynlm

# Quick exploration
brain, world, agent = pynlm.createAgentSystem("exploration")

# Advanced learning
brain, world, agent = pynlm.createAgentSystem("learning")

# Developmental learning
brain, world, agent = pynlm.createAgentSystem("development")

# Quick test
brain, world, agent = pynlm.createAgentSystem("quick")
```

Each preset has different characteristics:

- **Quick**: Small brain, fast simulation, good for testing
- **Exploration**: Focus on curiosity and novelty detection
- **Development**: Emphasis on developmental stages and plasticity
- **Learning**: Balanced approach for general learning tasks

### Error Handling and Best Practices

```python
import pynlm

try:
    # Create brain with configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(1000):
        brain.step(step)
        
except pynlm.NLMError as e:
    print(f"NLM Error: {e}")
except pynlm.BrainInitializationError as e:
    print(f"Brain initialization failed: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")

# Always call initialize() before using brain
# Always call reset() before re-running simulations
# Use try/except blocks for robust error handling
```

### Advanced Usage

#### Custom Configurations with Builder Pattern

```python
from pynlm import BrainConfigBuilder

# Create custom configuration using builder pattern
config = (BrainConfigBuilder()
          .withNeuronCount(2000)
          .withSTDP(0.05f, 0.048f, 50.0f)
          .withNeuromodulation(True, True, True)
          .setCustom("custom_param", "value")
          .build())

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Simulation Monitoring

```python
import pynlm
import time

brain, world, agent = pynlm.createAgentSystem("learning")

start_time = time.time()
steps_completed = 0

for step in range(10000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    steps_completed += 1
    
    # Monitor progress
    if step % 1000 == 0:
        elapsed = time.time() - start_time
        print(f"Step {step}: {steps_completed/elapsed:.0f} steps/sec")
        print(f"  Firing: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
```

### Migration Guide

If you're upgrading from the original Python bindings:

1. **Factory Methods**: Replace direct brain creation with `pynlm.createBrain()`
2. **Configuration**: Use `pynlm.createDefaultConfig()` instead of manual config creation
3. **Agent Setup**: Use `pynlm.createAgentBrain(brain)` for cleaner syntax
4. **Enhanced Features**: Take advantage of new factory presets and builder patterns

```python
# Old way (still works):
brain = pynlm.Brain(pynlm.createDefaultConfig())

# New way (recommended):
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Even better - using presets:
brain = pynlm.createLearningBrain()  # Pre-configured for learning
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

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
