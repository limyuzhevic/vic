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
# Install build dependencies (use a venv on PEP 668 systems, where
# system pip refuses to install packages)
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

> Phase 5 note: `pip install .` now genuinely builds `pynlm` (verified:
> fresh venv, `pip install .`, then `import pynlm` and the easy_usage
> Example 1 flow run). Previously the root CMakeLists never built the
> module, so `pip install .` succeeded yet `import pynlm` failed. A
> plain C++ `cmake` build still does NOT include pynlm unless you pass
> `-DNLM_BUILD_PYTHON_BINDINGS=ON` (and have pybind11 installed); the
> standalone `cmake -S python -B build` route also still works.

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

## Part 5: NLM Tools and Utilities

The NLM project now includes advanced tools for configuration management, simulation utilities, and development support.

### 5.1 Configuration Management Tool

The NLM Configuration Tool (`config_tool`) provides a command-line interface for managing brain configurations.

#### Installation

The config tool is built as part of the NLM project. After building NLM, you can run:

```bash
# Build the project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# The config_tool executable will be available in the build directory
cd build && ./bin/config_tool
```

#### Usage

```bash
# View all configuration keys
config_tool --list

# Get a specific value
config_tool --get neuron_count

# Set a value
config_tool --set neuron_count 2000

# Export configuration to file
config_tool --export config.json

# Import configuration from file
config_tool --import config.json

# Create default configuration
config_tool --create-default my_config.cfg

# Validate configuration
config_tool --validate
```

#### Available Options

- `--list`, `-l` - List all configuration keys
- `--get <key>`, `-g <k>` - Get value for key
- `--set <key> <value>`, `-s <k> <v>` - Set value for key
- `--delete <key>`, `-d <k>` - Delete key
- `--export <file>`, `-e <f>` - Export configuration to file
- `--import <file>`, `-i <f>` - Import configuration from file
- `--validate` - Validate configuration
- `--create-default <file>` - Create default configuration
- `--verbose`, `-v` - Verbose output
- `--backup` - Create backup before saving
- `--force` - Force operation without confirmation

#### Examples

```bash
# Create and save a custom configuration
config_tool --set neuron_count 3000
config_tool --set connection_probability 0.15
config_tool --set random_seed 123

# Backup and update configuration
cd configs
config_tool --set neuron_count 5000 --backup

# Compare two configurations
config_tool --import config1.json
config_tool --import config2.json
config_tool --merge --format json
```

### 5.2 Python Convenience Module

The NLM Convenience Module provides higher-level APIs for common operations, making it easier to create complex simulations.

#### Installation

After installing pynlm:

```bash
pip install -e .
```

Import the convenience module:

```python
import pynlm
from nlm_convenience import NLMBuilder, SimulationRunner, BrainProfiler
```

#### 5.2.1 NLMBuilder - Fluent Configuration

The `NLMBuilder` class provides a fluent API for creating complete NLM systems.

```python
from nlm_convenience import NLMBuilder

# Create system with all features
brain_system = (NLMBuilder()
    .set_brain_size(2000, 2)
    .set_plasticity(True, True, True, True)
    .set_neuromodulation(True, True, True, True)
    .set_developmental(True)
    .set_world(20, 20, 8, 8)
    .build())

brain = brain_system.brain
agent = brain_system.agent
world = brain_system.world
```

#### 5.2.2 SimulationRunner - Advanced Tracking

```python
from nlm_convenience import SimulationRunner

runner = SimulationRunner()
metrics = runner.run_episode(
    brain, world, agent,
    max_steps=1000,
    enable_learning=True
)

print(f"Total spikes: {metrics['total_spikes']}")
print(f"Average firing rate: {metrics['average_firing_rate']:.2f} Hz")
```

#### 5.2.3 BrainProfiler - Brain Analysis

```python
from nlm_convenience import BrainProfiler

profile = BrainProfiler.analyze_spike_patterns(brain, window_size=100)
print(f"E/I ratio: {profile['excitatory_inhibition_ratio']:.2f}")
print(f"Burst count: {profile['burst_count']}")
```

#### 5.2.4 NLMTools - Utility Functions

