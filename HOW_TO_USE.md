# NLM (Neural Learning Machine) - Enhanced HOW TO USE Documentation

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

**Version**: 0.1.0 (Phase 6: Final Integration)

This document covers:
1. Building the C++ project from source
2. Installing the Python library with enhanced features
3. Using the enhanced NLM Python API
4. Advanced usage patterns

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps (Enhanced)

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets with parallel jobs
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure

# Build Python bindings
pip install scikit-build-core pybind11 pytest numpy
pip install -e .
```

### Build Outputs (Enhanced)

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase6_demo` | Executable | **NEW** Phase 6 integration demo |
| `nlm_test` | Executable | Unit test suite |
| `pynlm` | Python Module | **ENHANCED** Python bindings |

### Build Options (Enhanced)

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Build with OpenMP for parallelism
cmake .. -DOpenMP_CXX_FLAGS="-fopenmp"

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install

# Build with performance optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_OPTIMIZATION=HIGH
```

---

## Part 2: Installing the Python Library (Enhanced)

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install with enhanced features
pip install -e .

# Or install from source
python -m pip install .
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

# Development installation
pip install -e --no-build-isolation .
```

### Verifying Enhanced Installation

```python
import pynlm

# Check version information
print(f"NLM Version: {pynlm.__version__}")
print(f"Description: {pynlm.__description__}")

# Test enhanced utility functions
config = pynlm.createDefaultConfig()
print(f"Default config keys: {config.getKeys()}")

# Test configuration validation
try:
    config2 = pynlm.createConfigFromDict({
        "brain.neuron_count": 1000,
        "neuromod.curiosity.enable": True
    })
    if pynlm.validateConfig(config2):
        print("Configuration validation: PASSED")
except Exception as e:
    print(f"Configuration validation: FAILED - {e}")

# Create a simple brain and test
brain = pynlm.createBrain(config)
if brain.initialize():
    print("Brain initialization: SUCCESS")
    print(f"Neurons created: {brain.getTotalNeuronCount()}")
else:
    print("Brain initialization: FAILED")
```

---

## Part 3: Using the Enhanced Python Library

### Basic Usage (Enhanced)

#### Creating a Brain with Advanced Configuration

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Configure specific parameters using enhanced config methods
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("development.enable", True)
config.set("plasticity.structural.enable", True)

# Create and initialize the brain
brain = pynlm.createBrain(config)
if not brain.initialize():
    raise RuntimeError("Brain initialization failed!")

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

#### Running Enhanced Simulation

```python
import pynlm

# Create brain with enhanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with enhanced monitoring
for step in range(1000):
    brain.step(step)
    
    # Enhanced statistics collection
    if step % 100 == 0:
        firing_rate = brain.getAverageFiringRate()
        firing_count = brain.getFiringNeuronCount()
        total_spikes = brain.getTotalSpikeCount()
        
        print(f"Step {step}:")
        print(f"  Average firing rate: {firing_rate:.2f} Hz")
        print(f"  Currently firing: {firing_count} neurons")
        print(f"  Total spikes: {total_spikes}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
```

#### Creating Enhanced World with Configuration

```python
import pynlm

# Create a simple 2D world with enhanced configuration
world = pynlm.createSimpleWorld()

# Enhanced world configuration
world.configure(
    width=30,
    height=30,
    visionWidth=12,
    visionHeight=12
)

# Enhanced world setup
world.reset()
world.setAgentStart(15.0, 15.0)
world.setMaxEnergy(100.0)
world.setEnergyDecayRate(0.01)
world.setRandomSeed(42)

# Verify world configuration
print(f"World dimensions: {world.getWidth()}x{world.getHeight()}")
print(f"Vision dimensions: {world.getVisionWidth()}x{world.getVisionHeight()}")
print(f"Max energy: {world.getMaxEnergy()}")
print(f"Random seed: {world.getRandomSeed()}")
```

### Complete Enhanced Agent Example

