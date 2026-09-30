# NLM (Neural Learning Machine) - Complete Development Guide

## Overview

NLM is a **brain-inspired spiking neural network simulator** written in **C++20** with experimental Python bindings. It implements **Leaky Integrate-and-Fire (LIF) neurons** with **event-driven spike propagation**, **synaptic delays**, and **multiple plasticity mechanisms** including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Building and using Python bindings
3. Using the NLM C++ API
4. System architecture and components
5. Development best practices
6. Troubleshooting

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)

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
| `nlm` | Executable | Main simulation executable (Phase 6 integration demo) |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system (brain-world interface) |
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

## Part 2: Building Python Bindings

### Prerequisites

- scikit-build-core (recommended)
- pybind11 >= 2.11.0
- Python >= 3.8

### Build and Install

```bash
# Install build dependencies
pip install scikit-build-core pybind11 numpy typing-extensions

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

## Part 3: Using the NLM Python API

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
brain.step(step_number, time)     # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()            # Get motor action

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

#### Neuron types
```python
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal
```

#### Developmental stages
```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

#### Action types
```python
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
# ... and more
```

#### World object types
```python
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

## Part 6: Command-Line Interface

The main executable `nlm` provides command-line access to the simulation:

### Usage

```bash
nlm [options]

options:
  --help, -h              Show this help message
  --version               Show version information
  --quiet                 Suppress informational output
  --steps <n>             Number of simulation steps (default: 1000)
```

### Examples

```bash
# Run default integration demo
./nlm

# Run with quiet output
./nlm --quiet

# Show version
./nlm --version
```

The command-line interface runs the **Phase 6 integration demo** which demonstrates:
- Complete artificial brain with all subsystems
- Interaction with a simple world environment
- Memory, prediction, and neuromodulation
- Checkpointing and development

---

## Part 7: System Architecture

### Core Components

NLM implements a comprehensive brain architecture with the following systems:

#### 1. **Neural Core**
- LIF (Leaky Integrate-and-Fire) neurons
- Event-driven spike propagation with synaptic delays
- Multiple plasticity rules (STDP, Hebbian, structural)
- Real neural computation (Phase 2+)

#### 2. **Memory Systems** (Phase 6)
- **Working Memory**: Transient active information with competition
- **Episodic Memory**: Experience storage with replay capability
- **Associative Memory**: Pattern associations through Hebbian learning
- All memory systems connected to neural processing

#### 3. **Neuromodulation Systems** (Phase 6)
- **Dopamine**: Affects neural excitability and plasticity
- **Curiosity**: Drives exploration behavior
- **Novelty**: Detects novel sensory patterns
- **Prediction Error**: Optimizes learning through prediction

#### 4. **Prediction System** (Phase 6)
- Sensory prediction and error computation
- Confidence tracking
- Integrated into brain loop
n
**Prediction error signals affect learning rates**, creating an adaptive learning mechanism that responds dynamically to prediction accuracy.

#### 5. **Cognition Systems** (Phase 6)
- **Neural Planner**: Action sequence evaluation
- **Concept Formation**: Pattern discovery from experience
- **Attentional Selection**: Competitive focus mechanisms
- All cognition systems connected to perception and action

#### 6. **Development System** (Phase 6)
- Developmental stages affecting plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### 7. **Performance Optimizations** (Phase 5)
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Integration Architecture

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

---

## Part 8: Development Best Practices

### Coding Standards

1. **Use the pImpl idiom** for Brain class (large, complex implementation)
2. **Follow RAII principles** for all resource management
3. **Use namespaces** to organize code (nlm::)
4. **Implement proper destructors** and cleanup
5. **Use const correctness** where appropriate

### Configuration Best Practices

1. **Use typed configuration keys** (e.g., `neuron_count` not `"neuron_count"`)
2. **Provide sensible defaults** for all parameters
3. **Validate configuration values** at initialization
4. **Use ranges** for parameters with physical meaning (e.g., voltage ranges)
5. **Document all configuration options** with units and constraints

### Testing Best Practices

1. **Write unit tests** for all core components
2. **Test integration** between major subsystems
3. **Run regression tests** before major changes
4. **Use continuous integration** for automated testing
5. **Profile performance** for critical code paths

### Performance Considerations

1. **Use SIMD** for vector operations where possible
2. **Implement sparse connectivity** for large networks
3. **Use memory pools** for object allocation
4. **Cache frequently accessed data**
5. **Profile with realistic workloads**

---

## Part 9: Advanced Features

### For Advanced Users

#### 1. Custom Neuron Models
Extend the LIF model with:
- Hodgkin-Huxley dynamics
- Ionic channel models
- Dendritic computation

#### 2. Advanced Plasticity Rules
- Trace-based learning rules
- Homeostatic plasticity
- Metaplasticity (plasticity of plasticity)

#### 3. Complex Environment Models
- 3D worlds with physics
- Social interactions
- Multi-agent systems

#### 4. Custom Memory Architectures
- Hierarchical memory
- Meta-learning systems
- Forgetting mechanisms

#### 5. Neuromodulation Extensions
- Additional neuromodulators
- Complex interaction rules
- Cognitive states

### Python API Extensions

The Python bindings provide access to all C++ functionality:

```python
# Advanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.region_count", 4)
config.set("plasticity.stdp.learning_rate", 0.0005)

