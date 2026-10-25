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

### Enhanced Python API (NEW!)

The NLM Python API has been significantly enhanced with the following new features:

#### Enhanced Brain Class

##### New Methods:

- **`stepMultiple(steps, timestep=0.001)`** - Perform multiple simulation steps efficiently
- **`simulateEpisode(duration, timestep=0.001)`** - Run simulation for a specified duration
- **`getPerformanceMetrics()`** - Get comprehensive performance metrics
- **`enableAllPlasticity()`** - Enable all plasticity mechanisms (Hebbian, STDP, structural)
- **`enableAllNeuromodulation()`** - Enable all neuromodulation systems
- **`getLearningStats()`** - Get statistics about learning mechanisms
- **`getMemoryStats()`** - Get statistics about memory systems

##### Usage Example:

```python
import pynlm

# Create enhanced brain with Phase 4 configuration
brain = pynlm.createBrainFromPhase(4)
brain.initialize()

# Enable all learning systems
brain.enableAllPlasticity()
brain.enableAllNeuromodulation()

# Run efficient multiple steps
brain.stepMultiple(list(range(1000)))

# Get comprehensive metrics
metrics = brain.getPerformanceMetrics()
print(f"Performance: {metrics['average_firing_rate']:.3f} Hz, "
      f"{metrics['firing_neurons']} neurons firing")

# Analyze learning systems
learning_stats = brain.getLearningStats()
memory_stats = brain.getMemoryStats()
```

#### Enhanced AgentBrain Class

##### New Methods:

- **`runEpisode(world, numSteps)`** - Run complete episode with agent and world
- **`runControlledEpisode(world, condition, callback, timestep=0.1)`** - Run episode with custom control logic
- **`getComprehensiveStats()`** - Get comprehensive agent statistics
- **`enableAllSubsystems(enable)`** - Enable/disable all adaptive systems

##### Usage Example:

```python
import pynlm

# Setup enhanced agent
brain = pynlm.createBrainFromPhase(3)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all adaptive systems
agent.enableAllSubsystems(True)

# Run complete episode
agent.runEpisode(world, 500)

# Get comprehensive statistics
stats = agent.getComprehensiveStats()
print(f"Stats: Curiosity={stats['curiosity_level']:.3f}, "
      f"Neuromod={stats['neuromodulation_level']:.3f}")
```

#### Global Functions

##### New Functions:

- **`createBrainFromPhase(phase, scale=1)`** - Create brain configured for specific development phase
- **`runPhase2Experiment(numSteps, scale=1)`** - Run Phase 2 basic neural computation experiment
- **`runPhase6IntegrationDemo()`** - Run Phase 6 integration demo
- **`getVersionInfo()`** - Get version information about the NLM system
- **`getSystemCapabilities()`** - Get information about supported system capabilities

##### Usage Example:

```python
import pynlm

# Get system information
version = pynlm.getVersionInfo()
print(f"NLM Version: {version['phase']} (Phase {version['major']}.{version['minor']})")

capabilities = pynlm.getSystemCapabilities()
print(f"Supported phases: {capabilities['phases_supported']}")
print(f"Memory systems: {capabilities['memory_systems']}")

# Run Phase 2 experiment
spikes = pynlm.runPhase2Experiment(1000)
print(f"Phase 2 experiment generated {spikes} spikes")

# Run Phase 6 demo
results = pynlm.runPhase6IntegrationDemo()
print(f"Integration test completed with {results['totalReward']:.3f} avg reward")
```

#### Configuration Management

##### New Features:

- **Phase-based configuration presets** - `configureForPhase(phase, scale=1)`
- **Enhanced configuration methods** - `getInt()`, `getFloat()`, `setInt()`, etc.
- **Configuration cloning** - `clone()` method
- **Configuration validation** - `validate()` method
- **Phase-specific configuration getter** - `getPhaseSpecificConfig(phase)`

##### Usage Example:

```python
import pynlm

# Create configuration for Phase 6
config = pynlm.createDefaultConfig()
config.configureForPhase(6, scale=0.5)  # Use 1000 neurons instead of 2000

# Validate configuration
if config.validate():
    brain = pynlm.createBrain(config)
    brain.initialize()
else:
    print("Configuration validation failed!")

# Get phase-specific configuration
phase6_config = config.getPhaseSpecificConfig(6)
print(f"Phase 6 config: {phase6_config.summary()}")
```

#### Exception Handling

##### New Exception Classes:

- **`pynlm.RuntimeError`** - General runtime errors
- **`pynlm.InvalidArgumentError`** - Invalid argument errors  
- **`pynlm.OutOfRangeError`** - Index out of range errors
- **`pynlm.LogicError`** - Logic errors

##### Usage Example:

```python
import pynlm

def safeBrainOperation():
    try:
        brain = pynlm.createBrainFromPhase(4)
        brain.initialize()
        
        for i in range(100):
            brain.step(i)
            
        print("Operation completed successfully")
        
    except pynlm.InvalidArgumentError as e:
        print(f"Invalid argument: {e}")
    except pynlm.RuntimeError as e:
        print(f"Runtime error: {e}")
    except Exception as e:
        print(f"Unexpected error: {type(e).__name__}: {e}")

safeBrainOperation()
```