```python
import pynlm
import time

def run_enhanced_simulation(num_steps=2000):
    """Enhanced simulation with advanced features and monitoring."""
    
    # Enhanced 1: Create configuration with all learning systems
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("brain.region_count", 2)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("development.enable", True)
    config.set("development.initial_stage", "CriticalPeriod")
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.stdp.enable", True)
    
    # Enhanced 2: Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enhanced 3: Create agent with advanced interface
    agent = pynlm.createAgentBrain(brain)
    
    # Enhanced 4: Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=40, 
        height=40, 
        visionWidth=16, 
        visionHeight=16
    )
    world.reset()
    world.setAgentStart(20.0, 20.0)
    
    agent.initialize(world)
    
    # Enhanced 5: Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("=== Enhanced NLM Simulation Started ===")
    print(f"Simulation duration: {num_steps} steps")
    print(f"Neuron count: {brain.getTotalNeuronCount()}")
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    
    # Enhanced 6: Performance monitoring
    start_time = time.time()
    metrics = {
        'steps': [],
        'firing_rates': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'development_stages': [],
        'total_rewards': [],
        'step_times': []
    }
    
    # Enhanced 7: Main simulation loop with advanced features
    for step in range(num_steps):
        step_start = time.time()
        
        # World update with adaptive timestep
        timestep = 0.1
        world.update(timestep)
        
        # Get enhanced sensory percept
        percept = world.getSensoryPercept()
        
        # Enhanced sensory processing with noise and preprocessing
        if agent:
            agent.processSensoryInput(percept)
        
        # Enhanced brain computation with plasticity modulation
        brain.step(step, step * timestep)
        
        # Enhanced action decoding with multiple strategies
        motor_cmd = agent.decodeMotorCommand()
        
        # Enhanced motor execution with collision detection
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Enhanced reward calculation with prediction error
        if agent and percept.getInternal():
            reward = percept.getInternal()[0]
            predicted_reward = 0.0  # Would be computed by prediction system
            agent.applyRewardModulation(reward, predicted_reward)
        
        # Enhanced development update with stage transitions
        if agent and step % 10 == 0:
            agent.updateDevelopment(timestep)
        
        # Enhanced statistics collection
        step_end = time.time()
        step_time = step_end - step_start
        
        metrics['steps'].append(step)
        metrics['firing_rates'].append(brain.getAverageFiringRate())
        metrics['firing_counts'].append(brain.getFiringNeuronCount())
        metrics['total_spikes'].append(brain.getTotalSpikeCount())
        
        if agent:
            metrics['curiosity_levels'].append(agent.getCuriosityLevel())
            metrics['novelty_levels'].append(agent.getNoveltyLevel())
            metrics['development_stages'].append(brain.getDevelopmentalStage())
            metrics['total_rewards'].append(agent.getNeuromodulationLevel())
        
        metrics['step_times'].append(step_time)
        
        # Enhanced progress reporting
        if step % 200 == 0:
            avg_step_time = sum(metrics['step_times']) / len(metrics['step_times'])
            print(f"Step {step:4d} ({step/num_steps*100:5.1f}%): "
                  f"Firing: {brain.getFiringNeuronCount():4d}, "
                  f"Rate: {brain.getAverageFiringRate():5.2f}Hz, "
                  f"Curiosity: {agent.getCuriosityLevel() if agent else 0:.3f}, "
                  f"Stage: {brain.getDevelopmentalStage()}, "
                  f"Time: {avg_step_time*1000:.1f}ms/step")
    
    total_time = time.time() - start_time
    
    # Enhanced results analysis
    print("\n=== Simulation Complete ===")
    print(f"Total time: {total_time:.2f} seconds")
    print(f"Average step time: {total_time/num_steps*1000:.2f} ms")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Final development stage: {brain.getDevelopmentalStage()}")
    
    if agent:
        print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"Final neuromodulation: {agent.getNeuromodulationLevel():.3f}")
    
    return brain, agent, world, metrics

# Run the enhanced simulation
brain, agent, world, metrics = run_enhanced_simulation(2000)
```

### Key Enhanced Classes and Methods

#### Brain Class

```python
# Creation with enhanced configuration
brain = pynlm.createBrain(config)

# Enhanced initialization with error checking
if not brain.initialize():
    raise RuntimeError("Brain initialization failed!")

# Enhanced simulation with timestamps
brain.step(step_number)                    # Basic step
brain.step(step_number, timestamp)         # Step with time
brain.reset()                              # Reset brain
brain.save("checkpoint.bin")              # Save state
brain.load("checkpoint.bin")              # Load state

# Enhanced statistics and monitoring
brain.getTotalNeuronCount()                # Total neurons
brain.getTotalSynapseCount()               # Total synapses
brain.getFiringNeuronCount()              # Currently firing
brain.getAverageFiringRate()               # Average firing rate
brain.getExcitationInhibitionRatio()      # E/I balance
brain.getTotalSpikeCount()                 # Total spikes
brain.getDevelopmentalStage()             # Current developmental stage
brain.getConfig()                          # Get configuration
brain.logStatus()                          # Detailed status logging
```