# Custom world with custom objects
world = pynlm.createSimpleWorld()
world.configure(width=100, height=100, visionWidth=16, visionHeight=16)

# Advanced agent features
agent.enableCuriosity(True)
agent.enableNovelty(True)
agent.enableRewardModulation(True)

# Performance monitoring
brain.logStatus()
```

---

## Part 10: Troubleshooting

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
- Check for null pointer dereferences in configuration

**Performance issues:**
- Increase memory pool sizes for large networks
- Use SIMD-optimized versions for critical code paths
- Profile with realistic workloads

### Common Errors and Solutions

1. **"Brain initialization failed"**
   - Check configuration parameters
   - Ensure required systems are initialized
   - Verify system resources (memory, file handles)

2. **"No neurons found!"**
   - Check neuron_count parameter
   - Verify brain initialization
   - Check region creation logic

3. **"Plasticity not triggering"**
   - Check plasticity enable flags
   - Verify spike history recording
   - Check weight update rules

4. **"Memory not integrating"**
   - Verify memory system initialization
   - Check connectivity between brain and memory
   - Verify message passing

---

## Part 11: Further Reading

### Documentation

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 implementation audit

### Research Papers

- "Spiking Neural Networks with Event-Driven Dynamics"
- "Neuromodulation in Artificial Neural Systems"
- "Memory Systems in Computational Brains"
- "Development and Learning in Artificial Brains"

### Online Resources

- [NLM Project Repository](https://github.com/nlm-project/nlm)
- [NLM Documentation](https://nlm.readthedocs.io)
- [Neural Network Simulation Blog](https://blog.nnsim.org)

---

## Version Information

**Current Version**: 0.1.0
**Phase**: 6 - Final Integration
**License**: MIT
**Authors**: NLM Authors

This document describes NLM version 0.1.0, which implements Phase 6 (Final Integration) of the Neural Learning Machine project. The current build includes:

- ✅ Complete neural computation engine (Phase 2)
- ✅ All memory systems (Phase 6)
- ✅ Neuromodulation (Phase 6)
- ✅ Prediction system (Phase 6)
- ✅ Cognition systems (Phase 6)
- ✅ Development (Phase 6)
- ✅ Performance optimizations (Phase 5)
- ✅ Checkpointing (Phase 5)

### Changes from Documentation

This document corrects several issues from previous documentation:

1. **Python API**: The Python bindings are now properly implemented
2. **Phase 6**: The main executable now demonstrates Phase 6 integration
3. **Architecture**: Documentation now accurately reflects the actual implementation
4. **Configuration**: Configuration system is properly documented and tested

### Future Development

Planned improvements for future releases:

1. **Phase 7**: Enhanced learning algorithms
2. **Phase 8**: Social learning and multi-brain systems
3. **Phase 9**: Cognitive architecture extensions
4. **Python 3.12**: Full support for latest Python features

---

**Thank you for using NLM (Neural Learning Machine)!

For support and questions, visit:
- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Documentation: https://nlm.readthedocs.io
- Discord: https://discord.gg/nlm"