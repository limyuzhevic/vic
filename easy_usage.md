# NLM - Complete Usage Guide

NLM (Neural Learning Machine) is a brain-inspired spiking neural network simulator that implements artificial developmental brains with multiple integrated systems including perception, memory, learning, and action.

This comprehensive guide covers:

1. **Getting Started** - Quick introduction and basic usage
2. **Building and Installation** - Building from source and Python bindings
3. **Advanced Usage** - Configuration, demonstrations, and practical examples
4. **API Reference** - Complete Python API documentation
5. **Troubleshooting** - Common issues and solutions

## Table of Contents

- [What is NLM?](#what-is-nlm)
- [Quick Start Examples](#quick-start-examples)
- [Building and Installation](#building-and-installation)
- [Running Demos](#running-demos)
- [Advanced Configuration](#advanced-configuration)
- [Key Features](#key-features)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)

---

# What is NLM?

NLM is an experimental artificial brain project that simulates biological neural computation using:

- **Leaky Integrate-and-Fire (LIF) neurons** - Biologically realistic neuron models
- **Event-driven spike propagation** - Efficient spike communication
- **Multiple plasticity mechanisms** - STDP, Hebbian learning, structural plasticity
- **Integrated brain systems** - Memory, prediction, development, neuromodulation
- **World interaction** - Environment sensing and action

The brain develops and learns through interaction with an environment, creating emergent cognitive capabilities.

### Key Capabilities

- **Autonomous learning** from experience and rewards
- **Memory formation** and consolidation
- **Developmental progression** through life stages
- **Predictive processing** of sensory input
- **Goal-directed behavior** through planning and action selection
- **Social learning** through observation and imitation

---

# Quick Start Examples

## Example 1: Simple Brain Simulation

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Simulation complete!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Example 2: Complete Agent in World

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.connection_probability", 0.1)
    
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
    
    # 6. Enable learning subsystems
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
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_agent_simulation(1000)
print("Agent simulation complete!")
```

## Example 3: Text Learning Demo

```python
import pynlm

# Create brain with text capability
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable text learning
agent.enableCuriosity(False)
agent.enableRewardModulation(False)  # Pure STDP association

print("=== Text Learning Demo ===")
print("Teaching 'apple' through co-presentation with resource scene...")

# This demo shows how text can be learned as a sensory pattern
# by presenting it together with visual scenes
for episode in range(5):
    # Present text with resource scene
    world.addObject(pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Resource, value=1.0))
    percept = world.getSensoryPercept()
    percept.setText("apple")  # Teach the word
    
    agent.processSensoryInput(percept)
    brain.step(episode * 100)
    
    print(f"Episode {episode + 1}: Brain learned text association")

print("Text learning complete!")
```

---

# Building and Installation

## Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (automatically included with scikit-build-core)

## Building from Source

### Standard C++ Build

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

### Python Bindings

```bash
# Install build dependencies (use a venv on PEP 668 systems)
pip install scikit-build-core pybind11 pytest numpy

# Build and install with Python bindings
pip install .

# Or install in development mode
pip install -e .
```

## Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_phase1_demo` | Executable | Phase 1 demo (project infrastructure) |
| `nlm_phase2_demo` | Executable | Phase 2 demo (neural computation) |
| `nlm_phase3_demo` | Executable | Phase 3 demo (world interaction) |
| `nlm_phase4_demo` | Executable | Phase 4 demo (emergent cognition) |
| `nlm_phase5_demo` | Executable | Phase 5 demo (performance optimizations) |
| `nlm_phase6_demo` | Executable | Phase 6 integration demo (all systems) |
| `nlm_phase6_text_demo` | Executable | Phase 6 text teaching demo |
| `nlm_benchmark` | Executable | Performance benchmarks |
| `nlm_test` | Executable | Unit test suite |
| `pynlm` | Python Module | Python bindings for NLM |

---

# Running Demos

## Unified Demo Launcher

NLM includes a unified demo launcher script for easy access to all demonstrations:

```bash
# Show help
./demo --help

# List all demos
./demo --list

# Run a specific demo
./demo phase1      # Phase 1: Project Infrastructure
./demo phase2      # Phase 2: Real Neural Computation
./demo phase3      # Phase 3: World Interaction
./demo phase4      # Phase 4: Emergent Cognition
./demo phase5      # Phase 5: Performance Optimizations
./demo phase6      # Phase 6: Integration Test
./demo phase6-text # Phase 6: Text Teaching
./demo benchmark   # Performance Benchmarks
```

## Demo Descriptions

### Phase 1: Project Infrastructure

**File**: `src/experiments/Phase1Demo.cpp`
**Executable**: `nlm_phase1_demo`

Demonstrates the basic project foundation:
- Core types and configuration system
- Basic neural interfaces (brain, neurons, synapses)
- Development and neuromodulation systems
- Memory systems
- Simple simulation

### Phase 2: Real Neural Computation

**File**: `src/main.cpp`
**Executable**: `nlm`

Shows real spiking neural computation:
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with delays
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)
- Learning experiment demonstrating measurable changes

### Phase 3: World Interaction

**File**: `src/experiments/Phase3Demo.cpp`
**Executable**: `nlm_phase3_demo`

Demonstrates world models and interaction:
- World interaction: experience → neural activity → action → consequence → learning
- Reward modulation, curiosity, development, plasticity
- Real-world simulation with sensory input and motor output

### Phase 4: Emergent Cognition

**File**: `src/experiments/Phase4Demo.cpp`
**Executable**: `nlm_phase4_demo`

Shows emergent cognitive capabilities:
- Temporal prediction
- Working memory
- Episodic recall
- Concept formation
- Neural attention
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5: Performance Optimization

**File**: `src/experiments/Phase5Demo.cpp`
**Executable**: `nlm_phase5_demo`

Demonstrates performance optimizations:
- Brain-personality/sandbox-teachable brain
- Pattern probe
- Sandbox run
- Checkpoint round-trip verification

### Phase 6: Integration

**Files**: `src/experiments/Phase6IntegrationDemo.cpp`, `src/experiments/Phase6TextDemo.cpp`
**Executables**: `nlm_phase6_demo`, `nlm_phase6_text_demo`

#### Integration Demo
Shows that all systems are properly connected:
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and neural dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional

#### Text Demo
Shows advanced Phase 6 capabilities:
- Text as a sense (word teaching through co-presentation)
- Planner in the loop (planner overrides and decisions)
- Concepts, self-model, social learning
- Advanced agent capabilities

### Benchmark

**File**: `src/benchmark.cpp`
**Executable**: `nlm_benchmark`

Performance benchmarks for measuring NLM efficiency.

---

# Advanced Configuration

## Configuration System

NLM uses a hierarchical configuration system for controlling all aspects of the brain and simulation. Configuration can be loaded from:

1. **Default configuration** (built-in defaults)
2. **Configuration file** (JSON or .cfg format)
3. **Command line arguments**
4. **Programmatic configuration**

### Configuration Parameters

#### Brain Configuration

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

#### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

#### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

#### Environment Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `environment.name` | string | GridWorld | Environment name |
| `environment.width` | int | 10 | Environment width |
| `environment.height` | int | 10 | Environment height |

### Configuration Examples

#### Python Configuration

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Modify parameters
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.15)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("neuromod.dopamine.scale", 2.0)

# Save to file
config.saveToFile("my_config.json")

# Create brain with custom configuration
brain = pynlm.createBrain(config)
```

#### Command Line Configuration

```bash
# Run with custom configuration
./nlm_phase4_demo --config my_config.json

# Override specific parameters
./nlm_phase4_demo --brain.neuron_count 5000 --plasticity.stdp.learning_rate 0.001
```

---

# Key Features

## Python API Features

### Core Classes

#### Brain
- **Creation**: `pynlm.createBrain(config)`
- **Initialization**: `brain.initialize()`
- **Simulation**: `brain.step(step_number)`
- **Input/Output**: `brain.receiveSensoryInput()`, `brain.produceAction()`
- **Statistics**: `brain.getTotalNeuronCount()`, `brain.getTotalSpikeCount()`, etc.
- **State Management**: `brain.save()`, `brain.load()`
- **Regions**: `brain.addRegion()`, `brain.getRegion()`

#### Config
- **Creation**: `pynlm.createDefaultConfig()`
- **Query**: `config.has(key)`, `config.getKeys()`
- **Modification**: `config.set(key, value)`
- **File I/O**: `config.loadFromFile()`, `config.saveToFile()`

#### AgentBrain
- **Creation**: `pynlm.createAgentBrain(brain)`
- **Initialization**: `agent.initialize(world)`
- **Sensory Processing**: `agent.processSensoryInput(percept)`
- **Motor Decoding**: `agent.decodeMotorCommand()`
- **Neuromodulation**: `agent.applyRewardModulation(reward, predicted_reward)`
- **Development**: `agent.updateDevelopment(timestep)`
- **Subsystems**: `agent.enableRewardModulation()`, `agent.enableCuriosity()`, etc.

#### SimpleWorld
- **Creation**: `pynlm.createSimpleWorld()`
- **Configuration**: `world.configure(width, height, visionWidth, visionHeight)`
- **Control**: `world.reset()`, `world.update(timestep)`
- **Agent Interaction**: `world.setAgentStart(x, y)`, `world.applyMotorCommand()`
- **State Access**: `world.getSensoryPercept()`, `world.getAgentBody()`
- **World Objects**: `world.addObject()`, `world.removeObject()`

### Available Enumerations

#### Neuron Types
```python
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
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

#### Motor Commands
```python
pynlm.MotorCommand.MoveForward
pynlm.MotorCommand.MoveBackward
pynlm.MotorCommand.TurnLeft
pynlm.MotorCommand.TurnRight
pynlm.MotorCommand.LookLeft
pynlm.MotorCommand.LookRight
pynlm.MotorCommand.Interact
pynlm.MotorCommand.Wait
```

#### World Object Types
```python
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

---

# Troubleshooting

## Common Issues and Solutions

### Build Issues

#### CMake cannot find Python

```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

#### Compilation errors

- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

#### ImportError: No module named 'pynlm'

```bash
pip install --force-reinstall .
```

#### Segmentation faults

- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify configuration parameters are valid

### Simulation Issues

#### "My brain isn't doing anything"

- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Ensure neurons have sufficient input to fire

#### "The agent isn't moving"

- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify motor commands are valid

#### "Everything is 0"

- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check configuration parameters (neuron count, connection probability)

### Configuration Issues

#### Invalid configuration values

```python
# Check if configuration keys exist
if config.has("brain.neuron_count"):
    config.set("brain.neuron_count", 1000)
```

#### Configuration file not found

```python
import os
config_path = "configs/my_config.json"
if os.path.exists(config_path):
    config.loadFromFile(config_path)
else:
    print("Using default configuration")
```

---

# Contributing

## Project Structure

```
NLM/
├── CMakeLists.txt              # Main build configuration
├── pyproject.toml              # Python package configuration
├── README.md                   # Project overview and setup
├── easy_usage.md               # Easy usage guide (this file)
├── HOW_TO_USE.md                # Comprehensive usage guide
├── configs/                    # Default configuration files
│   └── default.cfg
├── docs/                       # Documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   └── PHASE6_FINAL_AUDIT.md
├── src/                        # Source code
│   ├── core/                   # Core utilities
│   ├── agent/                  # Agent-brain interface
│   ├── world/                  # Simple world
│   ├── performance/            # Performance optimizations
│   ├── brain/                  # Neural components (integrated)
│   ├── dynamics/               # Neural dynamics
│   ├── plasticity/             # Plasticity rules
│   ├── development/            # Developmental system
│   ├── neuromodulation/         # Neuromodulators (integrated)
│   ├── memory/                 # Memory systems (integrated)
│   ├── prediction/             # Prediction systems (integrated)
│   ├── cognition/              # Cognitive mechanisms (integrated)
│   ├── sensory/                # Sensory processing
│   ├── motor/                  # Motor system
│   ├── environment/            # Environment interface
│   ├── experiments/            # Experiment framework
│   └── visualization/          # Visualization
├── tests/                      # Test suite
├── python/                     # Python bindings
│   ├── bindings.cpp            # Pybind11 bindings
│   └── CMakeLists.txt          # Python bindings build (deprecated)
├── demo                        # Unified demo launcher (executable)
└── build/                      # Build directory (ignore in git)
```

## Development Guidelines

### Code Style

- Follow C++20 best practices
- Use consistent naming conventions
- Include comprehensive comments
- Add appropriate error handling
- Use meaningful variable names

### Testing

- Write unit tests for core functionality
- Test all major demo scenarios
- Verify integration between systems
- Test edge cases and error conditions

### Documentation

- Maintain clear documentation
- Include API documentation for Python bindings
- Provide usage examples
- Document configuration parameters

## License

MIT

## Authors

Research project — See docs for scientific background.

---

Generated by Kilo - AI Assistant for Software Engineering
Learn more at https://github.com/kilo-ai/kilo