#### Enhanced Config Class

```python
# Enhanced creation
config = pynlm.createDefaultConfig()

# Enhanced configuration methods
config.set("brain.neuron_count", 1000)           # Set integer
config.set("neuromod.dopamine.scale", 1.5)      # Set float
config.set("neuromod.curiosity.enable", True)   # Set boolean
config.set("brain.name", "my_brain")           # Set string
config.set("plasticity.rules", [1, 2, 3])      # Set list

# Enhanced query and validation
if config.has("brain.neuron_count"):
    neuron_count = config.getOr("brain.neuron_count", 1000)
    print(f"Neuron count: {neuron_count}")

keys = config.getKeys()                        # Get all keys
config.summary()                               # Get configuration summary
config.loadFromFile("config.json")            # Load from JSON
config.saveToFile("config.json")              # Save to JSON

# Enhanced dictionary creation
config_dict = {
    "brain.neuron_count": 1000,
    "neuromod.curiosity.enable": True,
    "plasticity.stdp.learning_rate": 0.001
}
config2 = pynlm.createConfigFromDict(config_dict)

# Enhanced JSON-like string creation
json_config = """
brain.neuron_count = 800
brain.region_count = 2
neuromod.dopamine.scale = 1.2
neuromod.curiosity.enable = true
"""
config3 = pynlm.createConfigFromJson(json_config)

# Enhanced validation
try:
    if pynlm.validateConfig(config3):
        print("Configuration is valid!")
except Exception as e:
    print(f"Configuration error: {e}")
```

#### Enhanced AgentBrain Class

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Enhanced initialization
agent.initialize(world)

# Enhanced sensory processing
agent.processSensoryInput(percept)

# Enhanced motor decoding
motor_cmd = agent.decodeMotorCommand()

# Enhanced neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Enhanced development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Enhanced subsystem controls
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Enhanced statistics
agent.getCuriosityLevel()                    # Current curiosity
agent.getNoveltyLevel()                      # Current novelty level
agent.getPredictionError()                   # Prediction error
agent.getNeuromodulationLevel()              # Overall neuromodulation
```

#### Enhanced SimpleWorld Class

```python
# Creation
world = pynlm.createSimpleWorld()

# Enhanced configuration
world.configure(
    width=30,
    height=30,
    visionWidth=12,
    visionHeight=12
)

# Enhanced control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Enhanced agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# Enhanced state access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# Enhanced world objects
world.addObject(pynlm.WorldObject(
    x, y, 
    pynlm.WorldObjectType.Resource, 
    value=1.0,
    radius=0.5
))

# Enhanced properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

### Enhanced Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types (extensive list)
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.MoveLeft
pynlm.ActionType.MoveRight
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.LookUp
pynlm.ActionType.LookDown
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
```

---

## Part 4: Enhanced Configuration Options

### Brain Configuration (Enhanced)

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
| `brain.region_count` | int | 1 | Number of brain regions |

### Enhanced Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `plasticity.structural.pruning_rate` | float | 0.00001 | Rate of synapse elimination |

### Enhanced Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.dopamine.baseline` | float | 0.1 | Baseline dopamine level |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.sensitivity` | float | 1.0 | Curiosity sensitivity |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.prediction_error.scale` | float | 1.0 | Prediction error scaling |

### Enhanced Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.enable` | bool | true | Enable developmental processes |
| `development.initial_stage` | string | "Initial" | Starting developmental stage |
| `development.stage_transition_rate` | float | 0.001 | Rate of stage transitions |
| `development.plasticity_modulation` | bool | true | Development effects on plasticity |

### Enhanced Checkpoint Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `checkpoint.enable` | bool | true | Enable checkpointing |
| `checkpoint.path` | string | "./checkpoints" | Directory for checkpoints |
| `checkpoint.frequency` | int | 1000 | Steps between checkpoints |
| `checkpoint.max_files` | int | 10 | Maximum checkpoint files to keep |
| `checkpoint.compress` | bool | true | Enable checkpoint compression |

### Enhanced Replay Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `replay.enable` | bool | true | Enable memory replay |
| `replay.frequency` | int | 100 | Steps between replays |
| `replay.episode_count` | int | 5 | Number of episodes to replay |
| `replay.consolidation_strength` | float | 0.5 | Strength of replay consolidation |

---

## Part 5: Enhanced Example Scripts

### Enhanced Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate with enhanced monitoring
for i in range(100):
    brain.step(i)
    
    # Enhanced statistics
    if i % 25 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons, "
              f"{brain.getAverageFiringRate():.2f} Hz average rate")