```python
from nlm_convenience import NLMTools

# Export brain state
NLMTools.export_brain_state(brain, "checkpoint.bin")

# Benchmark configuration
metrics = NLMTools.benchmark_configuration(config, steps=1000)
```

#### Complete Example

```python
"""Advanced NLM simulation using convenience tools"""

import pynlm
from nlm_convenience import NLMBuilder, SimulationRunner

# Create system
brain_system = (NLMBuilder()
    .set_brain_size(3000, 2)
    .set_plasticity(True, True, True, True)
    .set_neuromodulation(True, True, True, True)
    .set_developmental(True)
    .set_world(30, 30, 12, 12)
    .build())

brain = brain_system.brain
agent = brain_system.agent
world = brain_system.world

# Run simulation with tracking
runner = SimulationRunner()
metrics = runner.run_episode(
    brain, world, agent,
    max_steps=1000,
    enable_learning=True
)

print(f"Simulation completed: {metrics['total_spikes']} spikes,")
print(f"{metrics['average_firing_rate']:.2f} Hz average firing rate")
```

### 5.3 Development and Debugging

#### Checkpoint System

```python
# Save brain state
brain.save("checkpoint.bin")

# Load brain state
brain.load("checkpoint.bin")
```

#### Logging Configuration

```python
# Enable verbose logging
Logger::getGlobal().setLevel(LogLevel::Debug)

# Configure file logging
auto logger = std::make_shared<Logger>();
auto fileLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
fileLogger->setOutputFile("simulation.log");
logger->addLogger(fileLogger);
Logger::setGlobal(logger);
```

### 5.4 Performance Tools

#### Benchmarking

```python
// Command line
./nlm_benchmark

// Python benchmark
from nlm_convenience import NLMTools
metrics = NLMTools.benchmark_configuration(config, steps=1000)
```

#### Performance Profiling

```python
from nlm_convenience import BrainProfiler

// Profile brain activity
profile = BrainProfiler.analyze_spike_patterns(brain, 1000)
```

### 5.5 Complete Workflow Example

```python
"""Complete NLM workflow using tools"""

import pynlm
from nlm_convenience import NLMBuilder, SimulationRunner

// 1. Create configuration using config_tool
// config_tool --create-default config.cfg

// 2. Load configuration in Python
config = pynlm.createDefaultConfig()
config.loadFromFile("config.cfg")

// 3. Create system
brain_system = (NLMBuilder()
    .set_brain_size(config.getOr<int64_t>("neuron_count", 1000))
    .set_plasticity(True, True, True, True)
    .set_neuromodulation(True, True, True, True)
    .build())

// 4. Run simulation with full tracking
runner = SimulationRunner()
metrics = runner.run_episode(
    brain_system.brain,
    brain_system.agent,
    brain_system.world,
    max_steps=2000,
    enable_learning=True
)

// 5. Export results
NLMTools.export_brain_state(brain_system.brain, "final_checkpoint.bin")

// 6. Log metrics
NLM_LOG_INFO("Simulation complete: " + std::to_string(metrics['total_spikes']) + " spikes")
```

## Part 6: Advanced Features

### 6.1 Configuration API

The configuration system now supports:

- **JSON/YAML export/import** (planned)
- **Template configurations** (planned)
- **Configuration validation** (implemented)
- **Merge operations** (planned)

### 6.2 Python Integration

- **Convenience module** (implemented)
- **Advanced APIs** (implemented)
- **Error handling** (improved)
- **Documentation** (comprehensive)

### 6.3 Development Tools

- **Configuration tool** (implemented)
- **Checkpoint system** (enhanced)
- **Profiling tools** (implemented)
- **Benchmarking** (implemented)

## Next Steps

1. **Explore the convenience module** - Try the advanced APIs for complex simulations
2. **Use the config tool** - Manage configurations from the command line
3. **Study the examples** - Review complete examples in the documentation
4. **Benchmark your system** - Test performance with different configurations
5. **Export results** - Save brain states and configuration for reproducibility

The NLM tools and utilities make it easier to create sophisticated simulations while maintaining full access to the system's power and flexibility.

---

**Note**: This documentation covers the NLM tools and utilities available in the current version. New features are continuously being added based on user feedback and requirements.