#### Advanced Examples

##### Real-Time Monitoring:

```python
import pynlm
import time

def monitorBrain(brain, duration=10.0):
    start_time = time.time()
    
    while time.time() - start_time < duration:
        metrics = brain.getPerformanceMetrics()
        memory_stats = brain.getMemoryStats()
        
        print(f"\r[{time.time() - start_time:5.1f}s] "
              f"Rate: {metrics['average_firing_rate']:6.3f} Hz | "
              f"Neurons: {metrics['firing_neurons']:4d} | "
              f"Spikes: {metrics['total_spikes']:6d} | "
              f"Dev: {brain.getDevelopmentalStage():2d}", end="")
        
        brain.step(static_cast<size_t>((time.time() - start_time) * 100.0))
        time.sleep(0.01)
    
    print("\nMonitoring completed!")

brain = pynlm.createBrainFromPhase(4)
brain.initialize()
monitorBrain(brain, 5.0)
```

##### Learning Analysis:

```python
import pynlm

def analyzeLearningProgression(brain, phases=[2, 3, 4, 6], scale=0.5):
    results = {}
    
    for phase in phases:
        print(f"\n=== Testing Phase {phase} ===")
        
        brain = pynlm.createBrainFromPhase(phase, scale)
        brain.initialize()
        brain.enableAllPlasticity()
        brain.enableAllNeuromodulation()
        
        # Run learning simulation
        for step in range(200):
            brain.step(step)
            
            if step % 50 == 0:
                metrics = brain.getPerformanceMetrics()
                learning_stats = brain.getLearningStats()
                memory_stats = brain.getMemoryStats()
                
                print(f"  Step {step}: Rate={metrics['average_firing_rate']:.3f} Hz, "
                      f"Spikes={metrics['total_spikes']}, "
                      f"Dev={brain.getDevelopmentalStage()}")
        
        results[phase] = {
            'metrics': brain.getPerformanceMetrics(),
            'learning_stats': learning_stats,
            'memory_stats': memory_stats
        }
    
    return results

# Run learning analysis
analysis = analyzeLearningProgression(scale=0.3)
```

##### Multi-Agent System:

```python
import pynlm

def createMultiAgentSystem(world_config, num_agents=3):
    """Create a multi-agent system sharing neural resources"""
    # Shared brain
    brain = pynlm.createBrainFromPhase(3)
    brain.initialize()
    
    agents = []
    worlds = []
    
    for i in range(num_agents):
        # Each agent gets its own world but shares the brain
        world = pynlm.createSimpleWorld()
        world.configure(**world_config)
        world.reset()
        world.setAgentStart(i * 5.0, i * 5.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        agents.append(agent)
        worlds.append(world)
    
    return agents, worlds, brain

# Create multi-agent system
agents, worlds, brain = createMultiAgentSystem(
    {'width': 20, 'height': 20, 'visionWidth': 8, 'visionHeight': 8},
    num_agents=3
)

# Run simulation for all agents
for step in range(100):
    for i, (agent, world) in enumerate(zip(agents, worlds)):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step} completed for all {len(agents)} agents")
```

### Key Enhancements Summary

1. **Performance Monitoring**: Real-time metrics and performance tracking
2. **Phase-based Configuration**: Pre-configured settings for all development phases
3. **Advanced Learning Analysis**: Comprehensive statistics on learning mechanisms
4. **Multi-Agent Support**: Systems where agents share neural resources
5. **Enhanced Error Handling**: Robust exception management with detailed error information
6. **Real-Time Monitoring**: Live visualization of brain activity and performance
7. **Learning Progression Analysis**: Compare behavior across different phases
8. **Resource Optimization**: Efficient batch processing and memory management

### Migration Guide

#### From Basic to Enhanced API

**Before:**

```python
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)
```

**After:**

```python
import pynlm

# Use phase-based configuration (recommended)
brain = pynlm.createBrainFromPhase(4)
brain.initialize()

# Enable all learning systems
brain.enableAllPlasticity()
brain.enableAllNeuromodulation()

# Use enhanced monitoring
for i in range(100):
    brain.step(i)
    if i % 10 == 0:
        metrics = brain.getPerformanceMetrics()
        print(f"Progress: {metrics['average_firing_rate']:.3f} Hz")
```

### Best Practices

1. **Use phase-based configurations** when creating brains
2. **Enable appropriate plasticity mechanisms** for your use case
3. **Monitor performance** during long simulations
4. **Handle exceptions** gracefully with specific exception types
5. **Validate configurations** before use
6. **Use the comprehensive API** for advanced features
7. **Consider memory usage** for large-scale simulations

### Troubleshooting

**"My simulation is slow"**
- Use `brain.stepMultiple(steps)` for efficient batch processing
- Monitor with `brain.getPerformanceMetrics()`
- Consider Phase 4 for better performance optimization

**"I need better error reporting"**
- Use specific exception types: `pynlm.InvalidArgumentError`, etc.
- Validate configurations with `config.validate()`

**"I want to analyze learning"**
- Use `brain.getLearningStats()` and `brain.getMemoryStats()`
- Compare different phases with `pynlm.createBrainFromPhase(phase)`

---

## Part 3: Using the Python Library
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