print("Enhanced simulation complete!")
```

### Enhanced Environment Interaction

```python
import pynlm

def run_enhanced_episode(config, world_config, max_steps=500):
    """Enhanced episode with learning and development."""
    
    # Enhanced setup
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(**world_config)
    world.reset()
    
    agent.initialize(world)
    
    # Enhanced learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print(f"Starting enhanced episode with {max_steps} steps")
    
    # Enhanced episode loop
    for step in range(max_steps):
        world.update(0.1)
        
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Enhanced reward calculation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
    
    return brain, agent, world

# Run enhanced episode
world_config = {
    "width": 20,
    "height": 20,
    "visionWidth": 8,
    "visionHeight": 8
}

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1500)

brain, agent, world = run_enhanced_episode(config, world_config)
```

---

## Part 6: Enhanced Troubleshooting

### Enhanced Build Issues

**"CMake cannot find Python:"**
```bash
pip install scikit-build-core pybind11 numpy
python -c "import pybind11; print(pybind11.get_include())"
cmake .. -DPython_EXECUTABLE=$(which python) -Dpybind11_DIR=$(python -c "import pybind11; import os; print(os.path.dirname(pybind11.__file__))")
```

**"Compilation errors with C++20:"**
- Ensure compiler supports C++20 (GCC 9+, Clang 7+, MSVC 2019+)
- Check CMake version >= 3.16
- Verify all source files are present
- Update compiler flags if needed:
  ```bash
  cmake .. -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_CXX_EXTENSIONS=OFF
  ```

### Enhanced Runtime Issues

**"ImportError: No module named 'pynlm':"**
```bash
# Uninstall any existing version
pip uninstall pynlm -y

# Force reinstall with enhanced features
pip install --force-reinstall --no-cache-dir -e .

# Or install from source
git clone https://github.com/nlm-project/nlm.git
cd nlm
pip install -e .
```

**"Segmentation faults:"**
```python
# Add enhanced error checking and validation
import sys
import traceback

try:
    brain = pynlm.createBrain(config)
    if not brain.initialize():
        raise RuntimeError("Brain initialization failed")
    
    for step in range(100):
        brain.step(step)
        
except Exception as e:
    print(f"Error occurred: {e}")
    print("Stack trace:")
    traceback.print_exc()
    sys.exit(1)

# Use enhanced checkpointing for debugging
brain.save("debug_checkpoint.bin")
```

**"Memory issues:"**
```python
import psutil
import os

process = psutil.Process(os.getpid())
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")

# Enhanced garbage collection
import gc
gc.collect()
print(f"Objects collected: {gc.collect()}")

# Use configuration to limit memory usage
config.set("brain.neuron_count", 500)  # Reduce neuron count
config.set("checkpoint.frequency", 500)  # Reduce checkpoint frequency
```

### Enhanced Performance Issues

**"Brain too slow:"**
```python
# Configuration for better performance
config.set("brain.neuron_count", 500)  # Reduce neurons
config.set("brain.synapse_density", 0.05)  # Reduce connections
config.set("development.enable", False)  # Disable development
config.set("plasticity.structural.enable", False)  # Disable structural plasticity

# Use incremental simulation
for step in range(0, 1000, 10):  # Every 10 steps
    brain.step(step)
```

**"Too much memory usage:"**
```python
# Monitor and limit memory
import tracemalloc

tracemalloc.start()

# Run simulation
for step in range(100):
    brain.step(step)
    
    # Check memory usage
    current, peak = tracemalloc.get_traced_memory()
    print(f"Current memory: {current / 1024 / 1024:.2f} MB")
    print(f"Peak memory: {peak / 1024 / 1024:.2f} MB")
    
    if current > 500 * 1024 * 1024:  # 500 MB limit
        print("Memory limit reached, stopping simulation")
        break

tracemalloc.stop()
```

---

## Part 7: Enhanced System Requirements

### Recommended Hardware

| Component | Minimum | Recommended |
|-----------|---------|-------------|
| CPU | Quad-core | Octo-core or higher |
| RAM | 4 GB | 16 GB or higher |
| GPU | None | Optional for visualization |
| Storage | 100 MB | 1 GB |

### Supported Platforms

- **Linux**: Ubuntu 18.04+, CentOS 8+, Debian 10+
- **Windows**: Windows 10, Windows Server 2019+
- **macOS**: macOS 10.15+, macOS 11+

### Software Dependencies

#### For Building:
- CMake 3.16+
- C++ compiler with C++20 support
- Python 3.8+
- pybind11 2.11.0+
- scikit-build-core
- numpy

#### For Running:
- Python 3.8+
- numpy (optional, for enhanced features)

---

## Part 8: Enhanced Integration with External Tools

### Python Integration

```python
# Use with popular scientific Python libraries
import pynlm
import numpy as np
import matplotlib.pyplot as plt

# Create brain with numpy arrays
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

brain = pynlm.createBrain(config)
brain.initialize()

# Use numpy for array operations
sensory_data = np.random.randn(8, 8)
# Convert to appropriate format for brain input

# Use matplotlib for visualization
plt.figure(figsize=(12, 8))
plt.plot(metrics['firing_rates'])
plt.title('Enhanced NLM Simulation: Firing Rate')
plt.xlabel('Step')
plt.ylabel('Firing Rate (Hz)')
plt.show()
```

### MATLAB Integration

```matlab
% MATLAB integration (if MATLAB is installed)
% Note: Requires MATLAB engine and Python integration
% python('import pynlm')
% 
% % Create brain from MATLAB
% config = py.pynlm.createDefaultConfig()
% config.set('brain.neuron_count', 1000)
% 
% brain = py.pynlm.createBrain(config)
% brain.initialize()
% 
% % Run simulation
% for step = 1:100
%     brain.step(step)
% end
```

---

## Part 9: Enhanced Educational Resources

### Learning Path

1. **Beginner**: Start with `easy_usage.md` and basic examples
2. **Intermediate**: Explore `HOW_TO_USE.md` and complete agent examples
3. **Advanced**: Study `docs/ARCHITECTURE.md` and experiment with parameters
4. **Research**: Use checkpointing and batch processing for large-scale studies

### Additional Resources

- **Source Documentation**: All .cpp and .hpp files have comprehensive comments
- **Architecture Guide**: `docs/ARCHITECTURE.md` - System design and components
- **Science Documentation**: `docs/SCIENCE.md` - Scientific background and methods
- **Experiments**: `src/experiments/` - Pre-built experiment frameworks
- **Tests**: `tests/` - Comprehensive test suite

### Advanced Topics

1. **Developmental Psychology**: Study brain maturation and learning stages
2. **Memory Systems**: Explore working memory, episodic memory, and associative memory
3. **Neuromodulation**: Understand dopamine, curiosity, and prediction error
4. **Plasticity**: Study synaptic changes and structural reorganization
5. **Attention**: Explore neural attention mechanisms
6. **Planning**: Study action planning and decision making

---

## Part 10: Enhanced Getting Help

### Community Support

- **GitHub Issues**: Report bugs and request features
- **Documentation**: Complete API documentation in docstrings
- **Examples**: Extensive examples in this document and source code
- **Tutorials**: Step-by-step guides in `easy_usage.md`

### Support Channels

- **Bug Reports**: Submit issues with reproduction steps
- **Feature Requests**: Suggest new functionality
- **Documentation Issues**: Report documentation errors
- **Performance Questions**: Ask about optimization

### Contact Information

For direct support:
- Check `README.md` for project links
- Review `docs/` directory for comprehensive documentation
- Explore source code with IDE for deeper understanding

---

## Part 11: Enhanced Future Directions

### Planned Enhancements

1. **Web Interface**: Web-based visualization and control
2. **Cloud Integration**: Cloud deployment and scaling
3. **Real-time Simulation**: Live interaction with external sensors
4. **Multi-agent Systems**: Coordination of multiple brains
5. **Hardware Acceleration**: GPU and specialized hardware support
6. **Virtual Reality**: Immersive simulation environments

### Research Opportunities

- **Brain-Computer Interfaces**: Integration with real neural signals
- **Neuromorphic Hardware**: Mapping to specialized hardware
- **Developmental Robotics**: Integration with robotics platforms
- **Cognitive Architecture**: Comparison with biological cognition

---

## Conclusion

NLM provides a comprehensive framework for simulating artificial brains with:

- **Phase 6 Integration**: All major brain systems working together
- **Enhanced Python API**: Rich set of utility functions and helper methods
- **Performance Optimizations**: SIMD vectorization and memory pools
- **Extensive Documentation**: Complete API reference and examples
- **Research Tools**: Checkpointing, replay, and performance monitoring

The enhanced version provides significantly improved usability, performance, and functionality while maintaining backward compatibility with existing code.

Enjoy exploring the world of artificial brain simulation with NLM!

**Version**: 0.1.0 (Enhanced)
**Phase**: 6 - Final Integration
**License**: MIT
